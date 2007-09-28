
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

#include "CategoriesThread.hh"
#include "CategoryItem.hh"
#include "DatabaseView.hh"
#include <paludis/environment.hh>
#include <paludis/package_database.hh>
#include <paludis/util/set.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/tr1_memory.hh>
#include <paludis/util/sequence.hh>
#include <libwrapiter/libwrapiter_forward_iterator.hh>
#include <libwrapiter/libwrapiter_output_iterator.hh>

#include <QDebug>
#include <QSet>
#include <QStringList>

pertubis::CategoriesThread::CategoriesThread(QObject* pobj,
        DatabaseView* main) : ThreadBase(pobj,main)
{
}

void pertubis::CategoriesThread::run()
{
    using namespace paludis;
    qDebug() << "CategoriesThread.run() - starting";
    QMap<QString, QStringList> cats;
    for (paludis::IndirectIterator<paludis::PackageDatabase::RepositoryConstIterator, const paludis::Repository>
         r(m_main->getEnv()->package_database()->begin_repositories()), r_end(m_main->getEnv()->package_database()->end_repositories()) ;
            r != r_end ; ++r)
    {
        paludis::tr1::shared_ptr<const paludis::CategoryNamePartSet> cat_names(r->category_names());
        for (paludis::CategoryNamePartSet::ConstIterator c(cat_names->begin()), c_end(cat_names->end()); c != c_end ; ++c)
        {
            cats[QString::fromStdString(stringify(*c))].push_back(QString::fromStdString(stringify(r->name())));
        }
    }
    QList<CategoryItem*> list;
    for(QMap<QString,QStringList>::const_iterator cStart(cats.constBegin()),
        end(cats.constEnd());
        cStart != end;
        ++cStart)
    {
        list.push_back(new CategoryItem(cStart.key(),cStart.value()));
    }
    emit categoriesResult(list);
    qDebug() << "CategoriesThread.run() - done";
}

