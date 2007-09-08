
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_THREAD_BASE_H
#define _PERTUBIS_ENTRY_PROTECTOR_THREAD_BASE_H

#include <QThread>
#include <paludis/util/tr1_memory.hh>

#include <paludis/environment.hh>

namespace paludis
{
    class Environment;
}

namespace pertubis
{
    class KeywordManager;

    /*! \brief provides the paludis infrastructure we need for the threads
    *
    */
    class ThreadBase : public QThread
    {
        Q_OBJECT

    public:

        ThreadBase(QObject* parent, paludis::tr1::shared_ptr<paludis::Environment> env);

    protected:
        std::tr1::shared_ptr<paludis::Environment>     m_env;
    };
}

#endif
