//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : databaseexplorer.h
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

#ifndef __DatabaseExplorer__
#define __DatabaseExplorer__

#include "plugin.h"
#include "DbViewerPanel.h"

class DatabaseExplorer : public IPlugin
{
public:
	DatabaseExplorer(IManager *manager);
	~DatabaseExplorer();
	
	//--------------------------------------------
	//Abstract methods
	//--------------------------------------------
	virtual clToolBar *CreateToolBar(wxWindow *parent);
	virtual void CreatePluginMenu(wxMenu *pluginsMenu);
	virtual void HookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnHookPopupMenu(wxMenu *menu, MenuType type);
	virtual void UnPlug();
	
	bool IsDbViewDetached();
	
	static IManager* GetManager();
	static DbViewerPanel* GetViewerPanel() {
		return m_dbViewerPanel;
	}
	
protected:
	bool m_addFileMenu;

	void OnAbout(wxCommandEvent &e);
	void OnOpenWithDBE(wxCommandEvent &e);
	void OnUpdateOpenWithDBE(wxUpdateUIEvent &e);

	static DbViewerPanel* m_dbViewerPanel;
};

#endif //DatabaseExplorer

