//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CscopeTabBase.cpp
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
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "CscopeTabBase.h"


// Declare the bitmap loading function
extern void wxC59CAInitBitmapResources();

static bool bBitmapLoaded = false;


CscopeTabBase::CscopeTabBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC59CAInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(mainSizer);
    
    wxBoxSizer* bSizer3 = new wxBoxSizer(wxHORIZONTAL);
    
    mainSizer->Add(bSizer3, 1, wxALL|wxEXPAND, 2);
    
    wxBoxSizer* boxSizer4 = new wxBoxSizer(wxVERTICAL);
    
    bSizer3->Add(boxSizer4, 1, wxEXPAND, 2);
    
    m_dataview = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(300,-1), wxDV_VERT_RULES|wxDV_ROW_LINES|wxDV_SINGLE|wxBORDER_THEME);
    
    m_dataviewModel = new CScoptViewResultsModel;
    m_dataviewModel->SetColCount( 3 );
    m_dataview->AssociateModel(m_dataviewModel.get() );
    
    boxSizer4->Add(m_dataview, 1, wxEXPAND, 2);
    
    m_dataview->AppendIconTextColumn(_("Scope"), m_dataview->GetColumnCount(), wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT);
    m_dataview->AppendTextColumn(_("Line"), m_dataview->GetColumnCount(), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT);
    m_dataview->AppendTextColumn(_("Pattern"), m_dataview->GetColumnCount(), wxDATAVIEW_CELL_INERT, 600, wxALIGN_LEFT);
    m_statusMessage = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    
    boxSizer4->Add(m_statusMessage, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
    
    wxBoxSizer* bSizer31 = new wxBoxSizer(wxVERTICAL);
    
    bSizer3->Add(bSizer31, 0, wxEXPAND|wxALIGN_RIGHT, 2);
    
    m_staticText2 = new wxStaticText(this, wxID_ANY, _("Search scope:"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer31->Add(m_staticText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    wxArrayString m_choiceSearchScopeArr;
    m_choiceSearchScope = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), m_choiceSearchScopeArr, 0);
    
    bSizer31->Add(m_choiceSearchScope, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    m_checkBoxUpdateDb = new wxCheckBox(this, wxID_ANY, _("Update Db if stale"), wxDefaultPosition, wxSize(-1, -1), 0);
    m_checkBoxUpdateDb->SetValue(false);
    m_checkBoxUpdateDb->SetToolTip(_("If checked, before executing a command CScope will look for any changed files and, if found, try to update the database. In practice this seems unreliable."));
    
    bSizer31->Add(m_checkBoxUpdateDb, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_checkBoxRevertedIndex = new wxCheckBox(this, wxID_ANY, _("Create reverted Index"), wxDefaultPosition, wxSize(-1, -1), 0);
    m_checkBoxRevertedIndex->SetValue(false);
    m_checkBoxRevertedIndex->SetToolTip(_("Create reverted Index database"));
    
    bSizer31->Add(m_checkBoxRevertedIndex, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    
    bSizer31->Add(0, 0, 1, wxALL, 5);
    
    m_buttonUpdateDbNow = new wxButton(this, wxID_ANY, _("&Update Database"), wxDefaultPosition, wxSize(-1, -1), 0);
    m_buttonUpdateDbNow->SetToolTip(_("Perform an immediate database update"));
    
    bSizer31->Add(m_buttonUpdateDbNow, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    m_buttonClear = new wxButton(this, wxID_ANY, _("&Clear Results"), wxDefaultPosition, wxSize(-1, -1), 0);
    
    bSizer31->Add(m_buttonClear, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_gauge = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1,-1), wxGA_SMOOTH|wxGA_HORIZONTAL);
    m_gauge->SetValue(0);
    
    bSizer31->Add(m_gauge, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    
    SetSizeHints(-1,-1);
    if ( GetSizer() ) {
         GetSizer()->Fit(this);
    }
    Centre(wxBOTH);
    // Connect events
    m_dataview->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(CscopeTabBase::OnItemActivated), NULL, this);
    m_dataview->Connect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler(CscopeTabBase::OnItemSelected), NULL, this);
    m_choiceSearchScope->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(CscopeTabBase::OnChangeSearchScope), NULL, this);
    m_checkBoxUpdateDb->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CscopeTabBase::OnChangeSearchScope), NULL, this);
    m_checkBoxUpdateDb->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopeTabBase::OnWorkspaceOpenUI), NULL, this);
    m_checkBoxRevertedIndex->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CscopeTabBase::OnChangeSearchScope), NULL, this);
    m_checkBoxRevertedIndex->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopeTabBase::OnWorkspaceOpenUI), NULL, this);
    m_buttonUpdateDbNow->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CscopeTabBase::OnCreateDB), NULL, this);
    m_buttonUpdateDbNow->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopeTabBase::OnWorkspaceOpenUI), NULL, this);
    m_buttonClear->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CscopeTabBase::OnClearResults), NULL, this);
    m_buttonClear->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopeTabBase::OnClearResultsUI), NULL, this);
    
}

CscopeTabBase::~CscopeTabBase()
{
    m_dataview->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(CscopeTabBase::OnItemActivated), NULL, this);
    m_dataview->Disconnect(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler(CscopeTabBase::OnItemSelected), NULL, this);
    m_choiceSearchScope->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(CscopeTabBase::OnChangeSearchScope), NULL, this);
    m_checkBoxUpdateDb->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CscopeTabBase::OnChangeSearchScope), NULL, this);
    m_checkBoxUpdateDb->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopeTabBase::OnWorkspaceOpenUI), NULL, this);
    m_checkBoxRevertedIndex->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CscopeTabBase::OnChangeSearchScope), NULL, this);
    m_checkBoxRevertedIndex->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopeTabBase::OnWorkspaceOpenUI), NULL, this);
    m_buttonUpdateDbNow->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CscopeTabBase::OnCreateDB), NULL, this);
    m_buttonUpdateDbNow->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopeTabBase::OnWorkspaceOpenUI), NULL, this);
    m_buttonClear->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CscopeTabBase::OnClearResults), NULL, this);
    m_buttonClear->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CscopeTabBase::OnClearResultsUI), NULL, this);
    
}
