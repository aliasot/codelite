//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : gitCommitDlg.h
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

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2011 by René Kraus (aka upCASE)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __gitCommitDlg__
#define __gitCommitDlg__

#include "gitui.h"
#include <map>

class GitCommitDlg : public GitCommitDlgBase
{
    std::map<wxString, wxString> m_diffMap;
    wxString m_workingDir;

public:
    GitCommitDlg(wxWindow* parent, const wxString& repoDir);
    ~GitCommitDlg();

    void AppendDiff(const wxString& diff);

    wxArrayString GetSelectedFiles();
    wxString GetCommitMessage();
    bool IsAmending() const {
        return m_checkBoxAmend->IsChecked();
    }
    
private:
    void OnChangeFile(wxCommandEvent& e);
protected:
    virtual void OnCommitOK(wxCommandEvent& event);
};

#endif
