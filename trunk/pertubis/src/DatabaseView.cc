
/* Copyright (C) 2007 Stefan Koegl.
*
* This file is part of pertubis.
*
* pertubis is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* pertubis is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http:*www.gnu.org/licenses/>.
*/

#include "CategoriesThread.hh"
#include "CategoryModel.hh"
#include "CategoryFilterModel.hh"
#include "DatabaseView.hh"
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
// #include "RepositoryInfoModel.hh"
#include "RepositoryListModel.hh"
#include "SearchThread.hh"
#include "SearchWindow.hh"
#include "Settings.hh"
#include "ShowSelectionsThread.hh"
#include "SyncTask.hh"
#include "TaskBox.hh"

#include <paludis/args/install_args_group.hh>
#include <paludis/environment_maker.hh>
#include <paludis/name.hh>
#include <paludis/package_id.hh>
#include <paludis/package_id-fwd.hh>
#include <paludis/util/set-fwd.hh>
#include <paludis/util/set.hh>
#include <paludis/util/set-impl.hh>
#include <paludis/util/stringify.hh>

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QDockWidget>
#include <QFile>
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
#include <QTextStream>
#include <QToolBar>
#include <QUrl>

#include <iostream>

namespace pertubis
{
    /*! \brief We only need press events and filter release events
    *
    */
    class ReleaseEater : public QObject
    {

    public:
        ReleaseEater(QObject* pobj) : QObject(pobj) {}
    protected:

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

bool pertubis::rootTest()
{
    if (0 != getuid() )
    {
        QMessageBox::warning(0,
                            QObject::tr("unpriviledged mode"),
                            QObject::tr("You are a normal user. Some features will only work for administrators ( root )"),
                                   QMessageBox::Ok,
                                   QMessageBox::Ok);
        return true;
    }
    return false;
}

pertubis::PackageView::PackageView(QWidget* pWidget) : QTreeView(pWidget)
{
}

void pertubis::PackageView::mousePressEvent(QMouseEvent* ev)
{
    m_mousePos = ev->pos();
    QTreeView::mousePressEvent(ev);
}

pertubis::DatabaseView::DatabaseView() :
    m_categoriesThread(0),
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
//     m_repoInfoView(0),
    m_repoListView(0),
    m_filter(0),
//     m_repoInfoModel(0),
//     m_repoInfoThread(0),
    m_repoListModel(0),
    m_repoListThread(0),
    m_searchThread(0),
    m_windowSearch(0),
    m_settings(0),
    m_selectionsThread(0),
    m_box(0)
{
    m_env = paludis::EnvironmentMaker::get_instance()->make_from_spec("");
    qRegisterMetaType<QList<RepositoryListItem*> >("QList<RepositoryListItem*>");
    qRegisterMetaType<QList<QVariantList> >("QList<QVariantList>");
    qRegisterMetaType<QMap<QString, QSet<QString> > >("QMap<QString, QSet<QString> >");
    initGUI();
    initLayout();
    loadSettings();
    m_repoListThread->start();
    slotRefreshCategories();
//     rootTest();
}

pertubis::DatabaseView::~DatabaseView()
{
    saveSettings();
}

void pertubis::DatabaseView::initGUI()
{
    createActions(); // no deps
    createTab(); // no deps
    createTaskBox(); // no deps
    createTasks(); // createTaskBox()
    createRepositoryBar(); // no deps
    createCatbar(); // createRepositoryBar()
    createPackageView(); // createRepositoryBar()
    createToolBar(); // createActions()
    createSettings(); // no deps
    createOptionsMenu(); // no deps
    createWindowSearch();
    createTrayMenu();
    statusBar()->show();
}

void pertubis::DatabaseView::initLayout()
{
    qDebug() << "pertubis::DatabaseView::initLayout()";
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
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    show();
    m_filter = new ReleaseEater(this);
    installEventFilter(m_filter);
    m_searchThread = new SearchThread(this,this);
    m_categoriesThread = new CategoriesThread(this,this);
    m_packageViewThread = new PackagesThread(this,this);
    m_detailsThread = new DetailsThread(this,this);
    m_selectionsThread = new ShowSelectionsThread(this,this);
    m_repoListThread = new RepositoryListThread(this,this);
//     m_repoInfoThread = new RepositoryInfoThread(this,this);
    m_syncTask = new OurSyncTask(m_env,this,m_output->output());
    createConnections();
    qDebug() << "pertubis::DatabaseView::initLayout() - done";
}

void pertubis::DatabaseView::createTaskBox()
{
    // no deps
    m_box = new TaskBox(this);
}

void pertubis::DatabaseView::closeEvent(QCloseEvent* ev)
{
    hide();
    ev->ignore();
}

paludis::tr1::shared_ptr<paludis::Environment> pertubis::DatabaseView::getEnv() const
{
    return m_env;
}

void pertubis::DatabaseView::createCatbar()
{
    qDebug() << "pertubis::DatabaseView::createCatbar()";
    // deps:
    // - createRepositoryBar()
    //
    m_dockCat = new QDockWidget(tr("category list"),this);
    m_dockCat->layout()->setMargin(0);
    m_dockCat->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    m_catModel = new CategoryModel(this);
    m_catModel->setHorizontalHeaderLabels(QStringList() << tr("category"));

    m_categoryFilterModel = new CategoryFilterModel(this,*m_repoListModel);
    m_categoryFilterModel->setSourceModel(m_catModel);
    m_categories = new QTableView(this);
    m_categories->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_categories->horizontalHeader()->setVisible(false);
    m_categories->verticalHeader()->setVisible(false);
    m_categories->setModel(m_categoryFilterModel);
    m_categories->setShowGrid(false);

    m_dockCat->setWidget(m_categories);

    m_acToggleCatBar = m_dockCat->toggleViewAction();

    m_acToggleCatBar->setText(tr("catbar"));

    m_acToggleCatBar->setIcon(QPixmap(":images/catbar_22.xpm"));
    m_acToggleCatBar->setShortcut(tr("F9"));
    m_acToggleCatBar->setToolTip(html_tooltip(tr("enable/disable the category sidebar"),m_acToggleCatBar->text()) );
    m_categories->setRowHidden(0,false);
}

void pertubis::DatabaseView::createSettings()
{
    qDebug() << "pertubis::DatabaseView::createSettings()";
    m_settings = new Settings(this);
}

void pertubis::DatabaseView::createPackageView()
{
    // deps
    // - createRepositoryBar()
    qDebug() << "pertubis::DatabaseView::createPackageView()";
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
    m_packageView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_packageFilterModel = new PackageFilterModel(this);
    m_packageFilterModel->setSourceModel(m_packageModel);
    m_packageView->setItemDelegateForColumn(Item::io_selected,new OptionsDelegate(this,m_packageFilterModel));
    m_packageView->setModel(m_packageFilterModel);
    m_packageView->header()->setResizeMode(QHeaderView::Stretch);
    m_packageView->header()->setVisible(true);
    m_packageView->header()->setResizeMode(QHeaderView::ResizeToContents);
}

void pertubis::DatabaseView::createToolBar()
{
    // deps
    // - createActions()
    qDebug() << "pertubis::DatabaseView::createToolBar()";
    m_toolBar = addToolBar( tr("database toolbar") );
    m_toolBar->addAction(m_acSync);
    m_toolBar->addAction(m_acToggleCatBar);
    m_toolBar->addAction(m_acToggleRepoBar);
    m_toolBar->addAction(m_acTogglePackageView);
    m_toolBar->addAction(m_acToggleSearchWindow);
    m_toolBar->addAction(m_acSelection);
    m_toolBar->addAction(m_acFinish);
    m_toolBar->addAction(m_acPref);
}

void pertubis::DatabaseView::createOutput()
{
    qDebug() << "pertubis::DatabaseView::createOutput()";
    m_output = new MessageOutput(this);
}

void pertubis::DatabaseView::createRepositoryBar()
{
    qDebug() << "pertubis::DatabaseView::createRepositoryBar()";
    m_repoListModel = new RepositoryListModel(this);
    m_repoListModel->setHorizontalHeaderLabels(QStringList() <<
            tr("activated") <<
            tr("Repository name") );

    qDebug() << "pertubis::DatabaseView::createRepositoryBar() - 1";

    m_repoListView = new QTableView(this);
    m_repoListView->setModel(m_repoListModel);
    m_repoListView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_repoListView->horizontalHeader()->setVisible(true);
    m_repoListView->horizontalHeader()->setToolTip(html_tooltip(tr("Here you can select the repositories pertubis will use.</p><p>Click of a repository name to get more information")));
    m_repoListView->verticalHeader()->setVisible(false);
    m_repoListView->setShowGrid(false);
    m_dockRepo = new QDockWidget(tr("repository list"),this);
    m_dockRepo->layout()->setMargin(0);
    m_dockRepo->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_dockRepo->setWidget(m_repoListView);
    qDebug() << "pertubis::DatabaseView::createRepositoryBar() - 2";

    m_acToggleRepoBar = m_dockRepo->toggleViewAction();
    qDebug() << "pertubis::DatabaseView::createRepositoryBar() - 3";
    m_acToggleRepoBar->setText(tr("repository bar"));
    m_acToggleRepoBar->setIcon(QPixmap(":images/repositories_22.xpm"));
    m_acToggleRepoBar->setShortcut( tr("F10"));
    m_acToggleRepoBar->setToolTip( html_tooltip(tr("enable/disable the repository sidebar"),m_acToggleRepoBar->text()) );
    qDebug() << "pertubis::DatabaseView::createRepositoryBar() - end";
}

// void pertubis::DatabaseView::createRepositoryView()
// {
//     qDebug() << "pertubis::DatabaseView::createRepositoryView()";
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

void pertubis::DatabaseView::createDetails()
{
    qDebug() << "pertubis::DatabaseView::createDetails()";
    m_details = new QTextBrowser(this);
    QPalette p(m_details->palette());
    p.setColor(QPalette::Base,QColor(170,170,170)); // background color  = black
    m_details->setPalette(p);
    m_details->setOpenLinks(false);
}

void pertubis::DatabaseView::createTab()
{
    m_tabs = new QTabWidget(this);
    createDetails();
//     createRepositoryView();
    createOutput();
    m_detailsTabID = m_tabs->addTab(m_details,tr("Package Details") );
//     m_repoViewTabID = m_tabs->addTab(m_repoInfoView,tr("repository details") );
    m_outputTabID = m_tabs->addTab(m_output,tr("Messages"));
}

void pertubis::DatabaseView::createActions()
{
    qDebug() << "pertubis::DatabaseView::createActions()";

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

    m_acSync = new QAction( QPixmap(":images/reload_22.xpm"), tr("sync"),this);
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

void pertubis::DatabaseView::createConnections()
{
    connect(m_acDeinstall,
            SIGNAL(toggled(bool)),
            this,
            SLOT(slotDeinstallTask(bool)));

    connect(m_acEditUse,
            SIGNAL(triggered()),
            this,
            SLOT(slotEditUseTask()));

    connect(m_acFinish,
            SIGNAL(triggered()),
            this,
            SLOT(slotFinish()));

    connect(m_acInstall,
            SIGNAL(toggled(bool)),
            this,
            SLOT(slotInstallTask(bool)));

    connect(m_acSync,
            SIGNAL(triggered()),
            this,
            SLOT(slotSync()));

    connect(m_acSelection,
            SIGNAL(triggered()),
            this,
            SLOT(slotShowSelectedPackages()));

    connect(m_acPref,
            SIGNAL(triggered()),
            this,
            SLOT(slotToggleSettings()));

    connect(m_acQuit,
            SIGNAL(triggered()),
            qApp,
            SLOT(quit()));

    connect(m_acToggleMainWindow,
            SIGNAL(triggered()),
            this,
            SLOT(slotToggleMainWindow()));

    connect(m_acTogglePackageView,
            SIGNAL(triggered()),
            this,
            SLOT(slotTogglePackageView()));

    connect(m_acToggleSearchWindow,
            SIGNAL(triggered()),
            this,
            SLOT(slotToggleSearchWindow()));

    connect(m_categories,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(slotCategoryChanged( const QModelIndex& )) );

    connect(m_categoriesThread,
            SIGNAL(sendCategory(QMap<QString, QSet<QString> >)),
            m_catModel,
            SLOT(slotAppendCategory(QMap<QString, QSet<QString> >)));

    connect(m_details,
            SIGNAL(anchorClicked(const QUrl&)),
            this,
            SLOT(slotOpenURL(const QUrl&)));

    connect(m_detailsThread,
            SIGNAL(detailsResult(QString)),
            this,
            SLOT(slotShowDetails(QString)));

    connect(m_packageViewThread,
            SIGNAL(addPackage(Item*)),
            m_packageModel,
            SLOT(slotPrependPackage(Item*)));

    connect(m_packageViewThread,
            SIGNAL(finished()),
            this,
            SLOT(slotResultCount()));

    connect(m_packageViewThread,
            SIGNAL(changeInCat(QString)),
            m_catModel,
            SLOT(slotChangeInCat(QString)));

    connect(m_packageView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(slotDetailsChanged( const QModelIndex& )) );

    connect(m_packageView,
            SIGNAL(clicked(const QModelIndex&)),
            this,
            SLOT(slotOptionsMenu(const QModelIndex&)));

    connect(m_searchThread,
            SIGNAL(itemResult(Item*)),
            m_packageModel,
            SLOT(slotAppendPackage(Item*)));

    connect(m_searchThread,
            SIGNAL(finished(int)),
            this,
            SLOT(slotResultCount()));

    connect(m_selectionsThread,
            SIGNAL(appendPackage(Item*)),
            m_packageModel,
            SLOT(slotAppendPackage(Item*)));

    connect(m_repoListModel,
            SIGNAL(modelReset()),
            this,
            SLOT(slotReposChanged()));

    connect(m_repoListThread,
            SIGNAL(sendNames(QList<RepositoryListItem*>)),
            m_repoListModel,
            SLOT(slotResult(QList<RepositoryListItem*>)));

    connect(m_repoListView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(slotRepositoryChanged( const QModelIndex& )) );

    connect(m_repoListView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(slotResultCount()) );

    connect(m_windowSearch,
            SIGNAL(search()),
            this,
            SLOT(slotSearchItem()));
}

void pertubis::DatabaseView::createTasks()
{
    // deps
    // - createTaskBox()
    m_tidInstall = m_box->addTask(new InstallTask( this,m_acInstall,tr("install")));
    m_tidDeinstall = m_box->addTask(new DeinstallTask( this,m_acDeinstall,tr("deinstall")));
}

void pertubis::DatabaseView::createWindowSearch()
{
     // deps
    // - createActions()
    m_windowSearch = new SearchWindow(this);

}

void pertubis::DatabaseView::createUseflagEditor()
{
    qDebug() << "pertubis::DatabaseView::createUseflagEditor()";
//     m_useflagEditor = new UseFlagEditor(this,m_env);
    m_dockUse = new QDockWidget(tr("useflag editor"),this);
    m_dockUse->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
//     m_dockUse->setWidget(m_useflagEditor);
    addDockWidget(Qt::RightDockWidgetArea, m_dockUse);
    m_acToggleUseBar = m_dockUse->toggleViewAction();
    m_acToggleUseBar->setIcon(QPixmap(":images/useflags_22.xpm"));
    m_acToggleUseBar->setToolTip(tr("<html><h1><u>useflag editor</u></h1><p>here you can manage useflag settings</html>"));
    m_toolBar->addAction(m_acToggleUseBar);
}

void pertubis::DatabaseView::createOptionsMenu()
{
    qDebug() << "pertubis::DatabaseView::createOptionsMenu()";
    m_options  = new QMenu(tr("Package Options"),this);
}

void pertubis::DatabaseView::createTrayMenu()
{
    qDebug() << "pertubis::DatabaseView::createTrayMenu()";
    m_trayMenu = new QMenu(this);
    m_trayMenu->addAction(m_acToggleMainWindow);
    m_trayMenu->addAction(m_acPref);
    m_trayMenu->addAction(m_acQuit);

    QSystemTrayIcon* tray = new QSystemTrayIcon(QPixmap(":images/logo.xpm"),this);
    tray->setContextMenu(m_trayMenu);
    tray->show();

    connect(tray,
            SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,
            SLOT(slotToggleTrayIcon(QSystemTrayIcon::ActivationReason)));
}

void pertubis::DatabaseView::loadSettings()
{
    QSettings settings;
    settings.beginGroup( "DatabaseView" );
        setVisible(settings.value("visible",true).toBool());
        resize(settings.value("size",QVariant(QSize(800,600))).toSize());
        move(settings.value("pos",QVariant(QPoint(341,21))).toPoint());

        m_acToggleCatBar->setChecked(settings.value("catbar_visible",true ).toBool()  );
        m_acToggleRepoBar->setChecked(settings.value("repobar_visible",true ).toBool()  );
//         m_acToggleUseBar->setChecked(settings.value("usebar_visible", true).toBool());

        m_acTogglePackageView->setChecked(settings.value("packageview_visible",true).toBool() );
        m_vSplit->restoreState(settings.value("vsplt").toByteArray());

        m_dockCat->setVisible(m_acToggleCatBar->isChecked());
        m_dockRepo->setVisible(m_acToggleRepoBar->isChecked());
//         m_dockUse->setVisible(m_acToggleUseBar->isChecked());
        m_vSplit->setVisible(m_acTogglePackageView->isChecked());
    settings.endGroup();
}

void pertubis::DatabaseView::saveSettings()
{
    qDebug() << "pertubis::DatabaseView::saveSettings()";
    QSettings settings;
    settings.beginGroup( "DatabaseView" );
        settings.setValue("visible", isVisible() );
        settings.setValue("size", size() );
        settings.setValue("pos", pos());

        settings.setValue("catbar_visible", m_acToggleCatBar->isChecked());
        settings.setValue("repobar_visible", m_acToggleRepoBar->isChecked());
//         settings.setValue("usebar_visible",m_acToggleUseBar->isChecked());

        settings.setValue("packageview_visible", m_acTogglePackageView->isChecked());
        settings.setValue("vsplt", m_vSplit->saveState());
    settings.endGroup();
    qDebug() << "pertubis::DatabaseView::saveSettings() - done";
}

void pertubis::DatabaseView::slotRefreshCategories()
{
    if (!m_categoriesThread->isRunning())
        m_categoriesThread->start();
}

void pertubis::DatabaseView::slotResultCount()
{
    statusBar()->showMessage(QString(tr("%1 packages found")).arg(m_packageFilterModel->rowCount()));
}

void pertubis::DatabaseView::slotCategoryChanged( const QModelIndex& /*proxyIndex*/ )
{
    QModelIndex origIndex(m_categoryFilterModel->mapToSource(m_categories->currentIndex()));
    if ( !origIndex.isValid() || m_packageViewThread->isRunning())
        return;
    QString cat = m_catModel->data(origIndex).toString();
    qDebug() << "pertubis::DatabaseView::slotCategoryChanged()" << cat;
    m_packageModel->slotClear();
    m_packageViewThread->start(cat);
}

void pertubis::DatabaseView::slotRepositoryChanged( const QModelIndex& index )
{
    qDebug() << "pertubis::DatabaseView::slotRepositoryChanged()";
    if ( !index.isValid())
        return;

    if (index.column() == 0 )
    {
        int state = m_repoListModel->data(index,Qt::CheckStateRole).toInt();
        qDebug() << "pertubis::DatabaseView::slotRepositoryChanged()" << m_repoListModel->data(index,Qt::CheckStateRole);
        m_repoListModel->setData(index, (state == Qt::Checked ) ? Qt::Unchecked :Qt::Checked);
        m_packageFilterModel->setFilter(m_repoListModel->activeRepositories());
        m_packageFilterModel->invalidate();
        m_categoryFilterModel->invalidate();
    }

//     if (index.column() == 1 )
//     {
//         QString repo = m_repoListModel->data(index).toString();
//         m_repoInfoThread->getInfo(repo);
//         m_tabs->setCurrentIndex(m_repoViewTabID);
//     }
}

void pertubis::DatabaseView::slotReposChanged()
{
    m_packageFilterModel->setFilter(m_repoListModel->activeRepositories());
    m_packageFilterModel->invalidate();
    m_categoryFilterModel->invalidate();
}

void pertubis::DatabaseView::slotInstallTask(bool mystate)
{
    QModelIndex origIndex(m_packageFilterModel->mapToSource(m_packageView->currentIndex()));
    m_packageModel->setSelectionData(origIndex,m_tidInstall,mystate);
}

void pertubis::DatabaseView::slotDeinstallTask(bool mystate)
{
    QModelIndex index(m_packageFilterModel->mapToSource(m_packageView->currentIndex()));
    m_packageModel->setSelectionData(index,m_tidDeinstall,mystate);
}

void pertubis::DatabaseView::slotFinish()
{
    m_box->doPendingTasks(this);
//     QMessageBox::information(0,
//                          QObject::tr("information"),
//                                      QObject::tr("feature is under development"),
//                                              QMessageBox::Ok,
//                                              QMessageBox::Ok);
}

void pertubis::DatabaseView::slotEditUseTask()
{
//     if (m_current != 0)
//     {
//         QString cat,pack,ver,rep;
//         m_current->entryData(cat,pack,ver,rep);
//         qDebug() << cat << pack << ver << rep;
//         m_useflagEditor->model()->slotEditItemFlags(cat,pack,ver,rep);
//     }
}

void pertubis::DatabaseView::slotDetailsChanged(const QModelIndex & index)
{
    qDebug() << "DatabaseView::slotDetailsChanged() - start" << index;
    QModelIndex ix = m_packageFilterModel->mapToSource(index);
    Item* item = static_cast<Item*>(ix.internalPointer());

    if (!ix.isValid() ||
        m_detailsThread->isRunning() ||
        ix.column() != Item::io_package)
        return;
    m_detailsThread->start(item->ID());
    qDebug() << "DatabaseView::slotDetailsChanged() - done" << *item << item->ID()->canonical_form(paludis::idcf_full).c_str();
}

void pertubis::DatabaseView::slotShowDetails(QString details)
{
    if (!details.isEmpty())
    {
        m_details->setText(details);
        m_tabs->setCurrentIndex(m_detailsTabID);
    }
    else
    {
        m_details->clear();
    }
}

void pertubis::DatabaseView::slotShowSelectedPackages()
{
    m_packageModel->slotClear();
    m_selectionsThread->start();
}

void pertubis::DatabaseView::slotSync()
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

void pertubis::DatabaseView::slotSearchItem()
{
    if (m_windowSearch->m_line->text().trimmed().isEmpty())
    {
        return;
    }

    if (m_searchThread->isRunning())
    {
        int res = QMessageBox::question( this, tr("Warning"),
                        tr("Search is already running! Yes for starting the new one or no for waiting until the pending is finished?"),QMessageBox::Yes,QMessageBox::No);
        if (res == QMessageBox::No )
        {
            m_windowSearch->hide();
            return;
        }
        if (res == QMessageBox::Yes )
            m_searchThread->terminate();
    }

    m_windowSearch->hide();
//     m_acToggleCatBar->setChecked(false);
    m_dockCat->setVisible(false);
    m_packageModel->slotClear();

    statusBar()->showMessage(QString(tr("searching for %1...")).arg(m_windowSearch->m_line->text()) );
    m_searchThread->start(m_windowSearch->m_line->text(),m_windowSearch->m_chkName->isChecked(),m_windowSearch->m_chkDesc->isChecked());
}

void pertubis::DatabaseView::slotOptionsMenu(const QModelIndex& mix)
{
    QModelIndex index(m_packageFilterModel->mapToSource(mix));
    m_current= static_cast<Item*>(index.internalPointer());
    qDebug() << "DatabaseView::slotOptionsMenu() - start" << index << *m_current;

    if (index.column() != Item::io_selected)
        return;

    m_options->clear();

    QVector<Task*>::const_iterator tStart(m_box->taskBegin());
    QVector<Task*>::const_iterator tEnd(m_box->taskEnd());

    while (tStart != tEnd )
    {
        if ((*tStart)->available(m_current) )
        {
            (*tStart)->fillAction(m_current);
            m_options->addAction( (*tStart)->action() );
        }
        ++tStart;
    }

    m_options->addAction(m_acEditUse);
    m_options->addAction(m_acMasking);
    m_options->popup(m_packageView->mapToGlobal(m_packageView->m_mousePos));

}

void pertubis::DatabaseView::slotToggleSettings()
{
    m_settings->exec();
}

void pertubis::DatabaseView::slotOpenURL(const QUrl& url)
{
    QString program = "/usr/bin/xdg-open";
    QStringList arguments;
    arguments << url.toString();
    QProcess *myProcess = new QProcess(this);
    myProcess->start(program, arguments);
}

void pertubis::DatabaseView::slotQuit()
{
    QApplication::instance()->quit();
}

void pertubis::DatabaseView::slotTogglePackageView()
{
    m_packageView->setVisible(!m_packageView->isVisible());
}

void pertubis::DatabaseView::slotToggleSearchWindow()
{
    if (!m_windowSearch)
        return;
    if (m_windowSearch->isHidden())
        m_windowSearch->exec();
    else
        m_windowSearch->hide();
}

void pertubis::DatabaseView::slotToggleTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            slotToggleMainWindow();
            break;
        case QSystemTrayIcon::Context:
            m_trayMenu->show();
            break;
        default:
            ;
    }
}

void pertubis::DatabaseView::slotToggleMainWindow()
{
    setVisible(!isVisible());
}
