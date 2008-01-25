

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
#include "TaskQueue.hh"
#include <QModelIndex>
#include <QFile>
#include <QPushButton>
#include <QHeaderView>
#include <QLayout>
#include <QSplitter>
#include <QTreeView>
#include <QFont>
#include <QTextBrowser>

namespace pertubis
{
    struct SystemReportPagePrivate
    {
        SystemReportPagePrivate()
        {
        }
        QSplitter*              m_hSplit;
        QTextBrowser*           m_details;
        QTreeView*              m_reportView;
        ReportPackageModel*     m_reportModel;
    };
}

pertubis::SystemReportPage::SystemReportPage(MainWindow * main) :
        Page(main),
        m_imp(new SystemReportPagePrivate)
{
    m_imp->m_reportModel = new ReportPackageModel(this);
    m_imp->m_reportModel->setHorizontalHeaderLabels(QStringList() <<
            tr("deinstall") <<
            tr("package") <<
            tr("category") <<
            tr("version"));

    m_imp->m_reportView = new QTreeView(this);
    m_imp->m_reportView->setItemsExpandable(true);
    m_imp->m_reportView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_imp->m_reportView->setItemDelegateForColumn(rpho_deinstall, new ReportModelDelegate(this));
    m_imp->m_reportView->setModel(m_imp->m_reportModel);
    m_imp->m_reportView->header()->setVisible(true);
    m_imp->m_reportView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_imp->m_reportView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_imp->m_reportView->header()->setResizeMode(QHeaderView::ResizeToContents);

    QFont myfont(m_imp->m_reportView->font());
    myfont.setBold(true);
    m_imp->m_reportView->setFont(myfont);

    m_imp->m_details = new QTextBrowser(this);
    m_imp->m_details->setOpenLinks(false);

    m_imp->m_hSplit = new QSplitter(Qt::Horizontal,this);
    m_imp->m_hSplit->addWidget(m_imp->m_reportView);
    m_imp->m_hSplit->addWidget(m_imp->m_details);

    QPushButton* b(new QPushButton(tr("&Check"),this));

    QHBoxLayout* blayout(new QHBoxLayout);
    blayout->addStretch();
    blayout->addWidget(b);

    QHBoxLayout* hlayout(new QHBoxLayout);
    hlayout->addWidget(m_imp->m_hSplit);

    connect(b,
            SIGNAL(pressed()),
            this,
            SLOT(onSystemReport()));

    QVBoxLayout* mylayout(new QVBoxLayout);
    mylayout->addLayout(blayout);
    mylayout->addLayout(hlayout);
    setLayout(mylayout);
    show();

    qDebug() << "pertubis::SystemReportPage::SystemReportPage() end";
}

pertubis::SystemReportPage::~SystemReportPage()
{
    qDebug() << "pertubis::SearchPage::~SearchPage()";
    delete m_imp;
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
        int mystate(mainWindow()->deinstallSelections()->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
        mainWindow()->deinstallSelections()->changeStates(package, mystate,rpho_deinstall);
        m_imp->m_reportModel->onUpdateModel();
    }
    else if ( tk_glsa == package->tag() )
    {
        QFile file((package->data(rpho_last).toString()));
        if (! file.exists() )
            return;
        QXmlInputSource inputSource(&file);
        QXmlSimpleReader reader;
        GlsaParser handler(m_imp->m_details);
        reader.setContentHandler(&handler);
        reader.setErrorHandler(&handler);
        reader.parse(inputSource);
    }
    else
        details(package->ID());
}

void pertubis::SystemReportPage::displaySystemReportFinished(int total, int count)
{
    mainWindow()->onEndOfPaludisAction();
    m_imp->m_reportView->expandAll();
    mainWindow()->displayNotice(tr("%1 installed packages processed, %2 issues found").arg(total).arg(count));
}

void pertubis::SystemReportPage::activatePage()
{
    if (used() && outOfDate())
        refreshPage();
}

void pertubis::SystemReportPage::refreshPage()
{
    onSystemReport();
    setOutOfDate(false);
}

void pertubis::SystemReportPage::clearPage()
{
    m_imp->m_reportModel->clear();
    m_imp->m_details->clear();
    setUsed(false);
    setOutOfDate(false);
}

void pertubis::SystemReportPage::onSystemReport()
{
    m_imp->m_reportModel->clear();

    SystemReport* t(new SystemReport(this,mainWindow()->env(),mainWindow()->deinstallSelections()));

    connect(m_imp->m_reportView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onViewUserInteraction( const QModelIndex& )) );

    connect(t,
            SIGNAL(appendPackage(Package*)),
            m_imp->m_reportModel,
            SLOT(appendPackage(Package*)));

    connect(t,
            SIGNAL(finished(int,int)),
            this,
            SLOT(displaySystemReportFinished(int,int)));

    mainWindow()->onStartOfPaludisAction();
    mainWindow()->taskQueue()->enqueue(t,true);
    setUsed(true);
}

void pertubis::SystemReportPage::details(const paludis::tr1::shared_ptr<const paludis::PackageID> & id)
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

void pertubis::SystemReportPage::displayDetails(QString mydetails)
{
    if (mydetails.isEmpty() ||
        this != mainWindow()->currentPage())
        return;
    m_imp->m_details->setText(mydetails);
    mainWindow()->onEndOfPaludisAction();
}
