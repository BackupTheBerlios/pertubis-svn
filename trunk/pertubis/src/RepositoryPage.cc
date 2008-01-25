
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



#include "FormatterUtils.hh"
#include "MainWindow.hh"
#include "MessageOutput.hh"
#include "PackageModelDelegate.hh"
#include "RepositoryInfoModel.hh"
#include "RepositoryListModel.hh"
#include "RepositoryPage.hh"
#include "Settings.hh"
#include "TaskQueue.hh"


#include <paludis/util/stringify.hh>
#include <paludis/syncer.hh>
#include <paludis/sync_task.hh>

#include <QDebug>
#include <QFont>
#include <QObject>
#include <QHeaderView>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QModelIndex>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QTableView>

#include <iostream>

namespace pertubis
{
    struct RepositoryPagePrivate
    {
        ~RepositoryPagePrivate()
        {
            delete m_repoListModel;
            delete m_repoInfoModel;
        }

        QTableView*             m_repoListView;
        QTableView*             m_repoInfoView;
        RepositoryListModel*    m_repoListModel;
        RepositoryInfoModel*    m_repoInfoModel;
        QSplitter*              m_hSplit;
    };
};

namespace
{
    class PertubisSyncTask :
        public pertubis::ThreadBase,
        public paludis::SyncTask
    {
        public:
            PertubisSyncTask(paludis::tr1::shared_ptr<paludis::Environment> myenv,
                const QSet<QString>& repos,
                QObject* pobj) :
                ThreadBase(pobj,paludis::tr1::shared_ptr<paludis::Environment>()),
                SyncTask(myenv.get(),true)
            {
                for (QSet<QString>::const_iterator rStart(repos.constBegin()),rEnd(repos.constEnd());
                    rStart != rEnd; ++rStart)
                    add_target(rStart->toStdString());
            }

            void on_sync_all_pre()
            {
            }

            void on_sync_pre(const paludis::RepositoryName & r)
            {
                std::cout << "Sync " << "\e[35;1m" << stringify(r) << "\e[0;0m" << std::endl;
            }

            void on_sync_post(const paludis::RepositoryName &r)
            {
                std::cout << "Sync " << "\e[35;1m" << stringify(r) << "\e[0;0m" << " finished" << std::endl;
            }

            void on_sync_skip(const paludis::RepositoryName & r)
            {
                std::cout << "Sync " << "\e[35;1m" << stringify(r) << "\e[0;0m" << " skipped" << std::endl;
            }

            void on_sync_succeed(const paludis::RepositoryName & r)
            {
                std::cout << "Sync "<< "\e[35;1m" << stringify(r) << "\e[0;0m" << " completed" << std::endl;
            }

            void on_sync_fail(const paludis::RepositoryName & /*r*/, const paludis::SyncFailedError & e)
            {
                std::cout << "\e[31;1m" << "Sync error " << e.backtrace("<br>  * ") << e.message() << std::endl;
            }

            void on_sync_all_post()
            {
                std::cout << "Sync complete" << std::endl;
            }

            void on_sync_status(int, int, int)
            {
            }

        protected:

            /// overloaded from QThread
            void run()
            {
                execute();
            }
    };
}


pertubis::RepositoryPage::RepositoryPage(MainWindow * main) :
        Page(main),
        m_imp(new RepositoryPagePrivate)
{
    m_imp->m_repoListModel = new RepositoryListModel(this,mainWindow()->firstRun());
    m_imp->m_repoListModel->setHorizontalHeaderLabels(QStringList() <<
            tr("repository") << tr("filter on"));

    qDebug() << "pertubis::MainWindow::createRepositoryView() 1";

    m_imp->m_repoListView = new QTableView(this);
    m_imp->m_repoListView->setModel(m_imp->m_repoListModel);
    m_imp->m_repoListView->horizontalHeader()->setVisible(false);
    m_imp->m_repoListView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_imp->m_repoListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_imp->m_repoListView->setSelectionMode(QAbstractItemView::SingleSelection);
    QFont myfont(m_imp->m_repoListView->font());
    myfont.setBold(true);
    m_imp->m_repoListView->setFont(myfont);

    QPushButton* acSync = new QPushButton(QPixmap(":images/sync0.png"),tr("sync"),this);
    acSync->setToolTip(html_tooltip(tr("To get the latest releases and bugfixes it is neccessary to update the package database.<br><br>It is sufficient to sync your repositories once a day"),acSync->text()));

    qDebug() << "pertubis::MainWindow::createRepositoryView()";
    m_imp->m_repoInfoModel = new RepositoryInfoModel(this);
    m_imp->m_repoInfoModel->setHorizontalHeaderLabels(QStringList() <<
            tr("name") <<
            tr("state"));

    m_imp->m_repoInfoView = new QTableView(this);
    m_imp->m_repoInfoView->setModel(m_imp->m_repoInfoModel);
    m_imp->m_repoInfoView->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    m_imp->m_repoInfoView->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
    m_imp->m_repoInfoView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_imp->m_repoInfoView->horizontalHeader()->setVisible(true);
    m_imp->m_repoInfoView->verticalHeader()->setVisible(false);

    QPalette p(m_imp->m_repoInfoView->palette());
    p.setColor(QPalette::Base,QColor(170,170,170)); // background color  = black
    m_imp->m_repoInfoView->setPalette(p);

    qDebug() << "pertubis::MainWindow::createRepositoryView() 2";

    m_imp->m_repoListView->verticalHeader()->setVisible(false);
    m_imp->m_repoListView->setShowGrid(false);


    QHBoxLayout* buttonLayout(new QHBoxLayout);
    buttonLayout->addStretch();
    buttonLayout->addWidget(acSync);

    m_imp->m_hSplit = new QSplitter(Qt::Horizontal,this);
    m_imp->m_hSplit->addWidget(m_imp->m_repoListView);
    m_imp->m_hSplit->addWidget(m_imp->m_repoInfoView);

    QVBoxLayout* vLayout(new QVBoxLayout);
    vLayout->addWidget(m_imp->m_hSplit);

    QVBoxLayout* mainLayout(new QVBoxLayout(this));
    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(vLayout);

    connect(acSync,
            SIGNAL(pressed()),
            this,
            SLOT(onSync()));

    connect(m_imp->m_repoListView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onRepositoryChanged( const QModelIndex& )) );

    loadSettings();
    show();
    refreshPage();

    qDebug() << "pertubis::SystemReportPage::RepositoryPage()" << this;
}

pertubis::RepositoryPage::~RepositoryPage()
{
    qDebug() << "pertubis::RepositoryPage::~RepositoryPage()";
    saveSettings();
    delete m_imp;
}

void pertubis::RepositoryPage::loadSettings()
{
    qDebug() << "pertubis::RepositoryPage::loadSettings() - start";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "RepositoryPage" );
    m_imp->m_hSplit->restoreState(settings.value("hSplit").toByteArray());
    settings.endGroup();
    qDebug() << "pertubis::RepositoryPage::doneSettings() - done";
}

void pertubis::RepositoryPage::saveSettings()
{
    qDebug() << "pertubis::RepositoryPage::saveSettings() - start";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "RepositoryPage" );
    settings.setValue("hSplit", m_imp->m_hSplit->saveState());
    settings.endGroup();
    qDebug() << "pertubis::RepositoryPage::saveSettings() - done";
}

void pertubis::RepositoryPage::activatePage()
{
    if (used() && outOfDate() )
        refreshPage();
}

void pertubis::RepositoryPage::refreshPage()
{
    m_imp->m_repoInfoModel->clear();
    m_imp->m_repoListModel->clear();
    RepositoryListThread* t(new RepositoryListThread(this,mainWindow()->env()));
    connect(t,
            SIGNAL(sendNames(QStringList)),
            m_imp->m_repoListModel,
            SLOT(slotResult(QStringList)));
    connect(t,
            SIGNAL(finished()),
            this,
            SLOT(restartFilters()));
    mainWindow()->taskQueue()->enqueue(t,true);
    setOutOfDate(false);
}

void pertubis::RepositoryPage::clearPage()
{
    m_imp->m_repoListModel->clear();
    m_imp->m_repoInfoModel->clear();
}

void pertubis::RepositoryPage::restartFilters()
{
    qDebug() << "pertubis::RepositoryPage::restartFilters()" << m_imp->m_repoListModel->activeRepositories();
    emit sendFilters(m_imp->m_repoListModel->activeRepositories());
}

void pertubis::RepositoryPage::displaySyncFinished()
{
    mainWindow()->onEndOfPaludisAction();
//     m_timer->stop();
//     mainWindow()->m_sysTray->setIcon( QPixmap(":images/logo.png"));
    QMessageBox q(QMessageBox::NoIcon,
                tr("information"),
                tr("syncing repositories finished"),
                QMessageBox::Ok,
                this);
    q.setIconPixmap(QPixmap(":images/repobar.png"));
    q.setDefaultButton(QMessageBox::Ok);
    q.exec();
}

void pertubis::RepositoryPage::onRepositoryChanged( const QModelIndex& index )
{
    if ( !index.isValid())
        return;

    if (rlmho_on == index.column())
    {
        int state = m_imp->m_repoListModel->data(index,Qt::CheckStateRole).toInt();
        m_imp->m_repoListModel->setData(index, (state == Qt::Checked ) ? Qt::Unchecked :Qt::Checked);
        restartFilters();
//         statusBar()->showMessage(tr("%1 packages found").arg(m_packageFilterModel->rowCount()));
    }
    else
    {
        RepositoryInfoThread* t = new RepositoryInfoThread(this,mainWindow()->env());
        connect(t,
                SIGNAL(sendResult(const QList<QVariantList>&)),
                m_imp->m_repoInfoModel,
                SLOT(slotResult(const QList<QVariantList>&)));

        QString repo = m_imp->m_repoListModel->data(index).toString();
        if (repo.isEmpty())
            return;
        t->setup(repo);
        mainWindow()->taskQueue()->enqueue(t,true);
    }
}

void pertubis::RepositoryPage::onSync()
{
    QSet<QString> repos(m_imp->m_repoListModel->activeRepositories());
    QString text;
    for (QSet<QString>::const_iterator sStart(repos.constBegin()),sEnd(repos.constEnd());
         sStart != sEnd;++sStart)
    {
        text.append(QString("* %1\n").arg(*sStart));
    }

    QMessageBox q(QMessageBox::NoIcon,
                tr("Question"),
                tr("Do you really want syncing the following repositories?\n\n%1").arg(text),
                QMessageBox::Ok | QMessageBox::Cancel,
                this);
    q.setIconPixmap(QPixmap(":images/repobar.png"));
    q.setDefaultButton(QMessageBox::Ok);
    if (QMessageBox::Cancel == q.exec())
        return;

    PertubisSyncTask* t(new PertubisSyncTask(mainWindow()->env(),repos,this));

    connect(t,
            SIGNAL(finished()),
            this,
            SLOT(displaySyncFinished()));
    mainWindow()->setPage(mainWindow()->messagePage());
    mainWindow()->onStartOfPaludisAction();
    mainWindow()->taskQueue()->enqueue(t,true);
}
