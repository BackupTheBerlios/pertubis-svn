

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

#include "DetailsThread.hh"
#include "SystemReportPage.hh"
#include "SystemReport.hh"
#include "ReportPackage.hh"
#include "ReportPackageModel.hh"
#include "ReportModelDelegate.hh"
#include "MainWindow.hh"
#include "GLSAParser.hh"
#include "InstallSelections.hh"
#include "DeinstallSelections.hh"
#include <QModelIndex>
#include <QFile>
#include <QPushButton>
#include <QHeaderView>
#include <QLayout>
#include <QSplitter>
#include <QTreeView>
#include <QFont>
#include <QTextBrowser>

pertubis::SystemReportPage::SystemReportPage(QWidget* pobj, MainWindow * mainWindow) :
        Page(pobj,mainWindow),
        m_reportThread(0)
{
    m_reportModel = new ReportPackageModel(this);
    m_reportModel->setHorizontalHeaderLabels(QStringList() <<
            tr("deinstall") <<
            tr("package") <<
            tr("category") <<
            tr("version"));

    m_reportView = new QTreeView(this);
    m_reportView->setItemsExpandable(true);
    m_reportView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_reportView->setItemDelegateForColumn(rpho_deinstall, new ReportModelDelegate(this));
    m_reportView->setModel(m_reportModel);
    m_reportView->header()->setVisible(true);
    m_reportView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_reportView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_reportView->header()->setResizeMode(QHeaderView::ResizeToContents);

    QFont myfont(m_reportView->font());
    myfont.setBold(true);
    m_reportView->setFont(myfont);

    m_details = new QTextBrowser(this);
    m_details->setOpenLinks(false);

    m_hSplit = new QSplitter(Qt::Horizontal,pobj);
    m_hSplit->addWidget(m_reportView);
    m_hSplit->addWidget(m_details);

    QPushButton* b(new QPushButton(tr("&Check"),this));

    QHBoxLayout* blayout(new QHBoxLayout);
    blayout->addStretch();
    blayout->addWidget(b);

    connect(b,
            SIGNAL(pressed()),
            this,
            SLOT(onSystemReport()));

    connect(m_mainWindow->m_detailsThread,
            SIGNAL(sendResult(QString)),
            this,
            SLOT(displayDetails(QString)));

    QVBoxLayout* mylayout(new QVBoxLayout);
    mylayout->addLayout(blayout);
    mylayout->addWidget(m_hSplit);
    setLayout(mylayout);

    qDebug() << "pertubis::SystemReportPage::SystemReportPage()" << pobj;
}

pertubis::SystemReportPage::~SystemReportPage()
{
    qDebug() << "pertubis::SearchPage::~SearchPage()";
    if (0 != m_reportThread &&
        !m_reportThread->isFinished())
    {
        m_reportThread->stopExec();
        m_reportThread->wait();
    }
    delete m_reportThread;
    qDebug() << "pertubis::SearchPage::~SearchPage() done";
}


void pertubis::SystemReportPage::onViewUserInteraction(const QModelIndex & ix)
{
    if (! ix.isValid())
        return;

    ReportPackage* package = static_cast<ReportPackage*>(ix.internalPointer());
    Q_ASSERT(package != 0);
    int column(ix.column());
    if (rpho_deinstall == column &&
        tk_normal == package->tag())
    {
        int mystate(m_mainWindow->m_deinstallSelections->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
        m_mainWindow->m_deinstallSelections->changeStates(package, mystate,rpho_deinstall);
        m_reportModel->onUpdateModel();
    }
    else if ( tk_glsa == package->tag() )
    {
        QFile file((package->data(rpho_last).toString()));
        if (! file.exists() )
            return;
        QXmlInputSource inputSource(&file);
        QXmlSimpleReader reader;
        GlsaParser handler(m_details);
        reader.setContentHandler(&handler);
        reader.setErrorHandler(&handler);
        reader.parse(inputSource);
    }
    else
        m_mainWindow->showDetails(package->ID());
}

void pertubis::SystemReportPage::displaySystemReportFinished(int total, int count)
{
    m_mainWindow->onEndOfPaludisAction();
    m_reportView->expandAll();
    m_mainWindow->displayNotice(tr("%1 installed packages processed, %2 issues found").arg(total).arg(count));
}

void pertubis::SystemReportPage::activatePage()
{
    if (m_dirty)
        onRefreshPage();
}

void pertubis::SystemReportPage::onRefreshPage()
{
    onSystemReport();
    m_dirty=false;
}

void pertubis::SystemReportPage::onSystemReport()
{
    m_reportModel->clear();
    if (m_reportThread != 0)
    {
        m_reportThread->stopExec();
        m_reportThread->wait();
        delete m_reportThread;
        m_reportThread = 0;
    }
    m_reportThread = new SystemReport(this,m_mainWindow->m_env,m_mainWindow->m_deinstallSelections);

    connect(m_reportView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onViewUserInteraction( const QModelIndex& )) );

    connect(m_reportThread,
            SIGNAL(appendPackage(Package*)),
            m_reportModel,
            SLOT(appendPackage(Package*)));

    connect(m_reportThread,
            SIGNAL(finished(int,int)),
            this,
            SLOT(displaySystemReportFinished(int,int)));

    m_mainWindow->onStartOfPaludisAction();
    m_reportThread->start();
}

void pertubis::SystemReportPage::displayDetails(QString details)
{
    if (details.isEmpty())
        return;
    m_details->setText(details);
    m_mainWindow->onEndOfPaludisAction();
}
