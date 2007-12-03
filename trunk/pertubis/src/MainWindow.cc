
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
#include "CategoryThread.hh"
#include "CategoryModel.hh"
#include "MainWindow.hh"
#include "DeinstallTask.hh"
#include "DetailsThread.hh"
#include "FormatterUtils.hh"
#include "InstallTask.hh"
#include "Item.hh"
#include "MessageOutput.hh"
#include "OptionsDelegate.hh"
#include "PackageFilterModel.hh"
#include "PackageModel.hh"
#include "PackagesThread.hh"
#include "RepositoryListModel.hh"
#include "SearchThread.hh"
#include "SearchWindow.hh"
#include "SetModel.hh"
#include "SetThread.hh"
#include "Settings.hh"
#include "ShowSelectionsThread.hh"
#include "SyncTask.hh"
#include "TaskBox.hh"

#include <paludis/args/install_args_group.hh>
#include <paludis/environment_maker.hh>
#include <paludis/name.hh>
#include <paludis/package_id-fwd.hh>
#include <paludis/package_id.hh>
#include <paludis/util/set-fwd.hh>
#include <paludis/util/set.hh>
#include <paludis/util/set-impl.hh>
#include <paludis/util/stringify.hh>

#include <QApplication>
#include <QCheckBox>
#include <QCursor>
#include <QDebug>
#include <QDockWidget>
#include <QFile>
#include <QFont>
#include <QHeaderView>
#include <QLayout>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPixmap>
#include <QProcess>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QTableView>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBar>
#include <QUrl>

#include <iostream>

static bool rootTest(const QString& message)
{
    if (0 != getuid() )
    {
        if (!message.isEmpty())
            QMessageBox::warning(0,
                                 QObject::tr("unpriviledged mode"),
                                             message,
                                             QMessageBox::Ok,
                                             QMessageBox::Ok);
        else
            QMessageBox::warning(0,
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

pertubis::MainWindow::MainWindow() :
    m_categoryThread(0),
    m_categoryFilterModel(0),
    m_catModel(0),
    m_detailsThread(0),
    m_current(0),
    m_output(0),
    m_syncTask(0),
    m_packageModel(0),
    m_packageFilterModel(0),
    m_packageViewThread(0),
    m_packageView(0),
    m_repoListView(0),
    m_repoListModel(0),
    m_repoListThread(0),
    m_searchThread(0),
    m_searchWindow(0),
    m_settings(0),
    m_selectionsThread(0),
    m_box(0)
{
    initGUI();
    initLayout();
    QTimer::singleShot(0,this,SLOT(initObject()));
}

pertubis::MainWindow::~MainWindow()
{
    saveSettings();
}

void pertubis::MainWindow::initObject()
{
    m_repoListThread->start();
    m_categoryThread->start();
    m_setThread->start();
    rootTest("");
}

void pertubis::MainWindow::initGUI()
{
    m_env = paludis::EnvironmentMaker::get_instance()->make_from_spec("");
    qRegisterMetaType<QList<RepositoryListItem*> >("QList<RepositoryListItem*>");
    qRegisterMetaType<QList<QVariantList> >("QList<QVariantList>");
    qRegisterMetaType<QMap<QString, QSet<QString> > >("QMap<QString, QSet<QString> >");
    createActions(); // no deps
    createTab(); // no deps
    createTaskBox(); // no deps
    createTasks(); // createTaskBox()
    createRepositoryBar(); // no deps
    createCatbar(); // createRepositoryBar()
    createSetListView(); // no deps
    createPackageView(); // createRepositoryBar()
    createToolBar(); // createActions()
    createSettings(); // no deps
    createOptionsMenu(); // no deps
    createWindowSearch();
    createTrayMenu();
    statusBar()->show();
}

void pertubis::MainWindow::initLayout()
{
    m_vSplit = new QSplitter(Qt::Vertical, this);
    m_vSplit->addWidget(m_packageView);
    m_vSplit->addWidget(m_tabs);
    m_vSplit->setStretchFactor(0,10);
    m_vSplit->setStretchFactor(1,10);
    setWindowTitle( tr("pertubis :: Main Window") );
    QPixmap logoIcon( ":images/logo.xpm" );
    setWindowIcon(logoIcon);
    setCentralWidget(m_vSplit);
    addDockWidget(Qt::LeftDockWidgetArea, m_dockCat);
    addDockWidget(Qt::LeftDockWidgetArea, m_dockRepo);
    addDockWidget(Qt::LeftDockWidgetArea, m_dockSet);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    show();
    ReleaseEater* filter = new ReleaseEater(this);
    installEventFilter(filter);
    m_searchThread = new SearchThread(this,m_env,m_box);
    m_categoryThread = new CategoryThread(this,m_env,m_box);
    m_packageViewThread = new PackagesThread(this,m_env,m_box);
    m_detailsThread = new DetailsThread(this,m_env,m_box);
    m_selectionsThread = new ShowSelectionsThread(this,m_env,m_box);
    m_setThread = new SetThread(this,m_env,m_box);
    m_repoListThread = new RepositoryListThread(this,m_env,m_box);
//     m_repoInfoThread = new RepositoryInfoThread(this,this);
    m_syncTask = new PertubisSyncTask(m_env,this);
    createConnections();
    loadSettings();
}

void pertubis::MainWindow::closeEvent(QCloseEvent* ev)
{
    hide();
    ev->ignore();
}

void pertubis::MainWindow::createTaskBox()
{
    // no deps
    m_box = new TaskBox(this);
}

void pertubis::MainWindow::createCatbar()
{
    // deps:
    // - createRepositoryBar()
    //

    m_catModel = new CategoryModel(this);
    m_catModel->setHorizontalHeaderLabels(QStringList() << tr("category"));

    m_categoryFilterModel = new CategoryFilterModel(this);
    m_categoryFilterModel->setSourceModel(m_catModel);

    m_categories = new QTableView(this);
    m_categories->horizontalHeader()->setVisible(false);
    m_categories->verticalHeader()->setVisible(false);
    m_categories->setModel(m_categoryFilterModel);
    m_categories->setShowGrid(false);
    m_categories->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    QFont myfont(m_categories->font());
    myfont.setBold(true);
    m_categories->setFont(myfont);

    m_dockCat = new QDockWidget(tr("category list"),this);
    m_dockCat->layout()->setMargin(0);
    m_dockCat->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    m_dockCat->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_dockCat->setWidget(m_categories);

    m_acToggleCatBar = m_dockCat->toggleViewAction();
    m_acToggleCatBar->setText(tr("category list"));
    m_acToggleCatBar->setIcon(QPixmap(":images/catbar_22.xpm"));
    m_acToggleCatBar->setShortcut(tr("F11"));
    m_acToggleCatBar->setToolTip(html_tooltip(tr("enable/disable the category sidebar"),m_acToggleCatBar->text()) );
}

void pertubis::MainWindow::createSettings()
{
    qDebug() << "pertubis::MainWindow::createSettings()";
    m_settings = new Settings(this);
}

void pertubis::MainWindow::createPackageView()
{
    // deps
    // - createRepositoryBar()
    m_packageModel = new PackageModel(this);
    m_packageModel->setBox(m_box);
    m_packageModel->setHorizontalHeaderLabels(QStringList() <<
        tr("marked") <<
        tr("package") <<
        tr("category") <<
        tr("repository") <<
        tr("installed") <<
        tr("masked by") <<
        tr("change"));

    m_packageView = new PackageView(this);
    m_packageFilterModel = new PackageFilterModel(this);
    m_packageFilterModel->setSourceModel(m_packageModel);
    m_packageView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_packageView->setItemDelegateForColumn(Item::io_selected,new OptionsDelegate(this,m_packageFilterModel));
    m_packageView->setModel(m_packageFilterModel);
    m_packageView->header()->setVisible(true);
    m_packageView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_packageView->setSelectionBehavior(QAbstractItemView::SelectRows);
    QFont myfont(m_packageView->font());
    myfont.setBold(true);
    m_packageView->setFont(myfont);
}

void pertubis::MainWindow::createToolBar()
{
    // deps
    // - createActions()
    m_toolBar = addToolBar( tr("database toolbar") );
    m_toolBar->addAction(m_acSync);
    m_toolBar->addAction(m_acToggleCatBar);
    m_toolBar->addAction(m_acToggleRepoBar);
    m_toolBar->addAction(m_acToggleSetBar);
    m_toolBar->addAction(m_acTogglePackageView);
    m_toolBar->addAction(m_acToggleSearchWindow);
    m_toolBar->addAction(m_acSelection);
    m_toolBar->addAction(m_acFinish);
    m_toolBar->addAction(m_acPref);
}

void pertubis::MainWindow::createOutput()
{
    m_output = new MessageOutput(this);
}

void pertubis::MainWindow::createRepositoryBar()
{
    m_repoListModel = new RepositoryListModel(this);
    m_repoListModel->setHorizontalHeaderLabels(QStringList() <<
            tr("Repository name") );

    m_repoListView = new QTableView(this);
    m_repoListView->setModel(m_repoListModel);
    m_repoListView->horizontalHeader()->setVisible(false);
    m_repoListView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    QFont myfont(m_repoListView->font());
    myfont.setBold(true);
    m_repoListView->setFont(myfont);

    m_repoListView->verticalHeader()->setVisible(false);
    m_repoListView->setShowGrid(false);

    m_dockRepo = new QDockWidget(tr("repository list"),this);
    m_dockRepo->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    m_dockRepo->layout()->setMargin(0);
    m_dockRepo->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_dockRepo->setWidget(m_repoListView);
    m_dockRepo->setToolTip(html_tooltip(tr("Here you can select the repositories pertubis will use.</p><p>Click on a repository to enable/disable filtering by this repository")));

    m_acToggleRepoBar = m_dockRepo->toggleViewAction();
    m_acToggleRepoBar->setText(tr("repository list"));
    m_acToggleRepoBar->setIcon(QPixmap(":images/repobar_22.xpm"));
    m_acToggleRepoBar->setShortcut( tr("F10"));
    m_acToggleRepoBar->setToolTip( html_tooltip(tr("enable/disable the repository sidebar"),m_acToggleRepoBar->text()) );
}

void pertubis::MainWindow::createSetListView()
{
    // deps:
    // - createRepositoryBar()
    //

    m_setModel = new SetModel(this);
    m_setModel->setHorizontalHeaderLabels(QStringList() << tr("set"));

    m_setFilterModel = new CategoryFilterModel(this);
    m_setFilterModel->setSourceModel(m_setModel);

    m_setListView = new QTableView(this);
    m_setListView->setModel(m_setFilterModel);
    m_setListView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_setListView->horizontalHeader()->setVisible(false);
    m_setListView->verticalHeader()->setVisible(false);
    m_setListView->setShowGrid(false);
    QFont myfont(m_setListView->font());
    myfont.setBold(true);
    m_setListView->setFont(myfont);

    m_dockSet = new QDockWidget(tr("set list"),this);
    m_dockSet->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    m_dockSet->setWidget(m_setListView);
    m_dockSet->layout()->setMargin(0);
    m_dockSet->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    m_acToggleSetBar = m_dockSet->toggleViewAction();
    m_acToggleSetBar->setText(tr("set list"));
    m_acToggleSetBar->setIcon(QPixmap(":images/setbar_22.xpm"));
    m_acToggleSetBar->setShortcut(tr("F12"));
    m_acToggleSetBar->setToolTip(html_tooltip(tr("enable/disable the set sidebar"),m_acToggleSetBar->text()) );
}

// void pertubis::MainWindow::createRepositoryView()
// {
//     qDebug() << "pertubis::MainWindow::createRepositoryView()";
//     m_repoInfoModel = new RepositoryInfoModel(this);
//     m_repoInfoModel->setHorizontalHeaderLabels(QStringList() <<
//             tr("name") <<
//             tr("state"));
//
//     m_repoInfoView = new QTableView(this);
//     m_repoInfoView->setModel(m_repoInfoModel);
//     m_repoInfoView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
//     m_repoInfoView->horizontalHeader()->setVisible(true);
//     m_repoInfoView->verticalHeader()->setVisible(false);
//
//     QPalette p(m_repoInfoView->palette());
//     p.setColor(QPalette::Base,QColor(170,170,170)); // background color  = black
//     m_repoInfoView->setPalette(p);
// }

void pertubis::MainWindow::createDetails()
{
    m_details = new QTextBrowser(this);
    QPalette p(m_details->palette());
    p.setColor(QPalette::Base,QColor(170,170,170)); // background color  = black
    m_details->setPalette(p);
    m_details->setOpenLinks(false);
}

void pertubis::MainWindow::createTab()
{
    m_tabs = new QTabWidget(this);
    createDetails();
//     createRepositoryView();
    createOutput();
    m_detailsTabID = m_tabs->addTab(m_details,tr("Package Details") );
//     m_repoViewTabID = m_tabs->addTab(m_repoInfoView,tr("repository details") );
    m_outputTabID = m_tabs->addTab(m_output,tr("Messages"));
}

void pertubis::MainWindow::createActions()
{
    m_acToggleMainWindow = new QAction( QPixmap(":images/logo.xpm"),tr("main window"),this );
    m_acToggleMainWindow->setCheckable(true);
    m_acToggleMainWindow->setChecked(true);
    m_acToggleMainWindow->setToolTip( html_tooltip(tr("show/hide the pertubis main window"),m_acToggleMainWindow->text() ));

    m_acTogglePackageView = new QAction( QPixmap(":images/packages_22.xpm"),tr("packages"),this );
    m_acTogglePackageView->setCheckable(true);
    m_acTogglePackageView->setChecked(true);
    m_acTogglePackageView->setToolTip( html_tooltip(tr("enable/disable the package window in the middle"),m_acTogglePackageView->text()));

    m_acToggleSearchWindow = new QAction( QPixmap(":images/find_22.xpm"),tr("find") ,this);
    m_acToggleSearchWindow->setShortcut( tr("CTRL+F"));
    m_acToggleSearchWindow->setToolTip(html_tooltip( tr("toggle search window"),m_acToggleSearchWindow->text() ) );

    m_acPref = new QAction( QPixmap(":images/settings_22.xpm"),tr( "Settings"),this);
    m_acPref->setToolTip(html_tooltip(tr("configure pertubis"),m_acPref->text()));

    m_acSelection = new QAction( QPixmap(":images/selections_22.xpm"),tr("selections") ,this);
    m_acSelection->setShortcut( tr("CTRL+F11"));
    m_acSelection->setToolTip( html_tooltip(tr("here you check the selections you made"),m_acSelection->text()) );

    m_acFinish = new QAction( QPixmap(":images/finish_22.xpm"),tr("start") ,this);
    m_acFinish->setShortcut( tr("CTRL+F12"));
    m_acFinish->setToolTip(html_tooltip( tr("starts all pending tasks you selected"),m_acFinish->text()) );

    m_acSync = new QAction( QPixmap(":images/sync_22.xpm"), tr("sync"),this);
    m_acSync->setToolTip(html_tooltip(tr("To get the latest releases and bugfixes it is neccessary to update the package database.<br><br>It is sufficient to sync your repositories once a day"),m_acSync->text()));

    m_acQuit = new QAction( QPixmap(":images/quit_22.xpm"),tr("quit") ,this);
    m_acQuit->setToolTip(html_tooltip(tr("closing the pertubis suite. All unsaved changes will be lost!"),m_acQuit->text()));

    m_acInstall= new QAction(tr("install"),this);
    m_acInstall->setToolTip( html_tooltip(tr("install this package")));
    m_acInstall->setCheckable(true);

    m_acDeinstall= new QAction(tr("deinstall"),this);
    m_acDeinstall->setToolTip(html_tooltip( tr("deinstall this package") ));
    m_acDeinstall->setCheckable(true);

    m_acEditUse = new QAction(tr("edit useflags"),this);
    m_acEditUse->setToolTip(html_tooltip( tr("opens a useflag editor for this item") ));

    m_acMasking = new QAction(tr("(un)mask"),this);
    m_acMasking->setToolTip(html_tooltip(tr("toggle the masking for this package") ));
}

void pertubis::MainWindow::createConnections()
{
    connect(m_acDeinstall,
            SIGNAL(toggled(bool)),
            this,
            SLOT(onDeinstallTask(bool)));

    connect(m_acFinish,
            SIGNAL(triggered()),
            this,
            SLOT(onStartTasks()));

    connect(m_acInstall,
            SIGNAL(toggled(bool)),
            this,
            SLOT(onInstallTask(bool)));

    connect(m_acSync,
            SIGNAL(triggered()),
            this,
            SLOT(onSync()));

    connect(m_acSelection,
            SIGNAL(triggered()),
            this,
            SLOT(displaySelectedPackages()));

    connect(m_acPref,
            SIGNAL(triggered()),
            this,
            SLOT(toggleSettings()));

    connect(m_acQuit,
            SIGNAL(triggered()),
            qApp,
            SLOT(quit()));

    connect(m_acToggleMainWindow,
            SIGNAL(triggered()),
            this,
            SLOT(toggleMainWindow()));

    connect(m_acTogglePackageView,
            SIGNAL(triggered()),
            this,
            SLOT(togglePackageView()));

    connect(m_acToggleSearchWindow,
            SIGNAL(triggered()),
            this,
            SLOT(toggleSearchWindow()));

    connect(m_categories,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onCategoryChanged( const QModelIndex& )) );

    connect(m_categoryThread,
            SIGNAL(sendCategory(QMap<QString, QSet<QString> >)),
            m_catModel,
            SLOT(slotAppendCategory(QMap<QString, QSet<QString> >)));

    connect(m_details,
            SIGNAL(anchorClicked(const QUrl&)),
            this,
            SLOT(onOpenURL(const QUrl&)));

    connect(m_detailsThread,
            SIGNAL(detailsResult(QString)),
            this,
            SLOT(displayPackageDetails(QString)));

    connect(m_packageViewThread,
            SIGNAL(addPackage(Item*)),
            m_packageModel,
            SLOT(slotPrependPackage(Item*)));

    connect(m_packageViewThread,
            SIGNAL(finished()),
            this,
            SLOT(displayResultCount()));

    connect(m_packageViewThread,
            SIGNAL(changeInCat(QString)),
            m_catModel,
            SLOT(slotChangeInCat(QString)));

    connect(m_searchThread,
            SIGNAL(changeInCat(QString)),
            m_catModel,
            SLOT(slotChangeInCat(QString)));

    connect(m_packageView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onDetailsChanged( const QModelIndex& )) );

    connect(m_packageView,
            SIGNAL(clicked(const QModelIndex&)),
            this,
            SLOT(displayOptionsMenu(const QModelIndex&)));

    connect(m_searchThread,
            SIGNAL(itemResult(Item*)),
            m_packageModel,
            SLOT(slotAppendPackage(Item*)));

    connect(m_searchWindow,
            SIGNAL(stopSearch()),
            this,
            SLOT(onSearchStopped()));

    connect(m_searchThread,
            SIGNAL(finished(int)),
            this,
            SLOT(displayResultCount()));

    connect(m_selectionsThread,
            SIGNAL(appendPackage(Item*)),
            m_packageModel,
            SLOT(slotAppendPackage(Item*)));

    connect(m_selectionsThread,
            SIGNAL(finished()),
            this,
            SLOT(displayResultCount()));

    connect(m_setThread,
            SIGNAL(sendSet(QMap<QString, QSet<QString> >)),
            m_setModel,
            SLOT(slotAppendSet(QMap<QString, QSet<QString> >)));

    connect(m_repoListThread,
            SIGNAL(finished()),
            this,
            SLOT(onReposChanged()));

    connect(m_repoListThread,
            SIGNAL(sendNames(QList<RepositoryListItem*>)),
            m_repoListModel,
            SLOT(slotResult(QList<RepositoryListItem*>)));

    connect(m_repoListView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onRepositoryChanged( const QModelIndex& )) );

    connect(m_repoListView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(displayResultCount()) );

    connect(m_searchWindow,
            SIGNAL(search()),
            this,
            SLOT(onSearch()));

    connect(m_sysTray,
            SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,
            SLOT(toggleTrayIcon(QSystemTrayIcon::ActivationReason)));

    connect(m_syncTask,
            SIGNAL(sendMessage(QString)),
            m_output,
            SLOT(append(QString)));
}

void pertubis::MainWindow::createTasks()
{
    // deps
    // - createTaskBox()
    InstallTask* tmp(new InstallTask( this,m_acInstall,tr("install")));
    connect(tmp,
            SIGNAL(finished()),
            this,
            SLOT(onTasksFinished()));
    m_tidInstall = m_box->addTask(tmp);
    m_tidDeinstall = m_box->addTask(new DeinstallTask( this,m_acDeinstall,tr("deinstall")));
}

void pertubis::MainWindow::createWindowSearch()
{
    m_searchWindow = new SearchWindow(this);
}

void pertubis::MainWindow::createOptionsMenu()
{
    m_options  = new QMenu(tr("Package Options"),this);
}

void pertubis::MainWindow::createTrayMenu()
{
    qDebug() << "pertubis::MainWindow::createTrayMenu()";
    m_trayMenu = new QMenu(this);
    m_trayMenu->addAction(m_acToggleMainWindow);
    m_trayMenu->addAction(m_acPref);
    m_trayMenu->addAction(m_acQuit);

    m_sysTray = new QSystemTrayIcon(QPixmap(":images/logo.xpm"),this);
    m_sysTray->setContextMenu(m_trayMenu);
    m_sysTray->show();
}

void pertubis::MainWindow::loadSettings()
{
    QSettings settings;
    settings.beginGroup( "MainWindow" );
        setVisible(settings.value("visible",true).toBool());
        resize(settings.value("size",QVariant(QSize(800,600))).toSize());
        move(settings.value("pos",QVariant(QPoint(341,21))).toPoint());

        m_acToggleCatBar->setChecked(settings.value("catbar_visible",true ).toBool()  );
        m_acToggleRepoBar->setChecked(settings.value("repobar_visible",true ).toBool()  );

        m_acTogglePackageView->setChecked(settings.value("packageview_visible",true).toBool() );
        m_vSplit->restoreState(settings.value("vsplt").toByteArray());

        m_dockCat->setVisible(m_acToggleCatBar->isChecked());
        m_dockRepo->setVisible(m_acToggleRepoBar->isChecked());
        m_vSplit->setVisible(m_acTogglePackageView->isChecked());
    settings.endGroup();
}

void pertubis::MainWindow::saveSettings()
{
    QSettings settings;
    settings.beginGroup( "MainWindow" );
        settings.setValue("visible", isVisible() );
        settings.setValue("size", size() );
        settings.setValue("pos", pos());
        settings.setValue("catbar_visible", m_acToggleCatBar->isChecked());
        settings.setValue("repobar_visible", m_acToggleRepoBar->isChecked());
        settings.setValue("packageview_visible", m_acTogglePackageView->isChecked());
        settings.setValue("vsplt", m_vSplit->saveState());
    settings.endGroup();
    qDebug() << "pertubis::MainWindow::saveSettings() - done";
}

void pertubis::MainWindow::displayResultCount()
{
    statusBar()->showMessage(tr("%1 packages found").arg(m_packageFilterModel->rowCount()));
    QApplication::restoreOverrideCursor();
}

void pertubis::MainWindow::displayPackageDetails(QString details)
{
    if (details.isEmpty())
        return;
    m_details->setText(details);
    m_tabs->setCurrentIndex(m_detailsTabID);
}

void pertubis::MainWindow::displaySelectedPackages()
{
    m_packageModel->slotClear();
    m_selectionsThread->start();
}

void pertubis::MainWindow::displayOptionsMenu(const QModelIndex& mix)
{
    QModelIndex index(m_packageFilterModel->mapToSource(mix));
    if (index.column() != Item::io_selected)
        return;

    m_options->clear();
    m_current= static_cast<Item*>(index.internalPointer());

    for (QVector<Task*>::const_iterator tStart(m_box->taskBegin()),
         tEnd(m_box->taskEnd());
         tStart != tEnd; ++tStart )
    {
        if ((*tStart)->available(m_current) )
        {
            (*tStart)->fillAction(m_current);
            m_options->addAction( (*tStart)->action() );
        }
    }

    m_options->addAction(m_acEditUse);
    m_options->addAction(m_acMasking);
    m_options->popup(m_packageView->mapToGlobal(m_packageView->m_mousePos));
}

void pertubis::MainWindow::onCategoryChanged( const QModelIndex& /*proxyIndex*/ )
{
    QModelIndex origIndex(m_categoryFilterModel->mapToSource(m_categories->currentIndex()));
    if ( !origIndex.isValid() || origIndex.column() != 0 || m_packageViewThread->isRunning())
        return;
    QString cat = m_catModel->data(origIndex).toString();
    qDebug() << "pertubis::MainWindow::slotCategoryChanged()" << cat;
    m_packageModel->slotClear();
    m_packageViewThread->start(cat);
}

void pertubis::MainWindow::onRepositoryChanged( const QModelIndex& index )
{
    if ( !index.isValid())
        return;

    int state = m_repoListModel->data(index,Qt::CheckStateRole).toInt();
    m_repoListModel->setData(index, (state == Qt::Checked ) ? Qt::Unchecked :Qt::Checked);
    onReposChanged();
}

void pertubis::MainWindow::onReposChanged()
{
    m_packageFilterModel->setFilter(m_repoListModel->activeRepositories());
    m_packageFilterModel->invalidate();
    m_categoryFilterModel->setFilter(m_repoListModel->activeRepositories());
    m_categoryFilterModel->invalidate();
    m_setFilterModel->setFilter(m_repoListModel->activeRepositories());
    m_setFilterModel->invalidate();
}

void pertubis::MainWindow::onInstallTask(bool mystate)
{
    QModelIndex origIndex(m_packageFilterModel->mapToSource(m_packageView->currentIndex()));
    m_packageModel->setSelectionData(origIndex,m_tidInstall,mystate);
}

void pertubis::MainWindow::onDeinstallTask(bool mystate)
{
    QModelIndex index(m_packageFilterModel->mapToSource(m_packageView->currentIndex()));
    m_packageModel->setSelectionData(index,m_tidDeinstall,mystate);
}

void pertubis::MainWindow::onStartTasks()
{
    if (rootTest(tr("This feature is only available for system administrators")))
    {
        m_sysTray->showMessage(tr("pertubis"),tr("Installing %1, Deleting %2 Items").arg(m_box->task(m_tidInstall)->itemCount()).arg(m_box->task(m_tidDeinstall)->itemCount()));
        toggleMainWindow();
        m_box->startAllTasks(m_env,m_output);
    }
}

void pertubis::MainWindow::onTasksFinished()
{
    m_box->slotClear();
    onCategoryChanged( QModelIndex());
    toggleMainWindow();
}

void pertubis::MainWindow::onDetailsChanged(const QModelIndex & index)
{
    QModelIndex ix = m_packageFilterModel->mapToSource(index);
    Item* item = static_cast<Item*>(ix.internalPointer());

    if (!ix.isValid() ||
         m_detailsThread->isRunning() ||
         ix.column() != Item::io_package)
        return;
    m_detailsThread->start(item->ID());
}

void pertubis::MainWindow::onSync()
{
    if (getuid() != 0 )
    {
        QMessageBox::critical(this,
                            tr("authentification error"),
                            tr("You must be root for syncing repositories"));
        return;
    }

    paludis::Context context("When performing sync action from command line:");
    m_syncTask->start();
}

void pertubis::MainWindow::onSearch()
{
    if (m_searchWindow->query().isEmpty())
    {
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    QString query(m_searchWindow->query());
    if (m_searchThread->isRunning())
    {
        int res = QMessageBox::question( this, tr("Warning"),
                        tr("Search is already running! Yes for starting the new one or no for waiting until the pending is finished?"),QMessageBox::Yes,QMessageBox::No);
        if (res == QMessageBox::No )
        {
            m_searchWindow->hide();
            return;
        }
        if (res == QMessageBox::Yes )
            m_searchThread->terminate();
    }

    m_packageModel->slotClear();

    statusBar()->showMessage(QString(tr("searching for %1...")).arg(query) );
    m_searchThread->start(query,
                        m_searchWindow->inName(),
                        m_searchWindow->inDesc());
}

void pertubis::MainWindow::onSearchStopped()
{
    m_searchThread->stopExec();
    statusBar()->showMessage(tr("Search stopped"));
    QApplication::restoreOverrideCursor();
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
    if (m_searchWindow->isHidden())
        m_searchWindow->exec();
    else
        m_searchWindow->hide();
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
