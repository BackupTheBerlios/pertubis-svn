
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
#include <paludis/util/system.hh>
#include <paludis/util/pstream.hh>
#include <paludis/util/fd_output_stream.hh>
#include "QTOutputStream.hh"
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

class Thread : public QThread
{
    public:
        Thread(QObject* pobj,  QTextEdit* output,int fd) : QThread(pobj), m_output(output), m_fd(fd),m_atwork(true)
        {
        }

        void setAtWork(bool t) { m_atwork = t;}
        bool atWork() const { return m_atwork;}

    protected:

        void run()
        {
            while (m_atwork)
            {
                static char buf[256];
                int res = read(m_fd,&buf,256);
                m_output->append(QString::fromLocal8Bit(buf,res));
                msleep(50);
            }
        }

    private:

        QTextEdit*  m_output;
        int         m_fd;
        bool        m_atwork;
};

pertubis::MessageOutput::MessageOutput(QWidget* mywidget) : QWidget(mywidget)
{
    qDebug() << "MessageOutput::MessageOutput() - start";
    QVBoxLayout* mylayout = new QVBoxLayout;
    mylayout->setMargin(0);
    setLayout(mylayout);
    m_output = new QTextEdit(this);
    m_output->setReadOnly(true);
//     m_output->setFontPointSize(10);
//     m_output->document()->setMaximumBlockCount(100);
    QPalette p(m_output->palette());
    p.setColor(QPalette::Base,QColor(0,0,0)); // background color  = black
    p.setColor(QPalette::Text,QColor(255,255,255)); // text color  = white
    m_output->setPalette(p);
    m_output->setAutoFillBackground(true);
    mylayout->addWidget(m_output);

    show();
//     redirectOutput_Simple();
//     redirectOutput_Paludis();
//     redirectOutput_Combined();
    paludis::Log::get_instance()->set_log_level(paludis::ll_debug);
    paludis::Log::get_instance()->set_program_name("pertubis");

    qDebug() << "MessageOutput::MessageOutput() - done";
}


void pertubis::MessageOutput::redirectOutput_Simple()
{
    m_cout.reset(new QTOutputStreamBuf(*m_output));
    std::cout.rdbuf(m_cout.get());
    m_input.reset(new QTOutputStream(*m_output));
    paludis::Log::get_instance()->set_log_stream(m_input.get());
}

void pertubis::MessageOutput::redirectOutput_Paludis()
{
    master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    grantpt(master_fd);
    unlockpt(master_fd);
    slave_fd = open(ptsname(master_fd), O_RDWR);
    messages_stream.reset(new paludis::FDOutputStream(slave_fd));
    paludis::set_run_command_stdout_fds(slave_fd, master_fd);
    paludis::set_run_command_stderr_fds(slave_fd, master_fd);
    paludis::PStream::set_stderr_fd(slave_fd, master_fd);
//     copy_fd = dup(master_fd);
    m_thread = new Thread(this,m_output,master_fd);
    m_thread->start();
    paludis::Log::get_instance()->set_log_stream(messages_stream.get());
}

void pertubis::MessageOutput::redirectOutput_Combined()
{
    m_input.reset(new QTOutputStream(*m_output));
    paludis::Log::get_instance()->set_log_stream(m_input.get());

    master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    grantpt(master_fd);
    unlockpt(master_fd);
    slave_fd = open(ptsname(master_fd), O_RDWR);
    messages_stream.reset(new paludis::FDOutputStream(slave_fd));
    paludis::set_run_command_stdout_fds(slave_fd, master_fd);
    paludis::set_run_command_stderr_fds(slave_fd, master_fd);
    paludis::PStream::set_stderr_fd(slave_fd, master_fd);
    messages_stream->tie(m_input.get());
}

pertubis::MessageOutput::~MessageOutput()
{
    m_thread->setAtWork(false);
    ::close(master_fd);
    ::close(slave_fd);
    paludis::Log::get_instance()->set_log_stream(&std::cerr);
    m_thread->wait();
}
