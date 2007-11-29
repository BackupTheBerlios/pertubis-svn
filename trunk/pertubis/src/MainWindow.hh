
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of the pertubis frontend for paludis package manager.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef _PERTUBIS_ENTRY_PROTECTOR_MAIN_WINDOW_H
#define _PERTUBIS_ENTRY_PROTECTOR_MAIN_WINDOW_H 1

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTreeView>
#include <paludis/util/tr1_memory.hh>
#include <paludis/environment-fwd.hh>

class QDockWidget;
class QMenu;
class QModelIndex;
class QSplitter;
class QString;
class QTableView;
class QTextBrowser;
class QToolBar;
class QTabWidget;
class QUrl;

namespace pertubis
{
    class CategoryFilterModel;
    class CategoryModel;
    class CategoryThread;
    class DetailsThread;
    class Item;
    class MessageOutput;
    class PackageDetails;
    class PackageFilterModel;
    class PackageModel;
    class PackagesThread;
    class PertubisSyncTask;
    class ReleaseEater;
    class RepositoryListModel;
    class RepositoryListThread;
    class SearchThread;
    class SearchWindow;
    class SetModel;
    class SetThread;
    class Settings;
    class ShowSelectionsThread;
    class TaskBox;


    /*! \brief This class only exists since we need correct mouse coordinates
    *
    */
    class PackageView : public QTreeView
    {
        Q_OBJECT
    public:
        PackageView(QWidget *parent);

        void mousePressEvent(QMouseEvent* event);

        QPoint m_mousePos;
    };

    /*! \brief Our main window
    *
    * This class is our application configuration. It connects all modules pertubis consists of with
    * the user.
    */
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:

        MainWindow();
        virtual ~MainWindow();

    protected:

        void closeEvent(QCloseEvent* event);

    private:

        void createActions();
        void createCatbar();
        void createConnections();
        void createDetails();
        void createOptionsMenu();
        void createOutput();
        void createPackageView();
        void createRepositoryBar();
        void createRepositoryView();
        void createSettings();
        void createSetListView();
        void createTab();
        void createTaskBox();
        void createTasks();
        void createToolBar();
        void createTrayMenu();
        void createWindowSearch();
        void initGUI();
        void initLayout();
        void loadCategories();
        void loadSettings();
        void saveSettings();

        paludis::tr1::shared_ptr<paludis::Environment>  m_env;
        CategoryThread*         m_categoryThread;
        CategoryFilterModel*    m_categoryFilterModel;
        CategoryFilterModel*    m_setFilterModel;
        CategoryModel*          m_catModel;
        DetailsThread*          m_detailsThread;
        Item*                   m_current;
        MessageOutput*          m_output;
        PertubisSyncTask*       m_syncTask;
        PackageModel*           m_packageModel;
        PackageFilterModel*     m_packageFilterModel;
        PackagesThread*         m_packageViewThread;
        PackageView*            m_packageView;
        QAction*                m_acDeinstall;
        QAction*                m_acEditUse;
        QAction*                m_acFinish;
        QAction*                m_acInstall;
        QAction*                m_acMasking;
        QAction*                m_acPref;
        QAction*                m_acQuit;
        QAction*                m_acSelected;
        QAction*                m_acSelection;
        QAction*                m_acSync;
        QAction*                m_acToggleCatBar;
        QAction*                m_acToggleMainWindow;
        QAction*                m_acTogglePackageView;
        QAction*                m_acToggleRepoBar;
        QAction*                m_acToggleSearchWindow;
        QAction*                m_acToggleSetBar;
        QAction*                m_acToggleUseBar;
        QDockWidget*            m_dockCat;
        QDockWidget*            m_dockDetails;
        QDockWidget*            m_dockRepo;
        QDockWidget*            m_dockSet;
        QDockWidget*            m_dockUse;
        QMenu*                  m_options;
        QMenu*                  m_trayMenu;
        QSplitter*              m_vSplit;
        QSystemTrayIcon*        m_sysTray;
        QTableView*             m_categories;
        QTableView*             m_repoListView;
        QTableView*             m_setListView;
        QTabWidget*             m_tabs;
        QTextBrowser*           m_details;
        QToolBar*               m_toolBar;
        ReleaseEater*           m_filter;
        RepositoryListModel*    m_repoListModel;
        RepositoryListThread*   m_repoListThread;
        SearchThread*           m_searchThread;
        SetModel*               m_setModel;
        SetThread*              m_setThread;
        SearchWindow*           m_windowSearch;
        Settings*               m_settings;
        ShowSelectionsThread*   m_selectionsThread;
        TaskBox*                m_box;
        int                     m_tidInstall;
        int                     m_tidDeinstall;
        int                     m_repoViewTabID;
        int                     m_outputTabID;
        int                     m_detailsTabID;

    private slots:

        void initObject();
        void slotCategoryChanged(  const QModelIndex& index );
        void slotDeinstallTask(bool mystate);
        void slotDetailsChanged(const QModelIndex & index);
        void slotFinish();
        void slotFinished();
        void slotInstallTask(bool mystate);
        void slotOpenURL(const QUrl&);
        void slotOptionsMenu(const QModelIndex& index);
        void slotQuit();
        void slotRepositoryChanged( const QModelIndex& index );
        void slotReposChanged();
        void slotSearchItem();
        void slotResultCount();
        void slotShowDetails(QString details);
        void slotShowSelectedPackages();
        void slotSync();
        void slotToggleMainWindow();
        void slotTogglePackageView();
        void slotToggleSearchWindow();
        void slotToggleSettings();
        void slotToggleTrayIcon(QSystemTrayIcon::ActivationReason reason);
    };
}

#endif
