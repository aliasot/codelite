//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : outputtabwindow.h
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
#ifndef __outputtabwindow__
#define __outputtabwindow__

#include <wx/panel.h>
#include <wx/stc/stc.h>
#include "theme_handler_helper.h"

class wxToolBar;
class wxBoxSizer;

class QuickFindBar;
class OutputTabWindow : public wxPanel
{
protected:
    wxString      m_name;
    wxToolBar   * m_tb;
    wxStyledTextCtrl * m_sci;
    bool          m_outputScrolls;
    bool          m_autoAppear;
    bool          m_autoAppearErrors;
    bool          m_errorsFirstLine;
    wxBoxSizer *  m_hSizer;
    ThemeHandlerHelper* m_themeHelper;
    
    static void DefineMarker(wxStyledTextCtrl *sci, int marker, int markerType, wxColor fore, wxColor back);
    static void InitStyle   (wxStyledTextCtrl *sci, int lexer, bool folding);

    void CreateGUIControls();

    virtual void AppendText(const wxString &text);
    virtual bool IsFocused();

    //Event handlers
    virtual void OnOutputScrolls  (wxCommandEvent   &e);
    virtual void OnClearAll       (wxCommandEvent   &e);
    virtual void OnWordWrap       (wxCommandEvent   &e);
    virtual void OnCollapseAll    (wxCommandEvent   &e);
    virtual void OnRepeatOutput   (wxCommandEvent   &e);
    virtual void OnEdit           (wxCommandEvent   &e);
    virtual void OnToggleHoldOpen (wxCommandEvent   &e);


    virtual void OnOutputScrollsUI(wxUpdateUIEvent  &e);
    virtual void OnClearAllUI     (wxUpdateUIEvent  &e);
    virtual void OnWordWrapUI     (wxUpdateUIEvent  &e);
    virtual void OnCollapseAllUI  (wxUpdateUIEvent  &e);
    virtual void OnRepeatOutputUI (wxUpdateUIEvent  &e);

    virtual void OnMouseDClick    (wxStyledTextEvent &e);
    virtual void OnHotspotClicked (wxStyledTextEvent &e);
    virtual void OnMarginClick    (wxStyledTextEvent &e);
    virtual void OnEditUI         (wxUpdateUIEvent  &e);
    virtual void OnThemeChanged   (wxCommandEvent   &e);
    
    DECLARE_EVENT_TABLE()

public:
    OutputTabWindow(wxWindow *parent, wxWindowID id, const wxString &name);
    ~OutputTabWindow();

    virtual void Clear();
    const wxString &GetCaption() const {
        return m_name;
    }
    wxStyledTextCtrl* GetSci() {
        return m_sci;
    }
};
#endif // __outputtabwindow__
