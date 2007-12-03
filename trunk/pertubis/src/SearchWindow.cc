
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

pertubis::SearchWindow::SearchWindow( QWidget *pwid) : QDialog(pwid)
{

    setObjectName("m_windowSearch");
    QGridLayout* main_layout = new QGridLayout;
    main_layout->setObjectName("layout");
    QVBoxLayout* leftLayout = new QVBoxLayout;

    m_line = new QLineEdit(this);

    m_dbox = new QDialogButtonBox(QDialogButtonBox::Close,Qt::Vertical);

    m_chkName = new QCheckBox(tr("&Name"));
    m_chkName->setChecked(true);
    m_chkDesc = new QCheckBox(tr("&Description"));
    m_chkHomepage = new QCheckBox(tr("&Homepage"));
    m_chkRegex = new QCheckBox(tr("treat search string as &regular expression"));

    QHBoxLayout* optLayout = new QHBoxLayout;
    optLayout->setMargin(0);

    optLayout->addWidget(m_chkName);
    optLayout->addWidget(m_chkDesc);
    optLayout->addWidget(m_chkHomepage);
    optLayout->addWidget(m_chkRegex);
    main_layout->addLayout(leftLayout,0,0);
    main_layout->addWidget(m_dbox,0,1);
    leftLayout->addWidget(m_line);
    leftLayout->addLayout(optLayout);
    setLayout(main_layout);

    QPushButton* bFind = new QPushButton(tr("&Start"));
    bFind->setDefault(true);
    m_dbox->addButton(bFind, QDialogButtonBox::ActionRole);

    QPushButton* bStop = new QPushButton(tr("&Stop"));
    bStop->setDefault(true);
    m_dbox->addButton(bStop, QDialogButtonBox::ActionRole);

    QPushButton* bClose = m_dbox->button(QDialogButtonBox::Close);

    connect(bClose,
            SIGNAL(clicked()),
            this,
            SLOT(hide()));

    connect(bFind,
            SIGNAL(clicked()),
            this,
            SIGNAL(search()));

    connect(bStop,
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
