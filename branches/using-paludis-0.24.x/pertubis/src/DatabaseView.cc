
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

#include "OptionsDelegate.hh"
#include "Settings.hh"

#include "ThreadFetchItem.hh"
#include "ThreadFetchCategories.hh"
#include "ThreadFetchDetails.hh"
#include "ThreadFetchPackages.hh"
#include "ThreadKeywordManager.hh"
#include "UseFlagEditor.hh"
#include "UseFlagModel.hh"

#include "PackageItem.hh"
#include "PackageModel.hh"
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QDockWidget>
#include <QCheckBox>
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
#include <QTableView>
#include <QTextBrowser>
#include <QTextStream>
#include <QSystemTrayIcon>
#include <QToolBar>
#include "SearchWindow.hh"
#include "TaskBox.hh"

#include <QDockWidget>

#include <paludis/environments/environment_maker.hh>
#include <paludis/util/log.hh>
// #include <tr1/memory>

namespace pertubis
{
	/*! \brief We only need press events and filter release events
	*
	*/
	class ReleaseEater : public QObject
	{

	public:
		ReleaseEater(QObject* parent) : QObject(parent) {}
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

pertubis::PackageView::PackageView(QWidget* parent) : QTreeView(parent)
{

}

void pertubis::PackageView::mousePressEvent(QMouseEvent* event)
{
	m_mousePos = event->pos();
	QTreeView::mousePressEvent(event);
}

pertubis::DatabaseView::DatabaseView()
{
	m_settings = new Settings(this);
// 	setAttribute(Qt::WA_DeleteOnClose);
	paludis::Log::get_instance()->set_log_level(paludis::ll_qa);
    paludis::Log::get_instance()->set_program_name("pertubis");
   	std::tr1::shared_ptr<paludis::Environment> env(paludis::EnvironmentMaker::get_instance()->make_from_spec(""));

	m_threadKeywords = new ThreadKeywordManager(this,env);
	m_threadKeywords->start();
	m_vSplit = new QSplitter(Qt::Vertical, this);
	m_details = new QTextBrowser(this);
	m_filter = new ReleaseEater(this);
	m_box = new TaskBox(this);
	installEventFilter(m_filter);
	setWindowTitle( tr("pertubis :: Main Window") );
	QPixmap logoIcon( ":logo.xpm" );
	setWindowIcon(logoIcon);

	setCentralWidget(m_vSplit);

	createTasks();

	m_threadItem = new ThreadFetchItem(this,env,m_threadKeywords,m_box);

	m_threadCategories = new ThreadFetchCategories(this,env);

	m_threadPackages = new ThreadFetchPackages(this,env,m_threadKeywords,m_box);
	m_threadDetails = new ThreadFetchDetails(this,env);

	createActions();

	createCatbar();

	createPackageView();

	createTrayMenu();

	createToolBar();
	m_useflagEditor = new UseFlagEditor(this,env);

	createUseflagEditor();
	createOptionsMenu();
	createWindowSearch();

	loadSettings();
}

pertubis::DatabaseView::~DatabaseView()
{
// 	qDebug() << "DatabaseView::~DatabaseView() - start";
	saveSettings();
// 	qDebug() << "DatabaseView::~DatabaseView() - done";
}

void pertubis::DatabaseView::closeEvent(QCloseEvent* event)
{
	hide();
	event->ignore();
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
	m_vSplit->addWidget(m_details);
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

	connect(m_threadPackages,
			SIGNAL(finished()),
			this,
			SLOT(slotSearchPackagesFinished()));

	connect(m_packages,
			SIGNAL(clicked(const QModelIndex&)),
			this,
			SLOT(slotOptionsMenu(const QModelIndex&)));

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
	m_box->addTask(tr("install"));
	m_box->addTask( tr("deinstall") );
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
	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	m_dockUse = new QDockWidget(tr("useflag editor"),this);
	m_dockUse->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	m_dockUse->setWidget(m_useflagEditor);
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
			SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
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
		m_acTogglePackageView->setChecked(settings.value("vsplit_visible",true).toBool() );
		m_acToggleUseBar->setChecked(settings.value("useedit_visible", true).toBool());
		m_acToggleCatBar->setChecked( settings.value("cat_visible",true ).toBool()  );
		m_dockCat->setVisible(m_acToggleCatBar->isChecked());
		m_dockUse->setVisible(m_acToggleUseBar->isChecked());
		m_vSplit->setVisible(m_acTogglePackageView->isChecked());
		m_vSplit->restoreState(settings.value("vsplt").toByteArray());
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
	QString cat = m_catModel->data(index).toString();
	m_details->hide();
	m_details->clear();
	m_threadPackages->searchPackages(cat);
}

void pertubis::DatabaseView::slotInstallTask(bool state)
{
	QModelIndex index = m_packages->currentIndex();
	m_packModel->setSelectionData(index,tr("install"),state);
}

void pertubis::DatabaseView::slotDeinstallTask(bool state)
{
	QModelIndex index = m_packages->currentIndex();
	m_packModel->setSelectionData(index,tr("deinstall"),state);
}

void pertubis::DatabaseView::slotEditUseTask()
{
	if (m_current != 0)
	{
		QString cat,pack,ver,rep;
		m_current->entryData(cat,pack,ver,rep);
		qDebug() << cat << pack << ver << rep;
		m_useflagEditor->model()->slotEditItemFlags(cat,pack,ver,rep);
	}
}

void pertubis::DatabaseView::slotShowDetails(QString details)
{
	if (details.isNull() or details.isEmpty())
	{
		m_details->hide();
		m_details->clear();
		return;
	}
	m_details->setText(details);
	m_details->show();
}

void pertubis::DatabaseView::slotDetailsChanged(const QModelIndex & index)
{
// 	qDebug() << "DatabaseView::slotDetailsChanged()" << index;
	Item* item = static_cast<Item*>(index.internalPointer());
	if (m_threadDetails->isRunning() ||
		!index.isValid() ||
		index.column() != Item::io_package ||
	   (item->type() == Item::it_package && item->status() == Item::is_masked) )
		return;

	QString cat,pack,ver,rep;
	item->entryData(cat,pack,ver,rep);

	if (cat.isEmpty() || cat.isNull() ||
	   pack.isEmpty() || pack.isNull() ||
	   ver.isEmpty() || ver.isNull() ||
	   rep.isEmpty() || rep.isNull() )
		return;
	m_threadDetails->search(cat,pack,ver,rep,item->status());
}

void pertubis::DatabaseView::slotSync()
{
	QMessageBox::information( this, tr("Sync"),
                        "Must be implemented!");
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
	m_packModel->slotSetRoot(new Item(QList<QVariant>() << QVariantMap() << "" << "" << "" << Qt::Unchecked,0,Item::it_category));

	statusBar()->showMessage(QString(tr("searching for %1...")).arg(m_windowSearch->m_line->text()) );
	m_threadItem->search(m_windowSearch->m_line->text(),m_windowSearch->m_chkName->isChecked(),m_windowSearch->m_chkDesc->isChecked());
}

void pertubis::DatabaseView::slotOptionsMenu(const QModelIndex& index)
{
// 	qDebug() << "DatabaseView::slotOptionsMenu()" << index;

	Item* item = static_cast<Item*>(index.internalPointer());

	connect(item,
		SIGNAL(taskChanged(Item*,QString,bool)),
		m_box,
		SLOT(slotTaskChanged(Item*,QString,bool)));

	m_current=item;

	if (index.column() != Item::io_selected ||  (item->type() == Item::it_package && item->status() == Item::is_masked) )
		return;
	m_options->clear();
	QString cat,pack,ver,rep;
	item->entryData(cat,pack,ver,rep);
	Entry entry = genPackEntry(cat,pack,ver,rep);
	QVariantMap optionsData = m_box->selectionData(entry);

	if (item->status() != Item::is_masked)
	{
		m_options->addAction(m_acInstall);
		m_acInstall->setChecked(optionsData[tr("install")].toBool() ? Qt::Checked : Qt::Unchecked);
	}

	if (item->data(Item::io_installed).toBool())
	{
		m_options->addAction(m_acDeinstall);
		m_acDeinstall->setChecked(optionsData[tr("deinstall")].toBool() ? Qt::Checked : Qt::Unchecked);
	}

	m_options->addAction(m_acEditUse);
	m_options->addAction(m_acMasking);
	m_options->popup(m_packages->mapToGlobal(m_packages->m_mousePos));
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
			show();
			break;
		case QSystemTrayIcon::Context:
			slotToggleMainWindow();
			break;
		default:
			;
	}
}

void pertubis::DatabaseView::slotToggleMainWindow()
{
	setVisible(!isVisible());
}
