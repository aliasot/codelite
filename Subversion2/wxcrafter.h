//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : wxcrafter.h
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

//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: wxcrafter.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef WXCRAFTER_BASE_CLASSES_H
#define WXCRAFTER_BASE_CLASSES_H

#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/checklst.h>
#include <wx/stc/stc.h>
#include <wx/statbox.h>
#include <wx/choice.h>
#include <wx/arrstr.h>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/checkbox.h>

class SvnCommitDialogBaseClass : public wxDialog
{
protected:
    wxStaticText* m_staticText32;
    wxTextCtrl* m_textCtrlFrID;
    wxStaticText* m_staticTextBugID;
    wxTextCtrl* m_textCtrlBugID;
    wxSplitterWindow* m_splitterV;
    wxPanel* m_splitterPage52;
    wxSplitterWindow* m_splitterH;
    wxPanel* m_panel1;
    wxStaticText* m_staticText17;
    wxCheckListBox* m_checkListFiles;
    wxPanel* m_splitterPage14;
    wxStaticText* m_staticText19;
    wxStyledTextCtrl* m_stcDiff;
    wxPanel* m_splitterPage56;
    wxStaticText* m_staticText62;
    wxStyledTextCtrl* m_stcMessage;
    wxChoice* m_choiceMessages;
    wxStdDialogButtonSizer* m_stdBtnSizer66;
    wxButton* m_buttonCancel;
    wxButton* m_buttonOK;

protected:
    virtual void OnFileSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnChoiceMessage(wxCommandEvent& event) { event.Skip(); }

public:
    SvnCommitDialogBaseClass(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Svn Commit"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~SvnCommitDialogBaseClass();
};


class SvnSyncDialogBaseClass : public wxDialog
{
protected:
    wxStaticText* m_staticText31;
    wxStaticText* m_staticTextSvnInfo;
    wxStaticText* m_staticText34;
    wxDirPickerCtrl* m_dirPickerRootDir;
    wxStaticText* m_staticText40;
    wxTextCtrl* m_textCtrlExclude;
    wxCheckBox* m_checkBoxBin;
    wxButton* m_button28;
    wxButton* m_button29;

protected:
    virtual void OnOkUI(wxUpdateUIEvent& event) { event.Skip(); }
    virtual void OnButtonOK(wxCommandEvent& event) { event.Skip(); }

public:
    SvnSyncDialogBaseClass(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Sync Workspace to SVN"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500,200), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~SvnSyncDialogBaseClass();
};

#endif
