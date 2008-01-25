
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

#include "DeinstallSelections.hh"
#include "DeinstallSettings.hh"
#include "DepListSettings.hh"
#include "DetailsThread.hh"
#include "FormatterUtils.hh"
#include "InstallSelections.hh"
#include "InstallSettings.hh"
#include "MainWindow.hh"
#include "MessageOutput.hh"
#include "Package.hh"
#include "PertubisDeinstallTask.hh"
#include "PertubisInstallTask.hh"
#include "SelectionModelDelegate.hh"
#include "SelectionModel.hh"
#include "SelectionPage.hh"
#include "Settings.hh"
#include "TaskQueue.hh"

#include <QTreeView>
#include <QModelIndex>
#include <QHeaderView>
#include <QMessageBox>
#include <QLayout>
#include <QThread>
#include <QPushButton>

#include <QSplitter>
#include <QSettings>
#include <QFont>
#include <QTextBrowser>


namespace pertubis
{
    struct SelectionPagePrivate
    {
        public:
        SelectionPagePrivate() :
            m_installTask(0),
            m_deinstallTask(0)
        {
        }

        ~SelectionPagePrivate()
        {
        }

        SelectionModel*           m_selectionModel;
        QTreeView*                m_selectionView;
        QSplitter*                m_hSplit;
        QTextBrowser*             m_details;
        PertubisInstallTask*      m_installTask;
        PertubisDeinstallTask*    m_deinstallTask;
    };
}

pertubis::SelectionPage::SelectionPage(MainWindow * main) :
        Page(main),
        m_imp(new SelectionPagePrivate)
{
    m_imp->m_selectionModel = new SelectionModel(this);
    m_imp->m_selectionModel->setHorizontalHeaderLabels(QStringList() <<
            tr("+") <<
            tr("-") <<
            tr("=") <<
            tr("package") <<
            tr("category") <<
            tr("old version") <<
            tr("?") <<
            tr("new version") <<
            tr("repository"));
    m_imp->m_selectionView = new QTreeView(this);
    m_imp->m_selectionView->setItemsExpandable(true);
    m_imp->m_selectionView->setItemDelegate(new SelectionModelDelegate(this,spho_install,spho_deinstall,spho_installed));
    m_imp->m_selectionView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_imp->m_selectionView->setModel(m_imp->m_selectionModel);
    m_imp->m_selectionView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_imp->m_selectionView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_imp->m_selectionView->header()->setVisible(true);
    m_imp->m_selectionView->header()->setResizeMode(QHeaderView::ResizeToContents);

    QFont myfont(m_imp->m_selectionView->font());
    myfont.setBold(true);
    m_imp->m_selectionView->setFont(myfont);

    m_imp->m_details = new QTextBrowser(this);
    m_imp->m_details->setOpenLinks(false);

    QPushButton* acShow = new QPushButton(QPixmap(":images/selections.png"),tr("start pretend"),this);
    acShow->setToolTip(html_tooltip(tr("Control your install and deinstall selections and watch out for blocks and errors!"),acShow->text()));

    QPushButton* acStart = new QPushButton(QPixmap(":images/install.png"),tr("start"),this);
    acShow->setToolTip(html_tooltip(tr("Control your install and deinstall selections and watch out for blocks and errors!"),acShow->text()));

    QPushButton* acUnselectAll = new QPushButton(tr("unselect all"),this);
    acUnselectAll->setToolTip(html_tooltip(tr("Clears all selections you made"),acUnselectAll->text()));

    QHBoxLayout* buttonLayout(new QHBoxLayout);
    buttonLayout->addStretch();
    buttonLayout->addWidget(acUnselectAll);
    buttonLayout->addWidget(acShow);
    buttonLayout->addWidget(acStart);

    m_imp->m_hSplit = new QSplitter(Qt::Horizontal,this);
    m_imp->m_hSplit->addWidget(m_imp->m_selectionView);
    m_imp->m_hSplit->addWidget(m_imp->m_details);

    QHBoxLayout* hLayout(new QHBoxLayout);
    hLayout->addWidget(m_imp->m_hSplit);

    QVBoxLayout* mainLayout(new QVBoxLayout);
    mainLayout->setMargin(0);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(hLayout);
    setLayout(mainLayout);

    connect(m_imp->m_selectionView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(selectionViewUserInteraction( const QModelIndex& )) );

    connect(acShow,
            SIGNAL(pressed()),
            this,
            SLOT(startPretend()));

    connect(acStart,
            SIGNAL(pressed()),
            this,
            SLOT(start()));

    connect(acUnselectAll,
            SIGNAL(pressed()),
            this,
            SLOT(unselectAll()));

    loadSettings();
    show();
    qDebug() << "pertubis::SystemReportPage::SelectionPage() end";
}

pertubis::SelectionPage::~SelectionPage()
{
    qDebug() << "pertubis::SelectionPage::~SelectionPage()";
    saveSettings();
    delete m_imp;
}

void pertubis::SelectionPage::loadSettings()
{
    qDebug() << "pertubis::SelectionPage::loadSettings() - start";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "SelectionPage" );
    m_imp->m_hSplit->restoreState(settings.value("hSplit").toByteArray());
    settings.endGroup();
    qDebug() << "pertubis::SelectionPage::doneSettings() - done";
}

void pertubis::SelectionPage::saveSettings()
{
    qDebug() << "pertubis::SelectionPage::saveSettings() - start";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "SelectionPage" );
    settings.setValue("hSplit", m_imp->m_hSplit->saveState());
    settings.endGroup();
    qDebug() << "pertubis::SelectionPage::saveSettings() - done";
}

void pertubis::SelectionPage::selectionViewUserInteraction(const QModelIndex & ix)
{
    if (! ix.isValid())
        return;
    Package* package = static_cast<Package*>(ix.internalPointer());

    Q_ASSERT(package != 0);
    int column(ix.column());
    if (spho_install == column )
    {
        int mystate(mainWindow()->installSelections()->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
        if (mainWindow()->installSelections()->available(package,-1))
            mainWindow()->installSelections()->changeStates(package,mystate,spho_install);
        m_imp->m_selectionModel->onUpdateModel();
    }
    else if (spho_deinstall == column )
    {
        int mystate(mainWindow()->deinstallSelections()->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
        if (mainWindow()->deinstallSelections()->available(package,spho_installed) )
            mainWindow()->deinstallSelections()->changeStates(package, mystate, spho_deinstall);
        m_imp->m_selectionModel->onUpdateModel();
    }
    else
    {
        details(package->ID());
    }
}

void pertubis::SelectionPage::activatePage()
{
    if (used() && outOfDate())
        refreshPage();
}

void pertubis::SelectionPage::clearPage()
{
    m_imp->m_selectionModel->clear();
    m_imp->m_details->clear();
    // perhaps that's now too much clearing, but trying it.
    unselectAll();
    setUsed(false);
    setOutOfDate(false);
}

void pertubis::SelectionPage::unselectAll()
{
    mainWindow()->installSelections()->clear();
    mainWindow()->deinstallSelections()->clear();
    mainWindow()->setAllPagesDirty();
    if (outOfDate())
    {
        start();
        setOutOfDate(false);
    }
}

void pertubis::SelectionPage::refreshPage()
{
    qDebug() << "pertubis::SelectionPage::refreshPage()";
    m_imp->m_selectionModel->clear();
    m_imp->m_details->clear();
    setOutOfDate(false);
    setUsed(false);
}

void pertubis::SelectionPage::setupDeinstallTask(bool pretend)
{
    qDebug() << "pertubis::SelectionPage::setupDeinstallTask() - start";

    if (0 < mainWindow()->deinstallSelections()->entryCount())
    {
        if (m_imp->m_deinstallTask != 0 &&
            m_imp->m_deinstallTask->isRunning())
            return;
        if (m_imp->m_deinstallTask != 0)
            delete m_imp->m_deinstallTask;

        m_imp->m_deinstallTask = new PertubisDeinstallTask(this,mainWindow()->env(),mainWindow()->installSelections(),mainWindow()->deinstallSelections());
        m_imp->m_deinstallTask->set_pretend(mainWindow()->settingsPage()->m_installView->m_model->m_pretend);
        m_imp->m_deinstallTask->set_no_config_protect(mainWindow()->settingsPage()->m_installView->m_model->m_config);
        m_imp->m_deinstallTask->set_preserve_world(mainWindow()->settingsPage()->m_installView->m_model->m_preserve);
        m_imp->m_deinstallTask->set_with_unused_dependencies(mainWindow()->settingsPage()->m_deinstallView->m_model->m_unusedDeps);
        m_imp->m_deinstallTask->set_with_dependencies(mainWindow()->settingsPage()->m_deinstallView->m_model->m_deps);
        m_imp->m_deinstallTask->set_check_safety(! mainWindow()->settingsPage()->m_deinstallView->m_model->m_unsafeUninstall);
        m_imp->m_deinstallTask->set_all_versions( mainWindow()->settingsPage()->m_deinstallView->m_model->m_allVersions);
        m_imp->m_deinstallTask->set_pretend(pretend);

        connect(m_imp->m_deinstallTask,
                SIGNAL(appendPackage(Package*)),
                m_imp->m_selectionModel,
                SLOT(appendPackage(Package*)));

        connect(m_imp->m_deinstallTask,
                SIGNAL(finished()),
                this,
                SLOT(deinstallTaskFinished()));

        for (paludis::PackageIDSet::ConstIterator i(mainWindow()->deinstallSelections()->entriesBegin()), i_end(mainWindow()->deinstallSelections()->entriesEnd());
             i != i_end ; ++i)
        {
            std::string target("=" + stringify((*i)->name()) + "-" + (*i)->canonical_form(paludis::idcf_version));
            m_imp->m_deinstallTask->add_target(target);
        }
        mainWindow()->taskQueue()->enqueue(m_imp->m_deinstallTask,true);
    }
    qDebug() << "pertubis::SelectionPage::setupDeinstallTask() - done";
}

void pertubis::SelectionPage::setupInstallTask(bool pretend, QString target)
{
    qDebug() << "pertubis::SelectionPage::setupInstallTask()";

    if (! target.isEmpty() ||
          0 < mainWindow()->installSelections()->entryCount())
    {
        if (m_imp->m_installTask != 0 &&
            m_imp->m_installTask->isRunning())
            return;
        if (m_imp->m_installTask != 0)
            delete m_imp->m_installTask;

        paludis::DepListOptions options;
        mainWindow()->settingsPage()->m_depListView->populate_dep_list_options(mainWindow()->env().get(),options);
        m_imp->m_installTask = new PertubisInstallTask(this,mainWindow()->env(),options,mainWindow()->env()->default_destinations(),mainWindow()->installSelections(),mainWindow()->deinstallSelections());
        mainWindow()->settingsPage()->m_installView->m_model->populate_install_task(mainWindow()->env().get(),*m_imp->m_installTask);
        mainWindow()->settingsPage()->m_depListView->populate_install_task(mainWindow()->env().get(),*m_imp->m_installTask);
        m_imp->m_installTask->set_pretend(pretend);

        connect(m_imp->m_installTask,
                SIGNAL(appendPackage(Package*)),
                m_imp->m_selectionModel,
                SLOT(appendPackage(Package*)));

        connect(m_imp->m_installTask,
                SIGNAL(finished()),
                this,
                SLOT(installTaskFinished()));

        if ( !target.isEmpty() )
        {
            try
            {
                m_imp->m_installTask->add_target(target.toStdString());
                mainWindow()->taskQueue()->enqueue(m_imp->m_installTask,true);
            }
            catch(...)
            {
            }
        }
        else if (0 < mainWindow()->installSelections()->entryCount() )
        {
            for (paludis::PackageIDSet::ConstIterator i(mainWindow()->installSelections()->entriesBegin()), i_end(mainWindow()->installSelections()->entriesEnd());
                 i != i_end ; ++i)
            {
                m_imp->m_installTask->add_exact_package(*i);
            }
            mainWindow()->taskQueue()->enqueue(m_imp->m_installTask,true);
        }
    }
    qDebug() << "pertubis::SelectionPage::setupInstallTask() - done";
}


void pertubis::SelectionPage::startPretend()
{
    m_imp->m_selectionModel->clear();
    mainWindow()->messagePage()->clearPage();
    setupDeinstallTask(true);
    setupInstallTask(true,"");
}

void pertubis::SelectionPage::start()
{
    m_imp->m_selectionModel->clear();
    mainWindow()->messagePage()->clearPage();
    mainWindow()->setPage(mainWindow()->messagePage());
    setupDeinstallTask(false);
    setupInstallTask(false,"");
    setUsed(true);
}

// void pertubis::SelectionPage::taskFinished()
// {
//     QMessageBox q(QMessageBox::NoIcon,
//                 tr("Finished"),
//                 tr("all tasks finished"),
//                 QMessageBox::Ok,
//                 this);
//     q.setIconPixmap(QPixmap(":images/run.png"));
//     q.setDefaultButton(QMessageBox::Ok);
//     q.exec();
//     mainWindow()->onInvalidate();
// }

void pertubis::SelectionPage::installTaskFinished()
{
    mainWindow()->installSelections()->clear();
}

void pertubis::SelectionPage::deinstallTaskFinished()
{
    mainWindow()->deinstallSelections()->clear();
}

void pertubis::SelectionPage::details(const paludis::tr1::shared_ptr<const paludis::PackageID> & id)
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

void pertubis::SelectionPage::displayDetails(QString mydetails)
{
    if (mydetails.isEmpty() ||
        this != mainWindow()->currentPage())
        return;
    m_imp->m_details->setText(mydetails);
    mainWindow()->onEndOfPaludisAction();
    setUsed(true);
}
