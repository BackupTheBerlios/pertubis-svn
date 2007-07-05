
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

#include "UseFlagModel.hh"


void pertubis::UseFlagModel::slotLoadUseFlagDescGlobal(std::tr1::shared_ptr<const paludis::Repository> repo)
{
	std::cout << __FUNCTION__ << "\n";
 	using namespace paludis;

	std::tr1::shared_ptr<const RepositoryInfo> info = repo->info(false);
	QString path(info->get_key_with_default("location","/usr/portage/gentoo").c_str() );
	path.append("/profiles/use.desc");
	std::cout << path.toLatin1() << "\n";
	QFile file(path);
	QString spl(" - ");
	if (file.open(QIODevice::ReadOnly) )
	{
		QTextStream stream( &file );
        QString line;

        while ( !stream.atEnd() )
		{
            line = stream.readLine();
			QRegExp rx("^#");

			if (rx.search(line) == -1 && line != "")
			{
				QStringList list = line.split(spl);
            	addDescription(stringify(repo->name()),"global",list[0].toLatin1(),list[1].toLatin1());
			}
        }
        file.close();
	}
}

void pertubis::UseFlagModel::slotLoadUseFlagDescLocal(std::tr1::shared_ptr<const paludis::Repository> repo)
{
	std::cout << __FUNCTION__ << "\n";
 	using namespace paludis;
	std::tr1::shared_ptr<const RepositoryInfo> info = repo->info(false);
	QString path(info->get_key_with_default("location","/usr/portage/gentoo").c_str() );
	path.append("/profiles/use.local.desc");
	std::cout << path.toLatin1() << "\n";
	QFile file(path);
	QString spl(" - ");
	if (file.open(QIODevice::ReadOnly) )
	{
		QTextStream stream( &file );
        QString line;

        while ( !stream.atEnd() )
		{
            line = stream.readLine().stripWhiteSpace();

			if (line.find("#") == -1 && line != "")
			{
				QStringList list = line.split(":");
				QStringList list2 = list.at(1).split(" - ");
            	addDescription(stringify(repo->name()),list.at(0).toLatin1(),list2.at(0).toLatin1(),list2.at(1).toLatin1());
			}
        }
        file.close();
	}
}

