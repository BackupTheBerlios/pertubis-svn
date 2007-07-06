
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

#ifndef _USEFLAGMANAGER_H
#define _USEFLAGMANAGER_H


#include <tr1/memory>
#include <string>
#include <vector>
#include <QMap>
#include <set>
#include <QTableWidget>
#include <QListWidget>

namespace paludis
{
    class Environment;
	class Repository;
}

namespace pertubis
{
	class UseFlagManagerError{};
	class DescNotFound : public UseFlagManagerError {};
	class DescSetNotFound : public UseFlagManagerError {};

	class UseFlagNotFound : public UseFlagManagerError {};
	class UseFlagSetNotFound : public UseFlagManagerError {};

	class UseFlagManager : public QWidget
	{
		Q_OBJECT

	public:

		UseFlagManager(QWidget* parent,std::tr1::shared_ptr<paludis::Environment> env);

	private:
		enum { USE,UPROFILE,UPROG,UUSER,DESC};
		std::string getDescription(const std::string& repository,const std::string& ufset, const std::string& useflag);

		void addDescription(const std::string& repository,const std::string& ufset, const std::string& useflag, const std::string& desc);
		void addUseflag(const std::string& repository,const std::string& ufset, const std::string& useflag, bool ison);
		bool isUseExpandVariable(QString line);
		void getUseExpandVariables(const std::string& repository);
		void getRepoVars(std::tr1::shared_ptr<const paludis::Repository> repo);
		QStringList expandVariable(QStringList& list);

		/*! we must store n useflag sets.
		* e.g. global useflags or specific ones for =x11-libs/qt-4.3.0
		* so we have a set -> useflag -> desc
		*/

		QListWidget*					m_setView;
		QTableWidget* 					m_table;

		std::set<std::string>			m_vset;
		std::tr1::shared_ptr<paludis::Environment> m_env;

	private slots:
// 		void slotLoadUseFlagDescPackages();

		/*! \brief loads all global useflags and their descriptions for the specific repository
		* \todo we must make this more failure tolerant
		*/

		void slotSetsChanged();
		void slotLoadUseFlagDescGlobal(std::tr1::shared_ptr<const paludis::Repository> repo);
		void slotLoadUseFlagDescLocal(std::tr1::shared_ptr<const paludis::Repository> repo);
		void slotFillTable(QListWidgetItem* ufset);
		void slotLoadTargetConfig(std::tr1::shared_ptr<const paludis::Repository> repo);
// 		void slotSaveGlobalUseflags();
	};

};
#endif

