
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_DETAILS_H
#define _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_DETAILS_H

class QString;

#include <QThread>
#include <tr1/memory>

namespace paludis
{
	class Environment;
}

namespace pertubis
{
	class PackageDetails : public QThread
	{
		Q_OBJECT
	public:
		PackageDetails(std::tr1::shared_ptr<paludis::Environment> env, QObject* parent);

		void search(QString cn,QString pn,QString ver,QString rep, int status);

	protected:
		void run();
	signals:
		void sendDetails(QString details);

	private:
		std::tr1::shared_ptr<paludis::Environment> m_env;
		QString m_pn;
		QString m_cn;
		QString m_ver;
		QString m_rep;
		int	m_status;
	};
}

#endif

