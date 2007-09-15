
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
#include <QTextCursor>
#include <QTextDocument>
#include <paludis/util/log.hh>
#include "QTOutputStream.hh"
#include <iostream>

pertubis::MessageOutput::MessageOutput(QWidget* mywidget) : QWidget(mywidget)
{
    QVBoxLayout* mylayout = new QVBoxLayout;
    mylayout->setMargin(0);
    setLayout(mylayout);
    m_output = new QTextEdit(this);
    m_output->setReadOnly(true);
//     m_output->setFontPointSize(12);
//     m_output->document()->setMaximumBlockCount(100);
    QPalette p(m_output->palette());
    p.setColor(QPalette::Base,QColor(0,0,0)); // background color  = black
    p.setColor(QPalette::Text,QColor(255,255,255)); // text color  = white
    m_output->setPalette(p);
    m_output->setAutoFillBackground(true);
    mylayout->addWidget(m_output);

    show();

    m_input.reset(new QTOutputStream(*m_output));
    paludis::Log::get_instance()->set_log_level(paludis::ll_debug);
    paludis::Log::get_instance()->set_program_name("pertubis");
    paludis::Log::get_instance()->set_log_stream(m_input.get());
}

pertubis::MessageOutput::~MessageOutput()
{
    paludis::Log::get_instance()->set_log_stream(&std::cerr);
}
