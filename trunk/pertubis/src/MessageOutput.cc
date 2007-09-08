
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

#include "MessageOutput.hh"
#include <QLayout>
#include <QColor>
#include <QTextEdit>

pertubis::MessageOutput::MessageOutput(QWidget* mywidget) : QWidget(mywidget)
{
    QVBoxLayout* mylayout = new QVBoxLayout;
    mylayout->setMargin(0);
    setLayout(mylayout);
    m_output = new QTextEdit(this);
    m_output->setReadOnly(true);
    mylayout->addWidget(m_output);
    QPalette p(m_output->palette());
    QColor c(0,0,0);
    p.setColor(QPalette::Base,c);
    m_output->setPalette(p);
    m_output->setAutoFillBackground(true);
    show();
}

pertubis::MessageOutput::~MessageOutput()
{
}
