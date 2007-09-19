
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
#define _PERTUBIS_ENTRY_PROTECTOR_DATABASE_VIEW_H

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
    class CategoryModel;
    class Item;
    class MessageOutput;
    class PackageDetails;
    class PackageModel;
    class ReleaseEater;
    class SearchWindow;
    class Settings;
    class TaskBox;
    class ThreadFetchCategories;
    class ThreadFetchDetails;
    class ThreadFetchItem;
    class ThreadFetchPackages;
    class ThreadKeywordManager;
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

    protected:

        void closeEvent(QCloseEvent* event);

    private:

        void createActions();
        void createCatbar();
        void createDetails();
        void createOptionsMenu();
        void createOutput();
        void createPackageView();
        void createTasks();
        void createToolBar();
        void createTrayMenu();
        void createUseflagEditor();
        void createWindowSearch();
        void loadCategories();
        void loadSettings();
        void saveSettings();

        paludis::tr1::shared_ptr<paludis::Environment> m_env;
        CategoryModel*          m_catModel;
        Item*                   m_current;
        MessageOutput*          m_output;
        PackageModel*           m_packModel;
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
        QAction*                m_acToggleSearchWindow;
        QAction*                m_acToggleUseBar;
        QDockWidget*            m_dockCat;
        QDockWidget*            m_dockDetails;
        QDockWidget*            m_dockUse;
        QMenu*                  m_options;
        QMenu*                  m_trayMenu;
        QSplitter*              m_vSplit;
        QTabWidget*             m_tabs;
        QTableView*             m_categories;
        QTextBrowser*           m_details;
        QToolBar*               m_toolBar;
        ReleaseEater*           m_filter;
        SearchWindow*           m_windowSearch;
        Settings*               m_settings;
        TaskBox*                m_box;
        ThreadFetchCategories*  m_threadCategories;
        ThreadFetchDetails*     m_threadDetails;
        ThreadFetchItem*        m_threadItem;
        ThreadFetchPackages*    m_threadPackages;
        ThreadKeywordManager*   m_threadKeywords;

        int                     m_tidInstall;
        int                     m_tidDeinstall;
//         UseFlagEditor*          m_useflagEditor;

    private slots:

        void slotCategoryChanged(  const QModelIndex& index );
        void slotDeinstallTask(bool mystate);
        void slotDetailsChanged(const QModelIndex & index);
        void slotEditUseTask();
        void slotInstallTask(bool mystate);
        void slotOpenSettings();
        void slotOpenURL(const QUrl&);
        void slotOptionsMenu(const QModelIndex& index);
        void slotQuit();
        void slotRefreshCategories();
        void slotSearchItem();
        void slotSearchItemFinished();
        void slotSearchPackagesFinished();
        void slotSelectedPackages();
        void slotShowDetails(QString details);
        void slotSync();
        void slotToggleTrayIcon(QSystemTrayIcon::ActivationReason reason);

        void slotToggleMainWindow();
        void slotTogglePackageView();
        void slotToggleSearchWindow();
    };
}

#endif
