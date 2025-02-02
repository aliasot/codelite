//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : LLDBVariable.h
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

#ifndef LLDBLOCALVARIABLE_H
#define LLDBLOCALVARIABLE_H

#include <vector>
#include <wx/string.h>
#include <wx/clntdata.h>
#include <wx/sharedptr.h>
#include "json_node.h"
#include <wx/treebase.h>
#ifndef __WXMSW__
#include <lldb/API/SBValue.h>
#endif

class LLDBVariable
{
public:
    typedef wxSharedPtr<LLDBVariable> Ptr_t;
    typedef std::vector<LLDBVariable::Ptr_t> Vect_t;

protected:
    wxString m_name;
    wxString m_value;
    wxString m_summary;
    wxString m_type;
    bool     m_valueChanged;
    int      m_lldbId;
    bool     m_hasChildren;
    
private:
#ifndef __WXMSW__
    void DoInitFromLLDBValue(lldb::SBValue value);
#endif

public:
#ifndef __WXMSW__
    LLDBVariable(lldb::SBValue value);
#endif

    LLDBVariable() : m_valueChanged(false), m_lldbId(wxNOT_FOUND), m_hasChildren(false) {}
    virtual ~LLDBVariable();

    void SetLldbId(int lldbId) {
        this->m_lldbId = lldbId;
    }

    int GetLldbId() const {
        return m_lldbId;
    }
    
    // Seriliazation API
    void FromJSON(const JSONElement& json);
    JSONElement ToJSON() const;

    void SetValueChanged(bool valueChanged) {
        this->m_valueChanged = valueChanged;
    }
    bool IsValueChanged() const {
        return m_valueChanged;
    }
    void SetSummary(const wxString& summary) {
        this->m_summary = summary;
    }
    const wxString& GetSummary() const {
        return m_summary;
    }
    void SetType(const wxString& type) {
        this->m_type = type;
    }
    const wxString& GetType() const {
        return m_type;
    }

    void SetName(const wxString& name) {
        this->m_name = name;
    }

    void SetValue(const wxString& value) {
        this->m_value = value;
    }

    const wxString& GetName() const {
        return m_name;
    }

    const wxString& GetValue() const {
        return m_value;
    }
    
    bool HasChildren() const {
        return m_hasChildren;
    }
    
    wxString ToString(const wxString &alternateName = wxEmptyString) const ;
};

class LLDBVariableClientData : public wxTreeItemData
{
    LLDBVariable::Ptr_t m_variable;
public:
    LLDBVariableClientData(LLDBVariable::Ptr_t variable) : m_variable(variable) {}
    LLDBVariable::Ptr_t GetVariable() const {
        return m_variable;
    }
};


#endif // LLDBLOCALVARIABLE_H
