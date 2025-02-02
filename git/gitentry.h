//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : gitentry.h
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

#ifndef __gitentry__
#define __gitentry__

#include <wx/colour.h>
#include <wx/event.h>
#include <vector>
#include <map>
#include "cl_config.h"

struct GitLabelCommand
{
    GitLabelCommand() {}

    GitLabelCommand(const wxString& l, const wxString& c) : label(l), command(c) {}

    wxString label;     // The menu label
    wxString command;   // The command string, without the initial 'git' or the extras like --no-pager
};

typedef std::vector<GitLabelCommand> vGitLabelCommands_t;
class GitCommandsEntries // Holds a command-list for a particular git command e.g. 'git pull' might be that, or 'git pull --rebase'
{
protected:
    vGitLabelCommands_t m_commands;
    wxString m_commandName;
    int m_lastUsed;
    
public:
    GitCommandsEntries() {}
    GitCommandsEntries(const wxString& commandName) : m_commandName(commandName), m_lastUsed(-1) {}
    GitCommandsEntries(const GitCommandsEntries& gce) 
        : m_commands(gce.m_commands)
        , m_commandName(gce.m_commandName)
        , m_lastUsed(gce.m_lastUsed)
    {}

    virtual ~GitCommandsEntries() {}
    void FromJSON(const JSONElement& json);
    void ToJSON(JSONElement& arr) const;
    
    const wxString& GetCommandname() const {
        return m_commandName;
    }

    const vGitLabelCommands_t& GetCommands() const {
        return m_commands;
    }

    void SetCommands(const vGitLabelCommands_t& commands) {
        m_commands = commands;
    }

    const wxString GetDefaultCommand() const {
        wxString str;
        if (m_lastUsed >= 0 && m_lastUsed < m_commands.size()) {
            str = m_commands.at(m_lastUsed).command;
        }
        return str;
    }

    int GetLastUsedCommandIndex() const {
        return m_lastUsed;
    }
    
    void SetLastUsedCommandIndex(int index) {
        m_lastUsed =  index;
    }

};

typedef std::map<wxString, GitCommandsEntries> GitCommandsEntriesMap_t;

extern const wxEventType wxEVT_GIT_CONFIG_CHANGED;
class GitEntry : public clConfigItem
{
    wxColour                   m_colourTrackedFile;
    wxColour                   m_colourDiffFile;
    wxString                   m_pathGIT;
    wxString                   m_pathGITK;
    JSONElement::wxStringMap_t m_entries;
    GitCommandsEntriesMap_t    m_commandsMap;
    size_t                     m_flags;
    int                        m_gitDiffDlgSashPos;
    int                        m_gitConsoleSashPos;
    int                        m_gitCommitDlgHSashPos;
    int                        m_gitCommitDlgVSashPos;

public:
    enum {
        Git_Verbose_Log      = 0x00000001,
        Git_Show_Terminal    = 0x00000002,
        Git_Colour_Tree_View = 0x00000004
    };
    
    struct GitProperties {
        wxString global_username;
        wxString global_email;
        wxString local_username;
        wxString local_email;
    };
    
public:
    GitEntry();
    virtual ~GitEntry();
public:
    static GitEntry::GitProperties ReadGitProperties(const wxString& localRepoPath = "" );
    static void WriteGitProperties(const wxString& localRepoPath, const GitEntry::GitProperties &props);
    
    void EnableFlag( size_t flag, bool b ) {
        if ( b ) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }
    
    void Save();
    GitEntry& Load();
    
    void SetGitCommitDlgHSashPos(int gitCommitDlgHSashPos) {
        this->m_gitCommitDlgHSashPos = gitCommitDlgHSashPos;
    }
    void SetGitCommitDlgVSashPos(int gitCommitDlgVSashPos) {
        this->m_gitCommitDlgVSashPos = gitCommitDlgVSashPos;
    }
    int GetGitCommitDlgHSashPos() const {
        return m_gitCommitDlgHSashPos;
    }
    int GetGitCommitDlgVSashPos() const {
        return m_gitCommitDlgVSashPos;
    }
    void SetGitDiffDlgSashPos(int gitDiffDlgSashPos) {
        this->m_gitDiffDlgSashPos = gitDiffDlgSashPos;
    }
    int GetGitDiffDlgSashPos() const {
        return m_gitDiffDlgSashPos;
    }
    void SetEntries(const JSONElement::wxStringMap_t& entries) {
        this->m_entries = entries;
    }
    void SetFlags(size_t flags) {
        this->m_flags = flags;
    }
    const JSONElement::wxStringMap_t& GetEntries() const {
        return m_entries;
    }
    size_t GetFlags() const {
        return m_flags;
    }
    void SetEntry(const wxString& workspace, const wxString& repo) {
        this->m_entries[workspace] = repo;
    }
    GitCommandsEntriesMap_t GetCommandsMap() const {
        return m_commandsMap;
    }
    void SetTrackedFileColour(const wxColour& colour) {
        this->m_colourTrackedFile = colour;
    }
    void SetDiffFileColour(const wxColour& colour) {
        this->m_colourDiffFile = colour;
    }
    void SetGITExecutablePath(const wxString& exe) {
        this->m_pathGIT = exe;
    }
    void SetGITKExecutablePath(const wxString& exe) {
        this->m_pathGITK = exe;
    }
    const wxString& GetPath(const wxString& workspace) {
        return m_entries[workspace];
    }
    const wxColour& GetTrackedFileColour() {
        return this->m_colourTrackedFile;
    }
    const wxColour& GetDiffFileColour() {
        return this->m_colourDiffFile;
    }
    wxString GetGITExecutablePath() const ;
    wxString GetGITKExecutablePath() const;
    void SetGitConsoleSashPos(int gitConsoleSashPos) {
        this->m_gitConsoleSashPos = gitConsoleSashPos;
    }
    int GetGitConsoleSashPos() const {
        return m_gitConsoleSashPos;
    }
    
    GitCommandsEntries& GetGitCommandsEntries(const wxString& entryName);
    
    void AddGitCommandsEntry(GitCommandsEntries& entries, const wxString& entryName);
    void DeleteGitCommandsEntry(const wxString& entryName) {
        m_commandsMap.erase(entryName);
    }

    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
};
#endif

