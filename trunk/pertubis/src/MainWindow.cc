
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

#include "CategoryFilterModel.hh"
#include "CategoryItem.hh"
#include "CategoryModel.hh"
#include "CategoryThread.hh"
#include "DetailsThread.hh"
#include "FormatterUtils.hh"
#include "GeneralSettings.hh"

#include "InstallSelections.hh"
#include "InstallSettings.hh"
#include "DeinstallSettings.hh"
#include "DepListSettings.hh"
#include "DeinstallSelections.hh"
#include "MainWindow.hh"
#include "MessageOutput.hh"
#include "PackageModelDelegate.hh"
#include "SelectionModelDelegate.hh"
#include "PackageFilterModel.hh"
#include "Package.hh"
#include "PackageModel.hh"
#include "PackageBrowsingPage.hh"
#include "SearchPage.hh"
#include "SelectionPage.hh"
#include "SystemReportPage.hh"
#include "PackagesThread.hh"
#include "PertubisInstallTask.hh"
#include "PertubisDeinstallTask.hh"
#include "QuerySettings.hh"
#include "ReportModelDelegate.hh"
#include "ReportPackage.hh"
#include "ReportPackageModel.hh"
#include "RepositoryPage.hh"
#include "RepositoryInfoModel.hh"
#include "RepositoryListModel.hh"
#include "SearchThread.hh"
#include "SearchWindow.hh"
#include "SetThread.hh"
#include "Settings.hh"
#include "SelectionModel.hh"
#include "SyncTask.hh"
#include "SystemReport.hh"
#include "version.hh"

#include <paludis/environment_maker.hh>

#include <iostream>

#include <QApplication>
#include <QCheckBox>
#include <QCursor>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDir>
#include <QLayout>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QCloseEvent>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextBrowser>
#include <QVBoxLayout>

static bool rootTest(const QString& message)
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

pertubis::MainWindow::MainWindow(bool firstrun) : m_firstrun(firstrun)
{
    rootTest("");
    initGUI();
    loadSettings();
}

pertubis::MainWindow::~MainWindow()
{
    qDebug() << "pertubis::MainWindow::~MainWindow() - start";
    saveSettings();
    if (!m_detailsThread->isFinished())
    {
        m_detailsThread->wait();
    }
    delete m_packageBrowsingPage;
    delete m_selectionsPage;
    delete m_settingsPage;
    delete m_repositoryPage;
    delete m_searchPage;
    qDebug() << "pertubis::MainWindow::~MainWindow() - done";
}

void pertubis::MainWindow::initGUI()
{
    m_output = new MessageOutput(this,this);
    m_env = paludis::EnvironmentMaker::get_instance()->make_from_spec("");
    qRegisterMetaType<QList<RepositoryListItem*> >("QList<RepositoryListItem*>");
    qRegisterMetaType<QList<QVariantList> >("QList<QVariantList>");
    qRegisterMetaType<Qt::ToolButtonStyle>("Qt::ToolButtonStyle");
    qRegisterMetaType<QMap<QString, QSet<QString> > >("QMap<QString, QSet<QString> >");
    m_settingsPage = new SettingsPage(this,this);

    QAction* acToggleMainWindow = new QAction( QPixmap(":images/logo.png"),tr("main window"),this );
    acToggleMainWindow->setChecked(true);
    acToggleMainWindow->setCheckable(true);
    acToggleMainWindow->setToolTip( html_tooltip(tr("show/hide the pertubis main window"),acToggleMainWindow->text() ));

    QAction* acQuit = new QAction( QPixmap(":images/quit.png"),tr("quit") ,this);
    acQuit->setToolTip(html_tooltip(tr("closing the pertubis suite. All unsaved changes will be lost!"),acQuit->text()));

    ReleaseEater* filter = new ReleaseEater(this);
    installEventFilter(filter);
    statusBar()->setVisible(true);

    setWindowTitle( tr("pertubis :: Main Window") );
    setWindowIcon(QPixmap( ":images/logo.png" ));

    // system tray
    m_trayMenu = new QMenu(this);
    m_trayMenu->addAction(acToggleMainWindow);
    m_trayMenu->addAction(acQuit);

    m_sysTray = new QSystemTrayIcon(QPixmap(":images/logo.png"),this);
    m_sysTray->setContextMenu(m_trayMenu);
    m_sysTray->show();

    m_installSelections = new InstallSelections( this,tr("install"));
    m_deinstallSelections = new DeinstallSelections( this,tr("deinstall"));

    m_pages = new QTabWidget(this);
    m_packageBrowsingPage = new PackageBrowsingPage(this,this);
    m_searchPage = new SearchPage(this,this);
    m_repositoryPage = new RepositoryPage(this,this);
    m_systemReportPage = new SystemReportPage(this,this);
    m_selectionsPage = new SelectionPage(this,this);
    m_pages->addTab(m_packageBrowsingPage, QPixmap(":images/packages.png"), tr("Package Browsing") );
    m_pages->addTab(m_searchPage, QPixmap(":images/find.png"), tr("Search") );
    m_pages->addTab(m_repositoryPage, QPixmap(":images/repobar.png"), tr("RepositoryPage") );
    m_pages->addTab(m_systemReportPage, QPixmap(":images/system_report.png"), tr("System Report") );
    m_pages->addTab(m_selectionsPage, QPixmap(":images/selections.png"), tr("Selections") );
    m_pages->addTab(m_output, QPixmap(":images/messages.png"), tr("Messages") );
    m_pages->addTab(m_settingsPage, QPixmap(":images/settings.png"), tr("Settings") );

    //
    m_details = new QTextBrowser(this);
    m_details->setOpenLinks(false);
    m_detailsThread = new DetailsThread(this,m_env);

    m_vSplit = new QSplitter(Qt::Vertical,this);
    m_vSplit->addWidget(m_pages);
    m_vSplit->addWidget(m_details);

    //

    setCentralWidget(m_vSplit);
    show();

    connect(acQuit,
            SIGNAL(triggered()),
            qApp,
            SLOT(quit()));

    connect(acToggleMainWindow,
            SIGNAL(toggled(bool)),
            this,
            SLOT(setVisible(bool)));

    connect(m_pages,
            SIGNAL(currentChanged(int)),
            this,
            SLOT(pageChanged(int)));

    connect(m_detailsThread,
            SIGNAL(sendResult(QString)),
            this,
            SLOT(displayPackageDetails(QString)));

    connect(m_repositoryPage,
            SIGNAL(sendFilters(QSet<QString>)),
            m_packageBrowsingPage,
            SLOT(restartFilters(QSet<QString>)));

    connect(m_repositoryPage,
            SIGNAL(sendFilters(QSet<QString>)),
            m_searchPage,
            SLOT(restartFilters(QSet<QString>)));
}

void pertubis::MainWindow::setPage(QWidget* page)
{
    Q_ASSERT(m_pages != 0);
    m_pages->setCurrentWidget(page);
}

void pertubis::MainWindow::closeEvent(QCloseEvent* ev)
{
    hide();
    ev->ignore();
}

void pertubis::MainWindow::loadSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "MainWindow" );
        resize(settings.value("size",QVariant(QSize(800,600))).toSize());
        move(settings.value("pos",QVariant(QPoint(341,21))).toPoint());
        QByteArray tmp(settings.value("mystate",true).toByteArray());
        m_vSplit->restoreState(settings.value("splitterSizes").toByteArray());
    settings.endGroup();
    restoreState(tmp);
}

void pertubis::MainWindow::saveSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "MainWindow" );
        settings.setValue("mystate", saveState() );
        settings.setValue("size", size() );
        settings.setValue("pos", pos());
        settings.setValue("splitterSizes", m_vSplit->saveState());
    settings.endGroup();
    qDebug() << "pertubis::MainWindow::saveSettings() - done";
}

void pertubis::MainWindow::displayNotice(const QString & notice)
{
    statusBar()->showMessage(notice);
}

void pertubis::MainWindow::displayPackageDetails(QString details)
{
    if (details.isEmpty())
        return;
    m_details->setText(details);
    onEndOfPaludisAction();
}

void pertubis::MainWindow::onEndOfPaludisAction()
{
    QApplication::restoreOverrideCursor();
    m_output->setPollingOff();
}

void pertubis::MainWindow::onStartOfPaludisAction()
{
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    m_output->setPollingOn();
}

void pertubis::MainWindow::onQuit()
{
    QApplication::instance()->quit();
}

void pertubis::MainWindow::pageChanged(int widget_index)
{
    Page* page(static_cast<Page*>(m_pages->widget(widget_index)));
    if (page != 0)
        page->activatePage();
}

void pertubis::MainWindow::showDetails(const paludis::tr1::shared_ptr<const paludis::PackageID> & id)
{
    onStartOfPaludisAction();
    m_detailsThread->start(id);
}

void pertubis::MainWindow::toggleTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Context:
            m_trayMenu->show();
            break;
        default:
            ;
    }
}
