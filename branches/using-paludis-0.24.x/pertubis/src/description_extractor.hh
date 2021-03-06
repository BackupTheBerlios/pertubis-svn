// modifications Copyright (C) 2007 Stefan Koegl.

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

#ifndef PALUDIS_GUARD_SRC_CLIENTS_INQUISITIO_DESCRIPTION_EXTRACTOR_HH
#define PALUDIS_GUARD_SRC_CLIENTS_INQUISITIO_DESCRIPTION_EXTRACTOR_HH

#include "extractor.hh"

namespace pertubis
{
    class DescriptionExtractor : public Extractor
    {
        public:
            DescriptionExtractor(std::tr1::shared_ptr<paludis::Environment>);
            virtual ~DescriptionExtractor();

            std::string operator() (const paludis::PackageDatabaseEntry &) const;

    };
}

#endif

