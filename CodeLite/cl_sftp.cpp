//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : cl_sftp.cpp
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

#if USE_SFTP
#include "cl_sftp.h"
#include <wx/ffile.h>
#include <string.h>
#include <sys/stat.h>
#include <wx/filefn.h>
#include <libssh/sftp.h>

class SFTPDirCloser
{
    sftp_dir m_dir;
public:
    SFTPDirCloser(sftp_dir d) : m_dir(d) {}
    ~SFTPDirCloser() {
        sftp_closedir(m_dir);
    }
};

clSFTP::clSFTP(clSSH::Ptr_t ssh)
    : m_ssh(ssh)
    , m_sftp(NULL)
    , m_connected(false)
{
}

clSFTP::~clSFTP()
{
    Close();
}

void clSFTP::Initialize() throw (clException)
{
    if ( m_sftp )
        return;

    m_sftp = sftp_new(m_ssh->GetSession());
    if (m_sftp == NULL) {
        throw clException(wxString() << "Error allocating SFTP session: " << ssh_get_error(m_ssh->GetSession()));
    }

    int rc = sftp_init(m_sftp);
    if (rc != SSH_OK) {
        throw clException(wxString() << "Error initializing SFTP session: " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }
    m_connected = true;
}

void clSFTP::Close()
{
    if ( m_sftp ) {
        sftp_free(m_sftp);
    }

    m_connected = false;
    m_sftp = NULL;
}

void clSFTP::Write(const wxFileName& localFile, const wxString& remotePath) throw (clException)
{
    if ( !m_connected ) {
        throw clException("scp is not initialized!");
    }

    if ( !localFile.Exists() ) {
        throw clException(wxString() << "scp::Write file '" << localFile.GetFullPath() << "' does not exist!");
    }

    wxFFile fp(localFile.GetFullPath(), "rb");
    if ( !fp.IsOpened() ) {
        throw clException(wxString() << "scp::Write could not open file '" << localFile.GetFullPath() << "'. " << ::strerror(errno) );
    }

    wxString fileContent;
    fp.ReadAll(&fileContent);

    Write(fileContent, remotePath);
}

void clSFTP::Write(const wxString& fileContent, const wxString& remotePath) throw (clException)
{
    if ( !m_sftp ) {
        throw clException("SFTP is not initialized");
    }

    int access_type = O_WRONLY | O_CREAT | O_TRUNC;
    sftp_file file;
    std::string str = fileContent.mb_str(wxConvUTF8).data();

    file = sftp_open(m_sftp, remotePath.mb_str(wxConvUTF8).data(), access_type, 0644);
    if (file == NULL) {
        throw clException(wxString() << _("Can't open file: ") << remotePath << ". " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }

    size_t nbytes = sftp_write(file, str.c_str(), str.length());
    if (nbytes != str.length()) {
        sftp_close(file);
        throw clException(wxString() << _("Can't write data to file: ") << remotePath << ". " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }

    sftp_close(file);
}

SFTPAttribute::List_t clSFTP::List(const wxString &folder, size_t flags, const wxString &filter) throw (clException)
{
    sftp_dir dir;
    sftp_attributes attributes;

    if ( !m_sftp ) {
        throw clException("SFTP is not initialized");
    }

    dir = sftp_opendir(m_sftp, folder.mb_str(wxConvUTF8).data());
    if ( !dir ) {
        throw clException(wxString() << _("Failed to list directory: ") << folder << ". " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }

    // Keep the current folder name
    m_currentFolder = dir->name;

    // Ensure the directory is closed
    SFTPDirCloser dc(dir);
    SFTPAttribute::List_t files;

    attributes = sftp_readdir(m_sftp, dir);
    while ( attributes ) {

        SFTPAttribute::Ptr_t attr( new SFTPAttribute(attributes) );
        attributes = sftp_readdir(m_sftp, dir);

        // Don't show files ?
        if ( !(flags & SFTP_BROWSE_FILES) && !attr->IsFolder()) {
            continue;

        } else if ( (flags & SFTP_BROWSE_FILES) && !attr->IsFolder()  // show files
                    && filter.IsEmpty() ) {                           // no filter is given
            files.push_back( attr );

        } else if ( (flags & SFTP_BROWSE_FILES) && !attr->IsFolder()  // show files
                    && !::wxMatchWild(filter, attr->GetName()) ) {    // but the file does not match the filter
            continue;

        } else {
            files.push_back( attr );

        }
    }
    files.sort( SFTPAttribute::Compare );
    return files;
}

SFTPAttribute::List_t clSFTP::CdUp(size_t flags, const wxString &filter) throw (clException)
{
    wxString curfolder = m_currentFolder;
    curfolder << "/../"; // Force a cd up

    wxFileName fn(curfolder, "", wxPATH_UNIX);
    fn.Normalize();
    return List(fn.GetPath(false, wxPATH_UNIX), flags, filter);
}

wxString clSFTP::Read(const wxString& remotePath) throw (clException)
{
    if ( !m_sftp ) {
        throw clException("SFTP is not initialized");
    }

    sftp_file file = sftp_open(m_sftp, remotePath.mb_str(wxConvISO8859_1).data(), O_RDONLY, 0);
    if (file == NULL) {
        throw clException(wxString() << _("Failed to open remote file: ") << remotePath << ". " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }

    wxString content;
    char buffer[1024];
    int nbytes = 0;
    memset(buffer, 0, sizeof(buffer));
    nbytes = sftp_read(file, buffer, sizeof(buffer));
    while (nbytes > 0) {
        content << wxString(buffer, nbytes);

        memset(buffer, 0, sizeof(buffer));
        nbytes = sftp_read(file, buffer, sizeof(buffer));
    }

    if ( nbytes < 0 ) {
        sftp_close(file);
        throw clException(wxString() << _("Failed to read remote file: ") << remotePath << ". " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }
    sftp_close( file );
    return content;
}

void clSFTP::CreateDir(const wxString& dirname) throw (clException)
{
    if ( !m_sftp ) {
        throw clException("SFTP is not initialized");
    }

    int rc;
    rc = sftp_mkdir(m_sftp, dirname.mb_str(wxConvISO8859_1).data(), S_IRWXU);

    if ( rc != SSH_OK ) {
        throw clException(wxString() << _("Failed to create directory: ") << dirname << ". " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }
}

void clSFTP::Rename(const wxString& oldpath, const wxString& newpath) throw (clException)
{
    if ( !m_sftp ) {
        throw clException("SFTP is not initialized");
    }

    int rc;
    rc = sftp_rename(m_sftp,
                     oldpath.mb_str(wxConvISO8859_1).data(),
                     newpath.mb_str(wxConvISO8859_1).data());

    if ( rc != SSH_OK ) {
        throw clException(wxString() << _("Failed to rename path. ") << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }
}

void clSFTP::RemoveDir(const wxString& dirname) throw (clException)
{
    if ( !m_sftp ) {
        throw clException("SFTP is not initialized");
    }

    int rc;
    rc = sftp_rmdir(m_sftp,
                    dirname.mb_str(wxConvISO8859_1).data());

    if ( rc != SSH_OK ) {
        throw clException(wxString() << _("Failed to remove directory: ") << dirname << ". " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }
}

void clSFTP::UnlinkFile(const wxString& path) throw (clException)
{
    if ( !m_sftp ) {
        throw clException("SFTP is not initialized");
    }

    int rc;
    rc = sftp_unlink(m_sftp,
                    path.mb_str(wxConvISO8859_1).data());

    if ( rc != SSH_OK ) {
        throw clException(wxString() << _("Failed to unlink path: ") << path << ". " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }
}

SFTPAttribute::Ptr_t clSFTP::Stat(const wxString& path) throw (clException)
{
    if ( !m_sftp ) {
        throw clException("SFTP is not initialized");
    }

    sftp_attributes attr = sftp_stat(m_sftp, path.mb_str(wxConvISO8859_1).data());
    if ( !attr ) {
        throw clException(wxString() << _("Could not stat: ") << path << ". " << ssh_get_error(m_ssh->GetSession()), sftp_get_error(m_sftp));
    }
    SFTPAttribute::Ptr_t pattr( new SFTPAttribute(attr) );
    return pattr;
}

void clSFTP::CreateFile(const wxString& remoteFullPath, const wxString &content) throw (clException)
{
    // Create the path to the file
    Mkpath(wxFileName(remoteFullPath).GetPath());
    Write(content, remoteFullPath);
}

void clSFTP::Mkpath(const wxString& remoteDirFullpath) throw (clException)
{
    if ( !m_sftp ) {
        throw clException("SFTP is not initialized");
    }
    
    wxString tmpPath = remoteDirFullpath;
    tmpPath.Replace("\\", "/");
    if ( !tmpPath.StartsWith("/") ) {
        throw clException("Mkpath: path must be absolute");
    }
    
    wxFileName fn(remoteDirFullpath, "");
    const wxArrayString& dirs = fn.GetDirs();
    wxString curdir;
    
    curdir << "/";
    for(size_t i=0; i<dirs.GetCount(); ++i) {
        curdir << dirs.Item(i);
        sftp_attributes attr = sftp_stat(m_sftp, curdir.mb_str(wxConvISO8859_1).data());
        if ( !attr ) {
            // directory does not exists
            CreateDir(curdir);
            
        } else {
            // directory already exists
            sftp_attributes_free( attr );
        }
        curdir << "/";
    }
}

void clSFTP::CreateFile(const wxString& remoteFullPath, const wxFileName& localFile) throw (clException)
{
    Mkpath(wxFileName(remoteFullPath).GetPath());
    Write(localFile, remoteFullPath);
}

#endif // USE_SFTP
