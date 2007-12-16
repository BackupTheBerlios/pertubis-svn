

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
#include <QButtonGroup>
#include <QLabel>
#include <QCheckBox>
#include <QHeaderView>
#include <QDebug>
#include <QStringList>
#include <QRadioButton>
#include <QGridLayout>
#include <QSizePolicy>

pertubis::QuerySettingsModel::QuerySettingsModel(QObject *pobj) :
        QObject(pobj),
        m_kindModel(-1),
        m_matcherModel(-1)
{
    qDebug() << "QuerySettingsModel::QuerySettingsModel()";
    loadSettings();
}

pertubis::QuerySettingsModel::~QuerySettingsModel()
{
    qDebug("QuerySettingsModel::~QuerySettingsModel() - start");
    saveSettings();
    qDebug("QuerySettingsModel::~QuerySettingsModel() - done");
}

void pertubis::QuerySettingsModel::loadSettings()
{
    qDebug("QuerySettingsModel::loadSettings() - start");
    QSettings settings;
    settings.beginGroup( "Query" );
    m_matcherModel = settings.value("matcher",0).toInt();
    m_kindModel  = settings.value("kind", 0).toInt();
    settings.endGroup();
    qDebug("QuerySettingsModel::loadSettings() - done");
}

void pertubis::QuerySettingsModel::saveSettings()
{
    qDebug("QuerySettingsModel::saveSettings() - start");
    QSettings settings;
    settings.beginGroup( "Query" );
    settings.setValue("matcher",m_matcherModel);
    settings.setValue("kind",m_kindModel);
    settings.endGroup();
    qDebug("QuerySettingsModel::saveSettings() - done");
}

pertubis::QuerySettingsView::QuerySettingsView(QWidget *pobj,QuerySettingsModel* model) :
        QWidget(pobj),
        m_model(model)
{
    QGroupBox* matcherGroup(new QGroupBox(tr("matcher")));
    matcherGroup->setToolTip( tr("Which match algorithm to use") );
    QRadioButton* mRadio1 = new QRadioButton(tr("text"));
    mRadio1->setToolTip(tr("simple text match"));
    QRadioButton* mRadio2 = new QRadioButton(tr("regex"));
    mRadio2->setToolTip(tr("regular expression"));

    QVBoxLayout* mLayout(new QVBoxLayout);
    mLayout->addWidget(mRadio1);
    mLayout->addWidget(mRadio2);
    matcherGroup->setLayout(mLayout);
    m_matcherGroup= new QButtonGroup(pobj);
    m_matcherGroup->addButton(mRadio1,0);
    m_matcherGroup->addButton(mRadio2,1);
    setMatcher(m_model->m_matcherModel);
    QGroupBox* kindGroup(new QGroupBox(tr("kind")));
    kindGroup->setToolTip(tr("Packages of this kind only"));
    QRadioButton* kRadio1 = new QRadioButton(tr("installable"));
    kRadio1->setToolTip(tr("Installable packages"));
    QRadioButton* kRadio2 = new QRadioButton(tr("installed"));
    kRadio2->setToolTip(tr("Installed packages"));
    QRadioButton* kRadio3 = new QRadioButton(tr("all"));
    kRadio3->setToolTip(tr("all packages"));
    QVBoxLayout* kLayout(new QVBoxLayout);
    m_kindGroup= new QButtonGroup(pobj);
    m_kindGroup->addButton(kRadio1,0);
    m_kindGroup->addButton(kRadio2,1);
    m_kindGroup->addButton(kRadio3,2);
    kLayout->addWidget(kRadio1);
    kLayout->addWidget(kRadio2);
    kLayout->addWidget(kRadio3);

    kindGroup->setLayout(kLayout);
    setKind(m_model->m_kindModel);

    QGridLayout *groupLayout = new QGridLayout;
    groupLayout->addWidget(matcherGroup, 0, 0);
    groupLayout->addWidget(kindGroup, 0, 1);

    QGroupBox* group(new QGroupBox(tr("Query Settings"),pobj));
    group->setToolTip(tr("Options which are relevant for searching."));
    group->setLayout(groupLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(group);
    mainLayout->addStretch(10);
    setLayout(mainLayout);
    loadSettings();

    connect(m_matcherGroup,
            SIGNAL(buttonClicked(int)),
            m_model,
            SLOT(onMatcherChanged(int)));

    connect(m_kindGroup,
            SIGNAL(buttonClicked(int)),
            m_model,
            SLOT(onKindChanged(int)));

    connect(m_model,
            SIGNAL(kindChanged(int)),
            this,
            SLOT(setKind(int)));

    connect(m_model,
            SIGNAL(matcherChanged(int)),
            this,
            SLOT(setMatcher(int)));
}

void pertubis::QuerySettingsView::setKind(int value)
{
    QAbstractButton* button(m_kindGroup->button(value));
    button->setChecked(!button->isChecked());
}

void pertubis::QuerySettingsView::setMatcher(int button)
{
    QAbstractButton* mybutton(m_matcherGroup->button(button));
    mybutton->setChecked(!mybutton->isChecked());
}

pertubis::QuerySettingsView::~QuerySettingsView()
{
    qDebug("QuerySettingsView::~QuerySettingsView() - start");
    qDebug("QuerySettingsView::~QuerySettingsView() - done");
}

void pertubis::QuerySettingsView::loadSettings()
{
    qDebug("QuerySettingsView::loadSettings() - start");
    qDebug("QuerySettings::loadSettings() - done");
}

void pertubis::QuerySettingsView::saveSettings()
{
    qDebug("QuerySettingsView::saveSettings() - start");
    qDebug("QuerySettingsView::saveSettings() - done");
}
