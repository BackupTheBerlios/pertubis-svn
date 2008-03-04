
/* Copyright (C) 2007-2008 Stefan Koegl
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


#include "RegexMatcher.hh"


using namespace perturbis;

RegexMatcher::RegexMatcher(const std::string& pattern) : m_regex(QString::fromStdString(pattern))
{
}

RegexMatcher::~RegexMatcher()
{
}

bool
RegexMatcher::operator() (const std::string & s) const
{
    return m_regex.indexIn(QString::fromStdString(s)) != -1;
}
