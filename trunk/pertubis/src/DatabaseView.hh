
/* Copyright (C) 2007 Stefan Koegl.
*
* This file is part of pertubis.
*
* pertubis is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* pertubis is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http:*www.gnu.org/licenses/>.
*/

#ifndef _PERTUBIS_ENTRY_PROTECTOR_DATABASE_VIEW_H
#define _PERTUBIS_ENTRY_PROTECTOR_DATABASE_VIEW_H


#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTreeView>

class QDockWidget;
class QMenu;
class QModelIndex;
class QSplitter;
class QString;
class QTableView;
class QTextBrowser;
class QToolBar;

namespace paludis
{
	class Environment;
}

namespace pertubis
{
	class CategoryModel;
	class Item;
	class PackageDetails;
	class PackageModel;
	class ReleaseEater;
	class SearchWindow;
	class TaskBox;
	class ThreadFetchCategories;
	class ThreadFetchDetails;
	class ThreadFetchItem;
	class ThreadFetchPackages;
	class ThreadKeywordManager;
	class UseFlagEditor;
	class Settings;

	class PackageView : public QTreeView
	{
		Q_OBJECT
	public:
		PackageView(QWidget *parent);

		void mousePressEvent(QMouseEvent* event);

		QPoint m_mousePos;
	};

	class DatabaseView : public QMainWindow
	{
		Q_OBJECT

	public:

		DatabaseView();
		virtual ~DatabaseView();

	protected:

		void closeEvent(QCloseEvent* event);

	private:

		void createActions();
		void createOptionsMenu();
		void createToolBar();
		void createWindowSearch();
		void createTasks();
		void createCatbar();
		void createPackageView();
		void createUseflagEditor();
		void createTrayMenu();

		void loadCategories();
		void loadSettings();

		void saveSettings();

		CategoryModel*		m_catModel;
		Item*				m_current;
		PackageModel*		m_packModel;
		PackageView*		m_packages;
		QAction*			m_acDeinstall;
		QAction*			m_acEditUse;
		QAction*			m_acFinish;
		QAction*			m_acInstall;
		QAction*			m_acMasking;
		QAction* 			m_acPref;
		QAction* 			m_acQuit;
		QAction*			m_acSelected;
		QAction*			m_acSelection;
		QAction* 			m_acSync;
		QAction*			m_acToggleCatBar;
		QAction*			m_acToggleMainWindow;
		QAction*			m_acTogglePackageView;
		QAction*			m_acToggleSearchWindow;
		QAction*			m_acToggleUseBar;
		QDockWidget*		m_dockCat;
		QDockWidget*		m_dockUse;
		QMenu*				m_options;
		QMenu*				m_trayMenu;
		QSplitter* 			m_vSplit;
		QTableView*			m_categories;
		QTextBrowser*		m_details;
		QToolBar* 			m_toolBar;
		ReleaseEater*		m_filter;
		SearchWindow*		m_windowSearch;
		Settings*			m_settings;
		TaskBox*			m_box;
		ThreadFetchCategories*	m_threadCategories;
		ThreadFetchDetails*		m_threadDetails;
		ThreadFetchItem*		m_threadItem;
		ThreadFetchPackages*	m_threadPackages;
		ThreadKeywordManager*	m_threadKeywords;
		UseFlagEditor*			m_useflagEditor;

	private slots:

		void slotCategoryChanged(  const QModelIndex& index );
		void slotDeinstallTask(bool state);
		void slotDetailsChanged(const QModelIndex & index);
		void slotEditUseTask();
		void slotInstallTask(bool state);
		void slotOpenSettings();
		void slotOptionsMenu(const QModelIndex& index);
		void slotQuit();
		void slotRefreshCategories();
		void slotSearchItem();
		void slotSearchItemFinished();
		void slotSearchPackagesFinished();
		void slotShowDetails(QString details);
		void slotSync();
		void slotTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
		void slotToggleMainWindow();
		void slotTogglePackageView();
		void slotToggleSearchWindow();
	};
}

#endif
