
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_KEYWORD_MANAGER_H
#define _PERTUBIS_ENTRY_PROTECTOR_THREADS_H

#include <tr1/memory>
#include <map>
#include <set>
#include <string>
#include <paludis/name.hh>
#include <paludis/util/collection_concrete.hh>

namespace pertubis
{
	class KeywordManager
	{
		public:
			KeywordManager() {loadKeywords(); }
			int classifyKeywords(std::tr1::shared_ptr< const paludis::KeywordNameCollection > keywords);
			void loadKeywords();
			std::set<std::string> m_stableKeywords;
			std::set<std::string> m_unstableKeywords;
	};

}
#endif
