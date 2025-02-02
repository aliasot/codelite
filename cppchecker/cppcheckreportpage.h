//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : cppcheckreportpage.h
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

#ifndef __cppcheckreportpage__
#define __cppcheckreportpage__

/**
@file
Subclass of CppCheckReportBasePage, which is generated by wxFormBuilder.
*/

#include "cppcheckreportbasepage.h"
#include "cppchecktestresults.h"
#include <vector>
#include <map>

extern const wxEventType wxEVT_CPPCHECK_ASK_STOP;

class IManager;
class CppCheckPlugin;

/** Implementing CppCheckReportBasePage */
class CppCheckReportPage : public CppCheckReportBasePage
{
    IManager*           m_mgr;
    CppCheckPlugin*     m_plugin;

protected:
    virtual void OnStyleNeeded(wxStyledTextEvent& event);
    //static int ColorLine ( int, const char *text, size_t &start, size_t &len );

protected:
    // Handlers for CppCheckReportBasePage events.

    void OnClearReport   ( wxCommandEvent& event );
    void OnStopChecking  ( wxCommandEvent& event );
    void OnStopCheckingUI(wxUpdateUIEvent &event);
    void OnClearReportUI (wxUpdateUIEvent &event);
    void OnThemeChanged  (wxCommandEvent &e);
    
    // Scintilla events
    void OnOpenFile             (wxStyledTextEvent& e);
    void DoInitStyle();
    
public:
    /** Constructor */
    CppCheckReportPage(wxWindow* parent, IManager* mgr, CppCheckPlugin* plugin);
    virtual ~CppCheckReportPage();

    void   Clear        ();
    size_t GetErrorCount() const;
    void   AppendLine   (const wxString &line);
    void   PrintStatusMessage();
    void   SetGaugeRange(int range);
    void   SetMessage(const wxString &msg);
};

#endif // __cppcheckreportpage__
