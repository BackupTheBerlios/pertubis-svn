
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

#include "RepositoryModel.hh"

namespace pertubis
{
    /*! \brief not finished
    *
    */
    class RepositoryThread : public QThread
    {
        public:
            RepositoryThread(QObject* parent) : QThread(parent) {}
            void run()
            {
                for (IndirectIterator<PackageDatabase::RepositoryIterator, const Repository>
                    r((*m_env).package_database()->begin_repositories()), r_end((*m_env).package_database()->end_repositories()) ;
                    r != r_end ; ++r)
                {
                    std::tr1::shared_ptr<const RepositoryInfo> info = r->info();

                }
            }
        private:
            std::tr1::shared_ptr<paludis::Environment>         m_env;
    };
}

pertubis::RepositoryModel::RepositoryModel(QObject* parent) : QAbstractListModel(parent)
{
}
