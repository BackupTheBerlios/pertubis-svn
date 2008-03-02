
/* Copyright (C) 2007 Stefan Koegl
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
#include "MessageOutput.hh"

#include <QComboBox>
#include <QDir>
#include <QDebug>
#include <QGridLayout>
#include <QSpinBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QFontComboBox>
#include <QFont>
#include <QLabel>
#include <QSettings>
#include <QTranslator>

using namespace pertubis;

static QStringList
findTranslationFiles()
{
    QDir dir(":i18n");
    QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files,
                                          QDir::Name);

    for (QList<QString>::iterator i(fileNames.begin()), iEnd(fileNames.end());
         i != iEnd;++i)
    {
        QFileInfo info(*i);
        QString tmp(dir.filePath(info.baseName()));
        qDebug() << "findTranslationFiles() - found" << tmp;
        *i = tmp;
    }

    return fileNames;
}

static QString
languageName(const QString &qmFile)
{
    QTranslator translator;
    translator.load(qmFile);

    return translator.translate("MainWindow", "English");
}

GeneralSettingsModel::GeneralSettingsModel(QObject *pobj) :
        QObject(pobj)
{
    loadSettings();
}

GeneralSettingsModel::~GeneralSettingsModel()
{
    qDebug() << "GeneralSettingsModel::~GeneralSettingsModel() start";
    saveSettings();
    qDebug() << "GeneralSettingsModel::~GeneralSettingsModel() done";
}

void
GeneralSettingsModel::loadSettings()
{
    qDebug() << "GeneralSettingsModel::loadSettings()";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup("GeneralSettingsModel");
    // use ts files only without file extensions!
    m_currentLanguage=settings.value("language",":i18n/pertubis-en").toString();
    settings.endGroup();
}

void
GeneralSettingsModel::saveSettings()
{
    qDebug() << "GeneralSettingsModel::saveSettings()";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup("GeneralSettingsModel");
    settings.setValue("language",m_currentLanguage);
    settings.endGroup();
}

GeneralSettingsView::GeneralSettingsView(QWidget *pobj,GeneralSettingsModel* model) :
    QWidget(pobj),
    m_model(model)
{
    // for translators: translate this into the name of your language in your language
    // e.g. for german take "Deutsch"
    tr("english");

    QGroupBox *languageGroup = new QGroupBox(tr("Language Settings"));
    QLabel *nameLabel = new QLabel(tr("Language"));

    QComboBox *choice = new QComboBox;

    QStringList list = findTranslationFiles();
    int i=0;
    for (QList<QString>::const_iterator fileName(list.constBegin()),fEnd(list.constEnd());
         fileName != fEnd;++fileName)
    {
        QString name = languageName(*fileName);
        m_model->m_languageToTranslation.insert(name,*fileName);
        choice->addItem(name);
        if (*fileName == m_model->m_currentLanguage)
        {
            choice->setCurrentIndex(i);
        }
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

    QGroupBox *messageGroup(new QGroupBox(tr("Message Page Settings")));
    m_fonts = new QFontComboBox(this);
    m_fonts->setFontFilters(QFontComboBox::MonospacedFonts);
    m_fontSize = new QSpinBox(this);
    m_fontSize->setRange(8,24);
    m_fontSize->setSuffix("px");
    QLabel *fontLabel(new QLabel(tr("Font"),m_fonts));

    m_vt100Mode = new QCheckBox(tr("vt100 mode (slow but with colors)"), this);

    QGridLayout* messageLayout(new QGridLayout);
//     messageLayout->setColumnStretch(2,1);
    messageLayout->addWidget(fontLabel,0,0);
    messageLayout->addWidget(m_vt100Mode,1,0);
    messageLayout->addWidget(m_fonts,0,2);
    messageLayout->addWidget(m_fontSize,0,3);
    messageGroup->setLayout(messageLayout);

    QVBoxLayout *mainLayout(new QVBoxLayout);
    mainLayout->addWidget(languageGroup);
    mainLayout->addWidget(messageGroup);
    mainLayout->addSpacing(12);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void
GeneralSettingsView::languageChanged(const QString& language)
{
    m_model->m_currentLanguage=m_model->m_languageToTranslation[language];
    QMessageBox::information(this,tr("pertubis info"),tr("language setting changes will be applied after a restart"));
}

GeneralSettingsView::~GeneralSettingsView()
{
}
