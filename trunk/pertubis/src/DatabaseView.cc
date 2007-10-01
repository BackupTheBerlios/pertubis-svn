
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
#include "RepositoryInfoModel.hh"
#include "RepositoryListModel.hh"
#include "SearchThread.hh"
#include "SearchWindow.hh"
#include "Settings.hh"
#include "ShowSelectionsThread.hh"
#include "SyncTask.hh"
#include "TaskBox.hh"

#include <paludis/environments/environment_maker.hh>
#include <paludis/name.hh>
#include <paludis/package_id.hh>
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
    m_catModel(0),
    m_detailsThread(0),
    m_current(0),
    m_output(0),
    m_syncTask(0),
    m_packModel(0),
    m_packageFilterModel(0),
    m_packagesThread(0),
    m_packages(0),
    m_repoInfoView(0),
    m_repoListView(0),
    m_filter(0),
    m_repoInfoModel(0),
    m_repoInfoThread(0),
    m_repoListModel(0),
    m_repoListThread(0),
    m_searchThread(0),
    m_windowSearch(0),
    m_settings(0),
    m_selectionsThread(0),
    m_box(0)
{
    qRegisterMetaType<QTextCursor>("QTextCursor");
    qRegisterMetaType<QList<QVariantList> >("QList<QVariantList>");
    qRegisterMetaType<QList<RepositoryListItem*> >("QList<RepositoryListItem*>");
    qRegisterMetaType<QList<CategoryItem*> >("QList<CategoryItem*>");
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

    createRepositoryBar();
    createPackageView();
    createRepositoryView();

    createCatbar();
    addDockWidget(Qt::LeftDockWidgetArea, m_dockCat);
    addDockWidget(Qt::LeftDockWidgetArea, m_dockRepo);
    createDetails();
    m_detailsTabID = m_tabs->addTab(m_details,tr("Package Details") );
    m_repoViewTabID= m_tabs->addTab(m_repoInfoView,tr("repository details") );
    m_outputTabID=m_tabs->addTab(m_output,tr("Messages"));
    createTrayMenu();
    createToolBar();

    createUseflagEditor();
    createOptionsMenu();
    createWindowSearch();
    m_syncTask = new OurSyncTask(m_env,this,m_output->output());
    statusBar()->showMessage(tr("ready"));
    loadSettings();
    slotRefreshCategories();
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


    connect(m_categories,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(slotCategoryChanged( const QModelIndex& )) );

    connect(m_categoriesThread,
            SIGNAL(categoriesResult(QList<CategoryItem*>)),
            m_catModel,
            SLOT(slotPopulateModel(QList<CategoryItem*>)));

    m_acToggleCatBar = m_dockCat->toggleViewAction();
    m_acToggleCatBar->setText(tr("catbar"));
    m_acToggleCatBar->setIcon(QPixmap(":images/catbar_22.xpm"));
    m_acToggleCatBar->setShortcut( tr("F9"));
    m_acToggleCatBar->setToolTip( html_tooltip(tr("enable/disable the category sidebar"),m_acToggleCatBar->text()) );
    m_categories->setRowHidden(0,false);

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
            tr("installed") <<
            tr("masked by"));

    m_packages = new PackageView(this);

    m_packages->setContextMenuPolicy(Qt::CustomContextMenu);


    m_packageFilterModel = new PackageFilterModel(this,*m_repoListModel);
    m_packageFilterModel->setSourceModel(m_packModel);
    m_packages->setItemDelegateForColumn(Item::io_selected,new OptionsDelegate(this,m_packageFilterModel));
//     m_packageFilterModel->setFilterKeyColumn(Item::io_repository);
//     m_packageFilterModel->setDynamicSortFilter(true);
    m_packages->setModel(m_packageFilterModel);
//     m_packages->setModel(m_packModel);
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
            SIGNAL(finished(int)),
            this,
            SLOT(slotResultCount(int)));

    connect(m_searchThread,
            SIGNAL(itemResult(Item*)),
            m_packModel,
            SLOT(slotAppendPackage(Item*)));

    connect(m_searchThread,
            SIGNAL(finished(int)),
            this,
            SLOT(slotResultCount(int)));

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

    connect(m_repoListModel,
            SIGNAL(dataChanged( const QModelIndex&,const QModelIndex&)),
            m_packageFilterModel,
            SLOT(invalidate() ));
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

}

void pertubis::DatabaseView::createRepositoryView()
{
    m_repoInfoModel = new RepositoryInfoModel(this);
    m_repoInfoModel->setHorizontalHeaderLabels(QStringList() <<
            tr("name") <<
            tr("state"));

    m_repoInfoView = new QTableView(this);
    m_repoInfoView->setModel(m_repoInfoModel);
    m_repoInfoView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_repoInfoView->horizontalHeader()->setVisible(true);
    m_repoInfoView->verticalHeader()->setVisible(false);
    QPalette p(m_output->palette());
    p.setColor(QPalette::Base,QColor(170,170,170)); // background color  = black
    m_repoInfoView->setPalette(p);

    connect(m_repoInfoThread,
            SIGNAL(sendResult(QList<QVariantList>)),
            m_repoInfoModel,
            SLOT(slotResult(QList<QVariantList>)));
}

void pertubis::DatabaseView::createRepositoryBar()
{
    m_repoListModel = new RepositoryListModel(this);


    m_repoListModel->setHorizontalHeaderLabels(QStringList() <<
            tr("activated") <<
            tr("Repository name") );


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


    m_acToggleRepoBar = m_dockRepo->toggleViewAction();
    m_acToggleRepoBar->setText(tr("repository bar"));
    m_acToggleRepoBar->setIcon(QPixmap(":images/catbar_22.xpm"));
    m_acToggleRepoBar->setShortcut( tr("F10"));
    m_acToggleRepoBar->setToolTip( html_tooltip(tr("enable/disable the repository sidebar"),m_acToggleRepoBar->text()) );

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
            SLOT(slotRepositoryChanged( const QModelIndex& )) );



    m_repoListThread->start();
}

void pertubis::DatabaseView::createDetails()
{
    m_details = new QTextBrowser(this);
    QPalette p(m_output->palette());
    p.setColor(QPalette::Base,QColor(170,170,170)); // background color  = black
    m_details->setPalette(p);

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
    m_acToggleMainWindow->setToolTip( html_tooltip(tr("show/hide the pertubis main window"),m_acToggleMainWindow->text() ));

    m_acTogglePackageView = new QAction( QPixmap(":images/packages_22.xpm"),tr("packages"),this );
    m_acTogglePackageView->setCheckable(true);
    m_acTogglePackageView->setChecked(true);
    m_acTogglePackageView->setToolTip( html_tooltip(tr("enable/disable the package window in the middle"),m_acTogglePackageView->text()));

    m_acPref = new QAction( QPixmap(":images/settings_22.xpm"),tr( "Settings"),this);
    m_acPref->setToolTip(html_tooltip(tr("configure pertubis"),m_acPref->text()));

    m_acToggleSearchWindow = new QAction( QPixmap(":images/find_22.xpm"),tr("find") ,this);
    m_acToggleSearchWindow->setShortcut( tr("CTRL+f"));
    m_acToggleSearchWindow->setToolTip(html_tooltip( tr("toggle search window"),m_acToggleSearchWindow->text() ) );

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

    m_acToggleRepoView = new QAction(QPixmap(":images/repositories_22.xpm"),tr("Repositories"),this);
    m_acToggleRepoView->setCheckable(true);
    m_acTogglePackageView->setChecked(true);
    m_acToggleRepoView->setToolTip(html_tooltip( tr("enable/disable the repositories window in the middle"),m_acToggleRepoView->text()) );

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
            SLOT(slotToggleSettings()));

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

void pertubis::DatabaseView::slotResultCount(int count)
{
    statusBar()->showMessage(QString(tr("%1 packages found")).arg(count));
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
    if ( !index.isValid() || index.column() != 1 || m_repoInfoThread->isRunning())
        return;
    QString repo = m_repoListModel->data(index).toString();
    m_repoInfoThread->getInfo(repo);
    m_tabs->setCurrentIndex(m_repoViewTabID);
}

void pertubis::DatabaseView::slotInstallTask(bool mystate)
{
    QModelIndex origIndex(m_packageFilterModel->mapToSource(m_packages->currentIndex()));
    m_packModel->setSelectionData(origIndex,m_tidInstall,mystate);
}

void pertubis::DatabaseView::slotDeinstallTask(bool mystate)
{
//     QModelIndex index = m_packageFilterModel->mapToSource(m_packageFilterModel->currentIndex());
//     m_packModel->setSelectionData(index,m_tidDeinstall,mystate);
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
//     qDebug() << "DatabaseView::slotDetailsChanged() - start" << index;
    QModelIndex ix = m_packageFilterModel->mapToSource(index);
    Item* item = static_cast<Item*>(ix.internalPointer());
    qDebug() << *item;
    if (!ix.isValid() ||
        m_detailsThread->isRunning() ||
        ix.column() != Item::io_package ||
        !item->available() )
        return;
    m_detailsThread->search(item->ID());
//     qDebug() << "DatabaseView::slotDetailsChanged() - done";
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
    m_packModel->slotSetRoot(new Item());

    statusBar()->showMessage(QString(tr("searching for %1...")).arg(m_windowSearch->m_line->text()) );
    m_searchThread->search(m_windowSearch->m_line->text(),m_windowSearch->m_chkName->isChecked(),m_windowSearch->m_chkDesc->isChecked());
}

void pertubis::DatabaseView::slotOptionsMenu(const QModelIndex& mix)
{
    QModelIndex index(m_packageFilterModel->mapToSource(mix));
    m_current= static_cast<Item*>(index.internalPointer());
//     qDebug() << "DatabaseView::slotOptionsMenu() - start" << index << *m_current;

    if (index.column() != Item::io_selected || !m_current->available() )
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
    m_options->popup(m_packages->mapToGlobal(m_packages->m_mousePos));

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
