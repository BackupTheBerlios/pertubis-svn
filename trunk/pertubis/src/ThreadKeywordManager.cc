
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

#include "ThreadKeywordManager.hh"
#include "PackageItem.hh"

#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QStringList>
#include <QRegExp>

pertubis::ThreadKeywordManager::ThreadKeywordManager(QObject* parent,
        paludis::tr1::shared_ptr<paludis::Environment> env) :
        ThreadBase(parent,env)
{
}

pertubis::ThreadKeywordManager::~ThreadKeywordManager()
{
}

int pertubis::ThreadKeywordManager::classifyKeywords(std::tr1::shared_ptr< const paludis::KeywordNameCollection > keywords) const
{
    QSet<QString> set;
    paludis::KeywordNameCollection::Iterator it=keywords->begin();
    while (it != keywords->end())
    {
        set.insert(stringify(*it).c_str());
        ++it;
    }
    return  classifyKeywords(set);
}

int pertubis::ThreadKeywordManager::classifyKeywords(QSet<QString> keywords)  const
{
    QSet<QString> tmp(keywords);

    if (!tmp.intersect(m_stableKeywords).empty() )
    {
        return Item::is_stable;
    }

    if (!keywords.intersect(m_unstableKeywords).empty())
    {
        return Item::is_unstable;
    }

    return Item::is_masked;
}

void pertubis::ThreadKeywordManager::run()
{
    QFile file("/etc/paludis/keywords.conf");

    if (!file.open(QIODevice::ReadOnly) )
        qFatal( "cannot open keywords.conf.reason: %d",file.error());
    QTextStream stream( &file );
    QString line;

    while ( !stream.atEnd() )
    {
        line = stream.readLine();
        if (line.isEmpty() ||
            line.indexOf('#') != -1 ||
            line.indexOf("*/*") == -1)
        {
            continue;
        }

        QStringList list = line.split(' ');
        list.removeFirst();

        QStringList::const_iterator keyword=list.constBegin();
        while (keyword != list.constEnd() )
        {
            if ((*keyword).startsWith(QString("~")))
                m_unstableKeywords.insert(*keyword);
            else
                m_stableKeywords.insert(*keyword);
            ++keyword;
        }
    }
    file.close();
}
