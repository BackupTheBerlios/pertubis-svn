
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

#include "MessageOutput.hh"
#include <QLayout>
#include <QDebug>
#include <QColor>
#include <QTextEdit>
#include <QTextCursor>
#include <QTextDocument>
#include <paludis/util/log.hh>
#include <paludis/util/system.hh>
#include <paludis/util/pstream.hh>
#include <paludis/util/fd_output_stream.hh>
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

pertubis::MessageOutput::MessageOutput(QWidget* pObj) : QTextEdit(pObj),
                                    m_thread(0),
                                    m_master_fd(-1),
                                    m_slave_fd(-1)
{
    redirectOutput();
    paludis::Log::get_instance()->set_log_level(paludis::ll_qa);
    paludis::Log::get_instance()->set_program_name("pertubis");
    QVBoxLayout* mylayout = new QVBoxLayout;
    mylayout->setMargin(0);
    setLayout(mylayout);
    setReadOnly(true);
    document()->setMaximumBlockCount(1000);
    QPalette p(palette());
    p.setColor(QPalette::Base,QColor(0,0,0)); // background color  = black
    p.setColor(QPalette::Text,QColor(255,255,255)); // text color  = white
    setPalette(p);
    setAutoFillBackground(true);
    show();
}

void pertubis::MessageThread::run()
{
    char buf[512];
    while (m_atwork)
    {
        int res = read(m_fd,&buf,512);
        if (res > 0)
        {
            sendMessage(QString::fromLocal8Bit(buf,res));
        }
        msleep(100);
    }
}

void pertubis::MessageOutput::redirectOutput()
{
    m_master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    grantpt(m_master_fd);
    unlockpt(m_master_fd);
    m_slave_fd = open(ptsname(m_master_fd), O_RDWR);
    messages_stream.reset(new paludis::FDOutputStream(m_slave_fd));
    paludis::set_run_command_stdout_fds(m_slave_fd, m_master_fd);
    paludis::set_run_command_stderr_fds(m_slave_fd, m_master_fd);
    paludis::PStream::set_stderr_fd(m_slave_fd, m_master_fd);
    fcntl(m_master_fd,F_SETFL,fcntl(m_master_fd,F_GETFL) | O_NONBLOCK);
    paludis::Log::get_instance()->set_log_stream(messages_stream.get());
    m_thread = new MessageThread(this,m_master_fd);
    connect(m_thread,
            SIGNAL(sendMessage(QString)),
            this,
            SLOT(append(QString)));
    m_thread->start();
}

pertubis::MessageOutput::~MessageOutput()
{
    if (m_thread)
    {
        m_thread->setAtWork(false);
        m_thread->wait();
        ::close(m_master_fd);
        ::close(m_slave_fd);
    }
    paludis::Log::get_instance()->set_log_stream(&std::cerr);
}
