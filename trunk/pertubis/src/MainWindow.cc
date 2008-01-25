
/* Copyright (C) 2007-2008 Stefan Koegl <hotshelf@users.berlios.de>
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

#include "DeinstallSelections.hh"
#include "InstallSelections.hh"
#include "DetailsThread.hh"
#include "FormatterUtils.hh"
#include "MainWindow.hh"
#include "MessageOutput.hh"
#include "PackageBrowsingPage.hh"
#include "RepositoryPage.hh"
#include "SearchPage.hh"
#include "SelectionPage.hh"
#include "Settings.hh"
#include "SystemReportPage.hh"
#include "TaskQueue.hh"

#include "version.hh"

#include <paludis/environment_maker.hh>
#include <paludis/package_database.hh>
#include <paludis/environment-fwd.hh>
#include <paludis/environment.hh>
#include <paludis/util/log.hh>
#include <iostream>
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDir>
#include <QLayout>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QStatusBar>
#include <QTabWidget>

static bool
rootTest(const QString& message)
{
    if (0 != getuid() )
    {
        if (!message.isEmpty())
            QMessageBox::critical(0,
                                QObject::tr("unpriviledged mode"),
                                message,
                                QMessageBox::Ok,
                                QMessageBox::Ok);
        else
            QMessageBox::critical(0,
                                QObject::tr("unpriviledged mode"),
                                QObject::tr("You are a normal user. Some features will only work for administrators ( root )"),
                                QMessageBox::Ok,
                                QMessageBox::Ok);
        return false;
    }
    return true;
}

namespace
{
    /*! \brief We only need press events and filter release events
    *
    */
    class ReleaseEater : public QObject
    {

    public:

        /// std constructor
        ReleaseEater(QObject* pobj) : QObject(pobj) {}
    protected:

        /// filters out release events
        bool eventFilter( QObject *o, QEvent *e )
        {
            Q_UNUSED(o);
            if ( e->type() == QEvent::KeyRelease || e->type() == QEvent::MouseButtonRelease)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    };


}


namespace pertubis
{
    struct MainWindowPrivate
    {
        MainWindowPrivate(bool firstrun) :
            m_firstrun(firstrun)
            {
            }

        ~MainWindowPrivate()
        {
            delete m_packageBrowsingPage;
            delete m_selectionsPage;
            delete m_settingsPage;
            delete m_repositoryPage;
            delete m_searchPage;
            delete m_messagePage;
        }

        paludis::tr1::shared_ptr<paludis::Environment>  m_env;
        DeinstallSelections*    m_deinstallSelections;
        InstallSelections*      m_installSelections;
        SettingsPage*           m_settingsPage;
        TaskQueue*              m_taskQueue;
        SelectionPage*          m_selectionsPage;
        MessagePage*            m_messagePage;
        SearchPage*             m_searchPage;
        RepositoryPage*         m_repositoryPage;
        SystemReportPage*       m_systemReportPage;
        PackageBrowsingPage*    m_packageBrowsingPage;
        DetailsThread*          m_detailsThread;
        QSystemTrayIcon*        m_sysTray;
        QMenu*                  m_trayMenu;
        QTabWidget*             m_pages;
        bool                    m_dirty;
        bool                    m_firstrun;
    };
}

pertubis::MainWindow::MainWindow(bool firstrun) :
        m_imp(new MainWindowPrivate(firstrun) )
{
    rootTest("");
    initGUI();
    loadSettings();
}

pertubis::MainWindow::~MainWindow()
{
    qDebug() << "pertubis::MainWindow::~MainWindow() - start";
    saveSettings();
    delete m_imp;
    qDebug() << "pertubis::MainWindow::~MainWindow() - done";
}

void
pertubis::MainWindow::initGUI()
{
//     qRegisterMetaType<QList<RepositoryListItem*> >("QList<RepositoryListItem*>");
//     qRegisterMetaType<QList<VariantList*> >("QList<QVariantList>");
    qRegisterMetaType<Qt::ToolButtonStyle>("Qt::ToolButtonStyle");
    qRegisterMetaType<QMap<QString, QSet<QString> > >("QMap<QString, QSet<QString> >");

    QAction* acToggleMainWindow = new QAction( QPixmap(":images/logo.png"),tr("main window"),this );
    acToggleMainWindow->setChecked(true);
    acToggleMainWindow->setCheckable(true);
    acToggleMainWindow->setToolTip( html_tooltip(tr("show/hide the pertubis main window"),acToggleMainWindow->text() ));

    QAction* acQuit = new QAction( QPixmap(":images/quit.png"),tr("quit") ,this);
    acQuit->setToolTip(html_tooltip(tr("closing the pertubis suite. All unsaved changes will be lost!"),acQuit->text()));

    QAction* acSearch = new QAction( QPixmap(":images/find.png"),tr("find") ,this);
    acSearch->setShortcut(tr("CTRL+f"));
    addAction(acSearch);

    ReleaseEater* filter = new ReleaseEater(this);
    installEventFilter(filter);
    statusBar()->setVisible(true);

    setWindowTitle( tr("pertubis :: Main Window") );
    setWindowIcon(QPixmap( ":images/logo.png" ));

    // system tray
    m_imp->m_trayMenu = new QMenu(this);
    m_imp->m_trayMenu->addAction(acToggleMainWindow);
    m_imp->m_trayMenu->addAction(acQuit);

    m_imp->m_sysTray = new QSystemTrayIcon(QPixmap(":images/logo.png"),this);
    m_imp->m_sysTray->setContextMenu(m_imp->m_trayMenu);
    m_imp->m_sysTray->show();

    m_imp->m_taskQueue = new TaskQueue(this);
    m_imp->m_pages = new QTabWidget(this);
    m_imp->m_messagePage = new MessagePage(this);
    m_imp->m_env = paludis::EnvironmentMaker::get_instance()->make_from_spec("");
//     paludis::tr1::shared_ptr<paludis::ConfigDomains> conf(m_imp->m_env->use_config_key());
    m_imp->m_settingsPage = new SettingsPage(this);
    m_imp->m_messagePage->postCreate();
    m_imp->m_installSelections = new InstallSelections( this,tr("install"));
    m_imp->m_deinstallSelections = new DeinstallSelections( this,tr("deinstall"));
    m_imp->m_packageBrowsingPage = new PackageBrowsingPage(this);
    m_imp->m_searchPage = new SearchPage(this);
    m_imp->m_repositoryPage = new RepositoryPage(this);
    m_imp->m_systemReportPage = new SystemReportPage(this);
    m_imp->m_selectionsPage = new SelectionPage(this);
    m_imp->m_pages->addTab(m_imp->m_packageBrowsingPage, QPixmap(":images/packages.png"), tr("Package Browsing") );
    m_imp->m_pages->addTab(m_imp->m_searchPage, QPixmap(":images/find.png"), tr("Search") );
    m_imp->m_pages->addTab(m_imp->m_repositoryPage, QPixmap(":images/repobar.png"), tr("RepositoryPage") );
    m_imp->m_pages->addTab(m_imp->m_systemReportPage, QPixmap(":images/system_report.png"), tr("System Report") );
    m_imp->m_pages->addTab(m_imp->m_selectionsPage, QPixmap(":images/selections.png"), tr("Selections") );
    m_imp->m_pages->addTab(m_imp->m_messagePage, QPixmap(":images/messages.png"), tr("Messages") );
    m_imp->m_pages->addTab(m_imp->m_settingsPage, QPixmap(":images/settings.png"), tr("Settings") );



    setCentralWidget(m_imp->m_pages);
    show();

    connect(acQuit,
            SIGNAL(triggered()),
            qApp,
            SLOT(quit()));

    connect(acSearch,
            SIGNAL(triggered()),
            this,
            SLOT(goToSearch()));

    connect(acToggleMainWindow,
            SIGNAL(toggled(bool)),
            this,
            SLOT(setVisible(bool)));

    connect(m_imp->m_pages,
            SIGNAL(currentChanged(int)),
            this,
            SLOT(pageChanged(int)));

    connect(m_imp->m_repositoryPage,
            SIGNAL(sendFilters(QSet<QString>)),
            m_imp->m_packageBrowsingPage,
            SLOT(restartFilters(QSet<QString>)));

    connect(m_imp->m_repositoryPage,
            SIGNAL(sendFilters(QSet<QString>)),
            m_imp->m_searchPage,
            SLOT(restartFilters(QSet<QString>)));
}

void
pertubis::MainWindow::loadSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "MainWindow" );
        resize(settings.value("size",QVariant(QSize(800,600))).toSize());
        move(settings.value("pos",QVariant(QPoint(341,21))).toPoint());
        QByteArray tmp(settings.value("mystate",true).toByteArray());
    settings.endGroup();
    restoreState(tmp);
}

void
pertubis::MainWindow::saveSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "MainWindow" );
        settings.setValue("mystate", saveState() );
        settings.setValue("size", size() );
        settings.setValue("pos", pos());
    settings.endGroup();
    qDebug() << "pertubis::MainWindow::saveSettings() - done";
}

void
pertubis::MainWindow::displayNotice(const QString & notice)
{
    statusBar()->showMessage(notice);
}


void
pertubis::MainWindow::setPage(QWidget* page)
{
    Q_ASSERT(m_imp->m_pages != 0);
    m_imp->m_pages->setCurrentWidget(page);
}

void
pertubis::MainWindow::closeEvent(QCloseEvent* ev)
{
    hide();
    ev->ignore();
}

QWidget*
pertubis::MainWindow::currentPage()
{
    return m_imp->m_pages->currentWidget();
}

void
pertubis::MainWindow::onEndOfPaludisAction()
{
    QApplication::restoreOverrideCursor();
    m_imp->m_messagePage->setPolling(false);
}

void
pertubis::MainWindow::onInvalidate()
{
    using namespace paludis;
    for (PackageDatabase::RepositoryConstIterator r(m_imp->m_env->package_database()->begin_repositories()),
         r_end(m_imp->m_env->package_database()->end_repositories()) ;
         r != r_end ; ++r)
    {
        (*r)->invalidate();
    }
}

void
pertubis::MainWindow::onStartOfPaludisAction()
{
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    m_imp->m_messagePage->setPolling(true);
}

void
pertubis::MainWindow::onQuit()
{
    QApplication::instance()->quit();
}

void
pertubis::MainWindow::setAllPagesDirty()
{
    for (int i(0),iEnd(m_imp->m_pages->count());i<iEnd;i++)
        qobject_cast<Page*>(m_imp->m_pages->widget(i))->setOutOfDate(true);
}

void
pertubis::MainWindow::pageChanged(int widget_index)
{
    Page* page(static_cast<Page*>(m_imp->m_pages->widget(widget_index)));
    if (page != 0)
        page->activatePage();
}

void
pertubis::MainWindow::toggleTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Context:
            m_imp->m_trayMenu->show();
            break;
        default:
            ;
    }
}

void
pertubis::MainWindow::goToSearch()
{
    m_imp->m_pages->setCurrentWidget(m_imp->m_searchPage);
}

pertubis::SettingsPage*
pertubis::MainWindow::settingsPage()
{
    return m_imp->m_settingsPage;
}

pertubis::MessagePage*
pertubis::MainWindow::messagePage()
{
    return m_imp->m_messagePage;
}

pertubis::InstallSelections*
pertubis::MainWindow::installSelections()
{
    return m_imp->m_installSelections;
}

pertubis::DeinstallSelections*
pertubis::MainWindow::deinstallSelections()
{
    return m_imp->m_deinstallSelections;
}

pertubis::TaskQueue*
pertubis::MainWindow::taskQueue()
{
    return m_imp->m_taskQueue;
}

pertubis::DetailsThread*
pertubis::MainWindow::detailsThread()
{
    return m_imp->m_detailsThread;
}

bool
pertubis::MainWindow::firstRun()
{
    return m_imp->m_firstrun;
}

paludis::tr1::shared_ptr<paludis::Environment>
pertubis::MainWindow::env()
{
    return m_imp->m_env;
}
