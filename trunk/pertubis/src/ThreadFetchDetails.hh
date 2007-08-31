
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_THREAD_FETCH_DETAILS_H
#define _PERTUBIS_ENTRY_PROTECTOR_THREAD_FETCH_DETAILS_H

#include "ThreadBase.hh"
#include "PackageItem.hh"

#include <QString>

namespace pertubis
{
	class KeywordManager;

	/*! \brief this thread asks pertubis for package details and returns preformatted text.
	*
	*/
	class ThreadFetchDetails : public ThreadBase
	{
		Q_OBJECT

	public:

		ThreadFetchDetails(QObject* pobj, paludis::tr1::shared_ptr<paludis::Environment> env);
		void search(const paludis::tr1::shared_ptr<const paludis::PackageID>& id);
		void appendOutput(QString text);

	protected:

		void run();

	private:

		paludis::tr1::shared_ptr<const paludis::PackageID> m_id;
		mutable QString	m_output;

	signals:
		void detailsResult(QString output);
	};
}

#endif


