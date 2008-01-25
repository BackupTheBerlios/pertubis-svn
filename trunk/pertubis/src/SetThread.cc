
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
    const paludis::tr1::shared_ptr<paludis::Environment>&  myenv) :
    ThreadBase(pobj,myenv)
{
}

void pertubis::SetThread::run()
{
    using namespace paludis;
    QMap<QString, QSet<QString> > result;
    for (paludis::IndirectIterator<paludis::PackageDatabase::RepositoryConstIterator, const paludis::Repository>
         r(env()->package_database()->begin_repositories()), r_end(env()->package_database()->end_repositories()) ;
         r != r_end ; ++r)
    {
        RepositorySetsInterface * const i(r->sets_interface);
        if (i)
        {
            tr1::shared_ptr<const SetNameSet> sets(i->sets_list());
            for (paludis::SetNameSet::ConstIterator s(sets->begin()), s_end(sets->end()); s != s_end ; ++s)
            {
                result[QString::fromStdString(stringify(*s))].insert(QString::fromStdString(stringify(r->name())));
            }
        }
    }
    emit sendSet(result);
}
