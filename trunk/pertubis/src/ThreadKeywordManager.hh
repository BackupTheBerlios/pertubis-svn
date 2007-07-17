
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
#define _PERTUBIS_ENTRY_PROTECTOR_KEYWORD_MANAGER_H


#include <QSet>
#include <QString>
#include "ThreadBase.hh"
#include <paludis/name.hh>

namespace pertubis
{
	/*! \brief Retrieves and holds information about keywords the system uses and classifies it as stable/unstable.
	*
	*/
	class ThreadKeywordManager : public ThreadBase
	{
		Q_OBJECT
		public:
			ThreadKeywordManager(QObject* parent,
								 std::tr1::shared_ptr<paludis::Environment> env);
			virtual ~ThreadKeywordManager();
			int classifyKeywords(QSet<QString> keywords) const;
			int classifyKeywords(std::tr1::shared_ptr< const paludis::KeywordNameCollection > keywords) const;

		protected:

			void run();

		private:

			void loadKeywords();
			QSet<QString>	m_stableKeywords;
			QSet<QString>	m_unstableKeywords;

	};
}

#endif
