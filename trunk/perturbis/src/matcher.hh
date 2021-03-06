
/* Copyright (C) 2007 Stefan Koegl
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_MATCHER_H
#define _PERTUBIS_ENTRY_PROTECTOR_MATCHER_H 1

#include <string>

namespace perturbis
{
    /*! \brief abstract base class of a class that tests strings for some conditions
     * \ingroup PaludisAdapter
     */
    class Matcher
    {
        public:

            Matcher();

            virtual ~Matcher();

            virtual bool operator() (const std::string &) const = 0;


    };

    class TextMatcher;
    class PCREMatcher;
}

#endif

