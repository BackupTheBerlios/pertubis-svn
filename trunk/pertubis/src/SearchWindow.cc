
/* Copyright (C) 2007 Stefan Koegl.
*
* This file is part of pertubis.
*
* pertubis is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* pertubis is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http:*www.gnu.org/licenses/>.
*/

#include "SearchWindow.hh"
#include "defines.hh"

#include <iostream>
#include <paludis/environment.hh>
#include <paludis/environments/environment_maker.hh>
#include <paludis/package_database.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/stringify.hh>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>


pertubis::SearchWindow::SearchWindow( QWidget *parent) : QDialog(parent)
{
	qDebug() << "SearchWindow::SearchWindow() - start";
	setObjectName("m_windowSearch");
	QGridLayout* layout = new QGridLayout;
	layout->setObjectName("layout");
	QVBoxLayout* leftLayout = new QVBoxLayout;

	m_line = new QLineEdit(this);

	m_label = new QLabel(tr("&Find"));
	m_label->setBuddy(m_line);


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


	layout->addLayout(leftLayout,0,0);

	layout->addWidget(m_dbox,0,1);

	leftLayout->addWidget(m_line);


	leftLayout->addLayout(optLayout);


// 	leftLayout->addStretch(1);
	setLayout(layout);

	QPushButton* bFind = new QPushButton(tr("&Start"));
	bFind->setDefault(true);
	m_dbox->addButton(bFind, QDialogButtonBox::ActionRole);
	QPushButton* bClose = m_dbox->button(QDialogButtonBox::Close);

	connect(bClose,
			SIGNAL(clicked()),
			this,
   			SLOT(hide()));

	connect(bFind,
			SIGNAL(clicked()),
			this,
   			SIGNAL(search()));

	setWindowTitle(tr("Search Dialog"));
	setWindowModality(Qt::WindowModal);
	hide();
}

pertubis::SearchWindow::~SearchWindow()
{
}

