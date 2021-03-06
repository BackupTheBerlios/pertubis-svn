
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

#include "text_matcher.hh"
#include <string.h>

using namespace perturbis;

TextMatcher::TextMatcher(const std::string & s) : m_pattern(s)
{
}

bool
TextMatcher::operator() (const std::string & s) const
{
    return 0 != strcasestr(s.c_str(), m_pattern.c_str());
}

TextMatcher::~TextMatcher()
{
}
