
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

#include "DatabaseView.hh"
#include "CategoryModel.hh"
#include "PackageModel.hh"
#include "PackageItem.hh"
#include "OptionsDelegate.hh"
#include "TaskBox.hh"
#include <QApplication>
#include <QFile>
#include <QHeaderView>
#include <QMenuBar>
#include <QMessageBox>
#include <QModelIndex>
#include <QPixmap>
#include <QMouseEvent>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QCheckBox>
#include <QTableView>
#include <QTextBrowser>
#include <QTextStream>
#include <QToolBar>

#include "TaskBox.hh"

namespace pertubis
{
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
				return TRUE;
			}
			else
			{
				return FALSE;
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

pertubis::DatabaseView::DatabaseView(QWidget *parent,TaskBox* box)
	: QMainWindow(parent,Qt::Window),
	m_box(box),
	m_windowSearch(0),
	m_filter(0),
	m_vSplit(0),
	m_categories(0),
	m_packages(0),
	m_details(0),
	m_numTotalPacks(0)
{
	m_hSplit = new QSplitter(Qt::Horizontal, this);
	m_vSplit = new QSplitter(Qt::Vertical, this);

	m_catModel = new CategoryModel(this);

	m_catModel->setHorizontalHeaderLabels(QStringList(tr("category")) );

	m_categories = new QTableView(this);

	m_categories->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	m_categories->verticalHeader()->setVisible(false);
	m_categories->setModel(m_catModel);
	m_categories->setShowGrid(false);

	m_packModel = new PackageModel(this);
	m_packModel->setBox(m_box);
	m_packModel->setHorizontalHeaderLabels(QStringList() <<
			tr("marked") <<
			tr("package") <<
			tr("category") <<
			tr("repository") <<
			tr("installed"));

	m_packages = new PackageView(this);
	m_packages->insertAction(0,new QAction(tr("install"),this));
	m_packages->insertAction(0,new QAction(tr("edit useflags for this package(version)"),this));
	m_packages->setContextMenuPolicy(Qt::CustomContextMenu);
	m_packages->setItemDelegateForColumn(Item::io_selected,new OptionsDelegate(this));

	m_packages->setModel(m_packModel);
	m_packages->header()->setResizeMode(QHeaderView::Stretch);
	m_packages->header()->setVisible(true);
	m_packages->header()->setResizeMode(QHeaderView::ResizeToContents);

	m_details = new QTextBrowser(this);
	m_filter = new ReleaseEater(this);

	m_hSplit->addWidget(m_categories);
	m_hSplit->addWidget(m_vSplit);

	m_vSplit->addWidget(m_packages);
	m_vSplit->addWidget(m_details);
	m_thread = new PackageThread(this);


	createActions();
	createToolBar();
	createMenuBar();

	createOptionsMenu();
	createTasks();
	createWindowSearch();
	createConnections();
	loadSettings();

	installEventFilter(m_filter);
	setWindowTitle( tr("pertubis :: Package Database") );
	QPixmap logoIcon( ":logo.xpm" );
	setWindowIcon(logoIcon);
	setCentralWidget(m_hSplit);
	statusBar()->showMessage( tr("ready") );
	show();
}

pertubis::DatabaseView::~DatabaseView()
{
	saveSettings();
}

void pertubis::DatabaseView::createMenuBar()
{
	QMenu* emenu = menuBar()->addMenu(tr("&Edit") );
	emenu->addAction(m_acSearch);
// 	emenu->addAction(m_acFinish);

	QMenu* vmenu = menuBar()->addMenu(tr("&View") );

	vmenu->addAction(m_acShowToolBar);
	vmenu->addAction(m_acShowSideBar);
}

void pertubis::DatabaseView::createToolBar()
{
	m_toolBar = addToolBar( tr("database toolbar") );
	m_toolBar->addAction(m_acShowSideBar);

	m_toolBar->addAction(m_acShowSideBar);
	m_toolBar->addAction(m_acSearch);
	m_toolBar->addAction(m_acSelection);
	m_toolBar->addAction(m_acFinish);
}

void pertubis::DatabaseView::createActions()
{
	m_acShowSideBar = new QAction( QPixmap(":catbar_32.xpm"),tr("&Category Sidebar"),this );
	m_acShowSideBar->setShortcut( tr("F9"));
	m_acShowSideBar->setStatusTip( tr("toggle category sidebar") );

	m_acShowToolBar = new QAction( tr("&Toolbar"),this );

	m_acShowToolBar->setCheckable( true );
	m_acShowToolBar->setStatusTip( tr("toggle toolbar") );

	m_acSearch = new QAction( QPixmap(":find_32.xpm"),tr("&Find") ,this);
	m_acSearch->setShortcut( tr("CTRL+F"));
	m_acSearch->setToolTip( tr("toggle search window") );

	m_acSelection = new QAction( QPixmap(":selections_32.xpm"),tr("F11") ,this);
	m_acSelection->setShortcut( tr("CTRL+F11"));
	m_acSelection->setStatusTip( tr("show selection") );
	m_acSelection->setToolTip( tr("here you check the selections you made") );

	m_acFinish = new QAction( QPixmap(":finish_32.xpm"),tr("F12") ,this);
	m_acFinish->setShortcut( tr("CTRL+F12"));
	m_acFinish->setStatusTip( tr("start") );
	m_acFinish->setToolTip( tr("starts all pending tasks you selected") );
}

void pertubis::DatabaseView::createConnections()
{
	connect(m_acShowSideBar,
			SIGNAL(triggered()),
			this,
			SLOT(slotToggleCatBar()));

	connect(m_acSearch,
			SIGNAL(triggered()),
			this,
			SLOT(slotToggleWindowSearch()));

	connect(m_acShowToolBar,
			SIGNAL(triggered()),
			this,
			SLOT(slotToggleToolBar()));

	connect(m_categories,
			SIGNAL(clicked( const QModelIndex&)),
			this,
			SLOT(slotCategoryChanged( const QModelIndex& )) );

	connect(m_packages,
			SIGNAL(clicked( const QModelIndex&)),
			this,
			SLOT(slotDetailsChanged( const QModelIndex& )) );

	connect(m_thread,
			SIGNAL(categoriesResult(QStringList)),
			m_catModel,
    		SLOT(slotPopulateModel(QStringList)));

	connect(m_windowSearch,
			SIGNAL(search()),
			this,
			SLOT(slotSearchItem()));

	connect(m_thread,
			SIGNAL(itemResult(Item*)),
			m_packModel,
    		SLOT(slotAppendPackage(Item*)));

	connect(m_thread,
			SIGNAL(packagesResult(Item*)),
			m_packModel,
			SLOT(slotSetRoot(Item*)));

	connect(m_thread,
			SIGNAL(searchPackagesFinished()),
			this,
			SLOT(slotSearchPackagesFinished()));

	connect(m_thread,
			SIGNAL(searchItemFinished()),
			this,
    		SLOT(slotSearchItemFinished()));

	connect(m_thread,
			SIGNAL(detailsResult(QString)),
			this,
			SLOT(slotShowDetails(QString)));

	connect(m_packages,
			SIGNAL(clicked(const QModelIndex&)),
			this,
			SLOT(slotOptionsMenu(const QModelIndex&)));

	m_thread->searchCategories();
}

void pertubis::DatabaseView::createTasks()
{
	m_box->addTask(tr("install"));
	m_box->addTask( tr("deinstall") );

	m_acInstall= new QAction(tr("install"),this);
	m_acInstall->setStatusTip( tr("install this package") );
	m_acInstall->setCheckable(true);

	m_acDeinstall= new QAction(tr("deinstall"),this);
	m_acDeinstall->setStatusTip( tr("deinstall this package") );
	m_acDeinstall->setCheckable(true);

	m_acEditUse = new QAction(tr("edit useflags"),this);
	m_acEditUse->setStatusTip( tr("edit now the useflags for this package") );

	m_acMasking = new QAction(tr("(un)mask"),this);
	m_acMasking->setStatusTip( tr("toggle the masking for this package") );

	connect(m_acInstall,
			SIGNAL(toggled(bool)),
			this,
    		SLOT(slotInstallTask(bool)));

	connect(m_acDeinstall,
			SIGNAL(toggled(bool)),
			this,
    		SLOT(slotDeinstallTask(bool)));
}

void pertubis::DatabaseView::createWindowSearch()
{
	m_windowSearch = new SearchWindow(this);
}

void pertubis::DatabaseView::createOptionsMenu()
{
	m_options  = new QMenu(tr("Package Options"),this);
}

void pertubis::DatabaseView::loadSettings()
{
	QSettings settings;
	settings.beginGroup( "DatabaseView" );
		setVisible(settings.value( "visible",true).toBool());
		resize(settings.value("size",QVariant(QSize(600,480))).toSize());
		move(settings.value("pos",QVariant(QPoint(400,0))).toPoint());
		m_hSplit->restoreState(settings.value("hsplt").toByteArray());
		m_vSplit->restoreState(settings.value("vsplt").toByteArray());
	settings.endGroup();
}

void pertubis::DatabaseView::saveSettings()
{
	QSettings settings;
	settings.beginGroup( "DatabaseView" );
		settings.setValue("visible", isVisible() );
		settings.setValue("size", size() );
		settings.setValue("pos", pos() );
		settings.setValue("hsplt", m_hSplit->saveState());
		settings.setValue("vsplt", m_vSplit->saveState());
	settings.endGroup();
}

void pertubis::DatabaseView::slotRefreshCatModel()
{
	if (!m_thread->isRunning())
		m_thread->searchCategories();
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
	if ( !index.isValid() || m_thread->isRunning())
		return;
	QString cat = m_catModel->data(index).toString();
	m_details->hide();
	m_details->clear();
	m_thread->searchPackages(cat);
}

void pertubis::DatabaseView::slotInstallTask(bool state)
{
	setTaskData(tr("install"),state);
}

void pertubis::DatabaseView::slotDeinstallTask(bool state)
{
	setTaskData(tr("deinstall"),state);
}

void pertubis::DatabaseView::slotEditUseTask(bool state)
{
	setTaskData(tr("edit useflags"),state);
}

void pertubis::DatabaseView::setTaskData(QString tname, bool state)
{
// 	qDebug() << "slotSetSelectionData() - start";
	QModelIndex index = m_packages->currentIndex();
	m_packModel->setSelectionData(index,tname,state);

// 	qDebug() << "slotSetSelectionData() - done";
}

void pertubis::DatabaseView::slotToggleCatBar()
{
	if (!m_categories)
		return;
	if (m_categories->isVisible())
		m_categories->hide();
	else
		m_categories->show();
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
	if (m_thread->isRunning() ||
		!index.isValid() ||
		index.column() != Item::io_package ||
	   (item->m_rtti == Item::it_package && item->m_status == Item::is_masked) )
		return;

	QString cat,pack,ver,rep;
	item->entryData(cat,pack,ver,rep);

	if (cat.isEmpty() || cat.isNull() ||
	   pack.isEmpty() || pack.isNull() ||
	   ver.isEmpty() || ver.isNull() ||
	   rep.isEmpty() || rep.isNull() )
		return;
	m_thread->searchDetails(cat,pack,ver,rep,item->m_status);
}

void pertubis::DatabaseView::slotSearchItem()
{
	if (m_windowSearch->m_line->text().trimmed().isEmpty())
	{
		m_windowSearch->hide();
		return;
	}

	if (m_thread->isRunning())
	{
		int res = QMessageBox::question( 0, tr("Question"),
                        tr("Search already running! Yes for starting the new one or no for waiting until the pending is finished?"),QMessageBox::Yes,QMessageBox::No);
		if (res == QMessageBox::No )
		{
			m_windowSearch->hide();
			return;
		}
		if (res == QMessageBox::Yes )
			m_thread->terminate();
	}

	m_details->clear();
	m_details->hide();
	m_windowSearch->hide();
	m_categories->hide();
	m_packModel->slotSetRoot(new Item(QList<QVariant>() << QVariantMap() << "" << "" << "" << Qt::Unchecked,0,Item::it_category));

	statusBar()->showMessage(QString(tr("searching for %1...")).arg(m_windowSearch->m_line->text()) );
	m_thread->searchItem(m_windowSearch->m_line->text(),m_windowSearch->m_chkName->isChecked(),m_windowSearch->m_chkDesc->isChecked());
}

void pertubis::DatabaseView::slotOptionsMenu(const QModelIndex& index)
{
// 	qDebug() << "DatabaseView::slotOptionsMenu()" << index.column();

	Item* item = static_cast<Item*>(index.internalPointer());
	connect(item,
			SIGNAL(taskChanged(Item*,QString,bool)),
			m_box,
   			SLOT(slotTaskChanged(Item*,QString,bool)));
	if (index.column() != Item::io_selected ||  (item->m_rtti == Item::it_package && item->m_status == Item::is_masked) )
		return;
	m_options->clear();
	QString cat,pack,ver,rep;
	item->entryData(cat,pack,ver,rep);
	Entry entry = genPackEntry(cat,pack,ver,rep);
	QVariantMap optionsData = m_box->selectionData(entry);
	QVariantMap::const_iterator it = optionsData.constBegin();

	if (item->m_status != Item::is_masked)
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

void pertubis::DatabaseView::slotToggleToolBar()
{
	m_toolBar->setVisible(!m_toolBar->isVisible());
}

void pertubis::DatabaseView::slotToggleWindowSearch()
{
	if (!m_windowSearch)
		return;
	if (m_windowSearch->isHidden())
		m_windowSearch->exec();
	else
		m_windowSearch->hide();
}
