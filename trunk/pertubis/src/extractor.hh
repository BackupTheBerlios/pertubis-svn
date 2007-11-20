
/* Copyright (C) 2007 Stefan Koegl.<hotshelf@users.berlios.de>
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

/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Ciaran McCreesh <ciaranm@ciaranm.org>
 *
 * This file is part of the Paludis package manager. Paludis is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * Paludis is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _PERTUBIS_ENTRY_PROTECTOR_EXTRACTOR_H
#define _PERTUBIS_ENTRY_PROTECTOR_EXTRACTOR_H 1

#include <string>
#include <paludis/environment-fwd.hh>
#include <paludis/package_id-fwd.hh>

namespace pertubis
{
    class Matcher;

    class Extractor
    {
        public:
            Extractor(const paludis::Environment * const env);
            virtual ~Extractor();

            virtual bool operator() (const Matcher& m, const paludis::PackageID& id) const = 0;

        private:

            const paludis::Environment * const m_env;
    };
}

#endif

