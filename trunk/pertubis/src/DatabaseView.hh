
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_DATABASE_VIEW_H
#define _PERTUBIS_ENTRY_PROTECTOR_DATABASE_VIEW_H 1

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTreeView>
#include <paludis/util/tr1_memory.hh>

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

namespace paludis
{
    class Environment;
}

namespace pertubis
{
    class CategoryFilterModel;
    class CategoryModel;
    class Item;
    class MessageOutput;
    class PackageDetails;
    class PackageModel;
    class PackageFilterModel;
    class ReleaseEater;
    class SearchWindow;
    class OurSyncTask;
    class RepositoryListModel;
    class RepositoryInfoModel;
    class RepositoryListView;
    class RepositoryInfoView;
    class Settings;
    class TaskBox;
    class CategoriesThread;
    class DetailsThread;
    class SearchThread;
    class RepositoryListThread;
    class RepositoryInfoThread;
    class ShowSelectionsThread;
    class PackagesThread;
    class UseFlagEditor;

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
    class DatabaseView : public QMainWindow
    {
        Q_OBJECT

    public:

        DatabaseView();
        virtual ~DatabaseView();

        int tidInstall() const { return m_tidInstall;}
        paludis::tr1::shared_ptr<paludis::Environment> getEnv() const { return m_env;}
        TaskBox* taskbox() const { return m_box;}
        MessageOutput* messages() const { return m_output;}
        RepositoryListModel* repositoryListModel() const { return m_repoListModel;}

    protected:

        void closeEvent(QCloseEvent* event);

    private:

        void createActions();
        void createCatbar();
        void createDetails();
        void createOptionsMenu();
        void createOutput();
        void createPackageView();
        void createRepositoryView();
        void createRepositoryBar();
        void createTasks();
        void createToolBar();
        void createTrayMenu();
        void createUseflagEditor();
        void createWindowSearch();
        void loadCategories();
        void loadSettings();
        void saveSettings();

        paludis::tr1::shared_ptr<paludis::Environment>  m_env;
        CategoriesThread*       m_categoriesThread;
        CategoryFilterModel*    m_categoryFilterModel;
        CategoryModel*          m_catModel;
        DetailsThread*          m_detailsThread;
        Item*                   m_current;
        MessageOutput*          m_output;
        OurSyncTask*            m_syncTask;
        PackageModel*           m_packModel;
        PackageFilterModel*     m_packageFilterModel;
        PackagesThread*         m_packagesThread;
        PackageView*            m_packages;
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
        QAction*                m_acToggleRepoView;
        QAction*                m_acToggleSearchWindow;
        QAction*                m_acToggleUseBar;
        QDockWidget*            m_dockCat;
        QDockWidget*            m_dockDetails;
        QDockWidget*            m_dockRepo;
        QDockWidget*            m_dockUse;
        QMenu*                  m_options;
        QMenu*                  m_trayMenu;
        QSplitter*              m_vSplit;
        QTableView*             m_categories;
        QTableView*             m_repoInfoView;
        QTableView*             m_repoListView;
        QTabWidget*             m_tabs;
        QTextBrowser*           m_details;
        QToolBar*               m_toolBar;
        ReleaseEater*           m_filter;
        RepositoryInfoModel*    m_repoInfoModel;
        RepositoryInfoThread*   m_repoInfoThread;
        RepositoryListModel*    m_repoListModel;
        RepositoryListThread*   m_repoListThread;
        SearchThread*           m_searchThread;
        SearchWindow*           m_windowSearch;
        Settings*               m_settings;
        ShowSelectionsThread*   m_selectionsThread;
        TaskBox*                m_box;
        UseFlagEditor*          m_useflagEditor;
        int                     m_tidInstall;
        int                     m_tidDeinstall;
        int                     m_repoViewTabID;
        int                     m_outputTabID;
        int                     m_detailsTabID;

    private slots:

        void slotCategoryChanged(  const QModelIndex& index );
        void slotDeinstallTask(bool mystate);
        void slotDetailsChanged(const QModelIndex & index);
        void slotEditUseTask();
        void slotFinish();
        void slotInstallTask(bool mystate);
        void slotOpenURL(const QUrl&);
        void slotOptionsMenu(const QModelIndex& index);
        void slotQuit();
        void slotRefreshCategories();

        void slotRepositoryChanged( const QModelIndex& index );

        void slotSearchItem();
        void slotResultCount();
        void slotShowDetails(QString details);
        void slotShowSelectedPackages();
        void slotSync();
        void slotToggleMainWindow();
        void slotTogglePackageView();
        void slotToggleRepoView();
        void slotToggleSearchWindow();
        void slotToggleSettings();
        void slotToggleTrayIcon(QSystemTrayIcon::ActivationReason reason);
    };

    bool rootTest();
}

#endif
