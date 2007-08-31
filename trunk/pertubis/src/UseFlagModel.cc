
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
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QRegExp>
#include <paludis/package_database.hh>
#include <paludis/util/iterator.hh>
#include <paludis/environments/environment_maker.hh>
#include <paludis/util/log.hh>
#include <paludis/util/stringify.hh>
#include <paludis/repository.hh>
#include <paludis/repository_info.hh>

pertubis::UseFlagModel::UseFlagModel(QObject* parent,std::tr1::shared_ptr<paludis::Environment> env) : QObject(parent),m_env(env)
{
	loadProfileUseflags();
}

pertubis::UseFlagModel::~UseFlagModel()
{
}

void pertubis::UseFlagModel::loadUseFlagDescGlobal(const paludis::Repository& repo)
{
	std::tr1::shared_ptr<const paludis::RepositoryInfo > info = repo.info(false);
	QString path(info->get_key_with_default("location","/usr/portage/gentoo").c_str() );
	path.append("/profiles/use.desc");
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

			if (rx.indexIn(line) == -1 && line != "")
			{
				QStringList list = line.split(spl);
            	addDescription(stringify(repo.name()).c_str(),"global",list[0],list[1]);
			}
        }
        file.close();
	}
}

void pertubis::UseFlagModel::loadUseFlagDescLocal(const paludis::Repository& repo)
{
 	using namespace paludis;
	std::tr1::shared_ptr<const RepositoryInfo> info = repo.info(false);
	QString path(info->get_key_with_default("location","/usr/portage/gentoo").c_str() );
	qDebug() << "loadUseFlagDescLocal - path" << path;
	path.append("/profiles/use.local.desc");
	qDebug() << "loadUseFlagDescLocal - path" << path;
	QFile file(path);

	if (file.open(QIODevice::ReadOnly) )
	{
		QTextStream stream( &file );

        while ( !stream.atEnd() )
		{
            QString line = stream.readLine().trimmed();

			if (line.indexOf('#') != -1 || line.isEmpty() || line.isNull() )
				continue;
			QStringList list = line.split(':');
			if (list.count() != 2)
				continue;
			QString use = list.at(1).section('-',0,0).trimmed();
			QString desc = list.at(1).section('-',1).trimmed();
			qDebug() << "line" << use << desc;
			addDescription(stringify(repo.name()).c_str(),list.at(0).trimmed(),use,desc);
        }
        file.close();
	}
}

void pertubis::UseFlagModel::addDescription(QString /*repository*/, QString ufset, QString useflag, QString desc)
{
	MSMap::iterator dset = m_descriptions.find(ufset);
	if (dset == m_descriptions.end() )
		dset = m_descriptions.insert(ufset,SMap());
	dset.value().insert(useflag,desc);
}

void pertubis::UseFlagModel::addUseflag(QString /*repository*/, QString ufset, QString useflag, bool ison)
{
// 	qDebug() << "UseFlagModel::addUseflag()" << ufset << useflag << ison;
	MBMap::iterator fset = m_useflags.find(ufset);
	if (fset == m_useflags.end() )
		fset = m_useflags.insert(ufset,BMap());
	fset.value().insert(useflag,ison);
}

QString pertubis::UseFlagModel::fileFinder(QString pathname, QString filename)
{
	qDebug() << pathname << filename;
	QDir path(pathname);
	forever
	{
		qDebug() << path.dirName();
		path.makeAbsolute();
		qDebug() << path.dirName();
		if (!path.exists())
			return QString();
		QStringList list = path.entryList();
		qDebug() << "list" << list;
		if (list.contains(filename) )
			return path.absoluteFilePath(filename);
		if (list.contains("base") && path.cd("base") )
		{
			list = path.entryList();
			if (list.contains(filename) )
				return path.absoluteFilePath(filename);
			path.cdUp();
		}

		if (!path.cdUp())
			break;
	}
	return QString();
}

void pertubis::UseFlagModel::loadRepoVars(const paludis::Repository& repo)
{
	using namespace paludis;

	MSMap::iterator vars = m_environVariables.find(stringify(repo.name()).c_str());
	if (vars == m_environVariables.end() )
	{
		vars = m_environVariables.insert(stringify(repo.name()).c_str(),SMap() ) ;
	}

	std::tr1::shared_ptr<const RepositoryInfo> info = repo.info(false);
	QString path( (info->get_key_with_default("location","/usr/portage/gentoo")+"/profiles/base/make.defaults").c_str() );

	QFile file(path);
	qDebug() << "path" << path;
	if ( !file.open(QIODevice::ReadOnly) )
	{
		qWarning("Warning: no make.defaults found in profile.");
		return;
	}

	QTextStream stream( &file );
	QString line;
	QString sy("*/*");
	QRegExp rcomment("#");
	while ( !stream.atEnd() )
	{
		line = stream.readLine().remove('"');
		if ( rcomment.indexIn(line) != -1 or line == QString::null or line == "")
			continue;
		QStringList list = line.split("=");
		if  (list.count() != 2)
		{
			qFatal("Repository Variable Parsing Error at line:\n%s\nat make.defaults",line.toLatin1().data());
		}
		vars.value().insert(list[0],list[1]);
	}
	file.close();
}

QString pertubis::UseFlagModel::description(QString /*repository*/, QString ufset, QString useflag) const
{
	MSMap::const_iterator fs = m_descriptions.find(ufset);
	if (fs == m_descriptions.constEnd() )
	{
		qWarning() << QString("\033[33mwarning: description for useflag %1 in useflagset %2 not found!\033[0m").arg(useflag).arg(ufset);
		return QString("unknown");
	}
	SMap::const_iterator uf = fs.value().find(useflag);
	if (uf != fs.value().constEnd() )
		return uf.value();
	return QString("unknown");
}

bool pertubis::UseFlagModel::useflag(QString /*repository*/, QString ufset, QString useflag) const
{
	BMap::const_iterator state = m_useflagChanges.find(useflag);
	if (state != m_useflagChanges.constEnd() )
		return state.value();
	MBMap::const_iterator fs = m_useflags.find(ufset);
	if (fs == m_useflags.constEnd() )
		return false;
	BMap::const_iterator uf = fs.value().find(useflag);
	if (uf == fs.value().constEnd() )
		return false;
	return uf.value();
}

bool pertubis::UseFlagModel::isExpandVariable(QString line) const
{
	if (line.indexOf(":") == -1)
		return false;
	return (m_environSet.find(line.remove(":")) != m_environSet.end());
}

void pertubis::UseFlagModel::loadExpandVariables(QString repository)
{
	using namespace paludis;
	MSMap::const_iterator it = m_environVariables.find(repository);
	if ( it == m_environVariables.constEnd() )
	{
		qWarning("No vars with repo name stored");
		return;
	}
	SMap::const_iterator it2 = it.value().find("USE_EXPAND");
	if ( it2 == it.value().end() )
	{
		qWarning("No USE_EXPAND var stored");
		return;
	}
// 	qDebug() << it2.value();
	QStringList vars  = QString(it2.value()).split(" ");

	for ( QStringList::ConstIterator var = vars.constBegin(); var != vars.constEnd(); ++var )
	{
        m_environSet.insert(*var);
    }
}

void pertubis::UseFlagModel::slotRefresh()
{

	m_useflags.clear();
	m_descriptions.clear();
	m_environVariables.clear();

// 	for (paludis::IndirectIterator<paludis::PackageDatabase::RepositoryIterator, const paludis::Repository>
// 			r((*m_env).package_database()->begin_repositories()), r_end((*m_env).package_database()->end_repositories()) ;
// 			r != r_end ; ++r)
// 	{
//
// 		if (r->format() == "ebuild")
// 		{
// 			loadRepoVars(*r);
// 			loadExpandVariables(stringify(r->name()).c_str());
// 			loadUseFlagDescGlobal( (*r) );
// 			loadUseFlagDescLocal( (*r) );
// 		}
// 	}
	loadTargetConfig();
}

void pertubis::UseFlagModel::expandVariables(QString /*repository*/, QString ufset, QStringList list)
{
	QString evar;
	QRegExp r_offPrefix("^-");
	evar = list.takeFirst().remove(":");
	GMBMap::iterator fset = m_expandVars.find(ufset);
	if (fset == m_expandVars.end() )
		fset = m_expandVars.insert(ufset,MBMap() );
	BMap subvars;

	for (QStringList::const_iterator subvar = list.constBegin();
			subvar != list.constEnd();
			++subvar)
		subvars.insert(*subvar,(r_offPrefix.indexIn( (*subvar).trimmed()) == 0) ? false : true);
}

bool pertubis::UseFlagModel::hasSet(QString /*repository*/, QString ufset)
{
	return m_useflags.find(ufset) != m_useflags.constEnd();
}

void pertubis::UseFlagModel::useflagSetData(QString /*repository*/, QString ufset)
{
	MBMap::const_iterator uset = m_useflags.find(ufset);
	QList<QVariantList> list;
	if (uset != m_useflags.constEnd() )
	{
		BMap::const_iterator use = uset.value().begin();
		while (use != uset.value().constEnd())
		{
			QVariantList row;
			QString desc(description("none",ufset,use.key()));
			bool md = m_makeDefaults.find(use.key()) != m_makeDefaults.end();
			bool ud = m_useDefaults.find(use.key()) != m_useDefaults.end();
			row <<
				use.key() <<
				(md ? Qt::Checked : Qt::Unchecked) <<
				(ud ? Qt::Checked : Qt::Unchecked) <<
				(use.value() ? Qt::Checked : Qt::Unchecked) <<
				desc;
			list << row;
			++use;
		}
	}
	emit useflagResult(list);
}

void pertubis::UseFlagModel::slotEditItemFlags(QString cat,QString pack,QString version,QString repository)
{
	using namespace paludis;
	QString ufset;
	CategoryNamePart c(cat.toLatin1().data());
	PackageNamePart p(pack.toLatin1().data());
	QualifiedPackageName qpn(c,p);
	RepositoryName n(repository.toLatin1().data());

	if (!m_env->package_database()->has_repository_named(n))
		return;
	std::tr1::shared_ptr< Repository > rep = m_env->package_database()->fetch_repository(n);
	QString iuse;

	if (version.isEmpty())
	{
		ufset =QString("%1/%2").arg(cat).arg(pack);

		std::tr1::shared_ptr< const VersionSpecCollection > specs = rep->version_specs(qpn);

		std::tr1::shared_ptr< const VersionMetadata > data = rep->version_metadata(qpn, *specs->last() );

		iuse = stringify(data->ebuild_interface->iuse).c_str();

	}
	else
	{

		ufset = QString("=%1/%2-%3").arg(cat).arg(pack).arg(version);

		std::tr1::shared_ptr< const VersionMetadata > data = rep->version_metadata(qpn, VersionSpec(version.toLatin1().data()) );

		iuse = stringify(data->ebuild_interface->iuse).c_str();

	}

	QList<QVariantList> res;
	QStringList list = iuse.split(" ");
	if (!list.isEmpty() )
	{
		bool found=hasSet("",ufset);

		QList<QString>::const_iterator use =  list.constBegin();
		while (use != list.constEnd() )
		{
			bool global = useflag("all","global",*use);
			bool local=false;
			if (found)
				local = useflag("all","global",*use);

			QString desc(description("none",ufset,*use));
			bool md = m_makeDefaults.find(*use) != m_makeDefaults.end();
			bool ud = m_useDefaults.find(*use) != m_useDefaults.end();
			QVariantList row;
			row <<
				*use <<
				(md ? Qt::Checked : Qt::Unchecked) <<
				(ud ? Qt::Checked : Qt::Unchecked) <<
				( (found ? local : global ) ? Qt::Checked : Qt::Unchecked) <<
				desc;
			res << row;
			++use;
		}
	}
	emit useflagResult(res);
}

void pertubis::UseFlagModel::loadTargetConfig()
{
	using namespace paludis;

	QString spl(" - ");
	QString leer(" ");

	QFile file("/etc/paludis/use.conf");
	if (! file.open(QIODevice::ReadOnly) )
		qFatal("cannot open useflag configuration file");

	QTextStream stream( &file );
	QString line;
	QString global_mark("*/*");
	QRegExp r_offPrefix("^-");

	QSet<QString> sets;
	while ( !stream.atEnd() )
	{
		line = stream.readLine().trimmed();
		QString ufset;
		QStringList list;

		if (line.indexOf("#") != -1 or line.isEmpty() or line.isNull() )
			continue;

		list = line.split(leer);

		if ( (*list.begin() == global_mark))
		{
			if (list.count() < 3)
				continue;
			ufset = "global";
			list.removeFirst();
		}
		else
		{
			if (list.count() < 2)
				continue;
			ufset = list.takeFirst();
		}

		sets.insert(ufset);

		if (isExpandVariable(list[0]) )
			expandVariables("all",ufset, list);
		else
		{
			QStringList::iterator it=list.begin();
			while (it != list.end() )
			{
				if ( r_offPrefix.indexIn(*it) == -1)
					addUseflag("all",ufset,*it,true);
				else
					addUseflag("all",ufset,(*it).remove(r_offPrefix),false);
				++it;
			}
		}
	}
	file.close();

	QStringList list2(sets.toList());
	list2.sort();
	emit setResult(list2);
}

void pertubis::UseFlagModel::loadProfileUseflags()
{
	QFile file("/etc/make.profile/make.defaults");

	if (file.open(QIODevice::ReadOnly) )
	{
		QTextStream stream( &file );
        QString line;

        while ( !stream.atEnd() )
		{
            line = stream.readLine().trimmed().remove('"');

			if (line.indexOf("#") != -1 || line.isEmpty())
				continue;

			if (line.indexOf("USE") != -1)
			{
				QStringList list = line.split("=");

				if (list.count() != 2)
					break;
				QStringList list2 = list.at(1).split(" ");
				QList<QString>::const_iterator it=  list2.constBegin();
				while (it != list2.constEnd())
				{
					m_makeDefaults.insert(*it);
					++it;
				}
			}
        }
        file.close();
	}

	QDir linkProfilePath("/etc/make.profile");
	QString realProfilePath = linkProfilePath.canonicalPath();

	file.setFileName(fileFinder(realProfilePath,"use.defaults"));

	if (file.open(QIODevice::ReadOnly) )
	{
		QTextStream stream( &file );
        QString line;

        while ( !stream.atEnd() )
		{
            line = stream.readLine();

			if (line.indexOf("#") != -1 || line.isEmpty())
				continue;

			QStringList list = line.split("\t",QString::SkipEmptyParts);

			m_useDefaults.insert(list[0]);
        }
        file.close();
	}
}
