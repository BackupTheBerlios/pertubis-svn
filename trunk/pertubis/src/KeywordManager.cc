
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

#include "KeywordManager.hh"
#include "PackageItem.hh"

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QStringList>

void pertubis::KeywordManager::loadKeywords()
{
	QFile file("/etc/paludis/keywords.conf");
	QString spl(" ");
	if (file.open(QIODevice::ReadOnly) )
	{
		QTextStream stream( &file );
        QString line;

        while ( !stream.atEnd() )
		{
            line = stream.readLine(); // line of text excluding '\n'
			QRegExp rex_comment("#");
			QRegExp rex_global("\\*/\\*");

			if (line.isEmpty() ||
				rex_comment.indexIn(line) != -1 ||
				rex_global.indexIn(line) == -1)
			{
				continue;
			}
			QStringList list = line.split(spl);
			QStringList::iterator start=++(list.begin()),end(list.end());
			while (start != end)
			{
				if ((*start).startsWith(QString("~")))
					m_unstableKeywords.insert(start->toLatin1().data());
				else
					m_stableKeywords.insert(start->toLatin1().data());
				++start;
			}
        }
        file.close();
	}
}

int pertubis::KeywordManager::classifyKeywords(std::tr1::shared_ptr< const paludis::KeywordNameCollection > keywords)
{
	std::set<std::string>::const_iterator mykeyword = m_stableKeywords.begin();
	while ( mykeyword != m_stableKeywords.end() )
	{
		if (keywords->find(paludis::KeywordName(*mykeyword)) != keywords->end())
		{
			return Item::is_stable;
		}
		++mykeyword;
	}

	mykeyword = m_unstableKeywords.begin();
	while ( mykeyword != m_unstableKeywords.end() )
	{
		if (keywords->find(paludis::KeywordName(*mykeyword)) != keywords->end())
		{
			return Item::is_unstable;
		}
		++mykeyword;
	}

	return Item::is_masked;
}
