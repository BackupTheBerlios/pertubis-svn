
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

#include "UseFlagEditor.hh"
#include "UseFlagModel.hh"

#include <QCloseEvent>
#include <QListWidget>
#include <QDebug>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include <QVector>
#include <QHeaderView>
#include <QSplitter>

#include <QVBoxLayout>
#include <QPixmap>

pertubis::UseFlagEditor::UseFlagEditor(QWidget* parent,
						  std::tr1::shared_ptr<paludis::Environment> env) : QWidget(parent), m_env(env)
{
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setMargin(0);
	setLayout(layout);
	m_model = new UseFlagModel(this,env);

	m_setView = new QTableWidget(1,1,this);
	m_setView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	m_setView->verticalHeader()->setVisible(false);
	m_setView->setShowGrid(false);

	m_table = new QTableWidget(0,5,this);
	m_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	m_table->verticalHeader()->setVisible(false);
	m_table->setShowGrid(false);
	QVector<QString> header(5);

	header[UseFlagModel::uo_flag] = tr("Useflag");
	header[UseFlagModel::uo_profile] = tr("Profile");
	header[UseFlagModel::uo_prog] = tr("Progs");
	header[UseFlagModel::uo_user] = tr("User") ;
	header[UseFlagModel::uo_desc] = tr("Description");

	m_table->setHorizontalHeaderLabels( header.toList() );
	setWindowTitle(tr("pertubis :: Useflag Editor"));
	QPixmap logoIcon(":images/logo.xpm" );
	setWindowIcon(logoIcon);

	connect(m_setView,
			SIGNAL(currentItemChanged( QTableWidgetItem *, QTableWidgetItem * )),
			this,
			SLOT(slotUseflagsChanged( QTableWidgetItem *, QTableWidgetItem *)) );

	connect(m_model,
			SIGNAL(setResult(QStringList) ),
			this,
			SLOT(slotSetResult(QStringList) ));

	connect(m_model,
			SIGNAL(useflagResult(QList<QVariantList>) ),
			this,
			SLOT(slotUseflagResult(QList<QVariantList>) ));

	createToolbar();

	m_hSplit = new QSplitter(Qt::Vertical,this);
	m_hSplit->addWidget(m_setView);
	m_hSplit->addWidget(m_table);
	layout->addWidget(m_toolbar);
	layout->addWidget(m_hSplit);
	loadSettings();
	m_model->slotRefresh();
	m_setView->setHorizontalHeaderLabels( QStringList() << tr("useflag set"));
}

pertubis::UseFlagEditor::~UseFlagEditor()
{
	saveSettings();
}

void pertubis::UseFlagEditor::createToolbar()
{
	m_toolbar = new QToolBar( tr("useflag actions"),this);
	m_toolbar->layout()->setMargin(0);
	m_toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
	QAction * ac_reload = m_toolbar->addAction( QPixmap(":images/reload_22.xpm"),"reload");
	ac_reload->setToolTip(tr("<html><body><h1><u>%1</u></h1><p>Pertubis only loads automatically the useflag configuration on startup.</p><p>If you manually change these files pertubis is out of sync with your system.</p><p>Press this button to reload everything!</p></body></html>").arg(ac_reload->text()));

	QAction * ac_setbar = m_toolbar->addAction( QPixmap(":images/setbar_22.xpm"),"setlist");
	ac_setbar->setToolTip(tr("<html><body><h1><u>%1</u></h1></p><p>Enable or disable the useflag set list for a better view on the useflags</p></body></html>").arg(ac_setbar->text()));

	QAction * ac_accept = m_toolbar->addAction( QPixmap(":images/save_22.xpm"),"save changes");
	ac_accept->setToolTip(tr("<html><body><h1><u>%1</u></h1><p>You have to save your changes when you are finished. Otherwise all changes will be lost</p></body></html>").arg(ac_accept->text()));

	QAction * ac_undo = m_toolbar->addAction( QPixmap(":images/undo_22.xpm"),"undo changes");
	ac_undo->setToolTip(tr("<html><body><h1><u>%1</u></h1><p>Press this button to restore the former settings!</p></body></html>").arg(ac_undo->text()));

	QAction * ac_filter = m_toolbar->addAction( QPixmap(":images/cleanflags_22.xpm"),"clean useflags");
	ac_filter->setToolTip(tr("<html><body><h1><u>%1</u></h1><p>From time to time some useflags are added or deleted.\nYou can delete these old, unknown or unused useflags.</p><p><b>Don't forget saving!</b></p></body></html>").arg(ac_filter->text()));

	connect(ac_setbar,
			SIGNAL(triggered()),
			this,
   			SLOT(slotToggleSetTable()),
			Qt::DirectConnection);
}

void pertubis::UseFlagEditor::slotToggleSetTable()
{
	m_setView->setVisible(!m_setView->isVisible());
}

void pertubis::UseFlagEditor::slotUseflagsChanged(QTableWidgetItem* current, QTableWidgetItem* /*previous*/ )
{
	QString ufset(current->text());

	if (ufset.isEmpty() or ufset.isNull())
		return;
	m_model->useflagSetData("none",ufset);
}

void pertubis::UseFlagEditor::slotUseflagResult(QList<QVariantList> list)
{
	m_table->setRowCount(list.count() );
	QList<QVariantList>::const_iterator row = list.constBegin();
	int i=0;
	while (row != list.constEnd() )
	{
		QTableWidgetItem* i_flag = new QTableWidgetItem(row->value(UseFlagModel::uo_flag).toString());
		m_table->setItem(i,UseFlagModel::uo_flag,i_flag);

		QTableWidgetItem* i_profile = new QTableWidgetItem;
		i_profile->setFlags(Qt::ItemIsSelectable);
		i_profile->setCheckState( static_cast<Qt::CheckState>(row->value(UseFlagModel::uo_profile).toInt()) );
		m_table->setItem(i,UseFlagModel::uo_profile,i_profile);

		QTableWidgetItem* i_prog = new QTableWidgetItem;
		i_prog->setFlags(Qt::ItemIsSelectable);
		i_prog->setCheckState( static_cast<Qt::CheckState>(row->value(UseFlagModel::uo_prog).toInt()) );
		m_table->setItem(i,UseFlagModel::uo_prog,i_prog);

		QTableWidgetItem* i_user = new QTableWidgetItem;
		i_user->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
		i_user->setForeground(QBrush(QColor(0,0,255)));
		i_user->setCheckState( static_cast<Qt::CheckState>(row->value(UseFlagModel::uo_user).toInt()));
		m_table->setItem(i,UseFlagModel::uo_user,i_user);

		QTableWidgetItem* i_desc = new QTableWidgetItem(row->value(UseFlagModel::uo_desc).toString());
		m_table->setItem(i,UseFlagModel::uo_desc,i_desc);
		++i;
		++row;
	}
}

void pertubis::UseFlagEditor::slotSetResult(QStringList list)
{
	m_setView->clear();
	m_setView->setRowCount(list.count());
	int i=0;
	QStringList::const_iterator it = list.constBegin();
	while (it != list.constEnd() )
	{
		m_setView->setItem(i,0,new QTableWidgetItem(*it));
		++it;
		++i;
	}
}

void pertubis::UseFlagEditor::loadSettings()
{
	QSettings settings;
	settings.beginGroup( "UseFlagEditor" );
		m_setView->setVisible(settings.value( "setview_visible",true).toBool());
		m_hSplit->restoreState(settings.value("hsplt").toByteArray());
	settings.endGroup();
}

void pertubis::UseFlagEditor::saveSettings()
{
	QSettings settings;
	settings.beginGroup( "UseFlagEditor" );
		settings.setValue("setview_visible", m_setView->isVisible() );
		settings.setValue("hsplt", m_hSplit->saveState());
	settings.endGroup();
}
