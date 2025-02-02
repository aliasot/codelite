//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : cppchecker.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include <wx/msgdlg.h>
#include <wx/app.h>
#include "processreaderthread.h"
#include <wx/imaglist.h>
#include <wx/ffile.h>
#include "imanager.h"
#include "procutils.h"
#include "event_notifier.h"
#include "cppcheckreportpage.h"
#include "cppchecksettingsdlg.h"
#include <wx/process.h>
#include <wx/dir.h>
#include "notebook_ex.h"
#include "workspace.h"
#include "project.h"
#include "fileextmanager.h"
#include "jobqueue.h"
#include "cppchecker.h"
#include "cl_process.h"
#include <wx/stdpaths.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include <wx/xml/xml.h>
#include <wx/sstream.h>
#include <wx/log.h>
#include <wx/tokenzr.h>

static CppCheckPlugin* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
    if (thePlugin == 0) {
        thePlugin = new CppCheckPlugin(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah & Jérémie (jfouche)"));
    info.SetName(wxT("CppChecker"));
    info.SetDescription(_("CppChecker integration for CodeLite IDE"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
    return PLUGIN_INTERFACE_VERSION;
}


BEGIN_EVENT_TABLE(CppCheckPlugin, wxEvtHandler)
    EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  CppCheckPlugin::OnCppCheckReadData)
    EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, CppCheckPlugin::OnCppCheckTerminated)
END_EVENT_TABLE()

CppCheckPlugin::CppCheckPlugin(IManager *manager)
    : IPlugin(manager)
    , m_cppcheckProcess(NULL)
    , m_canRestart( true )
    , m_explorerSepItem(NULL)
    , m_workspaceSepItem(NULL)
    , m_projectSepItem(NULL)
    , m_view(NULL)
    , m_analysisInProgress(false)
    , m_fileCount(0)
    , m_fileProcessed(1)
{
    FileExtManager::Init();

    m_longName = _("CppCheck integration for CodeLite IDE");
    m_shortName = wxT("CppCheck");

    // Load settings
    m_mgr->GetConfigTool()->ReadObject("CppCheck", &m_settings);
    // Now set default suppressions if none have been serialised
    m_settings.SetDefaultSuppressedWarnings();
    // NB we can't load any project-specific settings here, as the workspace won't yet have loaded. We do it just before they're used

    // Connect events
    m_mgr->GetTheApp()->Connect(XRCID("cppcheck_settings_item"),     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnSettingsItem),          NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("cppcheck_settings_item_project"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnSettingsItemProject), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("cppcheck_fileexplorer_item"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckFileExplorerItem), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("cppcheck_workspace_item"),    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckWorkspaceItem),    NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Connect(XRCID("cppcheck_project_item"),      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckProjectItem),      NULL, (wxEvtHandler*)this);

    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED,            wxCommandEventHandler(CppCheckPlugin::OnWorkspaceClosed),NULL, this);

    m_view = new CppCheckReportPage(m_mgr->GetOutputPaneNotebook(), m_mgr, this);

//	wxBookCtrlBase *book = m_mgr->GetOutputPaneNotebook();
    m_mgr->GetOutputPaneNotebook()->AddPage(m_view, wxT("CppCheck"), false, LoadBitmapFile(wxT("cppcheck.png")));
}

CppCheckPlugin::~CppCheckPlugin()
{
    m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_settings_item"),     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnSettingsItem),          NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_settings_item_project"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnSettingsItemProject), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_fileexplorer_item"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckFileExplorerItem), NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_workspace_item"),    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckWorkspaceItem),    NULL, (wxEvtHandler*)this);
    m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_project_item"),      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckProjectItem),      NULL, (wxEvtHandler*)this);

    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED,            wxCommandEventHandler(CppCheckPlugin::OnWorkspaceClosed),NULL, this);
}

clToolBar *CppCheckPlugin::CreateToolBar(wxWindow *parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar *tb(NULL);
    return tb;
}

void CppCheckPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item = new wxMenuItem(menu, XRCID("cppcheck_settings_item"), _("Settings"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, wxT("CppCheck"), menu);
}

void CppCheckPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
    if (type == MenuTypeEditor) {
        // The editor context menu situation is identical to FileExplore, so piggyback
        if (!menu->FindItem(XRCID("CPPCHECK_EXPLORER_POPUP"))) {
            menu->Append(XRCID("CPPCHECK_EXPLORER_POPUP"), _("CppCheck"), CreateFileExplorerPopMenu());
        }
    } else if (type == MenuTypeFileExplorer) {
        if (!menu->FindItem(XRCID("CPPCHECK_EXPLORER_POPUP"))) {
            m_explorerSepItem = menu->PrependSeparator();
            menu->Prepend(XRCID("CPPCHECK_EXPLORER_POPUP"), _("CppCheck"), CreateFileExplorerPopMenu());
        }

    } else if (type == MenuTypeFileView_Workspace) {
        if (!menu->FindItem(XRCID("CPPCHECK_WORKSPACE_POPUP"))) {
            m_workspaceSepItem = menu->PrependSeparator();
            menu->Prepend(XRCID("CPPCHECK_WORKSPACE_POPUP"), _("CppCheck"), CreateWorkspacePopMenu());
        }

    } else if (type == MenuTypeFileView_Project) {
        if (!menu->FindItem(XRCID("CPPCHECK_PROJECT_POPUP"))) {
            m_projectSepItem = menu->PrependSeparator();
            menu->Prepend(XRCID("CPPCHECK_PROJECT_POPUP"), _("CppCheck"), CreateProjectPopMenu());
        }
    }
}

void CppCheckPlugin::UnPlug()
{
    // before this plugin is un-plugged we must remove the tab we added
    for (size_t i=0; i<m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
        if (m_view == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
            m_mgr->GetOutputPaneNotebook()->RemovePage(i);
            m_view->Destroy();
            break;
        }
    }

    // terminate the cppcheck daemon
    if ( m_cppcheckProcess ) {
        wxLogMessage(_("CppCheckPlugin: Terminating cppcheck daemon..."));
        delete m_cppcheckProcess;
        m_cppcheckProcess = NULL;
    }
}

wxMenu* CppCheckPlugin::CreateFileExplorerPopMenu()
{
    //Create the popup menu for the file explorer
    //The only menu that we are interested in is the file explorer menu
    wxMenu* menu = new wxMenu();
    wxMenuItem *item(NULL);

    item = new wxMenuItem(menu, XRCID("cppcheck_fileexplorer_item"), _("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cppcheck_settings_item"), _("Settings"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    return menu;
}


wxMenu* CppCheckPlugin::CreateProjectPopMenu()
{
    wxMenu* menu = new wxMenu();
    wxMenuItem *item(NULL);

    item = new wxMenuItem(menu, XRCID("cppcheck_project_item"), _("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cppcheck_settings_item_project"), _("Settings"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    return menu;
}

wxMenu* CppCheckPlugin::CreateWorkspacePopMenu()
{
    wxMenu* menu = new wxMenu();
    wxMenuItem *item(NULL);

    item = new wxMenuItem(menu, XRCID("cppcheck_workspace_item"), _("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    item = new wxMenuItem(menu, XRCID("cppcheck_settings_item"), _("Settings"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);

    return menu;
}

void CppCheckPlugin::OnCheckFileExplorerItem(wxCommandEvent& e)
{
    if ( m_cppcheckProcess ) {
        wxLogMessage(_("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
        return;
    }

    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
    for(size_t i=0; i<item.m_paths.GetCount(); ++i) {
        if ( wxDir::Exists(item.m_paths.Item(i)) ) {
            // directory
            GetFileListFromDir( item.m_paths.Item(i) );
        } else {
            // filename
            m_filelist.Add( item.m_paths.Item(i) );
        }
    }
    DoStartTest();
}

void CppCheckPlugin::OnCheckWorkspaceItem(wxCommandEvent& e)
{
    if ( m_cppcheckProcess ) {
        wxLogMessage(_("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
        return;
    }

    if ( !m_mgr->GetWorkspace() || !m_mgr->IsWorkspaceOpen() ) {
        return;
    }

    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if ( item.m_itemType == ProjectItem::TypeWorkspace ) {

        // retrieve complete list of source files of the workspace
        wxArrayString projects;
        wxString      err_msg;
        std::vector< wxFileName > tmpfiles;
        m_mgr->GetWorkspace()->GetProjectList(projects);

        for (size_t i=0; i< projects.GetCount(); i++) {
            ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projects.Item(i), err_msg);
            if ( proj ) {
                proj->GetFiles(tmpfiles, true);
            }
        }

        // only C/C++ files
        for (size_t i=0; i< tmpfiles.size(); i++ ) {
            if (FileExtManager::GetType( tmpfiles.at(i).GetFullPath() ) == FileExtManager::TypeSourceC ||
                FileExtManager::GetType( tmpfiles.at(i).GetFullPath() ) == FileExtManager::TypeSourceCpp) {
                m_filelist.Add( tmpfiles.at(i).GetFullPath() );
            }
        }
    }
    DoStartTest();
}

ProjectPtr CppCheckPlugin::FindSelectedProject()
{
    ProjectPtr proj = NULL;

    if ( !m_mgr->GetWorkspace() || !m_mgr->IsWorkspaceOpen() ) {
        return proj;
    }

    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if ( item.m_itemType == ProjectItem::TypeProject) {
        wxString                  project_name (item.m_text);
        wxString                  err_msg;
        proj = m_mgr->GetWorkspace()->FindProjectByName(project_name, err_msg);
    }

    return proj;
}

void CppCheckPlugin::OnCheckProjectItem(wxCommandEvent& e)
{
    if ( m_cppcheckProcess ) {
        wxLogMessage(_("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
        return;
    }

    ProjectPtr proj = FindSelectedProject();
    if ( !proj ) {
        return;
    }

    // retrieve complete list of source files of the workspace
    std::vector< wxFileName > tmpfiles;
    proj->GetFiles(tmpfiles, true);

    // only C/C++ files
    for (size_t i=0; i< tmpfiles.size(); i++) {
        if (FileExtManager::GetType(tmpfiles.at(i).GetFullPath()) == FileExtManager::TypeSourceC ||
            FileExtManager::GetType(tmpfiles.at(i).GetFullPath()) == FileExtManager::TypeSourceCpp) {
            m_filelist.Add(tmpfiles.at(i).GetFullPath());
        }
    }
        
    DoStartTest(proj);
}

void CppCheckPlugin::OnCppCheckTerminated(wxCommandEvent& e)
{
    m_filelist.Clear();

    ProcessEventData* ped = (ProcessEventData*)e.GetClientData();
    delete ped;

    if( m_cppcheckProcess )
        delete m_cppcheckProcess;
    m_cppcheckProcess = NULL;

    m_view->PrintStatusMessage();
}


void CppCheckPlugin::OnSettingsItem(wxCommandEvent& WXUNUSED(e))
{
    DoSettingsItem();
}

void CppCheckPlugin::OnSettingsItemProject(wxCommandEvent& WXUNUSED(e))
{
    ProjectPtr proj = FindSelectedProject();
    DoSettingsItem(proj);
}

void CppCheckPlugin::DoSettingsItem(ProjectPtr project/*= NULL*/)
{
    // Find the default path for the CppCheckSettingsDialog's wxFileDialog
    wxString defaultpath;
    IEditor* ed = m_mgr->GetActiveEditor();
    if (ed && ed->GetFileName().IsOk()) {
        defaultpath = ed->GetFileName().GetPath();
    }

    // If there's an active project, first load any project-specific settings: definitions and undefines
    // (We couldn't do that with the rest of the settings as the workspace hadn't yet been loaded)
    m_settings.LoadProjectSpecificSettings(project); // NB we still do this if !project, as that will clear any stale settings

    CppCheckSettingsDialog dlg(m_mgr->GetTheApp()->GetTopWindow(), &m_settings, m_mgr->GetConfigTool(), defaultpath, project.Get() != NULL);
    if (dlg.ShowModal() == wxID_OK) {
        m_mgr->GetConfigTool()->WriteObject(wxT("CppCheck"), &m_settings);
        if (project) {
            // Also save any project-specific settings: definitions and undefines
            wxString definitions = wxJoin(m_settings.GetDefinitions(), ',');
            wxString undefines   = wxJoin(m_settings.GetUndefines(), ',');
            if (!(definitions.empty() && undefines.empty())) {
                project->SetPluginData("CppCheck", definitions + ';' + undefines);
            } else {
                project->SetPluginData("CppCheck", "");
            }
        }
    }
}

void CppCheckPlugin::GetFileListFromDir(const wxString& root)
{
    m_filelist.Clear();
    wxArrayString tmparr;
    wxDir::GetAllFiles(root, &tmparr);

    for (size_t i=0; i<tmparr.GetCount(); i++) {
        switch (FileExtManager::GetType( tmparr.Item(i) ) ) {
        case FileExtManager::TypeSourceC:
        case FileExtManager::TypeSourceCpp: {
            m_filelist.Add( tmparr.Item(i) );
            break;
        }

        default:
            break;
        }
    }
}

void CppCheckPlugin::DoProcess()
{
    wxString command = DoGetCommand();
    m_view->AppendLine(wxString::Format(_("Starting cppcheck: %s\n"), command.c_str()));

    m_cppcheckProcess = CreateAsyncProcess(this, command);
    if (!m_cppcheckProcess ) {
        wxMessageBox(_("Failed to launch codelite_cppcheck process!"), _("Warning"), wxOK|wxCENTER|wxICON_WARNING);
        return;
    }
}

/**
 * Ensure that the CppCheck tab is visible
 */
void CppCheckPlugin::SetTabVisible(bool clearContent)
{
    // Make sure that the Output pane is visible
    wxAuiManager *aui = m_mgr->GetDockingManager();
    if (aui) {
        wxAuiPaneInfo &info = aui->GetPane(wxT("Output View"));
        if (info.IsOk() && !info.IsShown()) {
            info.Show();
            aui->Update();
        }
    }

    // Set the focus to the CppCheck tab
    Notebook *book = m_mgr->GetOutputPaneNotebook();
    if (book->GetPageText((size_t)book->GetSelection()) != wxT("CppCheck")) {
        for (size_t i=0; i<book->GetPageCount(); i++) {
            if (book->GetPageText(i) == wxT("CppCheck")) {
                book->SetSelection(i);
                break;
            }
        }
    }

    // clear the view contents
    if ( clearContent ) {
        m_view->Clear();
        m_fileCount = m_filelist.GetCount();
        m_fileProcessed = 1;
    }
}

void CppCheckPlugin::StopAnalysis()
{
    // Clear the files queue
    if (m_cppcheckProcess) {
        // terminate the m_cppcheckProcess
        m_cppcheckProcess->Terminate();
    }
}

size_t CppCheckPlugin::GetProgress()
{
    double progress = (((double) m_fileProcessed) / (double) m_fileCount) * 100;
    return (size_t) progress;
}

void CppCheckPlugin::RemoveExcludedFiles()
{
    wxArrayString exclude = m_settings.GetExcludeFiles();

    wxArrayString tmpfiles ( m_filelist );
    m_filelist.Clear();

    for ( size_t i=0; i<tmpfiles.GetCount(); i++ ) {
        wxFileName fn ( tmpfiles.Item(i) );
        if ( exclude.Index(fn.GetFullPath()) == wxNOT_FOUND ) {
            // file does not exist in the excluded files list
            // add it
            m_filelist.Add( tmpfiles.Item(i) );
        }
    }

}

void CppCheckPlugin::OnWorkspaceClosed(wxCommandEvent& e)
{
    m_view->Clear();
    e.Skip();
}

void CppCheckPlugin::DoStartTest(ProjectPtr proj /*=NULL*/)
{
    RemoveExcludedFiles();
    if (!m_filelist.GetCount()) {
        wxMessageBox(_("No files to check"), "CppCheck", wxOK|wxCENTRE, m_mgr->GetTheApp()->GetTopWindow());
        return;
    }
    SetTabVisible(true);
    m_view->Clear();
    m_view->SetGaugeRange(m_filelist.GetCount());
    
    // We need to load any project-specific settings: definitions and undefines
    // (We couldn't do that with the rest of the settings as the workspace hadn't yet been loaded)
    m_settings.LoadProjectSpecificSettings(proj); // NB we still do this if !proj, as that will clear any stale settings

    // Start the test
    DoProcess();
}

wxString CppCheckPlugin::DoGetCommand()
{
    // Linux / Mac way: spawn the process and execute the command
    wxString cmd, path;
    path = clStandardPaths::Get().GetBinaryFullPath("codelite_cppcheck");

    wxString fileList = DoGenerateFileList();
    if(fileList.IsEmpty())
        return wxT("");

    // build the command
    cmd << wxT("\"") << path << wxT("\" ");
    cmd << m_settings.GetOptions();
    cmd << wxT(" --file-list=");
    cmd << wxT("\"") << fileList << wxT("\"");
    return cmd;
}

wxString CppCheckPlugin::DoGenerateFileList()
{
    //create temporary file and save the file there
    wxFileName fnFileList( WorkspaceST::Get()->GetPrivateFolder(), "cppcheck.list");

    //create temporary file and save the file there
    wxFFile file(fnFileList.GetFullPath(), wxT("w+b"));
    if (!file.IsOpened()) {
        wxMessageBox(_("Failed to open temporary file ") + fnFileList.GetFullPath(), _("Warning"), wxOK|wxCENTER|wxICON_WARNING);
        return wxEmptyString;
    }

    wxString content;
    for(size_t i=0; i<m_filelist.GetCount(); i++) {
        content << m_filelist.Item(i) << wxT("\n");
    }

    file.Write( content );
    file.Flush();
    file.Close();
    return fnFileList.GetFullPath();
}

void CppCheckPlugin::OnCppCheckReadData(wxCommandEvent& e)
{
    e.Skip();
    ProcessEventData *ped = (ProcessEventData *) e.GetClientData();
    m_view->AppendLine( ped->GetData() );

    delete ped;
}
