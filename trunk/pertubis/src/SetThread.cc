
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

#include "SetThread.hh"
#include <paludis/environment.hh>
#include <paludis/package_database.hh>
#include <paludis/util/set.hh>
#include <paludis/util/indirect_iterator.hh>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/tr1_memory.hh>
#include <paludis/util/sequence.hh>
#include <paludis/util/wrapped_forward_iterator.hh>

#include <QDebug>
#include <QSet>
#include <QStringList>

pertubis::SetThread::SetThread(QObject* pobj,
                                         const paludis::tr1::shared_ptr<paludis::Environment>&  env,
                                         TaskBox* box) : ThreadBase(pobj,env,box)
{
}

void pertubis::SetThread::run()
{
    using namespace paludis;
    qDebug() << "SetThread.run() - starting";
    QMap<QString, QSet<QString> > result;
    for (paludis::IndirectIterator<paludis::PackageDatabase::RepositoryConstIterator, const paludis::Repository>
         r(m_env->package_database()->begin_repositories()), r_end(m_env->package_database()->end_repositories()) ;
         r != r_end ; ++r)
    {
        RepositorySetsInterface * const i(r->sets_interface);
        if (i)
        {
            tr1::shared_ptr<const SetNameSet> sets(i->sets_list());
            for (paludis::SetNameSet::ConstIterator s(sets->begin()), s_end(sets->end()); s != s_end ; ++s)
            {
//             qDebug() << QString::fromStdString(stringify(*c));
                result[QString::fromStdString(stringify(*s))].insert(QString::fromStdString(stringify(r->name())));
            }
        }

    }

    emit sendSet(result);
    qDebug() << "SetThread.run() - done";
}

