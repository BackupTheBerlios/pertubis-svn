
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of pertubis
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

/*! \mainpage Pertubis
 *
 * \section intro_sec Introduction
 *
 * pertubis is a graphical frontend for the paludis package manager
 *
 * <a href="http://pertubis.berlios.de">http://pertubis.berlios.de</a>
 *
 * \section install_sec Installation
 *
 * please use the ebuild I'm providing in the project subversion repository
 *
 * best regards<br>
 * Stefan K&ouml;gl \<hotshelf AT users DOT berlios DOT de\><br>
 * germany, 2007
 */

/*!
 * \defgroup Widget Widget classes
 */

/*! \brief qt4 model subclasses
 * \defgroup PackageModelClass Package and Model Classes
 * this classes are subclasses of Qt4 abstract item or model classes
 */

/*! \brief these classes knows about paludis
 * \defgroup PaludisAdapter Adapter classes
 * classes accessing the paludis api and/or transfering information from paludis to pertubis
 */

/*! \brief subclasses of qt4 QThread
 * \defgroup Thread Threaded classes
 *
 */

/*! \brief selection data storage
 * \defgroup Selection Selection Storage classes
 * classes storing and/or handling selection and deselection of packages for destinct tasks
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
    class Package;
    class MessageOutput;
    class PackageFilterModel;
    class PackageModel;
    class PackagesThread;
    class PertubisSyncTask;
    class RepositoryListModel;
    class RepositoryListThread;
    class SearchThread;
    class SearchWindow;
    class SetModel;
    class SetThread;
    class Settings;
    class ShowSelectionsThread;
    class TaskBox;

    /*! \brief this widget is the package overview
     * \ingroup Widget
     *
     * This class only exists since we need correct mouse coordinates
     */
    class PackageView : public QTreeView
    {
        Q_OBJECT
    public:

        ///@name Constructors
        ///@{

        /// std constructor
        PackageView(QWidget *parent);
        ///@}
        /// saves the mouse coordinates into m_mousePos
        void mousePressEvent(QMouseEvent* event);

        /// actual mouse coordinates
        QPoint m_mousePos;
    };

    /*! \brief Our main window
     * \ingroup Widget
     *
     * This class is our application configuration. It connects all modules pertubis consists of with
     * the user.
     */
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:

        /// @name Constructors
        ///@{

        /// std constructor
        MainWindow();
        ///@}

        /// std destructor
        virtual ~MainWindow();

    protected:

        /// requests exit application
        void closeEvent(QCloseEvent* event);

    private slots:

        /// @name Creation methods
        ///@{

        /// starts threads and query for data
        void initObject();
        ///@}

        /// do the actual work when a category changed, query for the packages in that category
        void onCategoryChanged(  const QModelIndex& index );

        /// @name Action starting slots
        ///@{

        /// task state changed
        void onDeinstallTask(bool mystate);

        /// query for new detail data
        void onDetailsChanged(const QModelIndex & index);

        /// starts all tasks
        void onStartTasks();

        /// clean data structures and refetch displayed information
        void onTasksFinished();

        /// task state changed
        void onInstallTask(bool mystate);

        /// open an browser with link
        void onOpenURL(const QUrl&);

        /// exit
        void onQuit();

        /// changes filter attribute and restart filtering
        void onRepositoryChanged( const QModelIndex& index );

        /// restart filtering
        void onReposChanged();

        /// start search
        void onSearch();

        /// stopping a search
        void onSearchStopped();

        /// must be called before every access to paludis api
        void onEndOfPaludisAction();

        /// must be called after every access to paludis api
        void onStartOfPaludisAction();

        /// sync selected repositories
        void onSync();

        /// starts selections thread
        void onShowSelections();

        /// show the sync end to the user
        void displaySyncFinished();

        /// show the succcessful search end to the user
        void displaySearchFinished(int count);
        ///@}

        /// @name Displaying slots
        ///@{

        /// opens an context menu to select tasks
        void displayOptionsMenu(const QModelIndex& index);

        /// show the end of fetch action
        void displayCategoryChanged();

        /// displayes item details
        void displayPackageDetails(QString details);

        /// shows the result to the user
        void displaySelectedPackages();

        /// change sync icon gimmick
        void displayNextIcon();
        ///@}

        /// @name Layout modification
        ///@{

        /// show/hide main window
        void toggleMainWindow();
        /// show/hide package window
        void togglePackageView();
        /// show/hide search window
        void toggleSearchWindow();
        /// show/hide settings window
        void toggleSettings();
        /// activate system tray window
        void toggleTrayIcon(QSystemTrayIcon::ActivationReason reason);
        ///@}

    private:

        /// @name Creation methods
        ///@{
        /// create all needed model, view and control objects
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
        ///@}

        /// @name Session persistence
        ///@{

        /// loading application settings
        void loadSettings();

        /// saving application settings
        void saveSettings();
        ///@}

        QString                 m_currentCat;
        paludis::tr1::shared_ptr<paludis::Environment>  m_env;
        CategoryThread*         m_categoryThread;
        CategoryFilterModel*    m_categoryFilterModel;
        CategoryFilterModel*    m_setFilterModel;
        CategoryModel*          m_catModel;
        DetailsThread*          m_detailsThread;
        Package*                m_current;
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
        QTableView*             m_categoryView;
        QTableView*             m_repoListView;
        QTableView*             m_setListView;
        QTabWidget*             m_tabs;
        QTextBrowser*           m_details;
        QToolBar*               m_toolBar;
        QTimer*                 m_timer;
        RepositoryListModel*    m_repoListModel;
        RepositoryListThread*   m_repoListThread;
        SearchThread*           m_searchThread;
        SetModel*               m_setModel;
        SetThread*              m_setThread;
        SearchWindow*           m_searchWindow;
        Settings*               m_settings;
        ShowSelectionsThread*   m_selectionsThread;
        TaskBox*                m_box;
        int                     m_tidInstall;
        int                     m_tidDeinstall;
        int                     m_repoViewTabID;
        int                     m_outputTabID;
        int                     m_detailsTabID;
    };
}

#endif
