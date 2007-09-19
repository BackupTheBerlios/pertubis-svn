
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

#include "ThreadFetchCategories.hh"
#include "DatabaseView.hh"

#include <paludis/environment.hh>
#include <paludis/package_database.hh>
#include <paludis/package_id.hh>
#include <paludis/query.hh>
#include <paludis/mask.hh>
#include <paludis/util/visitor.hh>
#include <paludis/util/set.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/tr1_memory.hh>
#include <paludis/util/sequence.hh>
#include <libwrapiter/libwrapiter_forward_iterator.hh>
#include <libwrapiter/libwrapiter_output_iterator.hh>
#include "name_extractor.hh"
#include <QDebug>
#include <QSet>

#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <algorithm>

pertubis::ThreadFetchCategories::ThreadFetchCategories(QObject* pobj,
        DatabaseView* main) : ThreadBase(pobj,main)
{
}

void pertubis::ThreadFetchCategories::run()
{
    using namespace paludis;
    qDebug() << "ThreadFetchCategories.run() - starting";
    QSet<QString> cats;
    for (IndirectIterator<PackageDatabase::RepositoryIterator, const Repository>
         r(m_main->getEnv()->package_database()->begin_repositories()), r_end(m_main->getEnv()->package_database()->end_repositories()) ;
            r != r_end ; ++r)
    {
        tr1::shared_ptr<const CategoryNamePartSet> cat_names(r->category_names());
        for (CategoryNamePartSet::Iterator c(cat_names->begin()), c_end(cat_names->end()); c != c_end ; ++c)
        {
//             qDebug() << stringify(*c).c_str();
            cats << stringify(*c).c_str();
        }
    }
    emit categoriesResult(cats.toList());
    qDebug() << "ThreadFetchCategories.run() - done";
}
