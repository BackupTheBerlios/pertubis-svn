
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
 * \defgroup ItemModelClass Item and Model Classes
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

        ///@name Constructors
        ///@{

        /// std constructor
        MainWindow();

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
        void slotCategoryChanged(  const QModelIndex& index );

        /// @name Action starting slots
        ///@{

        /// task state changed
        void slotDeinstallTask(bool mystate);
        /// query for new detail data
        void slotDetailsChanged(const QModelIndex & index);
        /// starts all tasks
        void slotFinish();
        /// clean data structures and refetch displayed information
        void slotFinished();
        /// task state changed
        void slotInstallTask(bool mystate);
        /// open an browser with link
        void slotOpenURL(const QUrl&);
        /// opens an context menu to select tasks
        void slotOptionsMenu(const QModelIndex& index);
        /// exit
        void slotQuit();
        /// changes filter attribute and restart filtering
        void slotRepositoryChanged( const QModelIndex& index );
        /// restart filtering
        void slotReposChanged();


        /// @name Displaying slots
        ///@{

        /// start search
        void slotSearchItem();
        /// displayes number of fetch results
        void slotResultCount();
        /// displayes item details
        void slotShowDetails(QString details);
        /// displayes selected items
        void slotShowSelectedPackages();
        /// sync selected repositories
        void slotSync();
        /// show/hide main window
        void slotToggleMainWindow();
        /// show/hide package window
        void slotTogglePackageView();
        /// show/hide search window
        void slotToggleSearchWindow();
        /// show/hide settings window
        void slotToggleSettings();
        /// activate system tray window
        void slotToggleTrayIcon(QSystemTrayIcon::ActivationReason reason);
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

        /// @name Session management
        ///@{
        /// manages loading and saving application settings
        void loadSettings();
        void saveSettings();
        ///@}

        /// @name gui elements
        ///@{
        /// -
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
        ///@}
    };
}

#endif
