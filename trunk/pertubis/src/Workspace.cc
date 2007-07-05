
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

#include "Workspace.hh"
#include "TaskBox.hh"

#include <paludis/environment.hh>
#include <paludis/name.hh>
#include <paludis/environments/environment_maker.hh>

#include <QMenu>
#include <QVariant>
#include <QMap>
#include <QDebug>

pertubis::Workspace::Workspace(QObject* parent) : QObject(parent)
{
	m_env = paludis::EnvironmentMaker::get_instance()->make_from_spec("");
	m_box = new TaskBox(parent);
}


