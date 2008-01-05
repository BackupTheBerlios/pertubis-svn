
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

#include "PackageBrowsingPage.hh"
#include "CategoryFilterModel.hh"
#include "PackageFilterModel.hh"
#include "CategoryModel.hh"
#include "InstallSelections.hh"
#include "DeinstallSelections.hh"
#include "PackageModel.hh"
#include "PackageModelDelegate.hh"
#include "CategoryThread.hh"
#include "PackagesThread.hh"
#include "MainWindow.hh"
#include "PackageModel.hh"
#include <QSettings>
#include <QSplitter>

#include <QModelIndex>
#include <QTableView>
#include <QHeaderView>
#include <QLayout>
#include <QTreeView>
#include <QFont>

pertubis::PackageBrowsingPage::PackageBrowsingPage(QWidget* pobj, MainWindow * mainWindow) :
        Page(pobj, mainWindow)
{
    m_catModel = new CategoryModel(this);
    m_catModel->setHorizontalHeaderLabels(QStringList() << tr("category"));

    m_categoryFilterModel = new CategoryFilterModel(this);
    m_categoryFilterModel->setSourceModel(m_catModel);

    m_categoryView = new QTableView(this);
    m_categoryView->setObjectName("m_categoryView");
    m_categoryView->horizontalHeader()->setVisible(false);
    m_categoryView->verticalHeader()->setVisible(false);
    m_categoryView->setModel(m_categoryFilterModel);
    m_categoryView->setShowGrid(false);
    m_categoryView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    QFont myfont(m_categoryView->font());
    myfont.setBold(true);
    m_categoryView->setFont(myfont);

    m_packageModel = new PackageModel(this);

    m_packageModel->setHorizontalHeaderLabels(QStringList() <<
            tr("+") <<
            tr("-") <<
            tr("#") <<
            tr("package") <<
            tr("category") <<
            tr("repository") <<
            tr("mask reason") <<
            tr("change"));

    m_packageView = new QTreeView(this);
    m_packageView->setItemsExpandable(true);
    m_packageFilterModel = new PackageFilterModel(this,pho_repository);
    m_packageFilterModel->setSourceModel(m_packageModel);
    m_packageView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_packageView->setItemDelegate(new PackageModelDelegate(this,m_packageFilterModel,pho_install,pho_deinstall,pho_installed));
    m_packageView->setModel(m_packageFilterModel);
    m_packageView->header()->setVisible(true);
    m_packageView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_packageView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_packageView->setFont(myfont);

    m_hSplit = new QSplitter(Qt::Horizontal,pobj);
    m_hSplit->addWidget(m_categoryView);
    m_hSplit->addWidget(m_packageView);
    QHBoxLayout* mylayout(new QHBoxLayout);
    mylayout->addWidget(m_hSplit);
    setLayout(mylayout);

    m_packageViewThread = new PackagesThread(this,m_mainWindow->m_env,m_mainWindow->m_installSelections,m_mainWindow->m_deinstallSelections);
    m_categoryThread = new CategoryThread(this,m_mainWindow->m_env);

    connect(m_categoryView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onCategoryChanged( const QModelIndex& )) );

    connect(m_categoryThread,
            SIGNAL(sendCategory(QMap<QString, QSet<QString> >)),
            m_catModel,
            SLOT(appendCategory(QMap<QString, QSet<QString> >)));

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
            SLOT(onPackageViewUserInteraction( const QModelIndex& )) );

    loadSettings();

    show();

    m_categoryThread->start();
    if (!m_currentCat.isEmpty())
        m_packageViewThread->start(m_currentCat);

    qDebug() << "pertubis::SystemReportPage::PackageBrowsingPage()" << pobj;
}

pertubis::PackageBrowsingPage::~PackageBrowsingPage()
{
    qDebug() << "pertubis::PackageBrowsingPage::~PackageBrowsingPage() - start";
    if (!m_packageViewThread->isFinished())
    {
        m_packageViewThread->stopExec();
        m_packageViewThread->wait();
    }
    if (!m_categoryThread->isFinished())
    {
        m_categoryThread->stopExec();
        m_categoryThread->wait();
    }
    saveSettings();
    qDebug() << "pertubis::PackageBrowsingPage::~PackageBrowsingPage() - done";
}

void pertubis::PackageBrowsingPage::activatePage()
{
}

void pertubis::PackageBrowsingPage::loadSettings()
{
    qDebug() << "pertubis::PackageBrowsingPage::loadSettings() - start";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "PackageBrowsingPage" );
    m_currentCat = settings.value("currentCategory","").toString();
    m_hSplit->restoreState(settings.value("splitterSizes").toByteArray());
    settings.endGroup();
    qDebug() << "pertubis::PackageBrowsingPage::doneSettings() - done";
}

void pertubis::PackageBrowsingPage::saveSettings()
{
    qDebug() << "pertubis::PackageBrowsingPage::saveSettings() - start";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "PackageBrowsingPage" );
    settings.setValue("splitterSizes", m_hSplit->saveState());
    settings.setValue("currentCategory", m_currentCat);
    settings.endGroup();
    qDebug() << "pertubis::PackageBrowsingPage::saveSettings() - done";
}

void pertubis::PackageBrowsingPage::restartFilters(const QSet<QString> & set)
{
    m_packageFilterModel->setFilter(set);
    m_packageFilterModel->invalidate();
}

void pertubis::PackageBrowsingPage::onCategoryChanged( const QModelIndex &)
{
    QModelIndex origIndex(m_categoryFilterModel->mapToSource(m_categoryView->currentIndex()));
    if ( !origIndex.isValid() || 0 != origIndex.column() || m_packageViewThread->isRunning())
        return;

    m_mainWindow->onStartOfPaludisAction();
    m_packageModel->clear();
    m_currentCat = m_catModel->data(origIndex).toString();
    m_packageViewThread->start(m_currentCat);
}

void pertubis::PackageBrowsingPage::onPackageViewUserInteraction(const QModelIndex & mix)
{
    QModelIndex ix(m_packageFilterModel->mapToSource(mix));
    if (! ix.isValid())
        return;

    Package* package = static_cast<Package*>(ix.internalPointer());
    Q_ASSERT(package != 0);
    int column(ix.column());
    if (pho_install == column )
    {
        int mystate(m_mainWindow->m_installSelections->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
        if (m_mainWindow->m_installSelections->available(package,-1))
            m_mainWindow->m_installSelections->changeStates(package,mystate,pho_install);
        m_packageModel->onUpdateModel();
    }
    else if (pho_deinstall == column )
    {
        int mystate(m_mainWindow->m_deinstallSelections->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
        if (m_mainWindow->m_deinstallSelections->available(package,pho_installed) )
            m_mainWindow->m_deinstallSelections->changeStates(package, mystate, pho_deinstall);
        m_packageModel->onUpdateModel();
    }
    else
    {
        m_mainWindow->showDetails(package->ID());
    }
}

void pertubis::PackageBrowsingPage::displayCategoryChanged()
{
    m_mainWindow->displayNotice(tr("%1 packages found").arg(m_packageFilterModel->rowCount()));
    m_mainWindow->onEndOfPaludisAction();
}

