
/* Copyright (C) 2007-2008 Stefan Koegl <hotshelf@users.berlios.de>
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

#include "PackageBrowsingPage.hh"

#include "CategoryFilterModel.hh"
#include "CategoryModel.hh"
#include "CategoryThread.hh"
#include "DeinstallSelections.hh"
#include "DetailsThread.hh"
#include "InstallSelections.hh"
#include "MainWindow.hh"
#include "PackageFilterModel.hh"
#include "PackageModelDelegate.hh"
#include "PackageModel.hh"
#include "PackageModel.hh"
#include "PackagesThread.hh"
#include "SetThread.hh"
#include "TaskQueue.hh"

#include <QAction>
#include <QFont>
#include <QHeaderView>
#include <QLayout>
#include <QModelIndex>
#include <QSettings>
#include <QSplitter>
#include <QTableView>
#include <QTimer>
#include <QTextBrowser>
#include <QTreeView>

#include <paludis/util/tr1_memory.hh>
#include <paludis/package_id-fwd.hh>

namespace pertubis
{
    struct PackageBrowsingPage::PackageBrowsingPagePrivate
    {
        QString                   m_currentCat;
        CategoryFilterModel*      m_categoryFilterModel;
        CategoryModel*            m_catModel;
        PackageFilterModel*       m_packageFilterModel;
        Package*                  m_current;
        PackageModel*             m_packageModel;
        CategoryFilterModel*      m_setFilterModel;
        CategoryModel*            m_setModel;
        QTextBrowser*             m_details;
        QSplitter*                m_hSplit;
        QSplitter*                m_vSplit;
        QSplitter*                m_lSplit;
        QTableView*               m_categoryView;
        QTableView*               m_setView;
        QTreeView*                m_packageView;
    };
}

// pertubis::PackagesThread*           m_packageViewThread;
// pertubis::SetThread*                m_setThread;

pertubis::PackageBrowsingPage::PackageBrowsingPage(MainWindow * main) :
        Page(main),
        m_imp(new PackageBrowsingPagePrivate)
{
    m_imp->m_catModel = new CategoryModel(this);
    m_imp->m_catModel->setHorizontalHeaderLabels(QStringList(tr("categories")));

    m_imp->m_categoryFilterModel = new CategoryFilterModel(this);
    m_imp->m_categoryFilterModel->setSourceModel(m_imp->m_catModel);

    m_imp->m_categoryView = new QTableView(this);
    m_imp->m_categoryView->setObjectName("m_categoryView");
    m_imp->m_categoryView->setModel(m_imp->m_categoryFilterModel);
    m_imp->m_categoryView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_imp->m_categoryView->verticalHeader()->setVisible(false);
    m_imp->m_categoryView->setShowGrid(false);

    QFont myfont(m_imp->m_categoryView->font());
    myfont.setBold(true);
    m_imp->m_categoryView->setFont(myfont);

    m_imp->m_packageModel = new PackageModel(this);
    m_imp->m_packageModel->setHorizontalHeaderLabels(QStringList() <<
            tr("+") <<
            tr("-") <<
            tr("#") <<
            tr("package") <<
            tr("category") <<
            tr("repository") <<
            tr("mask reason") <<
            tr("change"));

    m_imp->m_packageFilterModel = new PackageFilterModel(this,pho_repository);
    m_imp->m_packageFilterModel->setSourceModel(m_imp->m_packageModel);

    m_imp->m_packageView = new QTreeView(this);
    m_imp->m_packageView->setObjectName("m_packageView");
    m_imp->m_packageView->setItemDelegate(new PackageModelDelegate(this,m_imp->m_packageFilterModel,pho_install,pho_deinstall,pho_installed));
    m_imp->m_packageView->setModel(m_imp->m_packageFilterModel);
    m_imp->m_packageView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_imp->m_packageView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_imp->m_packageView->setFont(myfont);
    m_imp->m_packageView->setSortingEnabled(true);
    m_imp->m_packageView->sortByColumn(pho_package,Qt::AscendingOrder);
    m_imp->m_packageView->header()->setMovable(false);
    m_imp->m_packageView->header()->setVisible(true);
    m_imp->m_packageView->header()->setResizeMode(QHeaderView::ResizeToContents);

    m_imp->m_setModel = new CategoryModel(this);
    m_imp->m_setModel->setHorizontalHeaderLabels(QStringList(tr("sets")));

    m_imp->m_setFilterModel = new CategoryFilterModel(this);
    m_imp->m_setFilterModel->setSourceModel(m_imp->m_setModel);

    m_imp->m_setView = new QTableView(this);
    m_imp->m_setView->setObjectName("m_packageView");
    m_imp->m_setView->setModel(m_imp->m_setFilterModel);
    m_imp->m_setView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_imp->m_setView->verticalHeader()->setVisible(false);
    m_imp->m_setView->setShowGrid(false);
    m_imp->m_setView->setFont(myfont);

    qDebug() << "pertubis::PackageBrowsingPage::PackageBrowsingPage 1";

    QAction* acRefresh(new QAction(tr("refresh"),this));
    acRefresh->setShortcut(tr("F5"));
    addAction(acRefresh);

    m_imp->m_details = new QTextBrowser(this);
    m_imp->m_details->setOpenLinks(false);

    m_imp->m_vSplit = new QSplitter(Qt::Vertical,this);
    m_imp->m_vSplit->addWidget(m_imp->m_packageView);
    m_imp->m_vSplit->addWidget(m_imp->m_details);

    m_imp->m_lSplit = new QSplitter(Qt::Vertical,this);
    m_imp->m_lSplit->addWidget(m_imp->m_categoryView);
    m_imp->m_lSplit->addWidget(m_imp->m_setView);

    m_imp->m_hSplit = new QSplitter(Qt::Horizontal,this);
    m_imp->m_hSplit->addWidget(m_imp->m_lSplit);
    m_imp->m_hSplit->addWidget(m_imp->m_vSplit);

    QHBoxLayout* mylayout(new QHBoxLayout);
    mylayout->addWidget(m_imp->m_hSplit);
    setLayout(mylayout);

//     m_imp->m_setThread = new SetThread(this,mainWindow()->env());

    connect(acRefresh,
            SIGNAL(triggered()),
            this,
            SLOT(refreshPage()));

    connect(m_imp->m_categoryView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onCategoryChanged( const QModelIndex& )) );

    connect(m_imp->m_packageView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onViewUserInteraction( const QModelIndex& )) );


//     connect(m_imp->m_setThread,
//             SIGNAL(sendSet(QMap<QString, QSet<QString> >)),
//             m_imp->m_setModel,
//             SLOT(appendCategory(QMap<QString, QSet<QString> >)));

//     connect(m_setView,
//             SIGNAL(clicked( const QModelIndex&)),
//             this,
//             SLOT(onStartSetInstallTask( const QModelIndex& )) );
    loadSettings();
    show();
    QTimer::singleShot(100,this,SLOT(refreshPage()));
}

pertubis::PackageBrowsingPage::~PackageBrowsingPage()
{
    qDebug() << "pertubis::PackageBrowsingPage::~PackageBrowsingPage() - start";
    saveSettings();
    delete m_imp;
    qDebug() << "pertubis::PackageBrowsingPage::~PackageBrowsingPage() - done";
}

void pertubis::PackageBrowsingPage::activatePage()
{
    if (used() && outOfDate() )
        refreshPage();
}

// void pertubis::PackageBrowsingPage::onShowSetTask(const QModelIndex & model)
// {
//     m_setModel->data(ix).toString().toStdString();
//     m_main->
// }


void pertubis::PackageBrowsingPage::loadSettings()
{
    qDebug() << "pertubis::PackageBrowsingPage::loadSettings() - start";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "PackageBrowsingPage" );
    m_imp->m_currentCat = settings.value("currentCategory","").toString();
    m_imp->m_hSplit->restoreState(settings.value("hSplit").toByteArray());
    m_imp->m_vSplit->restoreState(settings.value("vSplit").toByteArray());
    m_imp->m_lSplit->restoreState(settings.value("lSplit").toByteArray());
    settings.endGroup();
    qDebug() << "pertubis::PackageBrowsingPage::doneSettings() - done";
}

void pertubis::PackageBrowsingPage::saveSettings()
{
    qDebug() << "pertubis::PackageBrowsingPage::saveSettings() - start";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "PackageBrowsingPage" );
    settings.setValue("hSplit", m_imp->m_hSplit->saveState());
    settings.setValue("vSplit", m_imp->m_vSplit->saveState());
    settings.setValue("lSplit", m_imp->m_lSplit->saveState());
    settings.setValue("currentCategory", m_imp->m_currentCat);
    settings.endGroup();
    qDebug() << "pertubis::PackageBrowsingPage::saveSettings() - done";
}

void pertubis::PackageBrowsingPage::details(const paludis::tr1::shared_ptr<const paludis::PackageID> & id)
{
    DetailsThread* t(new DetailsThread(this,mainWindow()->env()));
    mainWindow()->onStartOfPaludisAction();
    connect(t,
            SIGNAL(sendResult(QString)),
            this,
            SLOT(displayDetails(QString)));
    t->setup(id);
    mainWindow()->taskQueue()->enqueue(t,true);
}

void pertubis::PackageBrowsingPage::displayDetails(QString mydetails)
{
    if (mydetails.isEmpty() ||
        this != mainWindow()->currentPage())
        return;
    m_imp->m_details->setText(mydetails);
    mainWindow()->onEndOfPaludisAction();
    setUsed(true);
}

void pertubis::PackageBrowsingPage::restartFilters(const QSet<QString> & set)
{
    m_imp->m_packageFilterModel->setFilter(set);
    m_imp->m_packageFilterModel->invalidate();
    m_imp->m_setFilterModel->setFilter(set);
    m_imp->m_setFilterModel->invalidate();
    m_imp->m_categoryFilterModel->setFilter(set);
    m_imp->m_categoryFilterModel->invalidate();
}


void pertubis::PackageBrowsingPage::getCategories()
{
    CategoryThread* categoryThread(new CategoryThread(this,mainWindow()->env()));

    connect(categoryThread,
            SIGNAL(sendCategory(QMap<QString, QSet<QString> >)),
            m_imp->m_catModel,
            SLOT(appendCategory(QMap<QString, QSet<QString> >)));
    mainWindow()->taskQueue()->enqueue(categoryThread,true);
}

void pertubis::PackageBrowsingPage::onCategoryChanged( const QModelIndex &)
{
    QModelIndex origIndex(m_imp->m_categoryFilterModel->mapToSource(m_imp->m_categoryView->currentIndex()));
    if ( !origIndex.isValid() || 0 != origIndex.column())
        return;
    m_imp->m_currentCat = m_imp->m_catModel->data(origIndex).toString();
    changeCategory();
}

void pertubis::PackageBrowsingPage::refreshPage()
{
    qDebug() << "onRefreshCategory()";
    getCategories();
    if (!m_imp->m_currentCat.isEmpty())
        changeCategory();
    setOutOfDate(false);
}

void pertubis::PackageBrowsingPage::clearPage()
{
    m_imp->m_catModel->clear();
    m_imp->m_details->clear();
    m_imp->m_packageModel->clear();
    setUsed(false);
}

void pertubis::PackageBrowsingPage::changeCategory()
{
    PackagesThread* packageViewThread = new PackagesThread(this,mainWindow()->env(),mainWindow()->installSelections(),mainWindow()->deinstallSelections());

    connect(packageViewThread,
            SIGNAL(addPackage(Package*)),
            m_imp->m_packageModel,
            SLOT(prependPackage(Package*)));

    connect(packageViewThread,
            SIGNAL(finished()),
            this,
            SLOT(displayCategoryChanged()));

    if (m_imp->m_currentCat.isEmpty())
        return;
    mainWindow()->onStartOfPaludisAction();
    m_imp->m_packageModel->clear();
    packageViewThread->setup(m_imp->m_currentCat);
    mainWindow()->taskQueue()->enqueue(packageViewThread,true);
    setUsed(true);
}

void pertubis::PackageBrowsingPage::onViewUserInteraction(const QModelIndex & mix)
{
    QModelIndex ix(m_imp->m_packageFilterModel->mapToSource(mix));
    if (! ix.isValid())
        return;

    Package* package = static_cast<Package*>(ix.internalPointer());
    Q_ASSERT(package != 0);
    int column(ix.column());
    if (pho_install == column )
    {
        if (mainWindow()->installSelections()->available(package,-1))
        {
            int mystate(mainWindow()->installSelections()->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
            mainWindow()->installSelections()->changeStates(package,mystate,pho_install);
        }
        m_imp->m_packageModel->onUpdateModel();
    }
    else if (pho_deinstall == column )
    {
        if (mainWindow()->deinstallSelections()->available(package,pho_installed) )
        {
            int mystate(mainWindow()->deinstallSelections()->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
            mainWindow()->deinstallSelections()->changeStates(package, mystate, pho_deinstall);
        }
        m_imp->m_packageModel->onUpdateModel();
    }
    else
    {
        details(package->ID());
    }
}

void pertubis::PackageBrowsingPage::displayCategoryChanged()
{
    mainWindow()->displayNotice(tr("%1 packages found").arg(m_imp->m_packageFilterModel->rowCount()));
    mainWindow()->onEndOfPaludisAction();
}
