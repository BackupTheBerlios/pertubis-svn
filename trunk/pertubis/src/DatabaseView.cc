
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

#include "CategoryModel.hh"
#include "DatabaseView.hh"
#include "DeinstallTask.hh"
#include "InstallTask.hh"
#include "MessageOutput.hh"
#include "OptionsDelegate.hh"
#include "PackageItem.hh"
#include "PackageModel.hh"
#include "RepositoryListModel.hh"
#include "RepositoryInfoModel.hh"
#include "RepositoryView.hh"
#include "SearchWindow.hh"
#include "Settings.hh"
#include "SyncTask.hh"
#include "TaskBox.hh"
#include "CategoriesThread.hh"
#include "DetailsThread.hh"
#include "PackagesThread.hh"
#include "SearchThread.hh"
#include "ShowSelectionsThread.hh"

#include <paludis/environments/environment_maker.hh>
#include <paludis/name.hh>
#include <paludis/package_id.hh>
#include <paludis/util/stringify.hh>

#include <QApplication>
#include <QCheckBox>
#include <QDesktopServices>
#include <QDebug>
#include <QDockWidget>
#include <QFile>
#include <QHeaderView>
#include <QLayout>
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
    m_catModel(0),
    m_current(0),
    m_output(0),
    m_packModel(0),
    m_packages(0),
    m_repoListView(0),
    m_repoInfoView(0),
    m_filter(0),
    m_repoListModel(0),
    m_repoInfoModel(0),
    m_windowSearch(0),
    m_settings(0),
    m_box(0),
    m_categoriesThread(0),
    m_detailsThread(0),
    m_searchThread(0),
    m_packagesThread(0),
    m_selectionsThread(0),
    m_syncTask(0)
{
    qRegisterMetaType<QTextCursor>("QTextCursor");
    qRegisterMetaType<QList<QVariantList> >("QList<QVariantList>");
    m_settings = new Settings(this);

    m_tabs = new QTabWidget(this);
    createOutput();
    m_env = paludis::EnvironmentMaker::get_instance()->make_from_spec("");

    m_vSplit = new QSplitter(Qt::Vertical, this);
    m_filter = new ReleaseEater(this);
    m_box = new TaskBox(this);

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    installEventFilter(m_filter);
    setWindowTitle( tr("pertubis :: Main Window") );
    QPixmap logoIcon( ":images/logo.xpm" );
    setWindowIcon(logoIcon);
    setCentralWidget(m_vSplit);
    createActions();
    createTasks();

    // "this,this" seems stupid, but the first this will only be accessible
    // as a const reference via "const QObject& QObject::parent()"
    m_searchThread = new SearchThread(this,this);
    m_categoriesThread = new CategoriesThread(this,this);
    m_packagesThread = new PackagesThread(this,this);
    m_detailsThread = new DetailsThread(this,this);
    m_selectionsThread = new ShowSelectionsThread(this,this);
    m_repoListThread = new RepositoryListThread(this,this);
    m_repoInfoThread = new RepositoryInfoThread(this,this);

    createCatbar();
    createDetails();

    createPackageView();
    createRepositoryView();
    createTrayMenu();
    createToolBar();

    createUseflagEditor();
    createOptionsMenu();
    createWindowSearch();
    m_syncTask = new OurSyncTask(m_env,this,m_output->output());
    statusBar()->showMessage(tr("ready"));
    loadSettings();
    rootTest();
}

pertubis::DatabaseView::~DatabaseView()
{
    saveSettings();
}

void pertubis::DatabaseView::closeEvent(QCloseEvent* ev)
{
    hide();
    ev->ignore();
}

void pertubis::DatabaseView::createCatbar()
{
    m_dockCat = new QDockWidget(tr("category list"),this);
    m_dockCat->layout()->setMargin(0);
    m_dockCat->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_catModel = new CategoryModel(this);
    m_catModel->setHorizontalHeaderLabels(QStringList(tr("category")) );
    m_categories = new QTableView(this);
    m_categories->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_categories->horizontalHeader()->setVisible(false);
    m_categories->verticalHeader()->setVisible(false);
    m_categories->setModel(m_catModel);
    m_categories->setShowGrid(false);

    m_dockCat->setWidget(m_categories);
    addDockWidget(Qt::LeftDockWidgetArea, m_dockCat);

    connect(m_categories,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(slotCategoryChanged( const QModelIndex& )) );

    connect(m_categoriesThread,
            SIGNAL(categoriesResult(QStringList)),
            m_catModel,
            SLOT(slotPopulateModel(QStringList)));

    m_acToggleCatBar = m_dockCat->toggleViewAction();
    m_acToggleCatBar->setText(tr("catbar"));
    m_acToggleCatBar->setIcon(QPixmap(":images/catbar_22.xpm"));
    m_acToggleCatBar->setShortcut( tr("F9"));
    m_acToggleCatBar->setToolTip( tr("<html><h1><u>%1</u></h1><p>enable/disable the category sidebar</p></html>").arg(m_acToggleCatBar->text()) );
    m_categories->setRowHidden(0,false);
    slotRefreshCategories();
}

void pertubis::DatabaseView::createPackageView()
{
    m_packModel = new PackageModel(this);
    m_packModel->setBox(m_box);
    m_packModel->setHorizontalHeaderLabels(QStringList() <<
            tr("marked") <<
            tr("package") <<
            tr("category") <<
            tr("repository") <<
            tr("installed"));

    m_packages = new PackageView(this);

    m_packages->setContextMenuPolicy(Qt::CustomContextMenu);
    m_packages->setItemDelegateForColumn(Item::io_selected,new OptionsDelegate(this));

    m_packages->setModel(m_packModel);
    m_packages->header()->setResizeMode(QHeaderView::Stretch);
    m_packages->header()->setVisible(true);
    m_packages->header()->setResizeMode(QHeaderView::ResizeToContents);

    m_vSplit->addWidget(m_packages);
    m_vSplit->addWidget(m_tabs);
    m_vSplit->setStretchFactor(0,10);
    m_vSplit->setStretchFactor(1,10);

    connect(m_packagesThread,
            SIGNAL(packagesResult(Item*)),
            m_packModel,
            SLOT(slotSetRoot(Item*)));

    connect(m_packages,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(slotDetailsChanged( const QModelIndex& )) );

    connect(m_packages,
            SIGNAL(clicked(const QModelIndex&)),
            this,
            SLOT(slotOptionsMenu(const QModelIndex&)));

    connect(m_packagesThread,
            SIGNAL(finished()),
            this,
            SLOT(slotSearchPackagesFinished()));

    connect(m_searchThread,
            SIGNAL(itemResult(Item*)),
            m_packModel,
            SLOT(slotAppendPackage(Item*)));

    connect(m_searchThread,
            SIGNAL(finished()),
            this,
            SLOT(slotSearchItemFinished()));

    connect(m_detailsThread,
            SIGNAL(detailsResult(QString)),
            this,
            SLOT(slotShowDetails(QString)));

    connect(m_acTogglePackageView,
            SIGNAL(triggered()),
            this,
            SLOT(slotTogglePackageView()));

    connect(m_acSelection,
            SIGNAL(triggered()),
            this,
            SLOT(slotShowSelectedPackages()));

    connect(m_selectionsThread,
            SIGNAL(sendRoot(Item*)),
            m_packModel,
            SLOT(slotSetRoot(Item*)));
}

void pertubis::DatabaseView::createToolBar()
{
    m_toolBar = addToolBar( tr("database toolbar") );
    m_toolBar->addAction(m_acSync);
    m_toolBar->addAction(m_acToggleCatBar);
    m_toolBar->addAction(m_acToggleRepoBar);
    m_toolBar->addAction(m_acTogglePackageView);
    m_toolBar->addAction(m_acToggleRepoView);
    m_toolBar->addAction(m_acToggleSearchWindow);
    m_toolBar->addAction(m_acSelection);
    m_toolBar->addAction(m_acFinish);
    m_toolBar->addAction(m_acPref);
}

void pertubis::DatabaseView::createOutput()
{
    m_output = new MessageOutput(this);
    m_tabs->addTab(m_output,tr("Messages"));

}

void pertubis::DatabaseView::createRepositoryView()
{
    m_repoListModel = new RepositoryListModel(this);
    m_repoListModel->setHorizontalHeaderLabels(QStringList() <<
            tr("Repository name"));

    m_repoListView = new QTableView(this);
    m_repoListView->setModel(m_repoListModel);
    m_repoListView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_repoListView->horizontalHeader()->setVisible(false);
    m_repoListView->verticalHeader()->setVisible(false);
    m_repoListView->setShowGrid(false);

    m_repoInfoModel = new RepositoryInfoModel(this);
    m_repoInfoModel->setHorizontalHeaderLabels(QStringList() <<
            tr("name") <<
            tr("state"));

    m_repoInfoView = new QTableView(this);
    m_repoInfoView->setModel(m_repoInfoModel);
    m_repoInfoView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_repoInfoView->horizontalHeader()->setVisible(true);
    m_repoInfoView->verticalHeader()->setVisible(false);
//     m_repoInfoView->setShowGrid(false);

    m_vSplit->addWidget(m_repoInfoView);

    m_dockRepo = new QDockWidget(tr("repository list"),this);
    m_dockRepo->layout()->setMargin(0);
    m_dockRepo->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_dockRepo->setWidget(m_repoListView);
    addDockWidget(Qt::LeftDockWidgetArea, m_dockRepo);

    m_acToggleRepoBar = m_dockRepo->toggleViewAction();
    m_acToggleRepoBar->setText(tr("repository bar"));
    m_acToggleRepoBar->setIcon(QPixmap(":images/catbar_22.xpm"));
    m_acToggleRepoBar->setShortcut( tr("F10"));
    m_acToggleRepoBar->setToolTip( tr("<html><h1><u>%1</u></h1><p>enable/disable the repository sidebar</p></html>").arg(m_acToggleRepoBar->text()) );

    connect(m_repoInfoThread,
            SIGNAL(sendResult(QList<QVariantList>)),
            m_repoInfoModel,
            SLOT(slotResult(QList<QVariantList>)));

    connect(m_repoListThread,
            SIGNAL(sendNames(QStringList)),
                   m_repoListModel,
                   SLOT(slotResult(QStringList)));

    connect(m_repoListView,
            SIGNAL(clicked( const QModelIndex&)),
                   this,
                   SLOT(slotRepositoryChanged( const QModelIndex& )) );

    m_repoListThread->start();
}

void pertubis::DatabaseView::createDetails()
{
    m_details = new QTextBrowser(this);
    m_tabs->insertTab(0,m_details,"Details");
    m_details->setOpenLinks(false);
    connect(m_details,
            SIGNAL(anchorClicked(const QUrl&)),
            this,
            SLOT(slotOpenURL(const QUrl&)));
}

void pertubis::DatabaseView::createActions()
{
    m_acToggleMainWindow = new QAction( QPixmap(":images/logo.xpm"),tr("main window"),this );
    m_acToggleMainWindow->setCheckable(true);
    m_acToggleMainWindow->setChecked(true);
    m_acToggleMainWindow->setToolTip( tr("<html><h1><u>%1</u></h1><p>show/hide the pertubis main window</p></html>").arg(m_acToggleMainWindow->text()) );

    m_acTogglePackageView = new QAction( QPixmap(":images/packages_22.xpm"),tr("packages"),this );
    m_acTogglePackageView->setCheckable(true);
    m_acTogglePackageView->setChecked(true);
    m_acTogglePackageView->setToolTip( tr("<html><h1><u>%1</u></h1><p>enable/disable the package window in the middle</p></html>").arg(m_acTogglePackageView->text()) );

    m_acPref = new QAction( QPixmap(":images/settings_22.xpm"),tr( "Settings"),this);
    m_acPref->setToolTip(tr("<html><h1><u>%1</u></h1><p>configure pertubis</p></html>").arg(m_acPref->text()));

    m_acToggleSearchWindow = new QAction( QPixmap(":images/find_22.xpm"),tr("find") ,this);
    m_acToggleSearchWindow->setShortcut( tr("CTRL+f"));
    m_acToggleSearchWindow->setToolTip( tr("<html><h1><u>%1</u></h1><p>toggle search window</p></html>").arg(m_acToggleSearchWindow->text()) );

    m_acSelection = new QAction( QPixmap(":images/selections_22.xpm"),tr("selections") ,this);
    m_acSelection->setShortcut( tr("CTRL+F11"));
    m_acSelection->setToolTip( tr("<html><h1><u>%1</u></h1><p>here you check the selections you made</p></html>").arg(m_acSelection->text()) );

    m_acFinish = new QAction( QPixmap(":images/finish_22.xpm"),tr("start") ,this);
    m_acFinish->setShortcut( tr("CTRL+F12"));
    m_acFinish->setToolTip( tr("<html><h1><u>%1</u></h1><p>starts all pending tasks you selected</p></html>").arg(m_acFinish->text()) );

    m_acSync = new QAction( QPixmap(":images/reload_22.xpm"), tr("sync"),this);
    m_acSync->setToolTip(tr("<html><h1><u>%1</u></h1><p>To get the latest releases and bugfixes it is neccessary to update the package database.</p><p><h2>Note:</h2><p>It is sufficient to sync your repositories on a daily base</p></html>").arg(m_acSync->text()));

    m_acQuit = new QAction( QPixmap(":images/quit_22.xpm"),tr("quit") ,this);
    m_acQuit->setToolTip(tr("<html><h1><u>%1</u></h1><p>closing the pertubis suite.</p><p><b>All unsaved changes will be lost!</b></p></html>").arg(m_acQuit->text()));

    m_acToggleRepoView = new QAction(QPixmap(":images/repositories_22.xpm"),tr("Repositories"),this);
    m_acToggleRepoView->setCheckable(true);
    m_acTogglePackageView->setChecked(true);
    m_acToggleRepoView->setToolTip( tr("<html><h1><u>%1</u></h1><p>enable/disable the repositories window in the middle</p></html>").arg(m_acToggleRepoView->text()) );

    m_acInstall= new QAction(tr("install"),this);
    m_acInstall->setToolTip( tr("install this package"));
    m_acInstall->setCheckable(true);

    m_acDeinstall= new QAction(tr("deinstall"),this);
    m_acDeinstall->setToolTip( tr("deinstall this package") );
    m_acDeinstall->setCheckable(true);

    m_acEditUse = new QAction(tr("edit useflags"),this);
    m_acEditUse->setToolTip( tr("opens a useflag editor for this item") );

    m_acMasking = new QAction(tr("(un)mask"),this);
    m_acMasking->setToolTip( tr("toggle the masking for this package") );

    connect(m_acSync,
            SIGNAL(triggered()),
            this,
            SLOT(slotSync()));

    connect(m_acToggleMainWindow,
            SIGNAL(triggered()),
            this,
            SLOT(slotToggleMainWindow()));

    connect(m_acPref,
            SIGNAL(triggered()),
            this,
            SLOT(slotOpenSettings()));

    connect(m_acQuit,
            SIGNAL(triggered()),
            qApp,
            SLOT(quit()));

    connect(m_acInstall,
            SIGNAL(toggled(bool)),
            this,
            SLOT(slotInstallTask(bool)));

    connect(m_acDeinstall,
            SIGNAL(toggled(bool)),
            this,
            SLOT(slotDeinstallTask(bool)));

    connect(m_acEditUse,
            SIGNAL(triggered()),
            this,
            SLOT(slotEditUseTask()));

    connect(m_acToggleRepoView,
            SIGNAL(triggered()),
            this,
            SLOT(slotToggleRepoView()));
}

void pertubis::DatabaseView::createTasks()
{
    m_tidInstall = m_box->addTask(new InstallTask( this,m_acInstall,tr("install")));
    m_tidDeinstall = m_box->addTask(new DeinstallTask( this,m_acDeinstall,tr("deinstall")));
}

void pertubis::DatabaseView::createWindowSearch()
{
    m_windowSearch = new SearchWindow(this);
    connect(m_windowSearch,
            SIGNAL(search()),
            this,
            SLOT(slotSearchItem()));

    connect(m_acToggleSearchWindow,
            SIGNAL(triggered()),
            this,
            SLOT(slotToggleSearchWindow()));
}

void pertubis::DatabaseView::createUseflagEditor()
{
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
    m_options  = new QMenu(tr("Package Options"),this);
}

void pertubis::DatabaseView::createTrayMenu()
{
    m_trayMenu = new QMenu("pertubis modules",this);

    m_trayMenu->addAction(m_acToggleMainWindow);
    m_trayMenu->addAction(m_acPref);
    m_trayMenu->addAction(m_acQuit);

    QSystemTrayIcon* tray = new QSystemTrayIcon(QPixmap(":images/logo.xpm"),this);
    connect(tray,
            SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,
            SLOT(slotToggleTrayIcon(QSystemTrayIcon::ActivationReason)));
    tray->setContextMenu(m_trayMenu);
    tray->show();
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
        m_acToggleUseBar->setChecked(settings.value("usebar_visible", true).toBool());

        m_acTogglePackageView->setChecked(settings.value("packageview_visible",true).toBool() );
        m_acToggleRepoView->setChecked(settings.value("repoview_visible",true).toBool() );
        m_vSplit->restoreState(settings.value("vsplt").toByteArray());

        m_dockCat->setVisible(m_acToggleCatBar->isChecked());
        m_dockRepo->setVisible(m_acToggleRepoView->isChecked());
        m_dockUse->setVisible(m_acToggleUseBar->isChecked());
        m_vSplit->setVisible(m_acTogglePackageView->isChecked());
    settings.endGroup();
}

void pertubis::DatabaseView::saveSettings()
{
    QSettings settings;
    settings.beginGroup( "DatabaseView" );
        settings.setValue("visible", isVisible() );
        settings.setValue("size", size() );
        settings.setValue("pos", pos());

        settings.setValue("catbar_visible", m_acToggleCatBar->isChecked());
        settings.setValue("repobar_visible", m_acToggleRepoBar->isChecked());
        settings.setValue("usebar_visible",m_acToggleUseBar->isChecked());

        settings.setValue("repoview_visible", m_acToggleRepoView->isChecked());
        settings.setValue("packageview_visible", m_acTogglePackageView->isChecked());
        settings.setValue("vsplt", m_vSplit->saveState());
    settings.endGroup();
}

void pertubis::DatabaseView::slotRefreshCategories()
{
    if (!m_categoriesThread->isRunning())
        m_categoriesThread->start();
}

void pertubis::DatabaseView::slotSearchItemFinished()
{
    statusBar()->showMessage(QString(tr("%1 packages found")).arg(m_packModel->root()->childCount()));
}

void pertubis::DatabaseView::slotSearchPackagesFinished()
{
    statusBar()->showMessage(QString(tr("%1 packages in category")).arg(m_packModel->root()->childCount()));
}

void pertubis::DatabaseView::slotCategoryChanged( const QModelIndex& index )
{
    if ( !index.isValid() || m_packagesThread->isRunning())
        return;
    QString cat = m_catModel->data(index).toString();
    m_packagesThread->searchPackages(cat);
}

void pertubis::DatabaseView::slotRepositoryChanged( const QModelIndex& index )
{
    if ( !index.isValid() || m_packagesThread->isRunning())
        return;
    QString repo = m_repoListModel->data(index).toString();
    m_repoInfoThread->getInfo(repo);
}

void pertubis::DatabaseView::slotInstallTask(bool mystate)
{
    QModelIndex index = m_packages->currentIndex();
    m_packModel->setSelectionData(index,m_tidInstall,mystate);
}

void pertubis::DatabaseView::slotDeinstallTask(bool mystate)
{
    QModelIndex index = m_packages->currentIndex();
    m_packModel->setSelectionData(index,m_tidDeinstall,mystate);
}

void pertubis::DatabaseView::slotEditUseTask()
{
    if (m_current != 0)
    {
//         QString cat,pack,ver,rep;
//         m_current->entryData(cat,pack,ver,rep);
//         qDebug() << cat << pack << ver << rep;
//         m_useflagEditor->model()->slotEditItemFlags(cat,pack,ver,rep);
    }
}

void pertubis::DatabaseView::slotDetailsChanged(const QModelIndex & index)
{
    qDebug() << "DatabaseView::slotDetailsChanged() - start" << index;
    Item* item = static_cast<Item*>(index.internalPointer());
    qDebug() << "item" << item->data(Item::io_selected).toList() << " , " <<
            item->data(Item::io_package) << " , " <<
            item->data(Item::io_category) << " , " <<
            item->data(Item::io_repository) << " , " <<
            item->data(Item::io_installed);
    if (m_detailsThread->isRunning() ||
        !index.isValid() ||
        index.column() != Item::io_package ||
        !item->available() )
        return;
    m_detailsThread->search(item->ID());
    qDebug() << "DatabaseView::slotDetailsChanged() - done";
}

void pertubis::DatabaseView::slotShowDetails(QString details)
{
    if (!details.isEmpty())
    {
        m_details->setText(details);
        m_tabs->setCurrentIndex(m_tabs->indexOf(m_details));
    }
    else
    {
        m_details->clear();
    }
}

void pertubis::DatabaseView::slotShowSelectedPackages()
{
    m_selectionsThread->start();
}

void pertubis::DatabaseView::slotSync()
{
    if (getuid() != 0 )
    {
        QMessageBox::critical(this,
                            tr("authentication error"),
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
    m_packModel->slotSetRoot(new RootItem());

    statusBar()->showMessage(QString(tr("searching for %1...")).arg(m_windowSearch->m_line->text()) );
    m_searchThread->search(m_windowSearch->m_line->text(),m_windowSearch->m_chkName->isChecked(),m_windowSearch->m_chkDesc->isChecked());
}

void pertubis::DatabaseView::slotOptionsMenu(const QModelIndex& index)
{
    qDebug() << "DatabaseView::slotOptionsMenu() - start" << index;

    Item* item = static_cast<Item*>(index.internalPointer());

    m_current=item;
    if (index.column() != Item::io_selected || !item->available() )
        return;

    m_options->clear();
    QVector<Task*>::const_iterator tStart(m_box->taskBegin());
    QVector<Task*>::const_iterator tEnd(m_box->taskEnd());
    while (tStart != tEnd )
    {
        if ((*tStart)->available(item) )
        {
            (*tStart)->fillAction(item);
            m_options->addAction( (*tStart)->action() );
        }
        ++tStart;
    }
    m_options->addAction(m_acEditUse);
    m_options->addAction(m_acMasking);
    m_options->popup(m_packages->mapToGlobal(m_packages->m_mousePos));
    qDebug() << "DatabaseView::slotOptionsMenu() - done";
}

void pertubis::DatabaseView::slotOpenSettings()
{
    m_settings->exec();
}

void pertubis::DatabaseView::slotOpenURL(const QUrl& url)
{
//     QDesktopServices::openUrl(url);
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
    m_packages->setVisible(!m_packages->isVisible());
}

void pertubis::DatabaseView::slotToggleRepoView()
{
    m_repoInfoView->setVisible(!m_repoInfoView->isVisible());
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
