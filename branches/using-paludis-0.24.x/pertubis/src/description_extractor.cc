
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
#include <paludis/package_database.hh>


pertubis::DescriptionExtractor::DescriptionExtractor(std::tr1::shared_ptr<paludis::Environment> env) : Extractor(env)
{
}

pertubis::DescriptionExtractor::~DescriptionExtractor()
{
}

std::string pertubis::DescriptionExtractor::operator() (const paludis::PackageDatabaseEntry & p) const
{
    return m_env->package_database()->fetch_repository(p.repository)->
        version_metadata(p.name, p.version)->description;
}
