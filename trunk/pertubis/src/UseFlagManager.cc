
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


#include <paludis/package_database.hh>
#include <paludis/environment.hh>

#include "UseFlagManager.hh"

#include "defines.hh"
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QCheckBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QLayout>
#include <QRegExp>
#include <QPixmap>

#include <iostream>


pertubis::UseFlagManager::UseFlagManager(QWidget* parent,std::tr1::shared_ptr<paludis::Environment> env) : QWidget(parent,Qt::Window),m_env(env)
{
	std::cout << __FUNCTION__ << "() - start\n";
	QVBoxLayout* layout = new QVBoxLayout(this,0,0);

	QSplitter* hsplit = new QSplitter(Qt::Vertical,this);
	layout->addWidget(hsplit,10);

	m_setView = new QListWidget(this);
	hsplit->addWidget(m_setView);
	m_table = new QTableWidget(0,5,this);
	QStringList h;
	h << tr("Useflag") << tr("Profile") << tr("Progs") <<  tr("User") << tr("Description");
	m_table->setHorizontalHeaderLabels( h );

// 	m_table->setColumnStretchable(USE,false);
// 	m_table->setColumnStretchable(UPROFILE,false);
// 	m_table->setColumnStretchable(UPROG,false);
// 	m_table->setColumnStretchable(UUSER,false);
// 	m_table->setColumnStretchable(DESC,false);
	m_table->setGridStyle(Qt::NoPen);
	hsplit->addWidget(m_table);

	paludis::RepositoryName rname("gentoo");
	std::tr1::shared_ptr<paludis::Repository> repo = m_env->package_database()->fetch_repository(rname);
// 	slotLoadUseFlagDescGlobal(repo);
// 	slotLoadUseFlagDescLocal(repo);
// 	getRepoVars(repo);
// 	getUseExpandVariables("gentoo");
// 	slotLoadTargetConfig(repo);
// 	fillTable(m_useflags.firstChild());
// 	slotSetsChanged();
	setCaption(PROGNAME " :: Useflag Manager");
	QPixmap logoIcon( "/home/gensk/dev/paludis_gui/pix/logo.xpm" );
	setIcon(logoIcon);
	resize(400,500);

	connect(m_setView,
			SIGNAL(selectionChanged( QListWidgetItem* )),
			this,
			SLOT(slotFillTable( QListWidgetItem* )) );
	show();
	std::cout << __FUNCTION__ << "() - done\n";
}


void pertubis::UseFlagManager::addDescription(const std::string& repository,const std::string& ufset, const std::string& useflag, const std::string& desc)
{
// 	std::cout << __FUNCTION__ << "\n";
	std::pair<std::string,std::string>  uf(useflag,desc);
	DescSet::iterator fset = m_descriptions.find(ufset);
	if (fset != m_descriptions.end() )
		fset->second.insert(uf);
	else
	{
		Desc sm;
		sm.insert(uf);
		m_descriptions.insert(std::pair<std::string,Desc>(ufset,sm));
	}
}

void pertubis::UseFlagManager::addUseflag(const std::string& repository,const std::string& ufset, const std::string& useflag, bool ison)
{
// 	std::cout << __FUNCTION__ << "\n";
	std::pair<std::string,bool>  uf(useflag,ison);
	UseFlagSet::iterator fset = m_useflags.find(ufset);
	if (fset != m_useflags.end() )
		fset->second.insert(uf);
	else
	{
		UseFlag sm;
		sm.insert(uf);
		m_useflags.insert(std::pair<std::string,UseFlag>(ufset,sm));
	}
}

void pertubis::UseFlagManager::slotSetsChanged()
{
	m_setView->clear();
	UseFlagSet::iterator it = m_useflags.begin();
	while (it != m_useflags.end() )
	{
		new QListWidgetItem(it->first.c_str(),m_setView);
		++it;
	}
}

std::string pertubis::UseFlagManager::getDescription(const std::string& repository,const std::string& ufset, const std::string& useflag)
{
// 	RepoSet::iterator rep = m_descriptions.find(repository);
// 	if (rep != m_descriptions.end() )
// 	{
	DescSet::iterator fs = m_descriptions.find(ufset);
	if (fs == m_descriptions.end() )
		throw DescSetNotFound();
	Desc::iterator uf = fs->second.find(useflag);
	if (uf == fs->second.end() )
		throw DescNotFound();
	return uf->second;
// 	}
}

bool pertubis::UseFlagManager::isUseExpandVariable(QString line)
{
	if (line.find(":") == -1)
		return false;

	bool res = (m_vset.find(line.remove(":").toLatin1()) != m_vset.end());
	std::cout << line.toLatin1() << " " << res << " res of expand\n";
	return res;
}

void pertubis::UseFlagManager::getUseExpandVariables(const std::string& repository)
{
	DescSet::iterator it = m_environVariables.find(repository);
	if ( it == m_environVariables.end() )
		qFatal("No vars with repo name stored");
	Desc::iterator it2 = it->second.find("USE_EXPAND");
	std::cout << "it2 = " << it2->second << "\n";
	if ( it2 == it->second.end() )
		qFatal("No USE_EXPAND var stored");
	QStringList vars  = QString(it2->second.c_str()).split(" ");

	for ( QStringList::Iterator it3 = vars.begin(); it3 != vars.end(); ++it3 )
	{
		std::cout << "use_expand: " << (*it3).toLatin1() << "\n";
        m_vset.insert((*it3).toLatin1());
    }
	std::cout << "size of vset " << m_vset.size() << "\n";

}

QStringList pertubis::UseFlagManager::expandVariable(QStringList& list)
{
	QString expandPrefix(list[0].remove(":").lower()+"_");
	list.pop_front();
	std::cout << "expandPrefix " << expandPrefix.toLatin1() << "\n";
	QStringList::iterator it=list.begin();
	++it;
	while (it != list.end() )
	{
		(*it) = expandPrefix+(*it);
		++it;
	}
	return list;
}

void pertubis::UseFlagManager::getRepoVars(std::tr1::shared_ptr<const paludis::Repository> repo)
{
	using namespace paludis;

	DescSet::iterator vars = m_environVariables.find(stringify(repo->name()));
	if (vars == m_environVariables.end() )
	{
		m_environVariables.insert(std::pair<std::string,Desc>(stringify(repo->name()),Desc() ) );
		vars = m_environVariables.find(stringify(repo->name()));
	}
	std::tr1::shared_ptr<const RepositoryInfo> info = repo->info(false);
	QString path( (info->get_key_with_default("location","/usr/portage/gentoo")+"/profiles/base/make.defaults").c_str() );
// 	std::cout << path.toLatin1() << "\n";
	QFile file(path);
	if ( !file.open(QIODevice::ReadOnly) )
	{
		qFatal("cannot open make.defaults in profile");
	}

	QTextStream stream( &file );
	QString line;
	QString sy("*/*");
	QRegExp rcomment("#");
	while ( !stream.atEnd() )
	{
		line = stream.readLine().remove('"');
		if ( rcomment.search(line) != -1 or line == QString::null or line == "")
			continue;
		QStringList list = line.split("=");
		if  (list.count() != 2)
		{
			qFatal(QString("Repository Variable Parsing Error at line:\n%1\nat make.defaults").arg(line));
		}
// 		std::cout << list[0] << " " << list[1] << "\n";
		vars->second.insert(std::pair<std::string,std::string>(list[0].toLatin1(),list[1].toLatin1()));
	}
	file.close();
}

void pertubis::UseFlagManager::slotLoadTargetConfig(std::tr1::shared_ptr<const paludis::Repository> repo)
{
	using namespace paludis;

	QString spl(" - ");
	QString leer(" ");
	std::string toExpand;

	QFile file("/etc/paludis/use.conf");
	if (! file.open(QIODevice::ReadOnly) )
	{
		throw UseFlagManagerError();
	}
	QTextStream stream( &file );
	QString line;
	QString global_mark("*/*");
	QRegExp ron("^-");
	std::string reponame(stringify(repo->name()));
	while ( !stream.atEnd() )
	{
		line = stream.readLine().stripWhiteSpace();
		std::string ufset;
		QStringList list;

		if (line.find("#") != -1 or line.isEmpty() or line.isNull() )
			continue;

		list = line.split(leer);

		if ( (*list.begin() == global_mark))
			ufset = "global";
		else
			ufset = (list.begin()->toLatin1() );
// 		std::cout << "ufset " << *list.begin() << " " << ufset << "\n";
		list.pop_front();

		if (isUseExpandVariable(*list.begin()))
			list = expandVariable(list);

		QStringList::iterator it=list.begin();
		while (it != list.end() )
		{
			if ( ron.search(*it) == -1)
				addUseflag(reponame,ufset,(*it).toLatin1(),true);
			else
				addUseflag(reponame,ufset,(*it).remove(ron).toLatin1(),false);
			++it;
		}
	}
	file.close();
}

void pertubis::UseFlagManager::slotFillTable(QListWidgetItem* ufset)
{
	std::cout << __FUNCTION__ << " - start\n";
	std::string setname;
	try
	{
		setname = ufset->text().toLatin1();
	}
	catch(...)
	{
		return;
	}
	UseFlagSet::const_iterator it = m_useflags.find(setname);
	if (it != m_useflags.end() )
	{
		int i=0;
		UseFlag::const_iterator use = it->second.begin();
		m_table->setRowCount(it->second.size() );
		while (use != it->second.end() )
		{
			m_table->item(i,0)->setText(use->first.c_str());
			try
			{
				m_table->item(i,DESC)->setText(getDescription("gentoo",setname,use->first ).c_str());
			}
			catch(...)
			{
			}
			QTableWidgetItem* tmp = new QTableWidgetItem("");
			QTableWidgetItem* tmp1 = new QTableWidgetItem("");
			QTableWidgetItem* tmp2 = new QTableWidgetItem("");

			tmp->setFlags(Qt::ItemIsTristate);
			tmp1->setFlags(Qt::ItemIsTristate);
			tmp2->setFlags(Qt::ItemIsTristate);
			tmp2->setCheckState( use->second ? Qt::Checked :Qt::Unchecked );
			m_table->setItem(i,UPROFILE,tmp);
			m_table->setItem(i,UPROG,tmp1);
			m_table->setItem(i,UUSER,tmp2);
			++use;
			++i;
		}
	}
	m_table->adjustSize();
	std::cout << __FUNCTION__ << " - done\n";
}

