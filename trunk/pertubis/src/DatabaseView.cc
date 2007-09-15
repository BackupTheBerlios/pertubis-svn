
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
#include <paludis/environments/environment_maker.hh>
#include <paludis/name.hh>
#include <paludis/package_id.hh>
// #include <paludis/util/log.hh>
#include <paludis/util/stringify.hh>

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QDockWidget>
#include <QDockWidget>
#include <QFile>
#include <QHeaderView>
#include <QLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPixmap>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QTableView>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTextStream>
#include <QToolBar>

#include "SearchWindow.hh"
#include "Settings.hh"
#include "TaskBox.hh"
#include "ThreadFetchCategories.hh"
#include "ThreadFetchDetails.hh"
#include "ThreadFetchItem.hh"
#include "ThreadFetchPackages.hh"

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

pertubis::PackageView::PackageView(QWidget* pWidget) : QTreeView(pWidget)
{
}

void pertubis::PackageView::mousePressEvent(QMouseEvent* ev)
{
    m_mousePos = ev->pos();
    QTreeView::mousePressEvent(ev);
}

pertubis::DatabaseView::DatabaseView()
{
    m_settings = new Settings(this);

    m_tabs = new QTabWidget(this);
    createOutput();
    paludis::tr1::shared_ptr<paludis::Environment> env(paludis::EnvironmentMaker::get_instance()->make_from_spec(""));

//     m_threadKeywords = new ThreadKeywordManager(this,env);
    m_threadKeywords = 0;
//     m_threadKeywords->start();
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

    m_threadItem = new ThreadFetchItem(this,env,m_threadKeywords,m_box);
    m_threadCategories = new ThreadFetchCategories(this,env);
    m_threadPackages = new ThreadFetchPackages(this,env,m_threadKeywords,m_box);
    m_threadDetails = new ThreadFetchDetails(this,env);

    createCatbar();
    createDetails();

    createPackageView();
    createTrayMenu();
    createToolBar();

    createUseflagEditor();
    createOptionsMenu();
    createWindowSearch();
    statusBar()->showMessage(tr("ready"));
    loadSettings();
}

pertubis::DatabaseView::~DatabaseView()
{
//     qDebug() << "DatabaseView::~DatabaseView() - start";
    saveSettings();
//     qDebug() << "DatabaseView::~DatabaseView() - done";
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

    connect(m_threadCategories,
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

    connect(m_threadPackages,
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

    connect(m_threadPackages,
            SIGNAL(finished()),
            this,
            SLOT(slotSearchPackagesFinished()));

    connect(m_threadItem,
            SIGNAL(itemResult(Item*)),
            m_packModel,
            SLOT(slotAppendPackage(Item*)));

    connect(m_threadItem,
            SIGNAL(finished()),
            this,
            SLOT(slotSearchItemFinished()));

    connect(m_threadDetails,
            SIGNAL(detailsResult(QString)),
            this,
            SLOT(slotShowDetails(QString)));

    connect(m_acTogglePackageView,
            SIGNAL(triggered()),
            this,
            SLOT(slotTogglePackageView()));
}

void pertubis::DatabaseView::createToolBar()
{
    m_toolBar = addToolBar( tr("database toolbar") );
    m_toolBar->addAction(m_acSync);
    m_toolBar->addAction(m_acToggleCatBar);
    m_toolBar->addAction(m_acTogglePackageView);
    m_toolBar->addAction(m_acToggleSearchWindow);
    m_toolBar->addAction(m_acSelection);
    m_toolBar->addAction(m_acFinish);
    m_toolBar->addAction(m_acPref);
//     m_toolBar->addAction(m_acToggleOutput);
}

void pertubis::DatabaseView::createOutput()
{
    m_output = new MessageOutput(this);
    m_tabs->addTab(m_output,tr("Messages"));
}

void pertubis::DatabaseView::createDetails()
{
//     m_dockDetails = new QDockWidget(tr("Messages"),this);
//     m_dockDetails->layout()->setMargin(0);
//     m_dockDetails->setAllowedAreas(Qt::BottomDockWidgetArea);
    m_details = new QTextBrowser(this);
    m_tabs->addTab(m_details,"Details");
//     m_dockDetails->setWidget(m_details);
//     addDockWidget(Qt::BottomDockWidgetArea, m_dockDetails);
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
    m_acToggleSearchWindow->setShortcut( tr("CTRL+F"));
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

    m_acInstall= new QAction(tr("install"),this);
    m_acInstall->setStatusTip( tr("install this package"));
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
        setVisible(settings.value( "visible",true).toBool());
        resize(settings.value("size",QVariant(QSize(800,600))).toSize());
        move(settings.value("pos",QVariant(QPoint(341,21))).toPoint());
        m_acToggleCatBar->setChecked(settings.value("cat_visible",true ).toBool()  );
        m_acTogglePackageView->setChecked(settings.value("vsplit_visible",true).toBool() );
        m_acToggleUseBar->setChecked(settings.value("useedit_visible", true).toBool());
        m_vSplit->restoreState(settings.value("vsplt").toByteArray());
        m_dockCat->setVisible(m_acToggleCatBar->isChecked());
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
        settings.setValue("cat_visible", m_acToggleCatBar->isChecked());
        settings.setValue("vsplit_visible", m_acTogglePackageView->isChecked());
        settings.setValue("useedit_visible",m_acToggleUseBar->isChecked());
        settings.setValue("vsplt", m_vSplit->saveState());
    settings.endGroup();
}

void pertubis::DatabaseView::slotRefreshCategories()
{
    if (!m_threadCategories->isRunning())
        m_threadCategories->start();
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
    if ( !index.isValid() || m_threadPackages->isRunning())
        return;
    qDebug() << "slotCategoryChanged() - 1";
    QString cat = m_catModel->data(index).toString();
    m_details->hide();
    m_details->clear();
    qDebug() << "slotCategoryChanged() - 2";
    m_threadPackages->searchPackages(cat);
    qDebug() << "slotCategoryChanged() - 3";
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
    if (m_threadDetails->isRunning() ||
        !index.isValid() ||
        index.column() != Item::io_package ||
        !item->available() )
        return;
    m_threadDetails->search(item->ID());
    qDebug() << "DatabaseView::slotDetailsChanged() - done";
}

void pertubis::DatabaseView::slotShowDetails(QString details)
{
    if (details.isNull() or details.isEmpty())
    {
        m_dockDetails->hide();
        m_details->clear();
        return;
    }
    m_details->setText(details);
    m_dockDetails->show();
}


void pertubis::DatabaseView::slotSelectedPackages()
{
    m_threadPackages->makePackageItems();
}

void pertubis::DatabaseView::slotSync()
{
    QMessageBox::information( this, tr("Sync"),"Must be implemented!");
}

void pertubis::DatabaseView::slotSearchItem()
{
    if (m_windowSearch->m_line->text().trimmed().isEmpty())
    {
        return;
    }

    if (m_threadItem->isRunning())
    {
        int res = QMessageBox::question( 0, tr("Question"),
                        tr("Search is already running! Yes for starting the new one or no for waiting until the pending is finished?"),QMessageBox::Yes,QMessageBox::No);
        if (res == QMessageBox::No )
        {
            m_windowSearch->hide();
            return;
        }
        if (res == QMessageBox::Yes )
            m_threadItem->terminate();
    }

    m_details->clear();
    m_details->hide();
    m_windowSearch->hide();
    m_acToggleCatBar->setChecked(!m_acToggleCatBar->isChecked());
    m_dockCat->setVisible(m_acToggleCatBar->isChecked());
    m_packModel->slotSetRoot(new RootItem());

    statusBar()->showMessage(QString(tr("searching for %1...")).arg(m_windowSearch->m_line->text()) );
    m_threadItem->search(m_windowSearch->m_line->text(),m_windowSearch->m_chkName->isChecked(),m_windowSearch->m_chkDesc->isChecked());
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

void pertubis::DatabaseView::slotQuit()
{
    QApplication::instance()->quit();
}

void pertubis::DatabaseView::slotTogglePackageView()
{
    m_vSplit->setVisible(!m_vSplit->isVisible());
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
