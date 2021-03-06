
/* Copyright (C) 2007-2008 Stefan Koegl
*
* This file is part of perturbis
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

#include "CategoryThread.hh"
#include "CategoryItem.hh"
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

using namespace perturbis;

CategoryThread::CategoryThread(QObject* pobj,
                                         const paludis::tr1::shared_ptr<paludis::Environment>&  myenv) : ThreadBase(pobj,myenv)
{
}

void
CategoryThread::run()
{
    using namespace paludis;
    qDebug() << "CategoryThread::run() start";

    QMap<QString, QSet<QString> > cats;
    for (paludis::IndirectIterator<paludis::PackageDatabase::RepositoryConstIterator, const paludis::Repository>
         r(env()->package_database()->begin_repositories()), r_end(env()->package_database()->end_repositories()) ;
            r != r_end ; ++r)
    {
        paludis::tr1::shared_ptr<const paludis::CategoryNamePartSet> cat_names(r->category_names());
        for (paludis::CategoryNamePartSet::ConstIterator c(cat_names->begin()), c_end(cat_names->end()); c != c_end ; ++c)
        {
            cats[QString::fromStdString(stringify(*c))].insert(QString::fromStdString(stringify(r->name())));
        }
    }

    emit sendCategory(cats);
    qDebug() << "CategoryThread::run() done";
}
