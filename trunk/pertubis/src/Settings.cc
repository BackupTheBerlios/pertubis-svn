
/* Copyright (C) 2007 Stefan Koegl
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

#include "Settings.hh"
#include "InstallSettings.hh"
#include "GeneralSettings.hh"
#include "DepListSettings.hh"
#include "QuerySettings.hh"
#include "DeinstallSettings.hh"
#include <QApplication>
#include <QComboBox>
#include <QDebug>

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QLocale>
#include <QMessageBox>
#include <QScrollArea>
#include <QPushButton>
#include <QSettings>
#include <QTranslator>
#include <QStackedWidget>
#include <QVBoxLayout>

using namespace pertubis;

SettingsPage::SettingsPage(MainWindow* main) : Page(main),
        m_generalView(new GeneralSettingsView(this,new GeneralSettingsModel(this))),
        m_installView(new InstallSettingsView(this,new InstallSettingsModel(this))),
        m_deinstallView(new DeinstallSettingsView(this,new DeinstallSettingsModel(this))),
        m_queryView(new QuerySettingsView(this,new QuerySettingsModel(this))),
        m_depListView(new DepListSettingsView(this, new DepListSettingsModel(this))),
        m_pagesView(new QListWidget(this)),
        m_pagesStore(new QStackedWidget(this))
{
    m_pagesStore->addWidget(m_generalView);
    m_pagesStore->addWidget(m_installView);
    m_pagesStore->addWidget(m_deinstallView);
    m_pagesStore->addWidget(m_queryView);
    m_pagesStore->addWidget(m_depListView);
    m_pagesView->setViewMode(QListView::IconMode);
    m_pagesView->setFlow(QListView::LeftToRight);
    m_pagesView->setMaximumWidth(150);
    m_pagesView->setMinimumWidth(150);
    m_pagesView->setIconSize(QSize(64, 64));
    m_pagesView->setSpacing(36);

    m_pagesView->setMovement(QListView::Static);

    QListWidgetItem *tGeneral = new QListWidgetItem(m_pagesView);
    tGeneral->setIcon(QIcon(":/images/settings.png"));
    tGeneral->setText(tr("General"));
    tGeneral->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *tInstall = new QListWidgetItem(m_pagesView);
    tInstall->setIcon(QIcon(":/images/install.png"));
    tInstall->setText(tr("Installation"));
    tInstall->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *tDeinstall = new QListWidgetItem(m_pagesView);
    tDeinstall->setIcon(QIcon(":/images/deinstall.png"));
    tDeinstall->setText(tr("Deinstallation"));
    tDeinstall->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *tQuery = new QListWidgetItem(m_pagesView);
    tQuery->setIcon(QIcon(":/images/find.png"));
    tQuery->setText(tr("Query List"));
    tQuery->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *tDepList = new QListWidgetItem(m_pagesView);
    tDepList->setIcon(QIcon(":/images/depList.png"));
    tDepList->setText(tr("Dependency List"));
    tDepList->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *horizontalLayout = new QHBoxLayout;

    horizontalLayout->addWidget(m_pagesView);
    horizontalLayout->addWidget(m_pagesStore);
    horizontalLayout->setSpacing(0);

    mainLayout->addLayout(horizontalLayout);

    setLayout(mainLayout);

    connect(m_pagesView,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this,
            SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));

    loadSettings();
    setWindowTitle(tr("pertubis :: settings"));
}

void
SettingsPage::activatePage()
{
}

SettingsPage::~SettingsPage()
{
    qDebug("SettingsPage::~SettingsPage() - start");
    saveSettings();
    delete m_depListView;
    delete m_queryView;
    delete m_deinstallView;
    delete m_installView;
    delete m_generalView;
    qDebug("SettingsPage::~SettingsPage() - done");
}

void
SettingsPage::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    m_pagesStore->setCurrentIndex(m_pagesView->row(current));
}

void
SettingsPage::loadSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup("SettingsPage");
    setVisible(settings.value("visible",false).toBool());
    resize(settings.value("size",QVariant(QSize(320,600))).toSize());
    move(settings.value("pos",QVariant(QPoint(481,143))).toPoint());
    m_pagesStore->setCurrentIndex(settings.value("store",0).toInt());
    m_pagesView->setCurrentRow(settings.value("view",0).toInt());
    settings.endGroup();
}

void
SettingsPage::saveSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup("SettingsPage");
    settings.setValue("visible", isVisible());
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("store", m_pagesStore->currentIndex());
    settings.setValue("view", m_pagesView->currentRow());
    settings.endGroup();
}
