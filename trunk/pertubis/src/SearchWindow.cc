
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
#include "QuerySettings.hh"

#include <QCheckBox>

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>

pertubis::SearchWindow::SearchWindow( QWidget* pwidQuery, QuerySettings* querySettings) : QDialog(pwidQuery),
        m_line(new QLineEdit(this)),
//         m_chkDesc(new QCheckBox(tr("&Description"))),
//         m_chkName(new QCheckBox(tr("&Name"))),
//         m_chkHomepage(new QCheckBox(tr("&Homepage"))),
//         m_chkRegex(new QCheckBox(tr("treat search string as &regular expression"))),
        m_dbox(new QDialogButtonBox(QDialogButtonBox::Close,Qt::Vertical)),
        m_bStart(new QPushButton(tr("&Start"))),
        m_bStop(new QPushButton(tr("&Stop"))),
        m_querySettings(querySettings)
{
    QHBoxLayout* optLayout = new QHBoxLayout;
    optLayout->setMargin(0);
//     optLayout->addWidget(m_chkName);
//     optLayout->addWidget(m_chkDesc);
//     optLayout->addWidget(m_chkHomepage);
//     optLayout->addWidget(m_chkRegex);
    optLayout->addWidget(m_querySettings);

    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(m_line);
    leftLayout->addLayout(optLayout);
    QGridLayout* main_layout = new QGridLayout;
    setLayout(main_layout);
    main_layout->addLayout(leftLayout,0,0);
    main_layout->addWidget(m_dbox,0,1);

    m_bStart->setDefault(true);
    m_dbox->addButton(m_bStart, QDialogButtonBox::ActionRole);
    m_bStop->setDisabled(true);
    m_dbox->addButton(m_bStop, QDialogButtonBox::ActionRole);

    QPushButton* bClose = m_dbox->button(QDialogButtonBox::Close);

    connect(bClose,
            SIGNAL(clicked()),
            this,
            SLOT(hide()));

    connect(m_bStart,
            SIGNAL(clicked()),
            this,
            SIGNAL(search()));

    connect(m_bStop,
            SIGNAL(clicked()),
            this,
            SIGNAL(stopSearch()));

    setWindowTitle(tr("Search Dialog"));
    setWindowModality(Qt::WindowModal);
    hide();
//     qDebug() << "SearchWindow::SearchWindow() - done";
}

QString pertubis::SearchWindow::query() const
{
    return m_line->text().trimmed();
}

void pertubis::SearchWindow::displaySearch(bool start)
{
    m_bStart->setDisabled(start);
    m_bStop->setEnabled(start);
}

void pertubis::SearchWindow::toggle()
{
    if (isHidden())
    {
//         m_querySettings->show();
        exec();
    }
    else
    {
        hide();
//         m_querySettings->hide();
    }
}

