
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

#include "name_extractor.hh"
#include "matcher.hh"
#include <paludis/package_id.hh>
#include <paludis/name.hh>
#include <paludis/util/stringify.hh>


pertubis::NameExtractor::NameExtractor(const paludis::Environment * const env) : Extractor(env)
{
}

bool pertubis::NameExtractor::operator() (const Matcher& m, const paludis::PackageID& id) const
{
    return m(paludis::stringify(id.name()));
}
