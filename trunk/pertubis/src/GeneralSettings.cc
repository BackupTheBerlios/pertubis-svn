
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of pertubis
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

#include "GeneralSettings.hh"

#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QTranslator>

static QStringList findTranslationFiles()
{
    QDir dir(":i18n");
    QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files,
                                          QDir::Name);


    for (QList<QString>::iterator i(fileNames.begin()), iEnd(fileNames.end());
         i != iEnd;++i)
    {
        QFileInfo info(*i);
        QString tmp(dir.filePath(info.baseName()));
        qDebug() << "pertubis::findTranslationFiles() - found" << tmp;
        *i = tmp;
    }

    return fileNames;
}

static QString languageName(const QString &qmFile)
{
    QTranslator translator;
    translator.load(qmFile);

    return translator.translate("MainWindow", "English");
}

pertubis::GeneralSettingsModel::GeneralSettingsModel(QObject *pobj) :
        QObject(pobj)
{
    loadSettings();
}

pertubis::GeneralSettingsModel::~GeneralSettingsModel()
{
    saveSettings();
}

void pertubis::GeneralSettingsModel::loadSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "GeneralSettingsModel" );
    m_toolbarDisplayMode = settings.value("toolbarDisplayMode",0).toInt();
    m_categoryDockPosition = settings.value("categoryDockPosition",0).toInt();
    m_repositoryDockPosition = settings.value("repositoryDockPosition",0).toInt();
    m_setDockPosition = settings.value("setDockPosition",0).toInt();
    m_glsaDockPosition = settings.value("glsaDockPosition",0).toInt();
    m_currentLanguage=settings.value("language",":i18n/pertubis-en.ts").toString();
    settings.endGroup();
}

void pertubis::GeneralSettingsModel::saveSettings()
{
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "GeneralSettingsModel" );
    settings.setValue("toolbarDisplayMode",m_toolbarDisplayMode);
    settings.setValue("categoryDockPosition",m_categoryDockPosition);
    settings.setValue("repositoryDockPosition",m_repositoryDockPosition);
    settings.setValue("setDockPosition",m_setDockPosition);
    settings.setValue("glsaDockPosition",m_glsaDockPosition);
    settings.setValue("language",m_currentLanguage);
    settings.endGroup();
}

pertubis::GeneralSettingsView::GeneralSettingsView(QWidget *pobj,GeneralSettingsModel* model) :
        QWidget(pobj),
            m_model(model),
            m_toolbarDisplayMode(new QComboBox(pobj)),
            m_categoryDockPosition(new QComboBox(pobj)),
            m_repositoryDockPosition(new QComboBox(pobj)),
            m_setDockPosition(new QComboBox(pobj)),
            m_glsaDockPosition(new QComboBox(pobj))
{
    // to translators: translate this into the name of your language in your language
    tr("english");
    QGroupBox* group(new QGroupBox(tr("General Settings"),pobj));

    m_toolbarDisplayMode->addItem( tr("icon only") );
    m_toolbarDisplayMode->addItem( tr("text under icon") );
    m_toolbarDisplayMode->setCurrentIndex(m_model->m_toolbarDisplayMode);

    m_categoryDockPosition->addItem( tr("left") );
    m_categoryDockPosition->addItem( tr("right") );
    m_categoryDockPosition->setCurrentIndex(m_model->m_categoryDockPosition);

    m_repositoryDockPosition->addItem( tr("left") );
    m_repositoryDockPosition->addItem( tr("right") );
    m_repositoryDockPosition->setCurrentIndex(m_model->m_repositoryDockPosition);

    m_setDockPosition->addItem( tr("left") );
    m_setDockPosition->addItem( tr("right") );
    m_setDockPosition->setCurrentIndex(m_model->m_setDockPosition);

    m_glsaDockPosition->addItem( tr("left") );
    m_glsaDockPosition->addItem( tr("right") );
    m_glsaDockPosition->setCurrentIndex(m_model->m_glsaDockPosition);

    QGridLayout *groupLayout = new QGridLayout;
    groupLayout->addWidget(new QLabel(tr("toolbar display mode"),m_toolbarDisplayMode), 0, 0);
    groupLayout->addWidget(m_toolbarDisplayMode, 0, 1);
    groupLayout->addWidget(new QLabel(tr("category dock position"),m_categoryDockPosition), 1, 0);
    groupLayout->addWidget(m_categoryDockPosition, 1, 1);
    groupLayout->addWidget(new QLabel(tr("repository dock position"),m_repositoryDockPosition), 2, 0);
    groupLayout->addWidget(m_repositoryDockPosition, 2, 1);
    groupLayout->addWidget(new QLabel(tr("set dock position"),m_setDockPosition), 3, 0);
    groupLayout->addWidget(m_setDockPosition, 3, 1);
    groupLayout->addWidget(new QLabel(tr("glsa dock position"),m_glsaDockPosition), 4, 0);
    groupLayout->addWidget(m_glsaDockPosition, 4, 1);

    group->setLayout(groupLayout);

    QGroupBox *languageGroup = new QGroupBox(tr("Language Settings"));
    QLabel *nameLabel = new QLabel(tr("Language"));

    QComboBox *choice = new QComboBox;

    QStringList list = findTranslationFiles();
    QString filename;
    int i=0;
    foreach (filename,list)
    {
        QString name = languageName(filename);
        m_model->m_languageToTranslation.insert(name,filename);
        choice->addItem(name);
        if (filename == m_model->m_currentLanguage)
            choice->setCurrentIndex(i);
        ++i;
    }

    connect(choice,
            SIGNAL(currentIndexChanged(const QString&)),
            this,
            SLOT(languageChanged(const QString&)));

    QGridLayout *languageLayout = new QGridLayout;
    languageLayout->addWidget(nameLabel, 0, 0);
    languageLayout->addWidget(choice, 0, 1);
    languageGroup->setLayout(languageLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(languageGroup);
    mainLayout->addWidget(group);
    mainLayout->addSpacing(12);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

    connect(m_toolbarDisplayMode,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeToolbarDisplayMode(int)));

    connect(m_model,
            SIGNAL(toolbarDisplayModeChanged(int)),
            m_toolbarDisplayMode,
            SLOT(setCurrentIndex(int)));

    connect(m_categoryDockPosition,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeCategoryDockPosition(int)));

    connect(m_model,
            SIGNAL(categoryDockChanged(int)),
            m_categoryDockPosition,
            SLOT(setCurrentIndex(int)));

    connect(m_repositoryDockPosition,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeRepositoryDockPosition(int)));

    connect(m_model,
            SIGNAL(repositoryDockChanged(int)),
            m_repositoryDockPosition,
            SLOT(setCurrentIndex(int)));

    connect(m_setDockPosition,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeSetDockPosition(int)));

    connect(m_model,
            SIGNAL(setDockChanged(int)),
            m_setDockPosition,
            SLOT(setCurrentIndex(int)));

    connect(m_glsaDockPosition,
            SIGNAL(currentIndexChanged(int)),
            m_model,
            SLOT(changeGlsaDockPosition(int)));

    connect(m_model,
            SIGNAL(glsaDockChanged(int)),
            m_glsaDockPosition,
            SLOT(setCurrentIndex(int)));
}

void pertubis::GeneralSettingsView::languageChanged(const QString& language)
{
    m_model->m_currentLanguage=m_model->m_languageToTranslation[language];
    QMessageBox::information(this,tr("pertubis info"),tr("language setting changes will be applied after a restart"));
}

pertubis::GeneralSettingsView::~GeneralSettingsView()
{
}
