

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

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSettings>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QCheckBox>
#include <QHeaderView>
#include <QStringList>
#include <QGridLayout>
#include <QSizePolicy>

pertubis::QuerySettings::QuerySettings(QWidget *pobj) :
        QWidget(pobj),
        m_enabledOnly(new QCheckBox(tr("enabled only"),pobj)),
        m_matcher(new QTableWidget(2,1,pobj)),
        m_extractor(new QTableWidget(2,1,pobj)),
        m_kind(new QComboBox(pobj))
{
    //{ install settings group



    m_enabledOnly->setToolTip(tr("When searching spec trees, only look in enabled subtrees"));
    m_enabledOnly->setChecked(true);

    m_matcher->horizontalHeader()->hide();
    m_matcher->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_matcher->setHorizontalHeaderLabels(QStringList() << "a");
    m_matcher->setRowCount(2);
    m_matcher->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_matcher->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
    m_matcher->setToolTip( tr("Which match algorithm to use") );
    m_matcher->verticalHeader()->hide();
    m_matcher->setShowGrid(false);

    m_extractor->horizontalHeader()->hide();
    m_extractor->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_extractor->setHorizontalHeaderLabels(QStringList() << "a");
    m_extractor->setRowCount(2);
    m_extractor->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_extractor->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum));
    m_extractor->setToolTip( tr("") );
    m_extractor->verticalHeader()->hide();
    m_extractor->setShowGrid(false);

    QTableWidgetItem* i1(new QTableWidgetItem(tr("name")));
    QTableWidgetItem* i2(new QTableWidgetItem(tr("description")));
    m_extractor->setItem(0,0,i1);
    m_extractor->setItem(1,0,i2);

    QTableWidgetItem* a1(new QTableWidgetItem(tr("text")));
    QTableWidgetItem* a2(new QTableWidgetItem(tr("regular expression")));
    m_matcher->setItem(0,0,a1);
    m_matcher->setItem(1,0,a2);

    m_matcher->setCurrentCell(0,0);

    m_kind->setToolTip(tr("Packages of this kind only"));
    m_kind->addItem(tr("installable"));
    m_kind->addItem(tr("installed"));
    m_kind->addItem(tr("all"));
    m_kind->setCurrentIndex(0);

    QGridLayout *groupLayout = new QGridLayout;

    groupLayout->addWidget(m_enabledOnly, 0, 0,1,2,Qt::AlignHCenter);
    groupLayout->addWidget(new QLabel(tr("kind"),m_kind),1,0);
    groupLayout->addWidget(m_kind, 1, 1);
    groupLayout->addWidget(new QLabel(tr("matchers"),m_matcher),2,0);
    groupLayout->addWidget(m_matcher, 2, 1);
    groupLayout->addWidget(new QLabel(tr("extractors"),m_extractor),3,0);
    groupLayout->addWidget(m_extractor, 3, 1);


    QGroupBox* group(new QGroupBox(tr("Query Settings"),pobj));
    group->setToolTip(tr("Options which are relevant for searching."));
    group->setLayout(groupLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(group);
    mainLayout->addSpacing(12);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
    loadSettings();
}

pertubis::QuerySettings::~QuerySettings()
{
    qDebug("QuerySettings::~QuerySettings() - start");
    saveSettings();
    qDebug("QuerySettings::~QuerySettings() - done");
}

void pertubis::QuerySettings::loadSettings()
{
    qDebug("QuerySettings::loadSettings() - start");
    QSettings settings;
    settings.beginGroup( "QuerySettings" );

    settings.value("enabled", m_enabledOnly->checkState());

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

void pertubis::QuerySettings::saveSettings()
{
    qDebug("QuerySettings::saveSettings() - start");
    QSettings settings;
    settings.beginGroup( "QuerySettings" );
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

