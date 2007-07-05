
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_THREADS_H
#define _PERTUBIS_ENTRY_PROTECTOR_THREADS_H

#include <QThread>

#include "defines.hh"
#include <tr1/memory>

namespace paludis
{
	class Environment;
}

class QMenu;

namespace pertubis
{
	class Item;

	class KeywordManager;


	class PackageThread : public QThread
	{
		Q_OBJECT

	public:

		PackageThread(QObject* parent,std::tr1::shared_ptr<paludis::Environment> env);

		void run();

		void searchItem(QString str,bool name,bool desc);
		void searchPackages(QString str);

	private:

		void fetchItem();
		void fetchPackages();

		std::string									m_txt;
		std::tr1::shared_ptr<paludis::Environment> 	m_env;

		int											m_state;
		bool										m_optName;
		bool										m_optDesc;
		KeywordManager*								m_keywords;

	signals:
		void itemResult(Item* root);
		void packagesResult(Item* root);
		void searchItemFinished();
		void searchPackagesFinished();
	};
}
#endif


