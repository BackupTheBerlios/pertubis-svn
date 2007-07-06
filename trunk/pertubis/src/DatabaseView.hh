
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


#include "SearchWindow.hh"
#include "Threads.hh"
#include <QMainWindow>
#include <QEvent>
#include <QTreeView>

#include <tr1/memory>

class QLabel;
class QModelIndex;
class QString;
class QSplitter;
class QTableView;
class QTextBrowser;
class QToolBar;

class QUrl;
class QMenu;

namespace paludis
{
	class Environment;
}

namespace pertubis
{
	class CategoryModel;
	class PackageDetails;
	class PackageModel;
	class ReleaseEater;
	class TaskBox;

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

		DatabaseView(QWidget *parent,
					TaskBox* box);
		~DatabaseView();

	private:

		enum { PACKAGE,CATEGORY,REPOSITORY,INSTALLED,SELECTED};

		void createConnections();
		void createActions();
		void createOptionsMenu();
		void createToolBar();
		void createMenuBar();
		void createWindowSearch();
		void createTasks();

		void loadCategories();
		void loadSettings();

		void saveSettings();

		void setTaskData(QString tname, bool state);

		TaskBox*			m_box;
		CategoryModel*		m_catModel;
		PackageModel*		m_packModel;
		QAction*			m_acShowSideBar;
		QAction*			m_acShowToolBar;
		QAction*			m_acSearch;
		QAction*			m_acSelected;
		QAction*			m_acInstall;
		QAction*			m_acDeinstall;
		QAction*			m_acEditUse;
		QAction*			m_acMasking;
		QAction*			m_acSelection;
		QAction*			m_acFinish;

		SearchWindow*		m_windowSearch;
		ReleaseEater*		m_filter;

		QMenu*				m_options;
		QSplitter* 			m_hSplit;
		QSplitter* 			m_vSplit;
		QToolBar* 			m_toolBar;
		QTableView*			m_categories;
		PackageView*		m_packages;
		QTextBrowser*		m_details;
		PackageThread*		m_thread;

		int					m_numTotalPacks;

	private slots:

		void slotCategoryChanged(  const QModelIndex& index );
		void slotSearchItem();
		void slotDetailsChanged(const QModelIndex & index);
		void slotInstallTask(bool state);
		void slotRefreshCatModel();
		void slotDeinstallTask(bool state);
		void slotEditUseTask(bool state);
		void slotShowDetails(QString details);
		void slotSearchItemFinished();
		void slotSearchPackagesFinished();
		void slotOptionsMenu(const QModelIndex& index);
		void slotToggleCatBar();
		void slotToggleToolBar();
		void slotToggleWindowSearch();
	};
}
#endif




