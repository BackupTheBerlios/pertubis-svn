
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_THREAD_FETCH_PACKAGES_H
#define _PERTUBIS_ENTRY_PROTECTOR_THREAD_FETCH_PACKAGES_H



#include "ThreadBase.hh"



namespace paludis
{
	class Environment;
}

class QMenu;


#include <QStringList>

namespace pertubis
{
	class ThreadKeywordManager;
	class Item;
	class TaskBox;

	class ThreadFetchPackages : public ThreadBase
	{
		Q_OBJECT

	public:

		ThreadFetchPackages(QObject* parent,
							std::tr1::shared_ptr<paludis::Environment> env,
							ThreadKeywordManager* keywords,
						   TaskBox* box);

		void run();

		void searchPackages(QString str);

	private:

		void fetchPackages();

		QString									m_query;
		ThreadKeywordManager*					m_keywords;
		TaskBox*								m_box;

	signals:
		void packagesResult(Item* root);
	};
}
#endif


