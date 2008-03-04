
/* Copyright (C) 2007-2008 Stefan Koegl
*
* This file is part of perturbis
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

#include <paludis/metadata_key.hh>
#include <paludis/environment_maker.hh>
#include <paludis/package_database.hh>
#include <paludis/environment-fwd.hh>
#include <paludis/environment.hh>
#include <paludis/util/visitor-impl.hh>
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

using namespace perturbis;

static bool
rootTest(const QString& message)
{
    if (0 != getuid())
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
                                QObject::tr("You are a normal user. Some features will only work for administrators (root)"),
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
        bool eventFilter(QObject *o, QEvent *e)
        {
            Q_UNUSED(o);
            if (e->type() == QEvent::KeyRelease || e->type() == QEvent::MouseButtonRelease)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    };

//     struct UseConfigDisplayer :
//         paludis::ConstVisitor<paludis::MutableMetadataKeyVisitorTypes>
//     {
//         void visit(const paludis::MutableMetadataSectionKey & k)
//         {
//             qDebug() << "mutable metadata section:" << QString::fromStdString(paludis::stringify(k.raw_name())) <<
//                     QString::fromStdString(paludis::stringify(k.human_name())) << "\n";
//             UseConfigDisplayer w;
//             std::for_each(indirect_iterator(k.begin_metadata()), indirect_iterator(k.end_metadata()),
//                     accept_visitor(w));
//         }
//
//         void visit(const paludis::MutableMetadataFSEntryKey & k)
//         {
//             qDebug() << "\t" << QString::fromStdString(paludis::stringify(k.value()));
//         }
//
//         void visit(const paludis::MutableMetadataStringKey & k)
//         {
//             qDebug() << "\t" << QString::fromStdString(paludis::stringify(k.value()));
//         }
//
//         void visit(const paludis::MutableMetadataCollectionKey<paludis::IUseFlagSet> & k)
//         {
//             qDebug() << "use config entry:" << QString::fromStdString(paludis::stringify(k.raw_name())) <<
//                     QString::fromStdString(paludis::stringify(k.human_name()));
//             for (paludis::IUseFlagSet::ConstIterator i(k.value()->begin()), i_end(k.value()->end()) ;
//                  i != i_end ; ++i)
//             {
//                 qDebug() << "\t" << QString::fromStdString(paludis::stringify(*i));
//             }
//         }
//
//         void visit(const paludis::MutableMetadataCollectionKey<paludis::KeywordNameSet> &)
//         {
//         }
//     };
}


namespace perturbis
{
    struct MainWindow::MainWindowPrivate
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

MainWindow::MainWindow(bool firstrun) :
        m_imp(new MainWindowPrivate(firstrun))
{
    if (rootTest(""))
        onQuit();
    initGUI();
    loadSettings();
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow::~MainWindow() - start";
    saveSettings();
    delete m_imp;
    qDebug() << "MainWindow::~MainWindow() - done";
}

void
MainWindow::initGUI()
{
//     qRegisterMetaType<QList<RepositoryListItem*> >("QList<RepositoryListItem*>");
//     qRegisterMetaType<QList<VariantList*> >("QList<QVariantList>");
    qRegisterMetaType<Qt::ToolButtonStyle>("Qt::ToolButtonStyle");
    qRegisterMetaType<QMap<QString, QSet<QString> > >("QMap<QString, QSet<QString> >");

    QAction* acToggleMainWindow = new QAction(QPixmap(":images/logo.png"),tr("main window"),this);
    acToggleMainWindow->setChecked(true);
    acToggleMainWindow->setCheckable(true);
    acToggleMainWindow->setToolTip(html_tooltip(tr("show/hide the perturbis main window"),acToggleMainWindow->text()));

    QAction* acQuit = new QAction(QPixmap(":images/quit.png"),tr("quit") ,this);
    acQuit->setToolTip(html_tooltip(tr("closing the perturbis suite. All unsaved changes will be lost!"),acQuit->text()));

    QAction* acSearch = new QAction(QPixmap(":images/find.png"),tr("find") ,this);
    acSearch->setShortcut(tr("CTRL+f"));
    addAction(acSearch);

    ReleaseEater* filter = new ReleaseEater(this);
    installEventFilter(filter);
    statusBar()->setVisible(true);

    setWindowTitle(tr("perturbis :: Main Window"));
    setWindowIcon(QPixmap(":images/logo.png"));

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

//     paludis::tr1::shared_ptr<paludis::MutableMetadataSectionKey> conf(m_imp->m_env->use_config_key());

//     UseConfigDisplayer displayer;

//     std::for_each(indirect_iterator(conf->begin_metadata()), indirect_iterator(conf->end_metadata()), accept_visitor(displayer));

    m_imp->m_settingsPage = new SettingsPage(this);
    m_imp->m_messagePage->postCreate();
    m_imp->m_installSelections = new InstallSelections(this,tr("install"));
    m_imp->m_deinstallSelections = new DeinstallSelections(this,tr("deinstall"));
    m_imp->m_packageBrowsingPage = new PackageBrowsingPage(this);
    m_imp->m_searchPage = new SearchPage(this);
    m_imp->m_repositoryPage = new RepositoryPage(this);
    m_imp->m_systemReportPage = new SystemReportPage(this);
    m_imp->m_selectionsPage = new SelectionPage(this);
    m_imp->m_pages->addTab(m_imp->m_packageBrowsingPage, QPixmap(":images/packages.png"), tr("Package Browsing"));
    m_imp->m_pages->addTab(m_imp->m_searchPage, QPixmap(":images/find.png"), tr("Search"));
    m_imp->m_pages->addTab(m_imp->m_repositoryPage, QPixmap(":images/repobar.png"), tr("RepositoryPage"));
    m_imp->m_pages->addTab(m_imp->m_systemReportPage, QPixmap(":images/system_report.png"), tr("System Report"));
    m_imp->m_pages->addTab(m_imp->m_selectionsPage, QPixmap(":images/selections.png"), tr("Selections"));
    m_imp->m_pages->addTab(m_imp->m_messagePage, QPixmap(":images/messages.png"), tr("Messages"));
    m_imp->m_pages->addTab(m_imp->m_settingsPage, QPixmap(":images/settings.png"), tr("Settings"));

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

    connect(m_taskQueue(),
            SIGNAL(finished()),
            this,
            SLOT(onInvalidate()));
}

void
MainWindow::loadSettings()
{
    QSettings settings("/etc/perturbis/perturbis.conf",QSettings::IniFormat);
    settings.beginGroup("MainWindow");
        resize(settings.value("size",QVariant(QSize(800,600))).toSize());
        move(settings.value("pos",QVariant(QPoint(341,21))).toPoint());
        QByteArray tmp(settings.value("mystate",true).toByteArray());
    settings.endGroup();
    restoreState(tmp);
}

void
MainWindow::saveSettings()
{
    QSettings settings("/etc/perturbis/perturbis.conf",QSettings::IniFormat);
    settings.beginGroup("MainWindow");
        settings.setValue("mystate", saveState());
        settings.setValue("size", size());
        settings.setValue("pos", pos());
    settings.endGroup();
    qDebug() << "MainWindow::saveSettings() - done";
}

void
MainWindow::displayNotice(const QString & notice)
{
    statusBar()->showMessage(notice);
}


void
MainWindow::setPage(QWidget* page)
{
    Q_ASSERT(m_imp->m_pages != 0);
    m_imp->m_pages->setCurrentWidget(page);
}

void
MainWindow::closeEvent(QCloseEvent* ev)
{
    hide();
    ev->ignore();
}

QWidget*
MainWindow::currentPage()
{
    return m_imp->m_pages->currentWidget();
}

void
MainWindow::onEndOfPaludisAction()
{
    QApplication::restoreOverrideCursor();
    m_imp->m_messagePage->setPolling(false);
}

void
MainWindow::onInvalidate()
{
    using namespace paludis;
    for (PackageDatabase::RepositoryConstIterator r(m_imp->m_env->package_database()->begin_repositories()),
         r_end(m_imp->m_env->package_database()->end_repositories()) ;
         r != r_end ; ++r)
    {
        (*r)->invalidate();
    }
    QMessageBox q(QMessageBox::NoIcon,
                tr("Finished"),
                tr("all tasks finished"),
                QMessageBox::Ok,
                this);
    q.setIconPixmap(QPixmap(":images/run.png"));
    q.setDefaultButton(QMessageBox::Ok);
    q.exec();
    setAllPagesDirty();
}

void
MainWindow::onStartOfPaludisAction()
{
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    m_imp->m_messagePage->setPolling(true);
}

void
MainWindow::onQuit()
{
    QApplication::instance()->quit();
}

void
MainWindow::setAllPagesDirty()
{
    for (int i(0),iEnd(m_imp->m_pages->count());i<iEnd;i++)
        qobject_cast<Page*>(m_imp->m_pages->widget(i))->setOutOfDate(true);
}

void
MainWindow::pageChanged(int widget_index)
{
    Page* page(static_cast<Page*>(m_imp->m_pages->widget(widget_index)));
    if (page != 0)
        page->activatePage();
}

void
MainWindow::toggleTrayIcon(QSystemTrayIcon::ActivationReason reason)
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
MainWindow::goToSearch()
{
    m_imp->m_pages->setCurrentWidget(m_imp->m_searchPage);
}

SettingsPage*
MainWindow::settingsPage()
{
    return m_imp->m_settingsPage;
}

MessagePage*
MainWindow::messagePage()
{
    return m_imp->m_messagePage;
}

InstallSelections*
MainWindow::installSelections()
{
    return m_imp->m_installSelections;
}

DeinstallSelections*
MainWindow::deinstallSelections()
{
    return m_imp->m_deinstallSelections;
}

TaskQueue*
MainWindow::taskQueue()
{
    return m_imp->m_taskQueue;
}

DetailsThread*
MainWindow::detailsThread()
{
    return m_imp->m_detailsThread;
}

bool
MainWindow::firstRun()
{
    return m_imp->m_firstrun;
}

paludis::tr1::shared_ptr<paludis::Environment>
MainWindow::env()
{
    return m_imp->m_env;
}
