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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_DESCRIPTION_EXTRACTOR_HH
#define _PERTUBIS_ENTRY_PROTECTOR_DESCRIPTION_EXTRACTOR_HH 1

#include "extractor.hh"

namespace pertubis
{
    /*! \brief tests if the PackageID's description matches the condition of a Matcher m
     * \ingroup PaludisAdapter
     */
    class DescriptionExtractor : public Extractor
    {
        public:
            DescriptionExtractor(const paludis::Environment * const env);
            virtual ~DescriptionExtractor();

        protected:

            bool operator() (const Matcher& m, const paludis::PackageID& id) const;
    };
}

#endif

