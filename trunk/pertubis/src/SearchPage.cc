
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
#include "CategoryModel.hh"
#include "DeinstallSelections.hh"
#include "InstallSelections.hh"
#include "MainWindow.hh"
#include "PackageFilterModel.hh"
#include "PackageModelDelegate.hh"
#include "PackageModel.hh"
#include "QuerySettings.hh"
#include "SearchPage.hh"
#include "SearchThread.hh"
#include "Settings.hh"

#include <QAction>
#include <QFont>
#include <QLineEdit>
#include <QHeaderView>
#include <QLayout>
#include <QLabel>
#include <QMessageBox>
#include <QModelIndex>
#include <QStatusBar>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QTreeView>

pertubis::SearchPage::SearchPage(QWidget* pobj, MainWindow * mainWindow) :
        Page(pobj,mainWindow)
{
    m_searchModel = new PackageModel(this);
    m_searchModel->setHorizontalHeaderLabels(QStringList() <<
            tr("+") <<
            tr("-") <<
            tr("#") <<
            tr("package") <<
            tr("category") <<
            tr("repository") <<
            tr("mask reason") <<
            tr("change"));
    m_searchView = new QTreeView(this);
    m_searchView->setItemsExpandable(true);
    m_searchFilterModel = new PackageFilterModel(this,pho_repository);
    m_searchFilterModel->setSourceModel(m_searchModel);
    m_searchView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_searchView->setItemDelegate(new PackageModelDelegate(this,m_searchFilterModel,pho_install,pho_deinstall,pho_installed));
    m_searchView->setModel(m_searchFilterModel);
    m_searchView->header()->setVisible(true);
    m_searchView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_searchView->setSelectionBehavior(QAbstractItemView::SelectRows);
    QFont myfont(m_searchView->font());
    myfont.setBold(true);
    m_searchView->setFont(myfont);

    m_searchThread = new SearchThread(this,m_mainWindow->m_env,m_mainWindow->m_settingsPage->m_queryView->m_model,m_mainWindow->m_installSelections,m_mainWindow->m_deinstallSelections);

    QLabel* label = new QLabel(tr("Search:"));
    m_line = new QLineEdit;
    label->setBuddy(m_line);

    m_bStart = new QPushButton(tr("&Search")),
    m_bStart->setDefault(true);
    m_bStart->setAutoDefault(true);
    m_bStart->setAutoExclusive(true);

    QHBoxLayout* slayout(new QHBoxLayout);
    slayout->addWidget(label);
    slayout->addWidget(m_line);
    slayout->addWidget(m_bStart);

    QVBoxLayout* mylayout(new QVBoxLayout);
    mylayout->addLayout(slayout);
    mylayout->addWidget(m_searchView);
    setLayout(mylayout);

    m_bar = new QProgressBar(this);
    m_bar->setRange(0,100);
    m_bar->hide();

    connect(m_searchView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onSearchViewUserInteraction( const QModelIndex& )) );

    connect(m_searchThread,
            SIGNAL(packageResult(Package*)),
            m_searchModel,
            SLOT(appendPackage(Package*)));

    connect(m_searchThread,
            SIGNAL(finished(int)),
            this,
            SLOT(displaySearchFinished(int)));

    connect(m_searchThread,
            SIGNAL(progress(int)),
            m_bar,
            SLOT(setValue(int)));

    connect(m_bStart,
            SIGNAL(clicked()),
            this,
            SLOT(onSearch()));

    connect(m_line,
            SIGNAL(returnPressed()),
            this,
            SLOT(onSearch()));
    qDebug() << "pertubis::SystemReportPage::SearchPage()" << pobj;
}

pertubis::SearchPage::~SearchPage()
{
    qDebug() << "pertubis::SearchPage::~SearchPage() start";
//     if (!m_searchThread->isFinished())
//     {
//         m_searchThread->stopExec();
//         m_searchThread->wait();
//     }
    delete m_searchThread;
    qDebug() << "pertubis::SearchPage::~SearchPage() 1";
    qDebug() << "pertubis::SearchPage::~SearchPage() 2";
    delete m_searchModel;
    qDebug() << "pertubis::SearchPage::~SearchPage() 3";
    delete m_searchFilterModel;
    delete m_bar;
    delete m_bStart;
    qDebug() << "pertubis::SearchPage::~SearchPage() 4";
    qDebug() << "pertubis::SearchPage::~SearchPage() done";
}

void pertubis::SearchPage::restartFilters(const QSet<QString> & set)
{
    m_searchFilterModel->setFilter(set);
    m_searchFilterModel->invalidate();
}

void pertubis::SearchPage::activatePage()
{
    m_line->setFocus(Qt::ActiveWindowFocusReason);
}

void pertubis::SearchPage::onSearchViewUserInteraction(const QModelIndex & mix)
{
    QModelIndex ix(m_searchFilterModel->mapToSource(mix));
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
        m_searchModel->onUpdateModel();
    }
    else if (pho_deinstall == column )
    {
        int mystate(m_mainWindow->m_deinstallSelections->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
        if (m_mainWindow->m_deinstallSelections->available(package,pho_installed) )
            m_mainWindow->m_deinstallSelections->changeStates(package, mystate,pho_deinstall);
        m_searchModel->onUpdateModel();
    }
    else
    {
        m_mainWindow->showDetails(package->ID());
    }
}

void pertubis::SearchPage::onSearch()
{
    QString query(m_line->text().trimmed());
    qDebug() << "SearchPage::onSearch()" << query;
    if (query.isEmpty())
        return;
    if (m_searchThread->isRunning())
    {
        onSearchStopped();
    }
    m_bStart->setText(tr("Stop"));
    m_mainWindow->onStartOfPaludisAction();
    m_searchModel->clear();
    m_searchThread->start(query);
    m_mainWindow->statusBar()->addWidget(m_bar);
    m_bar->show();
    m_mainWindow->displayNotice(QString(tr("searching for %1...")).arg(query) );
}

void pertubis::SearchPage::onSearchStopped()
{
    m_mainWindow->displayNotice(tr("Search stopped"));
    m_mainWindow->onEndOfPaludisAction();
    m_searchThread->stopExec();
    m_bStart->setText(tr("&start"));
    m_mainWindow->statusBar()->removeWidget(m_bar);
}

void pertubis::SearchPage::displaySearchFinished(int count)
{
    m_mainWindow->onEndOfPaludisAction();
    m_bStart->setText(tr("&start"));
    m_mainWindow->statusBar()->removeWidget(m_bar);
    m_mainWindow->displayNotice(tr("%1 packages found").arg(count));
}
