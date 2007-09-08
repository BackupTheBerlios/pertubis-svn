
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
#include <QDebug>
#include <stdio.h>

int main( int argc, char **argv )
{
    Q_INIT_RESOURCE(pertubis);

    if (argc == 1)
    {
        QApplication a( argc, argv );
        a.setApplicationName("pertubis");
        a.setOrganizationName("pertubis");
        QTranslator t;

        QSettings settings;
        settings.beginGroup( "i18npage" );
        QString lang = settings.value("language").toString();
        settings.endGroup();
        qDebug() << "lang = " << lang.toLatin1().data();
        t.load(lang);
        a.installTranslator(&t);
        pertubis::DatabaseView p;
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


// #include "PackageItem.hh"
// #include <QList>
// #include <QVariant>
//
// int main( int argc, char **argv )
// {
//     QApplication a( argc, argv );
//     QVariantList reslist;
//     QVariantMap resmap;
//     reslist << true << false;
//     resmap.insert("1",true);
//     resmap.insert("2",false);
//
//     QVariantList bl;
//     bl << reslist << 1 << 2 << 3 << "4";
//     QVariantList bm;
//     bm << resmap << 1 << 2 << 3 << "4";
//     QVariant iter;
//     foreach (iter,bl)
//         qDebug() << iter;
//     qDebug() << "\n\n";
//     foreach (iter,bm)
//         qDebug() << iter;
//     return 0;
// }
