
// Copyright (C) 2007 Stefan Koegl.

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

#include "description_extractor.hh"
#include <paludis/package_id.hh>
#include <paludis/metadata_key.hh>
#include "matcher.hh"

pertubis::DescriptionExtractor::DescriptionExtractor(const paludis::Environment * const env) : Extractor(env)
{
}

pertubis::DescriptionExtractor::~DescriptionExtractor()
{
}

bool pertubis::DescriptionExtractor::operator() (const Matcher& m, const paludis::PackageID& id) const
{
    if (!id.short_description_key())
        return false;

    return m(id.short_description_key()->value());
}
