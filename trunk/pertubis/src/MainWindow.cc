
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
#include "TaskBox.hh"
#include "DatabaseView.hh"
// #include "UseFlagManager.hh"

#include <paludis/util/log.hh>

#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QLabel>
#include <QLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QRegExp>
#include <QSettings>
#include <QStatusBar>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>

#include <paludis/environment.hh>
#include <paludis/environments/environment_maker.hh>

pertubis::Pertubis::Pertubis() : QMainWindow(0,Qt::Window), m_windowDatabaseView(0)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setAttribute(Qt::WA_QuitOnClose);
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	setWindowTitle( tr("pertubis :: Main Window")  );
	QPixmap logoIcon( ":logo.xpm" );
	setWindowIcon(logoIcon);

	m_box = new pertubis::TaskBox(this);

	createMenu();
	createToolbox();
	createDatabaseView();
	loadSettings();


	connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));
	show();
}

pertubis::Pertubis::~Pertubis()
{
	saveSettings();
	m_windowDatabaseView->close();
	if (m_windowDatabaseView !=0);
		delete m_windowDatabaseView;
}

void pertubis::Pertubis::loadSettings()
{
	QSettings settings;
	settings.beginGroup( "Pertubis" );
		resize(settings.value("size",QVariant(sizeHint())).toSize());
		move(settings.value("pos",QVariant(QPoint(0,0))).toPoint());
	settings.endGroup();
}

void pertubis::Pertubis::saveSettings()
{
	QSettings settings;
	settings.beginGroup( "Pertubis" );
		settings.setValue( "size", size() );
		settings.setValue( "pos", pos() );
	settings.endGroup();
}

void pertubis::Pertubis::createMenu()
{
	QMenu* menuFile = menuBar()->addMenu(tr("&File") );
	QMenu * menuOptions = menuBar()->addMenu(tr("&Options") );
	QMenu * menuHelp = menuBar()->addMenu(tr("&Help"));

	QAction * ac_q = menuFile->addAction( tr("&Quit") );
	ac_q->setShortcut(tr("ALT+F4"));
	ac_q->setStatusTip(tr("closing the pertubis suite"));
	connect(ac_q, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

	QAction* ac_pref = menuOptions->addAction(tr( "&Preferences"));
	ac_pref->setShortcut(tr("ALT+P"));
	ac_pref->setStatusTip(tr("configure pertubis"));
	connect(ac_pref,
			SIGNAL(triggered()),
			this,
   			SLOT(slotOpenPreferences()));

	QAction* ac_about = menuHelp->addAction( tr("&About"));
	ac_about->setShortcut(tr("F10"));
	ac_about->setStatusTip(tr("get program info"));
	connect(ac_about,
			SIGNAL(triggered()),
			this,
			SLOT(slotAbout()));

	QAction* ac_aqt = menuHelp->addAction(tr("About &Qt"));
    ac_aqt->setStatusTip(tr("Show the Qt library's About box"));
    connect(ac_aqt,
			SIGNAL(triggered()),
			qApp,
   			SLOT(aboutQt()));

	statusBar()->showMessage( tr("ready") );
}

void pertubis::Pertubis::createDatabaseView()
{
	m_windowDatabaseView = new DatabaseView(this,m_box);
	QPixmap pic(":documents-2.0_32.xpm");
	QAction * ac_browse = m_toolbar->addAction( pic, tr("&package window"));
	ac_browse->setShortcut(tr("F9"));
	ac_browse->setStatusTip(tr("browsing and searching"));
	connect(ac_browse,
		SIGNAL(triggered()),
		this,
		SLOT(slotToggleWindowDatabaseView()));
}

void pertubis::Pertubis::createToolbox()
{
	m_toolbar = addToolBar( tr("pertubis tasks") );
	QString dir = qApp->applicationDirPath();
	QPixmap p1(":reload-b-2.1_32.xpm");
	QAction * ac_sync = m_toolbar->addAction( p1, tr("sync"));
	ac_sync->setShortcut(tr("F9"));
	ac_sync->setStatusTip(tr("it is sufficient to sync your repositories on a daily base"));
	setCentralWidget(m_toolbar);
	connect(ac_sync,
			SIGNAL(triggered()),
			this,
   			SLOT(slotSync()),
			Qt::DirectConnection);
}

void pertubis::Pertubis::slotToggleWindowDatabaseView()
{
	if (m_windowDatabaseView == 0)
		return;
	m_windowDatabaseView->setVisible(!m_windowDatabaseView->isVisible());
}

void pertubis::Pertubis::slotSync()
{
	QMessageBox::information( this, tr("Sync"),
                        "Must be implemented!");
}

void pertubis::Pertubis::slotAbout()
{
    QMessageBox::about( this, tr("&About"),
                        "Use it for your daily gentoo housekeeping!");
}

void pertubis::Pertubis::slotOpenPreferences()
{
    QMessageBox::information( this, tr("Preferences"),
                        "Must be implemented!");
}

void pertubis::Pertubis::slotAboutQt()
{
    QMessageBox::aboutQt( this, tr("About &QT") );
}
