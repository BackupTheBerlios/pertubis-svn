
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

#include "MainWindow.hh"
#include "FormatterUtils.hh"
#include "PackageModelDelegate.hh"
#include "RepositoryPage.hh"
#include "MessageOutput.hh"
#include "SyncTask.hh"

#include "Settings.hh"

#include "RepositoryInfoModel.hh"
#include "RepositoryListModel.hh"

#include <QAction>
#include <QPixmap>
#include <QFont>
#include <QLineEdit>
#include <QHeaderView>
#include <QLayout>
#include <QDebug>
#include <QMessageBox>
#include <QModelIndex>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QTableView>

pertubis::RepositoryPage::RepositoryPage(QWidget* pobj, MainWindow * mainWindow) :
        Page(pobj,mainWindow)
{
    m_repoListModel = new RepositoryListModel(this,m_mainWindow->m_firstrun);
    m_repoListModel->setHorizontalHeaderLabels(QStringList() <<
            tr("repository") << tr("filter on"));

    qDebug() << "pertubis::MainWindow::createRepositoryView() 1";

    m_repoListView = new QTableView(this);
    m_repoListView->setModel(m_repoListModel);
    m_repoListView->horizontalHeader()->setVisible(false);
    m_repoListView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    QFont myfont(m_repoListView->font());
    myfont.setBold(true);
    m_repoListView->setFont(myfont);

    QPushButton* acSync = new QPushButton(QPixmap(":images/sync0.png"),tr("sync"),this);
    acSync->setToolTip(html_tooltip(tr("To get the latest releases and bugfixes it is neccessary to update the package database.<br><br>It is sufficient to sync your repositories once a day"),acSync->text()));

    qDebug() << "pertubis::MainWindow::createRepositoryView()";
    m_repoInfoModel = new RepositoryInfoModel(this);
    m_repoInfoModel->setHorizontalHeaderLabels(QStringList() <<
            tr("name") <<
            tr("state"));

    m_repoInfoView = new QTableView(this);
    m_repoInfoView->setModel(m_repoInfoModel);
    m_repoInfoView->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    m_repoInfoView->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
    m_repoInfoView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_repoInfoView->horizontalHeader()->setVisible(true);
    m_repoInfoView->verticalHeader()->setVisible(false);

    QPalette p(m_repoInfoView->palette());
    p.setColor(QPalette::Base,QColor(170,170,170)); // background color  = black
    m_repoInfoView->setPalette(p);

    qDebug() << "pertubis::MainWindow::createRepositoryView() 2";

    m_repoListView->verticalHeader()->setVisible(false);
    m_repoListView->setShowGrid(false);

    m_repoListThread = new RepositoryListThread(this,m_mainWindow->m_env);
    m_repoInfoThread = new RepositoryInfoThread(this,m_mainWindow->m_env);

    QHBoxLayout* buttonLayout(new QHBoxLayout);
    buttonLayout->addStretch();
    buttonLayout->addWidget(acSync);

    QHBoxLayout* viewLayout(new QHBoxLayout);
    viewLayout->addWidget(m_repoListView);
    viewLayout->addWidget(m_repoInfoView);

    QVBoxLayout* mainLayout(new QVBoxLayout);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(viewLayout);

    setLayout(mainLayout);

    m_syncTask = new PertubisSyncTask(m_mainWindow->m_env,this);

    connect(m_repoListThread,
            SIGNAL(sendNames(QStringList)),
            m_repoListModel,
            SLOT(slotResult(QStringList)));

    connect(acSync,
            SIGNAL(pressed()),
            this,
            SLOT(onSync()));

    connect(m_repoListThread,
            SIGNAL(finished()),
            this,
            SLOT(restartFilters()));

    connect(m_repoListView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onRepositoryChanged( const QModelIndex& )) );

    connect(m_repoInfoThread,
            SIGNAL(sendResult(const QList<QVariantList>&)),
            m_repoInfoModel,
            SLOT(slotResult(const QList<QVariantList>&)));

    connect(m_syncTask,
            SIGNAL(sendMessage(QString)),
            m_mainWindow->m_output,
            SLOT(append(QString)));

    connect(m_syncTask,
            SIGNAL(finished()),
            this,
            SLOT(displaySyncFinished()));

    m_repoListThread->start();

    qDebug() << "pertubis::SystemReportPage::RepositoryPage()" << pobj;
}

pertubis::RepositoryPage::~RepositoryPage()
{
    qDebug() << "pertubis::RepositoryPage::~RepositoryPage()";
    if (!m_repoListThread->isFinished())
    {
        m_repoListThread->stopExec();
        m_repoListThread->wait();
    }
    if (!m_repoInfoThread->isFinished())
    {
        m_repoInfoThread->stopExec();
        m_repoInfoThread->wait();
    }
    delete m_repoListModel;
    delete m_repoInfoModel;
}

void pertubis::RepositoryPage::restartFilters()
{
    qDebug() << "pertubis::RepositoryPage::restartFilters()" << m_repoListModel->activeRepositories();
    emit sendFilters(m_repoListModel->activeRepositories());
}


void pertubis::RepositoryPage::displaySyncFinished()
{
    m_mainWindow->onEndOfPaludisAction();
//     m_timer->stop();
    m_mainWindow->m_sysTray->setIcon( QPixmap(":images/logo.png"));
    QMessageBox q(QMessageBox::NoIcon,
                  tr("information"),
                     tr("syncing repositories finished"),
                        QMessageBox::Ok,
                        this);
    q.setIconPixmap(QPixmap(":images/repobar.png"));
    q.setDefaultButton(QMessageBox::Ok);
    q.exec();
}



void pertubis::RepositoryPage::activatePage()
{
}

void pertubis::RepositoryPage::onRepositoryChanged( const QModelIndex& index )
{
    if ( !index.isValid())
        return;

    if (rlmho_on == index.column())
    {
        int state = m_repoListModel->data(index,Qt::CheckStateRole).toInt();
        m_repoListModel->setData(index, (state == Qt::Checked ) ? Qt::Unchecked :Qt::Checked);
        restartFilters();
//         statusBar()->showMessage(tr("%1 packages found").arg(m_packageFilterModel->rowCount()));
    }
    else
    {
        QString repo = m_repoListModel->data(index).toString();
        m_repoInfoThread->start(repo);
    }
}


void pertubis::RepositoryPage::onSync()
{
//     if (!rootTest(tr("This feature is only available for system administrators")))
//         return;

    if (m_syncTask->isRunning())
    {
        QMessageBox::warning(this,
                             tr("Request denied"),
                                "You are already syncing the repositories",
                                QMessageBox::Ok,
                                QMessageBox::Ok);
        return;
    }

    QSet<QString> repos(m_repoListModel->activeRepositories());
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
    q.setIconPixmap(QPixmap(":images/repoba.png"));
    q.setDefaultButton(QMessageBox::Ok);
    if (QMessageBox::Cancel == q.exec())
        return;
    m_mainWindow->setPage(m_mainWindow->m_output);
//     m_timer->start(1000);
    m_mainWindow->onStartOfPaludisAction();
    m_syncTask->start(repos);
}

