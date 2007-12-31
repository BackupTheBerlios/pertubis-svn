
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
#include "GLSAParser.hh"
#include "InstallSelections.hh"
#include "InstallSettings.hh"
#include "DeinstallSettings.hh"
#include "DepListSettings.hh"
#include "DeinstallSelections.hh"
#include "MainWindow.hh"
#include "MessageOutput.hh"
#include "OptionsDelegate.hh"
#include "PackageFilterModel.hh"
#include "Package.hh"
#include "PackageModel.hh"
#include "PackagesThread.hh"
#include "PertubisInstallTask.hh"
#include "PertubisDeinstallTask.hh"
#include "QuerySettings.hh"
#include "ReportPackage.hh"
#include "ReportPackageModel.hh"
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
#include <QActionGroup>
#include <QApplication>
#include <QCheckBox>
#include <QCursor>
#include <QDataStream>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDir>
#include <QDockWidget>
#include <QFile>
#include <QFont>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPixmap>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QStackedWidget>
#include <QSplitter>
#include <QStatusBar>

#include <QTableView>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBar>
#include <QUrl>
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

pertubis::PackageView::PackageView(QWidget* pWidget) : QTreeView(pWidget)
{
}

void pertubis::PackageView::mousePressEvent(QMouseEvent* ev)
{
    m_mousePos = ev->pos();
    QTreeView::mousePressEvent(ev);
}

pertubis::MainWindow::MainWindow()
{
    initGUI();
    initLayout();
    loadSettings();
    QTimer::singleShot(0,this,SLOT(initObject()));
}

pertubis::MainWindow::~MainWindow()
{
    saveSettings();
    if (!m_searchThread->isFinished())
    {
        m_searchThread->stopExec();
        m_searchThread->wait();
    }
    if (!m_packageViewThread->isFinished())
    {
        m_packageViewThread->stopExec();
        m_packageViewThread->wait();
    }
    if (!m_sysRep->isFinished())
    {
        m_sysRep->stopExec();
        m_sysRep->wait();
    }
}

void pertubis::MainWindow::initObject()
{
    m_repoListThread->start();
    m_setThread->start();
    m_categoryThread->start();
    if (!m_currentCat.isEmpty())
        m_packageViewThread->start(m_currentCat);
    rootTest("");
}

void pertubis::MainWindow::initGUI()
{
    m_output = new MessageOutput(this);
    m_env = paludis::EnvironmentMaker::get_instance()->make_from_spec("");
    qRegisterMetaType<QList<RepositoryListItem*> >("QList<RepositoryListItem*>");
    qRegisterMetaType<QList<QVariantList> >("QList<QVariantList>");
    qRegisterMetaType<Qt::ToolButtonStyle>("Qt::ToolButtonStyle");
    qRegisterMetaType<QMap<QString, QSet<QString> > >("QMap<QString, QSet<QString> >");
    m_settings = new Settings(this);

    QAction* acToggleMainWindow = new QAction( QPixmap(":images/logo.png"),tr("main window"),this );
    acToggleMainWindow->setCheckable(true);
    acToggleMainWindow->setChecked(true);
    acToggleMainWindow->setToolTip( html_tooltip(tr("show/hide the pertubis main window"),acToggleMainWindow->text() ));

    m_acTogglePackageView = new QAction( QPixmap(":images/packages.png"),tr("packages"),this );
    m_acTogglePackageView->setCheckable(true);
    m_acTogglePackageView->setChecked(true);
    m_acTogglePackageView->setToolTip( html_tooltip(tr("enable/disable the package window in the middle"),m_acTogglePackageView->text()));

    QAction* acToggleSearchWindow = new QAction( QPixmap(":images/find.png"),tr("find") ,this);
    acToggleSearchWindow->setShortcut( tr("CTRL+f"));
    acToggleSearchWindow->setToolTip(html_tooltip( tr("toggle search window"),acToggleSearchWindow->text() ) );

    QAction* acPref = new QAction( QPixmap(":images/settings.png"),tr( "Settings"),this);
    acPref->setShortcut( tr("CTRL+P"));
    acPref->setToolTip( html_tooltip(tr("configure pertubis"),acPref->text()));

    QAction* acSysRep = new QAction( QPixmap(":images/system_report.png"),tr( "System Report"),this);
    acSysRep->setToolTip(html_tooltip(tr("show all stale packages in system"),acSysRep->text()));

    QAction* acSelection = new QAction( QPixmap(":images/selections.png"),tr("selections") ,this);
    acSelection->setShortcut( tr("CTRL+F11"));
    acSelection->setToolTip( html_tooltip(tr("here you check the selections you made"),acSelection->text()) );

    QAction* acUnselectAll = new QAction( QPixmap(":images/selections.png"),tr("unselect all") ,this);
    acUnselectAll->setShortcut( tr("CTRL+DEL"));
    acUnselectAll->setToolTip( html_tooltip(tr("here you can unselect all selections you made"),acSelection->text()) );

    QMenu* acSelectionMenu = new QMenu(this);
    acSelection->setMenu(acSelectionMenu);
    acSelectionMenu->addAction(acUnselectAll);

    QAction* acFinish = new QAction( QPixmap(":images/run.png"),tr("start") ,this);
    acFinish->setShortcut( tr("CTRL+F12"));
    acFinish->setToolTip(html_tooltip( tr("starts all pending tasks you selected"),acFinish->text())) ;

    m_acSync = new QAction( QPixmap(":images/sync0.png"), tr("sync"),this);
    m_acSync->setToolTip(html_tooltip(tr("To get the latest releases and bugfixes it is neccessary to update the package database.<br><br>It is sufficient to sync your repositories once a day"),m_acSync->text()));

    QAction* acQuit = new QAction( QPixmap(":images/quit.png"),tr("quit") ,this);
    acQuit->setToolTip(html_tooltip(tr("closing the pertubis suite. All unsaved changes will be lost!"),acQuit->text()));

    QAction* acInstall= new QAction(tr("install"),this);
    acInstall->setToolTip( html_tooltip(tr("install this package")));
    acInstall->setCheckable(true);

    QAction* acAbout = new QAction(QPixmap(":images/logo.png"), tr("about pertubis"),this);

    QAction* acDeinstall= new QAction(tr("deinstall"),this);
    acDeinstall->setToolTip(html_tooltip( tr("deinstall this package") ));
    acDeinstall->setCheckable(true);

    QAction* acEditUse = new QAction(tr("edit useflags"),this);
    acEditUse->setToolTip(html_tooltip( tr("opens a useflag editor for this item") ));

    QAction* acMasking = new QAction(tr("(un)mask"),this);
    acMasking->setToolTip(html_tooltip(tr("toggle the masking for this package") ));

    m_details = new QTextBrowser(this);
    m_details->setOpenLinks(false);

    qDebug() << "pertubis::MainWindow::createRepositoryView()";
    m_repoInfoModel = new RepositoryInfoModel(this);
    m_repoInfoModel->setHorizontalHeaderLabels(QStringList() <<
            tr("name") <<
            tr("state"));

    m_repoInfoView = new QTableView(this);
    m_repoInfoView->setModel(m_repoInfoModel);
    m_repoInfoView->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    m_repoInfoView->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
    m_repoInfoView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_repoInfoView->horizontalHeader()->setVisible(true);
    m_repoInfoView->verticalHeader()->setVisible(false);

    QPalette p(m_repoInfoView->palette());
    p.setColor(QPalette::Base,QColor(170,170,170)); // background color  = black
    m_repoInfoView->setPalette(p);

    m_tabs = new QTabWidget(this);
    m_dockTabs = new QDockWidget(tr("details"),this);
    m_dockTabs->setObjectName("details_dock");
    m_detailsTabID = m_tabs->addTab(m_details,tr("Package Details") );
    m_repoViewTabID = m_tabs->addTab(m_repoInfoView,tr("repository details") );
    m_outputTabID = m_tabs->addTab(m_output,tr("Messages"));
    m_dockTabs->setWidget(m_tabs);

    m_installTask = new InstallSelections( this,acInstall,tr("install"),po_install);

    m_deinstallTask = new DeinstallSelections( this,acDeinstall,tr("deinstall"),po_deinstall);

    m_catModel = new CategoryModel(this);
    m_catModel->setHorizontalHeaderLabels(QStringList() << tr("category"));

    m_categoryFilterModel = new CategoryFilterModel(this);
    m_categoryFilterModel->setSourceModel(m_catModel);

    m_categoryView = new QTableView(this);
    m_categoryView->horizontalHeader()->setVisible(false);
    m_categoryView->verticalHeader()->setVisible(false);
    m_categoryView->setModel(m_categoryFilterModel);
    m_categoryView->setShowGrid(false);
    m_categoryView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    QFont myfont(m_categoryView->font());
    myfont.setBold(true);
    m_categoryView->setFont(myfont);

    m_dockCat = new QDockWidget(tr("category list"),this);
    m_dockCat->setObjectName("category list");
    m_dockCat->layout()->setMargin(0);
    m_dockCat->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    m_dockCat->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_dockCat->setWidget(m_categoryView);

    QAction* acToggleCatBar = m_dockCat->toggleViewAction();
    acToggleCatBar->setText(tr("category list"));
    acToggleCatBar->setIcon(QPixmap(":images/catbar.png"));
    acToggleCatBar->setShortcut(tr("F11"));
    acToggleCatBar->setToolTip(html_tooltip(tr("enable/disable the category sidebar"),acToggleCatBar->text()) );

    m_repoListModel = new RepositoryListModel(this);
    m_repoListModel->setHorizontalHeaderLabels(QStringList() <<
            tr("filter on") << tr("repository"));

    m_repoListView = new QTableView(this);
    m_repoListView->setModel(m_repoListModel);
    m_repoListView->horizontalHeader()->setVisible(false);
    m_repoListView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    m_repoListView->setFont(myfont);

    m_repoListView->verticalHeader()->setVisible(false);
    m_repoListView->setShowGrid(false);

    m_dockRepo = new QDockWidget(tr("repository list"),this);
    m_dockRepo->setObjectName("repository list");
    m_dockRepo->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    m_dockRepo->layout()->setMargin(0);
    m_dockRepo->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_dockRepo->setWidget(m_repoListView);
    m_dockRepo->setToolTip(html_tooltip(tr("Here you can select the repositories pertubis will use.</p><p>Click on a repository to enable/disable filtering by this repository")));

    QAction* acToggleRepoBar = m_dockRepo->toggleViewAction();
    acToggleRepoBar->setText(tr("repository list"));
    acToggleRepoBar->setIcon(QPixmap(":images/repobar.png"));
    acToggleRepoBar->setShortcut( tr("F10"));
    acToggleRepoBar->setToolTip( html_tooltip(tr("enable/disable the repository sidebar"),acToggleRepoBar->text()) );

    m_glsaModel = new CategoryModel(this);
    m_glsaModel->setHorizontalHeaderLabels(QStringList() << tr("set"));

    m_glsaView = new QTableView(this);
    m_glsaView->setModel(m_glsaModel);
    m_glsaView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_glsaView->horizontalHeader()->setVisible(false);
    m_glsaView->verticalHeader()->setVisible(false);
    m_glsaView->setShowGrid(false);

    m_glsaView->setFont(myfont);

    m_dockGlsa = new QDockWidget(tr("security announcements"),this);
    m_dockGlsa->setObjectName("security announcements");
    m_dockGlsa->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    m_dockGlsa->setWidget(m_glsaView);
    m_dockGlsa->layout()->setMargin(0);
    m_dockGlsa->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QAction* acToggleGlsaBar = m_dockGlsa->toggleViewAction();
    acToggleGlsaBar->setIcon( QPixmap(":images/glsa.png"));
    acToggleGlsaBar->setText(tr("security"));
    acToggleGlsaBar->setShortcut( tr("CTRL+I"));
    acToggleGlsaBar->setToolTip(html_tooltip( tr("see Gentoo Linux Security Announcements"),acToggleGlsaBar->text())) ;

    //

    m_setModel = new CategoryModel(this);
    m_setModel->setHorizontalHeaderLabels(QStringList() << tr("set"));

    m_setFilterModel = new CategoryFilterModel(this);
    m_setFilterModel->setSourceModel(m_setModel);

    m_setListView = new QTableView(this);
    m_setListView->setModel(m_setFilterModel);
    m_setListView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_setListView->horizontalHeader()->setVisible(false);
    m_setListView->verticalHeader()->setVisible(false);
    m_setListView->setShowGrid(false);

    m_setListView->setFont(myfont);

    m_dockSet = new QDockWidget(tr("set list"),this);
    m_dockSet->setObjectName("set list");
    m_dockSet->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    m_dockSet->setWidget(m_setListView);
    m_dockSet->layout()->setMargin(0);
    m_dockSet->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QAction* acToggleSetBar = m_dockSet->toggleViewAction();
    acToggleSetBar->setText(tr("set list"));
    acToggleSetBar->setIcon(QPixmap(":images/setbar.png"));
    acToggleSetBar->setShortcut(tr("F12"));
    acToggleSetBar->setToolTip(html_tooltip(tr("enable/disable the set sidebar"),acToggleSetBar->text()) );

    m_mainWidget = new QStackedWidget(this);
    setCentralWidget(m_mainWidget);

    //
    m_packageModel = new PackageModel(this);

    m_packageModel->setHorizontalHeaderLabels(QStringList() <<
            tr("+") <<
            tr("-") <<
            tr("package") <<
            tr("category") <<
            tr("repository") <<
            tr("installed") <<
            tr("masked by") <<
            tr("change"));
    m_packageView = new PackageView(this);
    m_packageView->setItemsExpandable(true);
    m_packageFilterModel = new PackageFilterModel(this);
    m_packageFilterModel->setSourceModel(m_packageModel);
    m_packageView->setContextMenuPolicy(Qt::CustomContextMenu);
//     m_packageView->setItemDelegate(new OptionsDelegate(this,m_packageFilterModel));
    m_packageView->setModel(m_packageFilterModel);
    m_packageView->header()->setVisible(true);
    m_packageView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_packageView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_packageView->setFont(myfont);
    m_mainWidget->addWidget(m_packageView);

    //
    m_searchPackageModel = new PackageModel(this);
    m_searchPackageModel->setHorizontalHeaderLabels(QStringList() <<
            tr("marked") <<
            tr("package") <<
            tr("category") <<
            tr("repository") <<
            tr("installed") <<
            tr("masked by") <<
            tr("change"));
    m_searchPackageView = new PackageView(this);
    m_searchPackageView->setItemsExpandable(true);
    m_searchPackageFilterModel = new PackageFilterModel(this);
    m_searchPackageFilterModel->setSourceModel(m_searchPackageModel);
    m_searchPackageView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_searchPackageView->setItemDelegate(new OptionsDelegate(this,m_searchPackageFilterModel));
    m_searchPackageView->setModel(m_searchPackageFilterModel);
    m_searchPackageView->header()->setVisible(true);
    m_searchPackageView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_searchPackageView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_searchPackageView->setFont(myfont);
    m_mainWidget->addWidget(m_searchPackageView);

    //

    m_selectionModel = new SelectionModel(this);
    m_selectionModel->setHorizontalHeaderLabels(QStringList() <<
            tr("marked") <<
            tr("package") <<
            tr("category") <<
            tr("version") <<
            tr("repository"));
    m_selectionView = new PackageView(this);
    m_selectionView->setItemsExpandable(true);
    m_selectionView->setItemDelegate(new OptionsDelegate(this,0));
    m_selectionView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_selectionView->setModel(m_selectionModel);
    m_selectionView->header()->setVisible(true);
    m_selectionView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_selectionView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_selectionView->setFont(myfont);
    m_mainWidget->addWidget(m_selectionView);

    //

    m_reportPackageModel = new ReportPackageModel(this);
    m_mainWidget->addWidget(m_packageView);

    m_reportPackageModel->setHorizontalHeaderLabels(QStringList() <<
            tr("deinstall") <<
            tr("package") <<
            tr("category") <<
            tr("version"));
    m_reportPackageView = new QTreeView(this);
    m_reportPackageView->setItemsExpandable(true);
    m_reportPackageView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_reportPackageView->setModel(m_reportPackageModel);
    m_reportPackageView->header()->setVisible(true);
    m_reportPackageView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_reportPackageView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_reportPackageView->setFont(myfont);
    m_mainWidget->addWidget(m_reportPackageView);

    //
    m_toolBar = new QToolBar(tr("pertubis toolbar"),this);
    m_toolBar->setObjectName("pertubis toolbar");
    addToolBar(Qt::LeftToolBarArea,m_toolBar );
    setToolButtonStyle( m_settings->m_generalView->m_model->m_toolbarDisplayMode == 0 ? Qt::ToolButtonIconOnly : Qt::ToolButtonTextUnderIcon );
    m_toolBar->addAction(acToggleCatBar);
    m_toolBar->addAction(acToggleRepoBar);
    m_toolBar->addAction(acToggleSetBar);
    m_toolBar->addAction(acToggleGlsaBar);
    m_toolBar->addAction(m_acTogglePackageView);
    m_toolBar->addAction(acToggleSearchWindow);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_acSync);
    m_toolBar->addAction(acSysRep);
    m_toolBar->addAction(acSelection);
    m_toolBar->addAction(acFinish);
    m_toolBar->addAction(acPref);
    m_toolBar->addSeparator();
    m_toolBar->addAction(acAbout);
    m_options  = new QMenu(tr("Package Options"),this);

    m_searchThread = new SearchThread(this,m_env,m_settings->m_queryView->m_model,m_installTask,m_deinstallTask);
    m_searchWindow = new SearchWindow(this,m_settings->m_queryView->m_model,m_searchThread);

    qDebug() << "pertubis::MainWindow::createTrayMenu()";
    m_trayMenu = new QMenu(this);
    m_trayMenu->addAction(acToggleMainWindow);
    m_trayMenu->addAction(acPref);
    m_trayMenu->addAction(acQuit);

    m_sysTray = new QSystemTrayIcon(QPixmap(":images/logo.png"),this);
    m_sysTray->setContextMenu(m_trayMenu);
    m_sysTray->show();

    statusBar()->show();
    ReleaseEater* filter = new ReleaseEater(this);
    installEventFilter(filter);
    addDockWidget(m_settings->m_generalView->m_model->m_categoryDockPosition == 0  ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea  , m_dockCat);
    addDockWidget(m_settings->m_generalView->m_model->m_repositoryDockPosition == 0  ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea, m_dockRepo);
    addDockWidget(m_settings->m_generalView->m_model->m_setDockPosition == 0  ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea, m_dockSet);
    addDockWidget(m_settings->m_generalView->m_model->m_glsaDockPosition == 0  ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea, m_dockGlsa);
    addDockWidget(Qt::BottomDockWidgetArea, m_dockTabs);
    m_sysRep = new SystemReport(this,m_env,m_deinstallTask);
    m_repoInfoThread = new RepositoryInfoThread(this,m_env);
    m_categoryThread = new CategoryThread(this,m_env);
    m_packageViewThread = new PackagesThread(this,m_env,m_installTask,m_deinstallTask);
    m_detailsThread = new DetailsThread(this,m_env);
    m_setThread = new SetThread(this,m_env);
    m_repoListThread = new RepositoryListThread(this,m_env);
    m_syncTask = new PertubisSyncTask(m_env,this);
    m_timer = new QTimer(this);

    connect(acDeinstall,
            SIGNAL(toggled(bool)),
            this,
            SLOT(onDeinstallSelections(bool)));

    connect(acFinish,
            SIGNAL(triggered()),
            this,
            SLOT(onStartTasks()));

    connect(acInstall,
            SIGNAL(toggled(bool)),
            this,
            SLOT(onInstallSelections(bool)));

    connect(m_acSync,
            SIGNAL(triggered()),
            this,
            SLOT(onSync()));

    connect(acSelection,
            SIGNAL(triggered()),
            this,
            SLOT(onShowSelections()));

    connect(acUnselectAll,
            SIGNAL(triggered()),
            this,
            SLOT(onUnselectAll()));

    connect(acPref,
            SIGNAL(triggered()),
            this,
            SLOT(toggleSettings()));

    connect(acQuit,
            SIGNAL(triggered()),
            qApp,
            SLOT(quit()));

    connect(acToggleMainWindow,
            SIGNAL(triggered()),
            this,
            SLOT(toggleMainWindow()));

    connect(m_acTogglePackageView,
            SIGNAL(triggered()),
            this,
            SLOT(togglePackageView()));

    connect(acToggleSearchWindow,
            SIGNAL(triggered()),
            this,
            SLOT(toggleSearchWindow()));

    connect(m_installTask,
            SIGNAL(depListResult(QString)),
            this,
            SLOT(depListResult(QString)));

    connect(m_categoryView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onCategoryChanged( const QModelIndex& )) );

    connect(m_setListView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(startSetInstallTask( const QModelIndex& )) );

    connect(m_glsaView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(displayGlsa( const QModelIndex& )) );

    connect(m_categoryThread,
            SIGNAL(sendCategory(QMap<QString, QSet<QString> >)),
            m_catModel,
            SLOT(appendCategory(QMap<QString, QSet<QString> >)));

    connect(m_details,
            SIGNAL(anchorClicked(const QUrl&)),
            this,
            SLOT(onOpenURL(const QUrl&)));

    connect(m_detailsThread,
            SIGNAL(sendResult(QString)),
            this,
            SLOT(displayPackageDetails(QString)));

    connect(m_packageViewThread,
            SIGNAL(addPackage(Package*)),
            m_packageModel,
            SLOT(prependPackage(Package*)));

    connect(m_packageViewThread,
            SIGNAL(finished()),
            this,
            SLOT(displayCategoryChanged()));

    connect(m_packageViewThread,
            SIGNAL(changeInCat(QString)),
            m_catModel,
            SLOT(slotChangeInCat(QString)));

    connect(m_packageView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onDetailsChanged( const QModelIndex& )) );

    connect(m_searchPackageView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onDetailsChanged( const QModelIndex& )) );

    connect(m_packageView,
            SIGNAL(clicked(const QModelIndex&)),
            this,
            SLOT(displayOptionsMenu(const QModelIndex&)));

    connect(m_searchPackageView,
            SIGNAL(clicked(const QModelIndex&)),
            this,
            SLOT(displayOptionsMenu(const QModelIndex&)));

    connect(m_searchThread,
            SIGNAL(changeInCat(QString)),
            m_catModel,
            SLOT(slotChangeInCat(QString)));

    connect(m_searchThread,
            SIGNAL(packageResult(Package*)),
            m_searchPackageModel,
            SLOT(appendPackage(Package*)));

    connect(m_searchThread,
            SIGNAL(finished(int)),
            this,
            SLOT(displaySearchFinished(int)));

    connect(m_searchWindow,
            SIGNAL(stopSearch()),
            this,
            SLOT(onSearchStopped()));

    connect(m_searchWindow,
            SIGNAL(search(QString)),
            this,
            SLOT(onSearch(QString)));

    connect(acSysRep,
            SIGNAL(triggered()),
            this,
            SLOT(onSystemReport()));

    connect(m_sysRep,
            SIGNAL(appendPackage(ReportPackage*)),
            m_reportPackageModel,
            SLOT(appendPackage(ReportPackage*)));

    connect(m_sysRep,
            SIGNAL(finished(int,int)),
            this,
            SLOT(displaySystemReportFinished(int,int)));

    connect(m_setThread,
            SIGNAL(sendSet(QMap<QString, QSet<QString> >)),
            m_setModel,
            SLOT(appendCategory(QMap<QString, QSet<QString> >)));

    connect(m_repoListThread,
            SIGNAL(sendNames(QStringList)),
            m_repoListModel,
            SLOT(slotResult(QStringList)));

    connect(m_repoListThread,
            SIGNAL(finished()),
            this,
            SLOT(restartFilters()));

    connect(m_repoListView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onRepositoryChanged( const QModelIndex& )) );

    connect(m_sysTray,
            SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,
            SLOT(toggleTrayIcon(QSystemTrayIcon::ActivationReason)));

    connect(m_syncTask,
            SIGNAL(sendMessage(QString)),
            m_output,
            SLOT(append(QString)));

    connect(m_syncTask,
            SIGNAL(finished()),
            this,
            SLOT(displaySyncFinished()));

    connect(m_timer,
            SIGNAL(timeout()),
            this,
            SLOT(displayNextIcon()));

    connect(m_repoInfoThread,
            SIGNAL(sendResult(const QList<QVariantList>&)),
            m_repoInfoModel,
            SLOT(slotResult(const QList<QVariantList>&)));

    connect(acAbout,
            SIGNAL(triggered()),
            this,
            SLOT(aboutPertubis()));

    connect(m_sysRep,
            SIGNAL(notifyAboutGLSA(QString,QString)),
            this,
            SLOT(addGlsa(QString,QString)));

    connect(m_reportPackageView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(displayGlsaTag( const QModelIndex& )) );

    connect(m_settings->m_generalView->m_model,
            SIGNAL(toolbarDisplayModeChanged(int)),
            this,
            SLOT(setToolbarDisplayMode(int)));

    connect(m_settings->m_generalView->m_model,
            SIGNAL(categoryDockChanged(int)),
            this,
            SLOT(setCatDock(int)));

    connect(m_settings->m_generalView->m_model,
            SIGNAL(setDockChanged(int)),
            this,
            SLOT(setSetDock(int)));

    connect(m_settings->m_generalView->m_model,
            SIGNAL(repositoryDockChanged(int)),
            this,
            SLOT(setRepositoryDock(int)));

    connect(m_settings->m_generalView->m_model,
            SIGNAL(glsaDockChanged(int)),
            this,
            SLOT(setGlsaDock(int)));


}

void pertubis::MainWindow::initLayout()
{
    setWindowTitle( tr("pertubis :: Main Window") );
    QPixmap logoIcon( ":images/logo.png" );
    setWindowIcon(logoIcon);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    show();
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
        m_currentCat = settings.value("currentCategory","").toString();
        QByteArray tmp(settings.value("mystate",true).toByteArray());
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
        settings.setValue("currentCategory", m_currentCat);
    settings.endGroup();
    qDebug() << "pertubis::MainWindow::saveSettings() - done";
}

void pertubis::MainWindow::displayCategoryChanged()
{
    statusBar()->showMessage(tr("%1 packages found").arg(m_packageFilterModel->rowCount()));
    onEndOfPaludisAction();
}

void pertubis::MainWindow::displayPackageDetails(QString details)
{
    if (details.isEmpty())
        return;

    m_details->setText(details);
    m_tabs->setCurrentIndex(m_detailsTabID);
    onEndOfPaludisAction();
}

void pertubis::MainWindow::displaySelectedPackages()
{
    statusBar()->showMessage(tr("%1 packages found").arg(m_packageFilterModel->rowCount()));
    onEndOfPaludisAction();
}

void pertubis::MainWindow::displayNextIcon()
{
    static int i=0;
    ++i;
    if (i>3)
        i=0;
    QString file(QString(":images/sync%1.png").arg(i));
    qDebug() << file;
    m_acSync->setIcon(QPixmap(file));
    m_sysTray->setIcon(QPixmap(file));
}

void pertubis::MainWindow::displaySyncFinished()
{
    onEndOfPaludisAction();
    m_timer->stop();
    m_acSync->setIcon( QPixmap(":images/sync0.png"));
    m_sysTray->setIcon( QPixmap(":images/logo.png"));
    QMessageBox q(QMessageBox::NoIcon,
                    tr("information"),
                    tr("syncing repositories finished"),
                    QMessageBox::Ok,
                    this);
    q.setIconPixmap(QPixmap(":images/repobar.png"));
    q.setDefaultButton(QMessageBox::Ok);
    q.exec();
}

void pertubis::MainWindow::displayGlsa(const QModelIndex& ix)
{
    CategoryItem* item(static_cast<CategoryItem*>(ix.internalPointer()));
    if (item == 0)
        return;
    QFile file(*item->repos().begin());
    if (! file.exists() )
        return;
    QXmlInputSource inputSource(&file);
    QXmlSimpleReader reader;
    GlsaParser handler(m_details);
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    reader.parse(inputSource);
    m_tabs->setCurrentIndex(m_detailsTabID);
}

void pertubis::MainWindow::displayGlsaTag(const QModelIndex& ix)
{
    ReportPackage* item(static_cast<ReportPackage*>(ix.internalPointer()));
    if (item == 0)
        return;
    if ( tr("vulnerable") != item->data(ReportPackage::ro_package).toString() )
        return;
    int res = m_glsaModel->indexOf(item->data(ReportPackage::ro_category).toString());
    if(res == -1)
        return;
    QFile file(*m_glsaModel->getItem(res)->repos().begin());
    if (! file.exists() )
        return;
    QXmlInputSource inputSource(&file);
    QXmlSimpleReader reader;
    GlsaParser handler(m_details);
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    reader.parse(inputSource);
    m_tabs->setCurrentIndex(m_detailsTabID);
}

void pertubis::MainWindow::displayOptionsMenu(const QModelIndex &)
{
//     QModelIndex ix;
//     QPoint mousePos;
//     if (m_packageView == m_mainWidget->currentWidget())
//     {
//         ix = m_packageFilterModel->mapToSource(index);
//         mousePos = m_packageView->mapToGlobal(m_packageView->m_mousePos);
//     }
//     else if (m_searchPackageView == m_mainWidget->currentWidget() )
//     {
//         ix = m_searchPackageFilterModel->mapToSource(index);
//         mousePos =  m_packageView->mapToGlobal(m_searchPackageView->m_mousePos);
//     }
//     else
//         return;
//     if (ix.column() != po_selected)
//         return;
//
//     m_options->clear();
//     m_current= static_cast<Package*>(ix.internalPointer());
//
//     for (QVector<Task*>::const_iterator tStart(m_box->taskBegin()),
//          tEnd(m_box->taskEnd());
//          tStart != tEnd; ++tStart )
//     {
//         if ((*tStart)->available(m_current) )
//         {
//             (*tStart)->fillAction(m_current);
//             m_options->addAction( (*tStart)->action() );
//         }
//     }
//
// //     m_options->addAction(m_acEditUse);
// //     m_options->addAction(m_acMasking);
//     m_options->popup(mousePos);
}

void pertubis::MainWindow::displaySearchFinished(int count)
{
    onEndOfPaludisAction();
    m_searchWindow->displaySearch(false);
    statusBar()->showMessage(tr("%1 packages found").arg(count));
}

void pertubis::MainWindow::displaySystemReportFinished(int total,int count)
{
    onEndOfPaludisAction();
    m_reportPackageView->expandAll();
    statusBar()->showMessage(tr("%1 installed packages processed, %2 issues found").arg(total).arg(count));
}

void pertubis::MainWindow::addGlsa(QString name, QString path)
{
    qDebug() << "pertubis::MainWindow::addGlsa()" << name << path;
    m_dockGlsa->show();
    m_glsaModel->appendCategory(new CategoryItem(name,QSet<QString>() << path));
}


void pertubis::MainWindow::setToolbarDisplayMode(int state)
{
    if (state == 0)
        setToolButtonStyle(Qt::ToolButtonIconOnly);
    else if (state == 1)
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

void pertubis::MainWindow::setCatDock(int state)
{
    removeDockWidget(m_dockCat);
    if (state == 0)
        addDockWidget(Qt::LeftDockWidgetArea,m_dockCat);
    else if (state == 1)
        addDockWidget(Qt::RightDockWidgetArea,m_dockCat);
    m_dockCat->show();
}

void pertubis::MainWindow::setSetDock(int state)
{
    removeDockWidget(m_dockSet);
    if (state == 0)
        addDockWidget(Qt::LeftDockWidgetArea,m_dockSet);
    else if (state == 1)
        addDockWidget(Qt::RightDockWidgetArea,m_dockSet);
    m_dockSet->show();
}

void pertubis::MainWindow::setGlsaDock(int state)
{
    removeDockWidget(m_dockGlsa);
    if (state == 0)
        addDockWidget(Qt::LeftDockWidgetArea,m_dockGlsa);
    else if (state == 1)
        addDockWidget(Qt::RightDockWidgetArea,m_dockGlsa);
    m_dockGlsa->show();
}

void pertubis::MainWindow::setRepositoryDock(int state)
{
    removeDockWidget(m_dockRepo);
    if (state == 0)
        addDockWidget(Qt::LeftDockWidgetArea,m_dockRepo);
    else if (state == 1)
        addDockWidget(Qt::RightDockWidgetArea,m_dockRepo);
    m_dockRepo->show();
}

void pertubis::MainWindow::onCategoryChanged( const QModelIndex& /*proxyIndex*/ )
{
    QModelIndex origIndex(m_categoryFilterModel->mapToSource(m_categoryView->currentIndex()));
    if ( !origIndex.isValid() || 0 != origIndex.column() || m_packageViewThread->isRunning())
        return;

    onStartOfPaludisAction();
    m_packageModel->clear();
    m_mainWidget->setCurrentWidget(m_packageView);
    m_currentCat = m_catModel->data(origIndex).toString();
    m_packageViewThread->start(m_currentCat);
}

void pertubis::MainWindow::onRepositoryChanged( const QModelIndex& index )
{
    if ( !index.isValid())
        return;

    if (0 == index.column())
    {
        int state = m_repoListModel->data(index,Qt::CheckStateRole).toInt();
        m_repoListModel->setData(index, (state == Qt::Checked ) ? Qt::Unchecked :Qt::Checked);
        restartFilters();
        statusBar()->showMessage(tr("%1 packages found").arg(m_packageFilterModel->rowCount()));
    }
    else
    {
        QString repo = m_repoListModel->data(index).toString();
        m_repoInfoThread->start(repo);
        m_tabs->setCurrentIndex(m_repoViewTabID);
    }
}

void pertubis::MainWindow::restartFilters()
{
    m_packageFilterModel->setFilter(m_repoListModel->activeRepositories());
    m_packageFilterModel->invalidate();
    m_searchPackageFilterModel->setFilter(m_repoListModel->activeRepositories());
    m_searchPackageFilterModel->invalidate();
    m_categoryFilterModel->setFilter(m_repoListModel->activeRepositories());
    m_categoryFilterModel->invalidate();
    m_setFilterModel->setFilter(m_repoListModel->activeRepositories());
    m_setFilterModel->invalidate();
}

void pertubis::MainWindow::onInstallSelections(bool mystate)
{
    qDebug() << "pertubis::MainWindow::onInstallSelections()";
    Qt::CheckState state(mystate ? Qt::Checked : Qt::Unchecked);
    if (m_packageView == m_mainWidget->currentWidget())
    {
        QModelIndex ix(m_packageFilterModel->mapToSource(m_packageView->currentIndex()));
        Package* package(static_cast<Package*>(ix.internalPointer()));
        if (package == 0)
            return;
        m_installTask->changeStates(package, state);
    }
    else if (m_searchPackageView == m_mainWidget->currentWidget() )
    {
        QModelIndex ix(m_searchPackageFilterModel->mapToSource(m_searchPackageView->currentIndex()));
        Package* package(static_cast<Package*>(ix.internalPointer()));
        if (package == 0)
            return;
        m_installTask->changeStates(package, state);
    }
}

void pertubis::MainWindow::onDeinstallSelections(bool mystate)
{
    qDebug() << "pertubis::MainWindow::onDeinstallSelections()";
    Qt::CheckState state(mystate ? Qt::Checked : Qt::Unchecked);
    if (m_packageView == m_mainWidget->currentWidget())
    {
        QModelIndex ix(m_packageFilterModel->mapToSource(m_packageView->currentIndex()));
        Package* package(static_cast<Package*>(ix.internalPointer()));
        if (package == 0)
            return;
        m_deinstallTask->changeStates(package, state);
    }
    else if (m_searchPackageView == m_mainWidget->currentWidget() )
    {
        QModelIndex ix(m_searchPackageFilterModel->mapToSource(m_searchPackageView->currentIndex()));
        Package* package(static_cast<Package*>(ix.internalPointer()));
        if (package == 0)
            return;
        m_deinstallTask->changeStates(package, state);
    }
}

void pertubis::MainWindow::onSystemReport()
{
    if (!rootTest(tr("This feature is only available for system administrators")))
        return;
    onStartOfPaludisAction();
    m_reportPackageModel->clear();
    m_glsaModel->clear();
    m_mainWidget->setCurrentWidget(m_reportPackageView);
    m_sysRep->start();
}

void pertubis::MainWindow::onUnselectAll()
{
    m_installTask->clear();
    m_deinstallTask->clear();
    if (m_selectionView == m_mainWidget->currentWidget() )
    {
        onShowSelections();
    }
}

void pertubis::MainWindow::onStartTasks()
{
    if (!rootTest(tr("This feature is only available for system administrators")))
        return;

    m_sysTray->showMessage(tr("pertubis"),tr("Installing %1, Deleting %2 Packages").arg(m_installTask->entryCount()).arg(m_deinstallTask->entryCount()));
    m_sysTray->setIcon(QPixmap(":images/run.png"));
    m_selectionModel->clear();
    m_output->clear();
    m_mainWidget->setCurrentWidget(m_selectionView);
    startDeinstallTask(false);
    startInstallTask(false,"");
    m_sysTray->setIcon(QPixmap(":images/logo.png"));
}

void pertubis::MainWindow::startSetInstallTask(const QModelIndex & ix)
{
    qDebug() << "pertubis::MainWindow::startInstallTask()";
    m_selectionModel->clear();
    m_mainWidget->setCurrentWidget(m_selectionView);
    m_output->clear();

    paludis::DepListOptions options;
    PertubisInstallTask mytask(this,m_env.get(),options,m_env->default_destinations(),m_installTask,m_deinstallTask);
    m_settings->m_installView->m_model->populate_install_task(m_env.get(),mytask);
    m_settings->m_depListView->populate_install_task(m_env.get(),mytask);
    mytask.set_pretend(true);

    connect(&mytask,
            SIGNAL(sendMessage(QString)),
            m_output,
            SLOT(append(QString)));

    connect(&mytask,
            SIGNAL(appendPackage(Package*)),
            m_selectionModel,
            SLOT(appendPackage(Package*)));

    connect(&mytask,
            SIGNAL(depListResult(QString)),
            this,
            SLOT(depListResult(QString)));

    try
    {
        mytask.add_target(m_setModel->data(ix).toString().toStdString());
        mytask.execute();
    }
    catch(const paludis::NoSuchPackageError& e)
    {
        qDebug() << "no such package";
    }

    qDebug() << "pertubis::MainWindow::startInstallTask() - done";

}

void pertubis::MainWindow::startInstallTask(bool pretend, QString target)
{
    qDebug() << "pertubis::MainWindow::startInstallTask()";

    paludis::DepListOptions options;
    PertubisInstallTask mytask(this,m_env.get(),options,m_env->default_destinations(),m_installTask,m_deinstallTask);
    m_settings->m_installView->m_model->populate_install_task(m_env.get(),mytask);
    m_settings->m_depListView->populate_install_task(m_env.get(),mytask);
    mytask.set_pretend(pretend);

    connect(&mytask,
            SIGNAL(sendMessage(QString)),
            m_output,
            SLOT(append(QString)));

    connect(&mytask,
            SIGNAL(finished()),
            this,
            SIGNAL(finished()));

    connect(&mytask,
            SIGNAL(appendPackage(Package*)),
            m_selectionModel,
            SLOT(appendPackage(Package*)));

    connect(&mytask,
            SIGNAL(depListResult(QString)),
            this,
            SLOT(depListResult(QString)));

    if (target.isEmpty() && m_installTask->entryCount() > 0)
    {
        for (paludis::PackageIDSet::ConstIterator i(m_installTask->entriesBegin()), i_end(m_installTask->entriesEnd());
            i != i_end ; ++i)
        {
            mytask.add_exact_package(*i);
        }
        mytask.execute();
    }
    else
    {
        try
        {
            mytask.add_target(target.toStdString());
            mytask.execute();
        }
        catch(...)
        {
        }
    }
    qDebug() << "pertubis::MainWindow::startInstallTask() - done";
}

void pertubis::MainWindow::startDeinstallTask(bool pretend)
{
    qDebug() << "pertubis::MainWindow::startDeinstallTask() - start";

    PertubisDeinstallTask mytask(this,m_env);
    mytask.set_pretend(m_settings->m_installView->m_model->m_pretend);
    mytask.set_no_config_protect(m_settings->m_installView->m_model->m_config);
    mytask.set_preserve_world(m_settings->m_installView->m_model->m_preserve);
    mytask.set_with_unused_dependencies(m_settings->m_deinstallView->m_model->m_unusedDeps);
    mytask.set_with_dependencies(m_settings->m_deinstallView->m_model->m_deps);
    mytask.set_check_safety(! m_settings->m_deinstallView->m_model->m_unsafeUninstall);
    mytask.set_all_versions( m_settings->m_deinstallView->m_model->m_allVersions);
    mytask.set_pretend(pretend);

    connect(&mytask,
            SIGNAL(message(QString)),
            m_output,
            SLOT(append(QString)));

    if (m_deinstallTask->entryCount() > 0)
    {
        for (paludis::PackageIDSet::ConstIterator i(m_deinstallTask->entriesBegin()), i_end(m_deinstallTask->entriesEnd());
             i != i_end ; ++i)
        {
            std::string target("=" + stringify((*i)->name()) + "-" + (*i)->canonical_form(paludis::idcf_version));
            mytask.add_target(target);
        }
        mytask.execute();
    }
    qDebug() << "pertubis::MainWindow::startDeinstallTask() - done";
}

void pertubis::MainWindow::depListResult(QString text)
{
    statusBar()->showMessage(text);
}

void pertubis::MainWindow::onDetailsChanged(const QModelIndex & index)
{
    QModelIndex ix;
    if (m_packageView == m_mainWidget->currentWidget())
        ix = m_packageFilterModel->mapToSource(index);
    else if (m_searchPackageView == m_mainWidget->currentWidget() )
        ix = m_searchPackageFilterModel->mapToSource(index);
    else
        return;
    Package* item = static_cast<Package*>(ix.internalPointer());

    if (!ix.isValid() ||
         m_detailsThread->isRunning() ||
         ix.column() != po_package)
        return;
    onStartOfPaludisAction();
    m_detailsThread->start(item->ID());
    m_tabs->setCurrentIndex(m_detailsTabID);
}

void pertubis::MainWindow::onSync()
{
    if (!rootTest(tr("This feature is only available for system administrators")))
        return;

    if (m_syncTask->isRunning())
    {
        QMessageBox::warning(this,
                    tr("Request denied"),
                    "You are already syncing the repositories",
                    QMessageBox::Ok,
                    QMessageBox::Ok);
        return;
    }

    QSet<QString> repos(m_repoListModel->activeRepositories());
    QString text;
    for (QSet<QString>::const_iterator sStart(repos.constBegin()),sEnd(repos.constEnd());
        sStart != sEnd;++sStart)
    {
        text.append(QString("* %1\n").arg(*sStart));
    }
    QMessageBox q(QMessageBox::NoIcon,
                        tr("Question"),
                        tr("Do you really want syncing the following repositories?\n\n%1").arg(text),
                        QMessageBox::Ok | QMessageBox::Cancel,
                        this);
    q.setIconPixmap(QPixmap(":images/repoba.png"));
    q.setDefaultButton(QMessageBox::Ok);
    if (QMessageBox::Cancel == q.exec())
        return;
    m_timer->start(1000);
    onStartOfPaludisAction();
    m_syncTask->start(repos);
}

void pertubis::MainWindow::onStartOfPaludisAction()
{
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
}

void pertubis::MainWindow::onEndOfPaludisAction()
{
    QApplication::restoreOverrideCursor();
}

void pertubis::MainWindow::onShowSelections()
{
    onStartOfPaludisAction();
    m_selectionModel->clear();
    m_output->clear();
    m_mainWidget->setCurrentWidget(m_selectionView);
    startDeinstallTask(true);
    startInstallTask(true,"");
    onEndOfPaludisAction();
}

void pertubis::MainWindow::onSearch(QString query)
{
    onStartOfPaludisAction();
    m_searchPackageModel->clear();
    statusBar()->showMessage(QString(tr("searching for %1...")).arg(query) );
}

void pertubis::MainWindow::onSearchStopped()
{
    statusBar()->showMessage(tr("Search stopped"));
    onEndOfPaludisAction();
}

void pertubis::MainWindow::onOpenURL(const QUrl& url)
{
    QString program = "/usr/bin/xdg-open";
    QStringList arguments;
    arguments << url.toString();
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
}

void pertubis::MainWindow::onQuit()
{
    QApplication::instance()->quit();
}

void pertubis::MainWindow::togglePackageView()
{
    m_packageView->setVisible(!m_packageView->isVisible());
}

void pertubis::MainWindow::toggleSearchWindow()
{
    if (!m_searchWindow)
        return;
    m_searchWindow->show();
    m_searchWindow->raise();
    m_searchWindow->activateWindow();
    m_mainWidget->setCurrentWidget(m_searchPackageView);
}

void pertubis::MainWindow::toggleTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            toggleMainWindow();
            break;
        case QSystemTrayIcon::Context:
            m_trayMenu->show();
            break;
        default:
            ;
    }
}

void pertubis::MainWindow::toggleMainWindow()
{
    setVisible(!isVisible());
}

void pertubis::MainWindow::toggleSettings()
{
    m_settings->exec();
}

void pertubis::MainWindow::aboutPertubis()
{
    QVBoxLayout* mylayout(new QVBoxLayout());
    QTextBrowser* label(new QTextBrowser(this));
    label->setText(tr(
"<html><body><table>"
"<colgroup><col width=\"40%\"><col width=\"60%\">\n</colgroup>\n"
"<tbody>"
"<tr><td>version</td><td>%1.%2.%3</td><td></tr>"
"<tr><td>Revision</td><td>%4</td><td></tr>"
"<tr><td>author</td><td>©2007 by Stefan Kögl</td></tr>"
"<tr><td>homepage</td><td><a ref=\"http://pertubis.berlios.de\">http://pertubis.berlios.de/</a></td></tr>"
"</tbody>"
"</table></body></html>").arg(MAJOR_VERSION_NUMBER)
            .arg(MINOR_VERSION_NUMBER)
            .arg(PATCH_VERSION_NUMBER)
            .arg(PERTUBIS_REVISION_NUMBER));
    label->setOpenLinks(true);
    mylayout->addWidget(label);
    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Vertical);
    QPushButton* bClose = buttonBox->addButton(QDialogButtonBox::Close);
    bClose->setAutoDefault(false);
    mylayout->addWidget(bClose);


    QDialog about(this);
    about.setWindowTitle(tr("pertubis - an interactive frontend for paludis"));
    about.setLayout(mylayout);
    connect(bClose,
            SIGNAL(clicked()),
            &about,
            SLOT(close()));
    about.exec();
}
