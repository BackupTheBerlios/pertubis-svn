
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_MODEL_H

#include <QList>
#include <QString>
#include <QMap>
#include <QAbstractListModel>

namespace pertubis
{

	class Repository
	{
	public:
		QList<QMap<QString,QString> > m_sections;
	};

	class RepositoryModel : public QAbstractListModel
	{
		Q_OBJECT

	public:
		RepositoryModel(QObject* parent);

	private:
		QList<Repository>	m_repos;
	};

}

#endif

