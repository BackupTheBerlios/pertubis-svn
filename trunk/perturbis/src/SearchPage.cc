
/* Copyright (C) 2007-2008 Stefan Koegl
*
* This file is part of perturbis
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

#include "DetailsThread.hh"
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
#include "TaskQueue.hh"

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
#include <QTextBrowser>
#include <QSplitter>
#include <QPushButton>
#include <QSettings>
#include <QTreeView>

using namespace perturbis;

namespace perturbis
{
    struct SearchPagePrivate
    {

        ~SearchPagePrivate()
        {
            delete m_searchModel;
            delete m_searchFilterModel;
            delete m_bar;
            delete m_bStart;
        }
        QProgressBar*           m_bar;
        PackageModel*           m_searchModel;
        QTreeView*              m_searchView;
        QTextBrowser*           m_details;
        QLineEdit*              m_line;
        QPushButton*            m_bStart;
        QSplitter*              m_hSplit;
        PackageFilterModel*     m_searchFilterModel;
    };
}

SearchPage::SearchPage(MainWindow * main) :
        Page(main),
        m_imp(new SearchPagePrivate)
{
    m_imp->m_searchModel = new PackageModel(this);
    m_imp->m_searchModel->setHorizontalHeaderLabels(QStringList() <<
            tr("+") <<
            tr("-") <<
            tr("#") <<
            tr("package") <<
            tr("category") <<
            tr("repository") <<
            tr("mask reason") <<
            tr("change"));
    m_imp->m_searchView = new QTreeView(this);
    m_imp->m_searchView->setItemsExpandable(true);
    m_imp->m_searchFilterModel = new PackageFilterModel(this,pho_repository);
    m_imp->m_searchFilterModel->setSourceModel(m_imp->m_searchModel);
    m_imp->m_searchView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_imp->m_searchView->setItemDelegate(new PackageModelDelegate(this,m_imp->m_searchFilterModel,pho_install,pho_deinstall,pho_installed));
    m_imp->m_searchView->setModel(m_imp->m_searchFilterModel);
    m_imp->m_searchView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_imp->m_searchView->setSortingEnabled(true);
    m_imp->m_searchView->sortByColumn(pho_package,Qt::AscendingOrder);
    m_imp->m_searchView->header()->setMovable(false);
    m_imp->m_searchView->header()->setVisible(true);
    m_imp->m_searchView->header()->setResizeMode(QHeaderView::ResizeToContents);
    QFont myfont(m_imp->m_searchView->font());
    myfont.setBold(true);
    m_imp->m_searchView->setFont(myfont);

    QLabel* label = new QLabel(tr("Search:"));
    m_imp->m_line = new QLineEdit;
    label->setBuddy(m_imp->m_line);

    m_imp->m_bStart = new QPushButton(tr("&Search"),this);
    m_imp->m_bStart->setDefault(true);
    m_imp->m_bStart->setAutoDefault(true);
    m_imp->m_bStart->setAutoExclusive(true);

    QHBoxLayout* slayout(new QHBoxLayout);
    slayout->addWidget(label);
    slayout->addWidget(m_imp->m_line);
    slayout->addWidget(m_imp->m_bStart);

    m_imp->m_details = new QTextBrowser(this);
    m_imp->m_details->setOpenLinks(false);

    m_imp->m_hSplit = new QSplitter(Qt::Horizontal,this);
    m_imp->m_hSplit->addWidget(m_imp->m_searchView);
    m_imp->m_hSplit->addWidget(m_imp->m_details);

    QVBoxLayout* vlayout(new QVBoxLayout);
    vlayout->addWidget(m_imp->m_hSplit);

    QVBoxLayout* mainlayout(new QVBoxLayout);
    mainlayout->addLayout(slayout);
    mainlayout->addLayout(vlayout);
    setLayout(mainlayout);

    m_imp->m_bar = new QProgressBar(this);
    m_imp->m_bar->setRange(0,100);
    mainWindow()->statusBar()->addPermanentWidget(m_imp->m_bar);
    m_imp->m_bar->hide();

    connect(m_imp->m_searchView,
            SIGNAL(clicked(const QModelIndex&)),
            this,
            SLOT(onViewUserInteraction(const QModelIndex&)));

    connect(m_imp->m_bStart,
            SIGNAL(clicked()),
            this,
            SLOT(onSearch()));

    connect(m_imp->m_line,
            SIGNAL(returnPressed()),
            this,
            SLOT(onSearch()));

    loadSettings();
    show();
    qDebug() << "SystemReportPage::SearchPage()" << this;
}

SearchPage::~SearchPage()
{
    qDebug() << "SearchPage::~SearchPage()";
    saveSettings();
    delete m_imp;
    qDebug() << "SearchPage::~SearchPage() done";
}

void
SearchPage::loadSettings()
{
    qDebug() << "SearchPage::loadSettings() - start";
    QSettings settings("/etc/perturbis/perturbis.conf",QSettings::IniFormat);
    settings.beginGroup("SearchPage");
    m_imp->m_hSplit->restoreState(settings.value("hSplit").toByteArray());
    settings.endGroup();
    qDebug() << "SearchPage::doneSettings() - done";
}

void
SearchPage::saveSettings()
{
    qDebug() << "SearchPage::saveSettings() - start";
    QSettings settings("/etc/perturbis/perturbis.conf",QSettings::IniFormat);
    settings.beginGroup("SearchPage");
    settings.setValue("hSplit", m_imp->m_hSplit->saveState());
    settings.endGroup();
    qDebug() << "SearchPage::saveSettings() - done";
}

void
SearchPage::restartFilters(const QSet<QString> & set)
{
    m_imp->m_searchFilterModel->setFilter(set);
    m_imp->m_searchFilterModel->invalidate();
}

void
SearchPage::activatePage()
{
    m_imp->m_line->setFocus(Qt::ActiveWindowFocusReason);
    if (outOfDate())
        refreshPage();
}

void
SearchPage::onViewUserInteraction(const QModelIndex & mix)
{
    QModelIndex ix(m_imp->m_searchFilterModel->mapToSource(mix));
    if (! ix.isValid())
        return;

    Package* package = static_cast<Package*>(ix.internalPointer());
    Q_ASSERT(package != 0);
    int column(ix.column());
    if (pho_install == column)
    {
        int mystate(mainWindow()->installSelections()->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
        if (mainWindow()->installSelections()->available(package,-1))
            mainWindow()->installSelections()->changeStates(package,mystate,pho_install);
        m_imp->m_searchModel->onUpdateModel();
    }
    else if (pho_deinstall == column)
    {
        int mystate(mainWindow()->deinstallSelections()->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
        if (mainWindow()->deinstallSelections()->available(package,pho_installed))
            mainWindow()->deinstallSelections()->changeStates(package, mystate,pho_deinstall);
        m_imp->m_searchModel->onUpdateModel();
    }
    else
    {
        details(package->ID());
    }
}

void
SearchPage::clearPage()
{
    m_imp->m_searchModel->clear();
    m_imp->m_details->clear();
}

void
SearchPage::details(const paludis::tr1::shared_ptr<const paludis::PackageID> & id)
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

void
SearchPage::displayDetails(QString mydetails)
{
    if (mydetails.isEmpty() ||
        this != mainWindow()->currentPage())
        return;
    m_imp->m_details->setText(mydetails);
    mainWindow()->onEndOfPaludisAction();
}

void
SearchPage::onSearch()
{
    QString query(m_imp->m_line->text().trimmed());
    if (query.isEmpty())
        return;

    SearchThread* searchThread = new SearchThread(this,
                                    mainWindow()->env(),
                                    mainWindow()->settingsPage()->m_queryView->m_model,
                                    mainWindow()->installSelections(),
                                    mainWindow()->deinstallSelections());

    connect(searchThread,
            SIGNAL(packageResult(Package*)),
            m_imp->m_searchModel,
            SLOT(appendPackage(Package*)));

    connect(searchThread,
            SIGNAL(finished(int)),
            this,
            SLOT(displaySearchFinished(int)));

    connect(searchThread,
            SIGNAL(progress(int)),
            m_imp->m_bar,
            SLOT(setValue(int)));

    searchThread->setup(query);
    m_imp->m_bStart->setText(tr("Stop"));
    mainWindow()->onStartOfPaludisAction();
    m_imp->m_searchModel->clear();
    mainWindow()->taskQueue()->enqueue(searchThread,true);
    m_imp->m_bar->show();
    mainWindow()->displayNotice(QString(tr("searching for %1...")).arg(query));
}

void
SearchPage::onSearchStopped()
{
    mainWindow()->displayNotice(tr("Search stopped"));
    mainWindow()->onEndOfPaludisAction();
    m_imp->m_bStart->setText(tr("&start"));
    m_imp->m_bar->hide();
}

void
SearchPage::displaySearchFinished(int count)
{
    mainWindow()->onEndOfPaludisAction();
    m_imp->m_bStart->setText(tr("&start"));
    m_imp->m_bar->hide();
    mainWindow()->displayNotice(tr("%1 packages found").arg(count));
}
