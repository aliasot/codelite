//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : FileExplorerTab.cpp
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

#include "FileExplorerTab.h"
#include "pluginmanager.h"
#include "plugin.h"
#include "localworkspace.h"
#include "tags_options_data.h"
#include "frame.h"
#include "manager.h"
#include <wx/wupdlock.h>
#include "globals.h"
#include "dirsaver.h"
#include "environmentconfig.h"
#include "procutils.h"
#include <wx/mimetype.h>
#include "editor_config.h"

FileExplorerTab::FileExplorerTab(wxWindow* parent)
    : FileExplorerBase(parent)
{
    m_rclickMenu = wxXmlResource::Get()->LoadMenu(wxT("file_explorer_menu"));
    Connect(XRCID("open_file"),                     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTab::OnOpenFile), NULL, this);
    Connect(XRCID("open_file_in_text_editor"),      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTab::OnOpenFileInTextEditor), NULL, this);
    Connect(XRCID("refresh_node"),                  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTab::OnRefreshNode), NULL, this);
    Connect(XRCID("delete_node"),                   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTab::OnDeleteNode), NULL, this);
    Connect(XRCID("search_node"),                   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTab::OnSearchNode), NULL, this);
    Connect(XRCID("tags_add_global_include"),       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTab::OnTagNode), NULL, this);
    Connect(XRCID("tags_add_global_exclude"),       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTab::OnTagNode), NULL, this);
    Connect(XRCID("tags_add_workspace_include"),    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTab::OnTagNode), NULL, this);
    Connect(XRCID("tags_add_workspace_exclude"),    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTab::OnTagNode), NULL, this);
    Connect(XRCID("open_shell"),                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTab::OnOpenShell), NULL, this);
    Connect(GetId(),                                wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(FileExplorerTab::OnKeyDown));

    Connect(XRCID("open_with_default_application"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTab::OnOpenWidthDefaultApp), NULL, this);
}

FileExplorerTab::~FileExplorerTab()
{
    wxDELETE(m_rclickMenu);
}

void FileExplorerTab::OnContextMenu(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    if (item.IsOk() && m_rclickMenu ) {
        // let the plugins hook their content. HookPopupMenu can work only once
        // further calls are harmless
        PluginManager::Get()->HookPopupMenu(m_rclickMenu, MenuTypeFileExplorer);
        PopupMenu(m_rclickMenu);
    }
}

void FileExplorerTab::OnItemActivated(wxTreeEvent& event)
{
    event.Skip();
    wxArrayString paths;
    m_genericDirCtrl->GetPaths(paths);
    for(size_t i=0; i<paths.GetCount(); ++i) {
        if ( !wxDir::Exists( paths.Item(i)) ) {
            DoOpenItem(paths.Item(i));
        }
    }
}

void FileExplorerTab::DoOpenItem(const wxString &path)
{
    wxFileName fn(path);
    wxString fp = fn.GetFullPath();
    if (fn.GetExt() == wxT("workspace")) {
        //open workspace
        ManagerST::Get()->OpenWorkspace(fp);

    } else {

        // Send event to the plugins to see if any plugin want to handle this file differently
        if (SendCmdEvent(wxEVT_TREE_ITEM_FILE_ACTIVATED, &fp)) {
            return;
        }

        clMainFrame::Get()->GetMainBook()->OpenFile(fp, wxEmptyString);
    }
}

void FileExplorerTab::OnDeleteNode(wxCommandEvent& event)
{
    wxArrayString paths;
    m_genericDirCtrl->GetPaths(paths);
    
    
    wxFileName gobackPath;
    for(size_t i=0; i<paths.GetCount(); i++) {
        wxString path = paths.Item(i);
        if ( !gobackPath.IsOk() ) {
            gobackPath = wxFileName(path);
        }
        
        if ( wxDir::Exists(path) ) {
            //////////////////////////////////////////////////
            // Remove a folder
            //////////////////////////////////////////////////

            wxString msg;
            msg << _("'") << path << _("' is a directory. Are you sure you want to remove it and its content?");
            if (wxMessageBox(msg, _("Remove Directory"), wxICON_WARNING|wxYES_NO|wxCANCEL) == wxYES) {
                if (!::RemoveDirectory( path )) {
                    wxMessageBox(_("Failed to remove directory"), _("Remove Directory"), wxICON_ERROR | wxOK);
                }
            }
        } else {
            //////////////////////////////////////////////////
            // Remove a file
            //////////////////////////////////////////////////
            wxLogNull noLog;
            ::wxRemoveFile(path);
        }
    }
    
    wxWindowUpdateLocker locker( m_genericDirCtrl );
    m_genericDirCtrl->ReCreateTree();
    m_genericDirCtrl->ExpandPath(gobackPath.GetPath());
}

void FileExplorerTab::OnKeyDown(wxTreeEvent& event)
{
    if (event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE) {
        wxCommandEvent dummy;
        OnDeleteNode(dummy);

    } else {
        event.Skip();
    }
}

void FileExplorerTab::OnOpenFile(wxCommandEvent& event)
{
    wxArrayString paths;
    m_genericDirCtrl->GetPaths(paths);
    for(size_t i=0; i<paths.GetCount(); ++i) {
        if ( !wxDir::Exists( paths.Item(i)) ) {
            DoOpenItem(paths.Item(i));
        }
    }
}

void FileExplorerTab::OnOpenFileInTextEditor(wxCommandEvent& event)
{
    wxArrayString paths;
    m_genericDirCtrl->GetPaths(paths);
    for(size_t i=0; i<paths.GetCount(); ++i) {
        if ( !wxDir::Exists( paths.Item(i)) ) {
            clMainFrame::Get()->GetMainBook()->OpenFile(paths.Item(i), wxEmptyString);
        }
    }
}

void FileExplorerTab::OnOpenShell(wxCommandEvent& event)
{
    wxArrayString paths;
    m_genericDirCtrl->GetPaths(paths);
    if(paths.IsEmpty())
        return;

    wxFileName fullpath( paths.Item(0) );

    DirSaver ds;
    wxSetWorkingDirectory(fullpath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));

    // Apply the environment before launching the console
    EnvSetter env;

    if (!ProcUtils::Shell( EditorConfigST::Get()->GetOptions()->GetProgramConsoleCommand() )) {
        wxMessageBox(_("Failed to load shell terminal"), _("CodeLite"), wxICON_WARNING|wxOK);
        return;
    }
}

void FileExplorerTab::OnOpenWidthDefaultApp(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxArrayString paths;
    m_genericDirCtrl->GetPaths(paths);
    
    // Apply our environment before executing the file
    EnvSetter es;
    for(size_t i=0; i<paths.GetCount(); i++) {

        bool bOpenOK = false;
        wxFileName fullpath(paths.Item(i));
        
        // switch directory to the current files' path
        DirSaver ds;
        ::wxSetWorkingDirectory( fullpath.GetPath() );
        
        wxMimeTypesManager *mgr = wxTheMimeTypesManager;
        wxFileType *type = mgr->GetFileTypeFromExtension(fullpath.GetExt());
        if ( type ) {
            wxString cmd = type->GetOpenCommand(fullpath.GetFullPath());
            delete type;

            if ( cmd.IsEmpty() == false ) {
                wxExecute(cmd);
                bOpenOK = true;
            }
        }

#ifdef __WXGTK__
        if( !bOpenOK ) {
            // All hell break loose, try xdg-open
            wxString cmd;
            wxString escapedString( fullpath.GetFullPath() );
            escapedString.Replace(" ", "\\ ");
            cmd << "xdg-open " << escapedString;
            ::wxExecute(cmd);
            bOpenOK = true;
        }
#endif

        // fallback code: suggest to the user to open the file with CL
        if (!bOpenOK && wxMessageBox(wxString::Format(_("Could not find default application for file '%s'\nWould you like CodeLite to open it?"), fullpath.GetFullName().c_str()), _("CodeLite"),
                                     wxICON_QUESTION|wxYES_NO) == wxYES) {
            DoOpenItem( fullpath.GetFullPath() );
        }
    }
}

void FileExplorerTab::OnRefreshNode(wxCommandEvent& event)
{
    wxWindowUpdateLocker locker( m_genericDirCtrl );
    m_genericDirCtrl->ReCreateTree();
}

void FileExplorerTab::OnSearchNode(wxCommandEvent& event)
{
    wxArrayString paths;
    m_genericDirCtrl->GetPaths(paths);
    
    if ( paths.GetCount() != 1 ) {
        return;
    }
    
    wxFileName fn;
    if ( wxFileName::DirExists(paths.Item(0)) ) {
        // the selection is a folder
        fn = wxFileName(paths.Item(0), "");
        
    } else {
        // a file was selected
        fn = wxFileName(paths.Item(0));
    }
    
    wxCommandEvent ff(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_in_files"));
    ff.SetString(fn.GetPath());
    clMainFrame::Get()->GetEventHandler()->AddPendingEvent(ff);
}

void FileExplorerTab::OnTagNode(wxCommandEvent& event)
{
    bool retagRequires (false);
    wxArrayString paths;
    m_genericDirCtrl->GetPaths(paths);

    TagsOptionsData tod = clMainFrame::Get()->GetTagsOptions();
    wxArrayString includePaths, excludePaths;
    LocalWorkspaceST::Get()->GetParserPaths(includePaths, excludePaths);

    for(size_t i=0; i<paths.GetCount(); i++) {

        wxString path = paths.Item(i);
        if ( path.EndsWith(wxT("\\")) || path.EndsWith(wxT("/")) )  {
            path.RemoveLast();
        }

        if ( event.GetId() == XRCID("tags_add_global_include") ) {
            // add this directory as include path
            wxArrayString arr = tod.GetParserSearchPaths();
            if ( arr.Index( path ) == wxNOT_FOUND ) {
                arr.Add( path );
                tod.SetParserSearchPaths( arr );
                retagRequires = true;
            }
        } else if ( event.GetId() == XRCID("tags_add_global_exclude") ) {
            wxArrayString arr = tod.GetParserExcludePaths();
            if ( arr.Index(path) == wxNOT_FOUND ) {
                arr.Add( path );
                tod.SetParserExcludePaths( arr );
                retagRequires = true;
            }
        } else if ( event.GetId() == XRCID("tags_add_workspace_include") ) {


            if ( includePaths.Index(path) == wxNOT_FOUND ) {
                includePaths.Add( path );
                retagRequires = true;
            }
        } else if ( event.GetId() == XRCID("tags_add_workspace_exclude") ) {

            if ( excludePaths.Index(path) == wxNOT_FOUND ) {
                excludePaths.Add( path );
                retagRequires = true;
            }
        }


        clMainFrame::Get()->UpdateTagsOptions( tod );
        LocalWorkspaceST::Get()->SetParserPaths(includePaths, excludePaths);

        // Update the parser
        ManagerST::Get()->UpdateParserPaths();

        // send notification to the main frame to perform retag
        if ( retagRequires ) {
            wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace") );
            clMainFrame::Get()->GetEventHandler()->AddPendingEvent( event );
        }
    }

}

TreeItemInfo FileExplorerTab::GetSelectedItemInfo()
{
    TreeItemInfo info;
    wxArrayString paths;
    m_genericDirCtrl->GetPaths( paths );

    if(paths.IsEmpty())
        return info;

    info.m_paths = paths;
    info.m_item = wxTreeItemId();
    return info;
}

size_t FileExplorerTab::GetSelectionCount() const
{
    wxArrayTreeItemIds items;
    return Tree()->GetTreeCtrl()->GetSelections(items);
}

void FileExplorerTab::ClearSelections()
{
    if ( GetSelectionCount() ) {
        // multiple selections
        Tree()->GetTreeCtrl()->UnselectAll();
    }
}
