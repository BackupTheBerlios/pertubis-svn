

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

#include "QuerySettings.hh"

#include <QTableView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QStandardItem>
#include <QSettings>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QCheckBox>
#include <QHeaderView>
#include <QDebug>
#include <QStringList>
#include <QGridLayout>
#include <QSizePolicy>

pertubis::QuerySettingsModel::QuerySettingsModel(QWidget *pobj) :
        QWidget(pobj),
                m_extractorModel(new QStandardItemModel(2,1,pobj)),
                m_kindModel(new QStandardItemModel(3,1,pobj)),
                m_matcherModel(new QStandardItemModel(2,1,pobj)),
                m_extractorSelectionModel(new QItemSelectionModel(m_extractorModel,pobj)),
                m_matcherSelectionModel(new QItemSelectionModel(m_matcherModel,pobj)),
                m_enabledOnly(0)
{
    qDebug() << "QuerySettingsModel::QuerySettingsModel()";
    m_matcherModel->setHorizontalHeaderLabels(QStringList() << "a");
    m_extractorModel->setHorizontalHeaderLabels(QStringList() << "a");
    m_extractorModel->setItem(0,0,new QStandardItem(tr("name")));
    m_extractorModel->setItem(1,0,new QStandardItem(tr("description")));
    m_matcherModel->setItem(0,0,new QStandardItem(tr("text")));
    m_matcherModel->setItem(1,0,new QStandardItem(tr("regular expression")));

//     m_matcher->setCurrentCell(0,0);

    m_kindModel->setItem(0,0,new QStandardItem(tr("installable")));
    m_kindModel->setItem(1,0,new QStandardItem(tr("installed")));
    m_kindModel->setItem(2,0,new QStandardItem(tr("all")));
//     m_kindModel->setCurrentIndex(0);

    loadSettings();
}

pertubis::QuerySettingsModel::~QuerySettingsModel()
{
    qDebug("QuerySettings::~QuerySettings() - start");
    saveSettings();
    qDebug("QuerySettings::~QuerySettings() - done");
}

void pertubis::QuerySettingsModel::loadSettings()
{
    qDebug("QuerySettings::loadSettings() - start");
    QSettings settings;
    settings.beginGroup( "query" );
    settings.value("enabled", m_enabledOnly);

//     QVariantList list;
//     settings.value("matcher", list);
//     for (QVariantList::const_iterator i(list.constBegin()),iEnd(list.constEnd());
//         i != iEnd;++i)
//     {
//         QTableItem* item(m_matcher->item(i->toInt()));
//         item->setSelected(true);
//     }
//     settings.value("extractor", list);
//     for (QVariantList::const_iterator i(list.constBegin()),iEnd(list.constEnd());
//          i != iEnd;++i)
//     {
//         QTableItem* item(m_extractor->item(i->toInt()));
//         item->setSelected(true);
//     }
//     settings.value("kind",m_kind->currentIndex());
    settings.endGroup();
    qDebug("QuerySettings::loadSettings() - done");
}

void pertubis::QuerySettingsModel::saveSettings()
{
    qDebug("QuerySettings::saveSettings() - start");
    QSettings settings;
    settings.beginGroup( "query" );
    m_enabledOnly = static_cast<int>(settings.value("enabled").toInt());
//     QVariantList list;
//     int count=m_matcher->count();
//     for (int i=0;i<count;i++)
//     {
//         if (m_matcher->item(i)->isSelected())
//             list << i;
//     }
//     settings.value("matcher",list);
//     list.clear();
//     count=m_extractor->count();
//     for (int i=0;i<count;i++)
//     {
//         if (m_extractor->item(i)->isSelected())
//             list << i;
//     }
//     m_matcher->setCurrentRow(settings.value("matcher").toInt());
//     m_extractor->setCurrentRow(settings.value("extractor").toInt());
//     m_kind->setCurrentIndex(settings.value("kind").toInt());
    settings.endGroup();
    qDebug("QuerySettings::saveSettings() - done");
}




pertubis::QuerySettingsView::QuerySettingsView(QWidget *pobj) :
        QWidget(pobj),
        m_model(new QuerySettingsModel(pobj)),
        m_enabledOnly(new QCheckBox(tr("enabled only"),pobj)),
        m_matcherView(new QTableView(pobj)),
        m_extractorView(new QTableView(pobj)),
        m_kindView(new QComboBox(pobj))
{
    qDebug() << "QuerySettingsView::QuerySettingsView() - start";
    m_enabledOnly->setToolTip(tr("When searching spec trees, only look in enabled subtrees"));
    m_enabledOnly->setChecked(m_model->m_enabledOnly);

    m_matcherView->horizontalHeader()->hide();
    m_matcherView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    qDebug() << "QuerySettingsView::QuerySettingsView() - 1";
    m_matcherView->setModel(m_model->m_matcherModel);
    qDebug() << "QuerySettingsView::QuerySettingsView() - 2";
    m_matcherView->setSelectionModel(m_model->m_matcherSelectionModel);
    qDebug() << "QuerySettingsView::QuerySettingsView() - 3";


    m_matcherView->setSelectionMode(QAbstractItemView::ExtendedSelection);
//     m_matcherView->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
    m_matcherView->setToolTip( tr("Which match algorithm to use") );
    m_matcherView->verticalHeader()->hide();
    m_matcherView->setShowGrid(false);
//     setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));

    m_extractorView->horizontalHeader()->hide();
    m_extractorView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);

    qDebug() << "QuerySettingsView::QuerySettingsView() - 3";

    m_extractorView->setModel(m_model->m_extractorModel);
    m_extractorView->setSelectionModel(m_model->m_extractorSelectionModel);

    m_extractorView->setSelectionMode(QAbstractItemView::ExtendedSelection);
//     m_extractorView->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
    m_extractorView->setToolTip( tr("") );
    m_extractorView->verticalHeader()->hide();
    m_extractorView->setShowGrid(false);

//     m_matcher->setCurrentCell(0,0);

    m_kindView->setToolTip(tr("Packages of this kind only"));
    m_kindView->setModel(m_model->m_kindModel);

//     m_kind->setCurrentIndex(0);

    QGridLayout *groupLayout = new QGridLayout;

    groupLayout->addWidget(m_enabledOnly, 0, 0,1,2,Qt::AlignHCenter);
    groupLayout->addWidget(new QLabel(tr("kind"),m_kindView),1,0);
    groupLayout->addWidget(m_kindView, 1, 1);
    groupLayout->addWidget(new QLabel(tr("matchers"),m_matcherView),2,0);
    groupLayout->addWidget(m_matcherView, 2, 1);
    groupLayout->addWidget(new QLabel(tr("extractors"),m_extractorView),3,0);
    groupLayout->addWidget(m_extractorView, 3, 1);
//     groupLayout->setColumnStretch(2,10);
//     groupLayout->setRowStretch(4,10);

    QGroupBox* group(new QGroupBox(tr("Query Settings"),pobj));
    group->setToolTip(tr("Options which are relevant for searching."));
    group->setLayout(groupLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(group);
//     mainLayout->addSpacing(12);
    mainLayout->addStretch(10);
    setLayout(mainLayout);
    loadSettings();
}

pertubis::QuerySettingsView::~QuerySettingsView()
{
    qDebug("QuerySettings::~QuerySettings() - start");
    saveSettings();
    qDebug("QuerySettings::~QuerySettings() - done");
}

void pertubis::QuerySettingsView::loadSettings()
{
    qDebug("QuerySettings::loadSettings() - start");
    QSettings settings;
    settings.beginGroup( "query" );

    int enabled(m_enabledOnly->checkState());
    settings.value("enabled", enabled);

//     QVariantList list;
//     settings.value("matcher", list);
//     for (QVariantList::const_iterator i(list.constBegin()),iEnd(list.constEnd());
//         i != iEnd;++i)
//     {
//         QTableItem* item(m_matcher->item(i->toInt()));
//         item->setSelected(true);
//     }
//     settings.value("extractor", list);
//     for (QVariantList::const_iterator i(list.constBegin()),iEnd(list.constEnd());
//          i != iEnd;++i)
//     {
//         QTableItem* item(m_extractor->item(i->toInt()));
//         item->setSelected(true);
//     }
//     settings.value("kind",m_kind->currentIndex());
    settings.endGroup();
    qDebug("QuerySettings::loadSettings() - done");
}

void pertubis::QuerySettingsView::saveSettings()
{
    qDebug("QuerySettings::saveSettings() - start");
    QSettings settings;
    settings.beginGroup( "query" );
    m_enabledOnly->setChecked(settings.value("enabled").toInt());
//     QVariantList list;
//     int count=m_matcher->count();
//     for (int i=0;i<count;i++)
//     {
//         if (m_matcher->item(i)->isSelected())
//             list << i;
//     }
//     settings.value("matcher",list);
//     list.clear();
//     count=m_extractor->count();
//     for (int i=0;i<count;i++)
//     {
//         if (m_extractor->item(i)->isSelected())
//             list << i;
//     }
//     m_matcher->setCurrentRow(settings.value("matcher").toInt());
//     m_extractor->setCurrentRow(settings.value("extractor").toInt());
//     m_kind->setCurrentIndex(settings.value("kind").toInt());
    settings.endGroup();
    qDebug("QuerySettings::saveSettings() - done");
}

