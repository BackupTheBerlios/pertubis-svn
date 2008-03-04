
/* Copyright (C) 2007 Stefan Koegl
*
* This file is part of perturbis
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "MainWindow.hh"
#include "version.hh"
#include <QApplication>
#include <QTranslator>
#include <QString>
#include <QFile>
#include <QSettings>
#include <QDebug>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
    Q_INIT_RESOURCE(perturbis);

    if (argc == 1)
    {
        QFile config("/etc/perturbis/perturbis.conf");
        bool first_start(!config.exists());
        qDebug() << "first_start ?" << first_start;
        QSettings settings("/etc/perturbis/perturbis.conf",QSettings::IniFormat);
        settings.beginGroup("GeneralSettingsModel");
        QString lang = settings.value("language",":i18n/perturbis-en").toString();
        settings.endGroup();
        QTranslator t;
        t.load(lang);
        QApplication a(argc, argv);
        a.setApplicationName("perturbis");
        a.setOrganizationName("perturbis");
        a.installTranslator(&t);
        perturbis::MainWindow p(first_start);
        a.setActiveWindow(&p);
        p.show();
        return a.exec();
    }

    if (argc == 2 && strcmp(argv[1],"-v") == 0)
    {
        if (!QString(PERTUBIS_REVISION_NUMBER).isEmpty())
            printf("revision:\t%s\n",PERTUBIS_REVISION_NUMBER);
        else
            printf("version:\t%d.%d.%d\n",MAJOR_VERSION_NUMBER, MINOR_VERSION_NUMBER, PATCH_VERSION_NUMBER);
    }
    else
    {
        printf("usage: %s [-v]\n\n-v\t\tshow programm version\n",argv[0]);
    }

    return 0;
}
