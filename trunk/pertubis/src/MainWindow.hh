
/* Copyright (C) 2007 Stefan Koegl
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
#include <paludis/util/tr1_memory.hh>
#include <paludis/package_id-fwd.hh>
#include <paludis/environment-fwd.hh>

namespace pertubis
{
    class DetailsThread;
    class SettingsPage;
    class MessagePage;
    class InstallSelections;
    class DeinstallSelections;
    class TaskQueue;
    class PertubisInstallTask;
    class PertubisDeinstallTask;
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

        struct MainWindowPrivate;

        /// @name Constructors
        ///@{

        /// std constructor
        MainWindow(bool firstrun);
        ///@}

        /// std destructor
        virtual ~MainWindow();

        /// @name getters
        ///@{
        paludis::tr1::shared_ptr<paludis::Environment> env();
        InstallSelections* installSelections();
        DeinstallSelections* deinstallSelections();
        DetailsThread* detailsThread();
        QWidget* currentPage();
        SettingsPage* settingsPage();
        MessagePage* messagePage();
        TaskQueue*  taskQueue();
        /// changes the actual visble page
        void setPage(QWidget* page);
        bool firstRun();
        ///@}

    public slots:

        /// must be called before every access to paludis api
        void onEndOfPaludisAction();

        /// must be called after every access to paludis api
        void onStartOfPaludisAction();

        /// displays a notice in the status bar
        void displayNotice(const QString & notice);

        /// will be activated if user changes the actual page \p pn. It calls the dependend Page::activatePage()
        void pageChanged(int pn);

        /// hard reset of all information stored to save memory and get the lastest information from paludis
        void onInvalidate();

        /// indicates all pages that they could be out of date and should refetch their information
        void setAllPagesDirty();

    protected:

        /// requests exit application
        void closeEvent(QCloseEvent* event);

    private slots:

        /// exit
        void onQuit();

        /// @name Layout modification
        ///@{

        /// activate system tray window
        void toggleTrayIcon(QSystemTrayIcon::ActivationReason reason);

        void goToSearch();
        ///@}

    private:

        void initGUI();

        ///@}

        /// @name Session persistence
        ///@{

        /// loading application settings
        void loadSettings();

        /// saving application settings
        void saveSettings();
        ///@}

        MainWindowPrivate* const      m_imp;
    };
}

#endif
