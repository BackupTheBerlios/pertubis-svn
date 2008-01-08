
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
#include "SelectionPage.hh"
#include "Package.hh"
#include "MainWindow.hh"
#include "FormatterUtils.hh"
#include "MessageOutput.hh"
#include "DeinstallSelections.hh"
#include "InstallSelections.hh"
#include "SelectionModel.hh"
#include "SelectionModelDelegate.hh"
#include <QTreeView>
#include <QModelIndex>
#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include <QSplitter>
#include <QFont>
#include <QTextBrowser>

pertubis::SelectionPage::SelectionPage(QWidget* pobj, MainWindow * mainWindow) :
        Page(pobj,mainWindow)
{
    m_selectionModel = new SelectionModel(this);
    m_selectionModel->setHorizontalHeaderLabels(QStringList() <<
            tr("+") <<
            tr("-") <<
            tr("=") <<
            tr("package") <<
            tr("category") <<
            tr("old version") <<
            tr("?") <<
            tr("new version") <<
            tr("repository"));
    m_selectionView = new QTreeView(this);
    m_selectionView->setItemsExpandable(true);
    m_selectionView->setItemDelegate(new SelectionModelDelegate(this,spho_install,spho_deinstall,spho_installed));
    m_selectionView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_selectionView->setModel(m_selectionModel);
    m_selectionView->header()->setVisible(true);
    m_selectionView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_selectionView->setSelectionBehavior(QAbstractItemView::SelectRows);

    QPushButton* acShow = new QPushButton(QPixmap(":images/selections.png"),tr("start"),this);
    acShow->setToolTip(html_tooltip(tr("Control your install and deinstall selections and watch out for blocks and errors!"),acShow->text()));

    QFont myfont(m_selectionView->font());
    myfont.setBold(true);
    m_selectionView->setFont(myfont);

    m_details = new QTextBrowser(this);
    m_details->setOpenLinks(false);

    m_hSplit = new QSplitter(Qt::Horizontal,pobj);
    m_hSplit->addWidget(m_selectionView);
    m_hSplit->addWidget(m_details);

    QHBoxLayout* buttonLayout(new QHBoxLayout);
    buttonLayout->addStretch();
    buttonLayout->addWidget(acShow);

    QHBoxLayout* viewLayout(new QHBoxLayout);
    viewLayout->addWidget(m_selectionView);
    viewLayout->addWidget(m_details);

    QVBoxLayout* mainLayout(new QVBoxLayout);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(viewLayout);

    setLayout(mainLayout);

    connect(m_selectionView,
            SIGNAL(clicked( const QModelIndex&)),
            this,
            SLOT(onSelectionViewUserInteraction( const QModelIndex& )) );

    connect(acShow,
            SIGNAL(pressed()),
            this,
            SLOT(onShowSelections()));

    connect(m_mainWindow->m_detailsThread,
            SIGNAL(sendResult(QString)),
            this,
            SLOT(displayDetails(QString)));

    qDebug() << "pertubis::SystemReportPage::SelectionPage()" << pobj;
}

pertubis::SelectionPage::~SelectionPage()
{
    qDebug() << "pertubis::SelectionPage::~SelectionPage()";
}

void pertubis::SelectionPage::onSelectionViewUserInteraction(const QModelIndex & ix)
{
    if (! ix.isValid())
        return;
    Package* package = static_cast<Package*>(ix.internalPointer());

    Q_ASSERT(package != 0);
    int column(ix.column());
    if (spho_install == column )
    {
        int mystate(m_mainWindow->m_installSelections->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
        if (m_mainWindow->m_installSelections->available(package,-1))
            m_mainWindow->m_installSelections->changeStates(package,mystate,spho_install);
        m_selectionModel->onUpdateModel();
    }
    else if (spho_deinstall == column )
    {
        int mystate(m_mainWindow->m_deinstallSelections->hasEntry(package->ID()) ? Qt::Unchecked : Qt::Checked);
        if (m_mainWindow->m_deinstallSelections->available(package,spho_installed) )
            m_mainWindow->m_deinstallSelections->changeStates(package, mystate, spho_deinstall);
        m_selectionModel->onUpdateModel();
    }
    else
    {
        m_mainWindow->showDetails(package->ID());
    }
}

void pertubis::SelectionPage::activatePage()
{
}

void pertubis::SelectionPage::onShowSelections()
{
    m_mainWindow->onStartOfPaludisAction();
    m_selectionModel->clear();
    m_mainWindow->m_output->clear();
    m_mainWindow->setPage(m_selectionView);
    m_mainWindow->startDeinstallTask(true);
    m_mainWindow->startInstallTask(true,"",false);
    m_mainWindow->onEndOfPaludisAction();
}

void pertubis::SelectionPage::displayDetails(QString details)
{
    if (details.isEmpty())
        return;
    m_details->setText(details);
    m_mainWindow->onEndOfPaludisAction();
}