
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PERTUBIS_H
#define _PERTUBIS_ENTRY_PROTECTOR_PERTUBIS_H

#include <QMainWindow>

class QCloseEvent;

#include <tr1/memory>

namespace paludis
{
	class Environment;
}

namespace pertubis
{
	class DatabaseView;
	class RepoConf;
	class TaskBox;
// 	class UseFlagManager;

	class Pertubis : public QMainWindow
	{
		Q_OBJECT

	public:
		Pertubis();
		~Pertubis();

	private:

		void createMenu();
		void createDatabaseView();
		void createToolbox();

		void loadSettings();
		void saveSettings();

// 		std::tr1::shared_ptr<paludis::Environment> 	m_env;
		DatabaseView* 			m_windowDatabaseView;
		QToolBar* 				m_toolbar;
		TaskBox*				m_box;

	private slots:

		void slotAbout();
		void slotAboutQt();
		void slotOpenPreferences();
		void slotToggleWindowDatabaseView();
		void slotSync();

	};
}
#endif

