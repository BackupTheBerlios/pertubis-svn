
/* Copyright (C) 2007-2008 Stefan Koegl <hotshelf@users.berlios.de>
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

#include "ThreadBase.hh"
#include <paludis/environment.hh>
#include <QDebug>

namespace pertubis
{
    struct ThreadBasePrivate
    {

        ThreadBasePrivate(const paludis::tr1::shared_ptr<paludis::Environment>&  env) :
            m_env(env),
            m_execMode(true)
        {
        }

        paludis::tr1::shared_ptr<paludis::Environment>  m_env;
        bool                                            m_execMode;
    };
}

pertubis::ThreadBase::ThreadBase(QObject* pobj,
        const paludis::tr1::shared_ptr<paludis::Environment>&  myenv) :
        QThread(pobj),
        m_imp(new ThreadBasePrivate(myenv))
{
}

pertubis::ThreadBase::~ThreadBase()
{
    delete m_imp;
}

paludis::tr1::shared_ptr<paludis::Environment> pertubis::ThreadBase::env()
{
    return m_imp->m_env;
}

bool pertubis::ThreadBase::execMode()
{
    return m_imp->m_execMode;
}

void pertubis::ThreadBase::setExecMode(bool mode)
{
    qDebug() << "setExecMode()" << mode;
    m_imp->m_execMode=mode;
}
