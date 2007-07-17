
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

#include "DatabaseView.hh"
#include "version.hh"
#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QSettings>
#include <stdio.h>

int main( int argc, char **argv )
{
	Q_INIT_RESOURCE(pertubis);

	if (argc == 1)
	{
		QApplication a( argc, argv );
		QSettings settings;
		QTranslator t;
		pertubis::DatabaseView p;

		a.setApplicationName("pertubis");
		a.setOrganizationName("pertubis");
		t.load(settings.value("i18npage/language",":i18n/pertubis-de").toString());
		a.installTranslator(&t);

		p.show();
		a.setActiveWindow(&p);
		return a.exec();
	}

	if (argc == 2 && strcmp(argv[1],"-v") == 0)
	{
		printf("%s\n",VERSION);
	}
	else
	{
		printf("usage: %s [OPTION]\n\n\
  -v\t\tshow programm version\n",argv[0]);
	}

	return 0;
}
