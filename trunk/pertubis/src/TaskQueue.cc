
/* Copyright (C) 2007-2008 Stefan Koegl
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

#include "TaskQueue.hh"

#include <QQueue>
#include <QDebug>
#include <QThread>

using namespace pertubis;

namespace pertubis
{
    struct TaskQueuePrivate
    {
        QQueue<QThread*>                    m_taskQueue;
    };
}

TaskQueue::TaskQueue(QObject* pobj) :
    QObject(pobj),
    m_imp(new TaskQueuePrivate)
{

}

TaskQueue::~TaskQueue()
{
    delete m_imp;
}

void
TaskQueue::enqueue(QThread* task, bool start)
{
    m_imp->m_taskQueue.enqueue(task);
    connect(task,
            SIGNAL(finished()),
            this,
            SLOT(process()));
    if (start)
        next();
}

void
TaskQueue::dequeue()
{
    qDebug() << "TaskQueue::dequeue()";
    QThread* t(m_imp->m_taskQueue.dequeue());
    delete t;
    t = 0;
}

void
TaskQueue::next()
{
    qDebug() << "TaskQueue::next()";
    if (!m_imp->m_taskQueue.isEmpty())
    {
        m_imp->m_taskQueue.head()->start();
    }
    else
    {
        emit finished();
    }
}

bool
TaskQueue::running()
{
    qDebug() << "TaskQueue::running()";
    return (!m_imp->m_taskQueue.isEmpty() &&
            m_imp->m_taskQueue.head()->isRunning());
}

void
TaskQueue::process()
{
    qDebug() << "TaskQueue::process()";
    if (!m_imp->m_taskQueue.isEmpty())
        dequeue();
    next();
}
