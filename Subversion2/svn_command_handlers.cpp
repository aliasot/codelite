//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : svn_command_handlers.cpp
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

#include <wx/app.h>
#include <wx/tokenzr.h>
#include <wx/regex.h>
#include "svn_console.h"
#include <wx/file.h>
#include "svnsettingsdata.h"
#include "svn_command_handlers.h"
#include "subversion_view.h"
#include <wx/xrc/xmlres.h>
#include "subversion2.h"
#include "changelogpage.h"
#include "imanager.h"
#include "ieditor.h"
#include "event_notifier.h"
#include "SvnBlameFrame.h"

void SvnCommitHandler::Process(const wxString& output)
{
    SvnDefaultCommandHandler::Process(output);
}

void SvnUpdateHandler::Process(const wxString& output)
{
    bool conflictFound ( false  );
    wxString svnOutput ( output );

    svnOutput.MakeLower();
    if (svnOutput.Contains(wxT("summary of conflicts:"))) {
        // A conflict was found
        conflictFound = true;
    }

    // Reload any modified files
    EventNotifier::Get()->PostReloadExternallyModifiedEvent( false );

    // After 'Update' we usually want to do the following:
    // Reload workspace (if a project file or the workspace were modified)
    // or retag the workspace
    if ( !conflictFound ) {

        // Retag workspace only if no conflict were found
        // send an event to the main frame indicating that a re-tag is required
        if( GetPlugin()->GetSettings().GetFlags() & SvnRetagWorkspace ) {
            wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
            GetPlugin()->GetManager()->GetTheApp()->GetTopWindow()->GetEventHandler()->AddPendingEvent(e);
        }
    }

    // And finally, update the Subversion view
    SvnDefaultCommandHandler::Process(output);
}

void SvnDiffHandler::Process(const wxString& output)
{
    // Open the changes inside the editor only if we are not using an external
    // diff viewer
    if(GetPlugin()->GetSettings().GetFlags() & SvnUseExternalDiff)
        return;

    IEditor *editor = GetPlugin()->GetManager()->NewEditor();
    if(editor) {
        // Set the lexer name to 'Diff'
        editor->SetLexerName(wxT("Diff"));
        editor->AppendText(output);
    }
}

void SvnPatchHandler::Process(const wxString& output)
{
    // Print the patch output to the subversion console
    GetPlugin()->GetConsole()->EnsureVisible();
    GetPlugin()->GetConsole()->AppendText(output);
    GetPlugin()->GetConsole()->AppendText(wxT("-----\n"));

    if(delFileWhenDone) {
        wxRemoveFile(patchFile);
    }

    // Retag workspace only if no conflict were found
    // send an event to the main frame indicating that a re-tag is required
    if( GetPlugin()->GetSettings().GetFlags() & SvnRetagWorkspace ) {
        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
        GetPlugin()->GetManager()->GetTheApp()->GetTopWindow()->GetEventHandler()->AddPendingEvent(e);
    }

    // And finally, update the Subversion view
    SvnDefaultCommandHandler::Process(output);
}

void SvnPatchDryRunHandler::Process(const wxString& output)
{
    GetPlugin()->GetConsole()->EnsureVisible();
    GetPlugin()->GetConsole()->AppendText(_("===== APPLYING PATCH - DRY RUN =====\n"));
    GetPlugin()->GetConsole()->AppendText(output);
    GetPlugin()->GetConsole()->AppendText(_("===== OUTPUT END =====\n"));

    if(delFileWhenDone) {
        // delete the patch file
        wxRemoveFile(patchFile);
    }
}

void SvnVersionHandler::Process(const wxString& output)
{
    //GetPlugin()->GetConsole()->AppendText(output);
    wxRegEx reVersion(wxT("svn, version ([0-9]\\.[0-9])(\\.[0-9])"));
    if(reVersion.Matches(output)) {
        wxString strVersion = reVersion.GetMatch(output, 1);

        double version(0.0);
        strVersion.ToDouble(&version);

        GetPlugin()->GetConsole()->AppendText(wxString::Format(wxT("== Svn client version: %s ==\n"), strVersion.c_str()));
        GetPlugin()->SetSvnClientVersion(version);
    }
}

void SvnLogHandler::Process(const wxString& output)
{
    // create new editor and set the output to it
    wxString changeLog (output);
    if(m_compact) {
        // remove non interesting lines
        changeLog = Compact(changeLog);
    }

    ChangeLogPage *page = new ChangeLogPage(GetPlugin()->GetManager()->GetTheApp()->GetTopWindow(), GetPlugin());
    page->SetUrl(m_url);
    page->AppendText( changeLog );
    GetPlugin()->GetManager()->AddPage( page, _("Change Log"), wxNullBitmap, true );
}

wxString SvnLogHandler::Compact(const wxString& message)
{
    wxString compactMsg (message);
    compactMsg.Replace(wxT("\r\n"), wxT("\n"));
    compactMsg.Replace(wxT("\r"),   wxT("\n"));
    compactMsg.Replace(wxT("\v"),   wxT("\n"));
    wxArrayString lines = wxStringTokenize(compactMsg, wxT("\n"), wxTOKEN_STRTOK);
    compactMsg.Clear();
    for(size_t i=0; i<lines.GetCount(); i++) {
        wxString line = lines.Item(i);
        line.Trim().Trim(false);

        if(line.IsEmpty())
            continue;

        if(line.StartsWith(wxT("----------"))) {
            continue;
        }

        if(line == wxT("\"")) {
            continue;
        }
        static wxRegEx reRevisionPrefix(wxT("^(r[0-9]+)"));
        if(reRevisionPrefix.Matches(line)) {
            continue;
        }
        compactMsg << line << wxT("\n");
    }
    if(compactMsg.IsEmpty() == false) {
        compactMsg.RemoveLast();
    }
    return compactMsg;
}

void SvnCheckoutHandler::Process(const wxString& output)
{
    wxUnusedVar(output);
}

void SvnBlameHandler::Process(const wxString& output)
{
    if(output.StartsWith(wxT("svn:"))) {
        // error occured
        GetPlugin()->GetConsole()->AppendText(output);
        GetPlugin()->GetConsole()->AppendText(wxT("--------\n"));
        return;
    }

    GetPlugin()->GetConsole()->AppendText(_("Loading Svn blame dialog...\n"));
    GetPlugin()->GetConsole()->AppendText(wxT("--------\n"));
    SvnBlameFrame *blameFrame = new SvnBlameFrame(GetPlugin()->GetManager()->GetTheApp()->GetTopWindow(), m_filename, output);
    blameFrame->Show();
}

void SvnRepoListHandler::Process(const wxString& output)
{
    if(output.StartsWith(wxT("svn:"))) {
        // error occured
        GetPlugin()->GetConsole()->AppendText(output);
        GetPlugin()->GetConsole()->AppendText(wxT("--------\n"));
        return;
    }
    GetPlugin()->FinishSyncProcess(m_proj, m_workDir, m_excludeBin, m_excludeExtensions, output);
}
