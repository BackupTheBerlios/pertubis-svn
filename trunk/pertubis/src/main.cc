
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

#include "MainWindow.hh"
#include "version.hh"
#include <iostream>
#include <QApplication>


int main( int argc, char **argv )
{


	Q_INIT_RESOURCE(pertubis);

	if (argc == 1)
	{
		QApplication a( argc, argv );
		a.setApplicationName("pertubis");
		pertubis::Pertubis* p = new pertubis::Pertubis();
		a.setActiveWindow(p);
		return  a.exec();
	}

	if (argc == 2 && strcmp(argv[1],"-v") == 0)
	{
		std::cout << "\n\033[32mpertubis version: " <<  VERSION << "\033[0m\n";
	}

	if (argc == 2 && strcmp(argv[1],"-h") == 0)
	{
		std::cout << "\n\033[32musage:\n";
		std::cout << argv[0] << "    = starting the application\n";
		std::cout << argv[0] << " -h = help\n";
		std::cout << argv[0] << " -v = version information\n\033[0m\n";
	}

	return 0;
}
