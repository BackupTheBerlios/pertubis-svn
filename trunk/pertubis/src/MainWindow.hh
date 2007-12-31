
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

#include <paludis/util/tr1_memory.hh>
#include <paludis/environment-fwd.hh>

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTreeView>

class QActionGroup;
class QDockWidget;
class QMenu;
class QModelIndex;
class QSplitter;
class QString;
class QStackedWidget;
class QTableView;
class QTextBrowser;
class QToolBar;
class QTabWidget;

namespace pertubis
{
    class CategoryFilterModel;
    class CategoryModel;
    class CategoryItem;
    class CategoryThread;
    class DeinstallSelections;
    class DetailsThread;
    class MessageOutput;
    class InstallSelections;
    class Package;
    class PackageFilterModel;
    class PackageModel;
    class PackagesThread;
    class PertubisSyncTask;
    class ReportPackageModel;
    class RepositoryInfoModel;
    class RepositoryInfoThread;
    class RepositoryListModel;
    class RepositoryListThread;
    class SearchThread;
    class SearchWindow;
    class SetThread;
    class Settings;
    class SelectionModel;
    class SystemReport;

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
        void onDeinstallSelections(bool mystate);

        /// query for new detail data
        void onDetailsChanged(const QModelIndex & index);

        /// starts all tasks like installation and deinstallation
        void onStartTasks();

        /// task state changed
        void onInstallSelections(bool mystate);

        /// open an browser with link
        void onOpenURL(const QUrl&);

        /// exit
        void onQuit();

        /// changes active repositories and calls onReposChanged()
        void onRepositoryChanged( const QModelIndex& index );

        /// provides new filter sets for the item filters and restarts filtering
        void restartFilters();

        /// start search
        void onSearch(QString query);

        /// stopping a search
        void onSearchStopped();

        /// adding a glsa entry into the glsa list
        void addGlsa(QString name, QString path);

        /// must be called before every access to paludis api
        void onEndOfPaludisAction();

        /// must be called after every access to paludis api
        void onStartOfPaludisAction();

        void setToolbarDisplayMode(int state);
        void setCatDock(int state);
        void setRepositoryDock(int state);
        void setSetDock(int state);
        void setGlsaDock(int state);

        /// sync selected repositories
        void onSync();

        /// --report
        void onSystemReport();

        /// starts selections thread
        void onShowSelections();

        /// unselects all user selections
        void onUnselectAll();

        /// shows the sync end
        void displaySyncFinished();

        /// shows a succcessful search
        void displaySearchFinished(int count);

        /// shows the statistics for a system report in statusbar
        void displaySystemReportFinished(int total,int issues);

        void displayGlsa(const QModelIndex&);

        void displayGlsaTag(const QModelIndex&);

        ///@}

        /// @name Displaying slots
        ///@{

        /// opens an context menu to select tasks
        void displayOptionsMenu(const QModelIndex& index);

        /// show the end of fetch action
        void displayCategoryChanged();

        /// displayes item details
        void displayPackageDetails(QString text);

        /// shows the result to the user
        void displaySelectedPackages();

        /// change sync icon gimmick
        void displayNextIcon();

        void depListResult(QString);

        void startInstallTask(bool pretend, QString target ="");
        void startDeinstallTask(bool pretend);

        void startSetInstallTask(const QModelIndex & ix);

        void aboutPertubis();
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

        CategoryFilterModel*    m_categoryFilterModel;
        CategoryFilterModel*    m_setFilterModel;
        CategoryModel*          m_catModel;
        CategoryModel*          m_glsaModel;
        CategoryModel*          m_setModel;
        CategoryThread*         m_categoryThread;
        DetailsThread*          m_detailsThread;
        InstallSelections*            m_installTask;
        DeinstallSelections*          m_deinstallTask;
        MessageOutput*          m_output;
        PackageFilterModel*     m_packageFilterModel;
        PackageFilterModel*     m_searchPackageFilterModel;
        Package*                m_current;
        PackageModel*           m_packageModel;
        PackageModel*           m_searchPackageModel;
        ReportPackageModel*     m_reportPackageModel;
        PackagesThread*         m_packageViewThread;
        PackageView*            m_packageView;
        PackageView*            m_searchPackageView;
        PackageView*            m_selectionView;
        PertubisSyncTask*       m_syncTask;
        QAction*                m_acSync;
        QAction*                m_acTogglePackageView;
        QDockWidget*            m_dockCat;
        QDockWidget*            m_dockGlsa;
        QDockWidget*            m_dockRepo;
        QDockWidget*            m_dockTabs;
        QDockWidget*            m_dockSet;
        QMenu*                  m_options;
        QMenu*                  m_trayMenu;
        QStackedWidget*         m_mainWidget;
        QSystemTrayIcon*        m_sysTray;
        QTableView*             m_categoryView;
        QTableView*             m_glsaView;
        QTableView*             m_repoInfoView;
        QTableView*             m_repoListView;
        QTableView*             m_setListView;
        QTabWidget*             m_tabs;
        QTextBrowser*           m_details;
        QTimer*                 m_timer;
        QToolBar*               m_toolBar;
        QTreeView*              m_reportPackageView;
        RepositoryInfoModel*    m_repoInfoModel;
        RepositoryInfoThread*   m_repoInfoThread;
        RepositoryListModel*    m_repoListModel;
        RepositoryListThread*   m_repoListThread;
        SearchThread*           m_searchThread;
        SearchWindow*           m_searchWindow;
        SetThread*              m_setThread;
        Settings*               m_settings;
        SelectionModel*         m_selectionModel;
        SystemReport*           m_sysRep;

        int                     m_tidInstall;
        int                     m_tidDeinstall;
        int                     m_repoViewTabID;
        int                     m_outputTabID;
        int                     m_detailsTabID;
    };
}

#endif
