
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

#include "PackageItem.hh"

#include <tr1/memory>

namespace paludis
{
	class Environment;
}

class QMenu;


#include <QStringList>

namespace pertubis
{


	class KeywordManager;


	class PackageThread : public QThread
	{
		Q_OBJECT

	public:

		PackageThread(QObject* parent);

		void run();

		void searchItem(QString str,bool name,bool desc);
		void searchPackages(QString str);
		void searchCategories();
		void searchDetails(QString cn,QString pn,QString ver,QString rep, Item::ItemStatus status);

	private:

		enum FetchState { fs_item,fs_package,fs_details,fs_categories};

		void fetchItem();
		void fetchPackages();
		void fetchDetails();
		void fetchCategories();

		QString									m_txt1;
		QString									m_txt2;
		QString									m_txt3;
		QString									m_txt4;
		std::tr1::shared_ptr<paludis::Environment> 	m_env;
		KeywordManager*								m_keywords;
		Item::ItemStatus							m_status;
		FetchState									m_state;
		bool										m_optName;
		bool										m_optDesc;

	signals:
		void itemResult(Item* root);
		void categoriesResult(QStringList cl);
		void packagesResult(Item* root);
		void detailsResult(QString output);
		void searchItemFinished();
		void searchPackagesFinished();
	};
}
#endif


