
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

#include "SearchWindow.hh"
#include "SearchThread.hh"
#include "QuerySettings.hh"

#include <QCheckBox>
#include <QSettings>
#include <QProgressBar>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>

pertubis::SearchWindow::SearchWindow( QWidget* pobj,
                                    QuerySettingsModel* querySettings,
                                    SearchThread* sthread) : QDialog(pobj),
                                    m_querySettings(querySettings),
                                    m_thread(sthread),
                                    m_bStart(new QPushButton(tr("&Find"))),
                                    m_bStop(new QPushButton(tr("&Stop"))),
                                    m_bOptions(new QPushButton(tr("&More")))
{
    QuerySettingsView* querySettingsView(new QuerySettingsView(pobj,querySettings));

    QLabel* label = new QLabel(tr("&Search:"));
    m_line = new QLineEdit;
    label->setBuddy(m_line);

    m_bStart->setDefault(true);
    m_bOptions->setAutoDefault(false);
    m_bStop->setDisabled(true);
    m_bOptions->setCheckable(true);
    m_bOptions->setAutoDefault(false);

    m_bar=new QProgressBar(this);
    m_bar->setRange(0,100);
    m_bar->hide();
//     m_bar->setValue(0);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Vertical);
    buttonBox->addButton(m_bStart, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_bStop, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_bOptions, QDialogButtonBox::ActionRole);
    m_bClose = buttonBox->addButton(QDialogButtonBox::Close);
    m_bClose->setAutoDefault(false);

    QWidget* myextension = new QWidget;

    connect(m_bStart,
            SIGNAL(clicked()),
            this,
            SLOT(onStart()));

    connect(m_bOptions,
            SIGNAL(toggled(bool)),
            myextension,
            SLOT(setVisible(bool)));

    connect(m_bClose,
            SIGNAL(clicked()),
            this,
            SLOT(close()));

    connect(m_bStop,
            SIGNAL(clicked()),
            this,
            SLOT(onStop()));

    connect(m_thread,
            SIGNAL(progress(int)),
            m_bar,
            SLOT(setValue(int)));

    QVBoxLayout *extensionLayout = new QVBoxLayout;
    extensionLayout->setMargin(0);
    extensionLayout->addWidget(querySettingsView);
    myextension->setLayout(extensionLayout);

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(label);
    topLeftLayout->addWidget(m_line);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);
    leftLayout->addWidget(m_bar);
    leftLayout->addStretch(1);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addLayout(leftLayout, 0, 0);
    mainLayout->addWidget(buttonBox, 0, 1);
    mainLayout->addWidget(myextension, 1, 0, 1, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Searching"));
    myextension->hide();
    loadSettings();
}

void pertubis::SearchWindow::progress(int res)
{
    m_bar->setValue(res);
}

void pertubis::SearchWindow::onStart()
{
    m_bar->show();
    QString query(m_line->text().trimmed());
    if (query.isEmpty())
        return;
    if (m_thread->isRunning())
    {
        int res = QMessageBox::question( this, tr("Warning"),
                                         tr("Search is already running! Yes for starting the new one or no for waiting until the pending is finished?"),QMessageBox::Yes,QMessageBox::No);
        if (res == QMessageBox::No )
        {
            return;
        }
        if (res == QMessageBox::Yes )
            m_thread->stopExec();
            m_thread->wait();
    }
    m_bStop->setDefault(true);
    displaySearch(true);
    m_thread->start(query);
    emit search(query);
}

void pertubis::SearchWindow::onStop()
{
    m_thread->stopExec();
    displaySearch(false);
    emit stopSearch();
}

void pertubis::SearchWindow::displaySearch(bool start)
{
    m_bStart->setDisabled(start);
    m_bStart->setDefault(start);
    m_bar->setVisible(start);
    m_bStop->setEnabled(start);
}

void pertubis::SearchWindow::loadSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "SearchWindow" );
    resize(settings.value("size",QVariant(QSize(320,600))).toSize());
    move(settings.value("pos",QVariant(QPoint(481,143))).toPoint());
    settings.endGroup();
}

void pertubis::SearchWindow::saveSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "SearchWindow" );
    settings.setValue("size", size() );
    settings.setValue("pos", pos());
    settings.endGroup();
}
