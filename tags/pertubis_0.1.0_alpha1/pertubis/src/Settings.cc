
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

#include "Settings.hh"
#include "InstallSettings.hh"
#include "GeneralSettings.hh"
#include "DepListSettings.hh"
#include "QuerySettings.hh"
#include "UninstallSettings.hh"
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


pertubis::Settings::Settings(QWidget* pobj) : QDialog(pobj),
        m_generalView(new GeneralSettingsView(pobj,new GeneralSettingsModel(pobj))),
        m_installView(new InstallSettingsView(pobj,new InstallSettingsModel(pobj))),
        m_deinstallView(new UninstallSettingsView(pobj,new UninstallSettingsModel(pobj))),
        m_queryView(new QuerySettingsView(pobj,new QuerySettingsModel(pobj))),
        m_depListView(new DepListSettingsView(pobj, new DepListSettingsModel(pobj))),
        m_pagesView(new QListWidget(pobj)),
        m_pagesStore(new QStackedWidget())
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
    QHBoxLayout *buttonsLayout = new QHBoxLayout;

    horizontalLayout->addWidget(m_pagesView);
    horizontalLayout->addWidget(m_pagesStore);
    horizontalLayout->setSpacing(0);

    mainLayout->addLayout(horizontalLayout);
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);

    QPushButton *closeButton = new QPushButton(tr("Close"));

    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);

    connect(m_pagesView,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this,
            SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
    connect(closeButton,
            SIGNAL(clicked()),
            this,
            SLOT(close()));
    loadSettings();
    setWindowTitle(tr("pertubis :: settings"));
}

pertubis::Settings::~Settings()
{
    qDebug("Settings::~Settings() - start");
    saveSettings();
    qDebug("Settings::~Settings() - done");
}

void pertubis::Settings::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    m_pagesStore->setCurrentIndex(m_pagesView->row(current));
}

void pertubis::Settings::loadSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "Settings" );
    setVisible(settings.value( "visible",false).toBool());
    resize(settings.value("size",QVariant(QSize(320,600))).toSize());
    move(settings.value("pos",QVariant(QPoint(481,143))).toPoint());
    m_pagesStore->setCurrentIndex(settings.value("store",0).toInt());
    m_pagesView->setCurrentRow(settings.value("view",0).toInt());
    settings.endGroup();
}

void pertubis::Settings::saveSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "Settings" );
    settings.setValue("visible", isVisible() );
    settings.setValue("size", size() );
    settings.setValue("pos", pos());
    settings.setValue("store", m_pagesStore->currentIndex());
    settings.setValue("view", m_pagesView->currentRow());
    settings.endGroup();
}
