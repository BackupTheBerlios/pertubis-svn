
/* Copyright (C) 2007-2008 Stefan Koegl
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_UNINSTALL_SETTINGS_H
#define _PERTUBIS_ENTRY_PROTECTOR_UNINSTALL_SETTINGS_H 1

#include <QWidget>


class QCheckBox;
class QComboBox;

namespace perturbis
{

    class DeinstallSettingsModel : public QObject
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            DeinstallSettingsModel(QObject *pobj);
            ///@}

            ~DeinstallSettingsModel();

            ///@name Content modification
            ///@{

            /// loads all settings for the gui
            void loadSettings();

            /// loads all settings for the gui
//             void setDefaults();

            /// saves all settings for the gui
            void saveSettings();

            ///@}

            bool    m_deps;
            bool    m_unusedDeps;
            bool    m_allVersions;
            bool    m_unsafeUninstall;

        signals:

            void depsChanged(bool checked);
            void unusedDepsChanged(bool checked);
            void allVersionsChanged(bool checked);
            void unsafeUninstallChanged(bool checked);

        public slots:

            void onDepsChanged(bool checked)
            {
                if (m_deps != checked)
                    emit depsChanged(checked);
                m_deps = checked;
            }

            void onUnusedDepChanged(bool checked)
            {
                if (m_unusedDeps != checked)
                    emit unusedDepsChanged(checked);
                m_unusedDeps = checked;
            }

            void onAllVersionsChanged(bool checked)
            {
                if (m_allVersions != checked)
                    emit allVersionsChanged(checked);
                m_allVersions = checked;
            }

            void onUnsafeUninstallChanged(bool checked)
            {
                if (m_unsafeUninstall != checked)
                    emit unsafeUninstallChanged(checked);
                m_unsafeUninstall = checked;
            }

    };

    class DeinstallSettingsView : public QWidget
    {
        Q_OBJECT

    public:

            ///@name Constructors
            ///@{

            /// std constructor
            DeinstallSettingsView(QWidget *parent,DeinstallSettingsModel* model);
            ///@}

            ~DeinstallSettingsView();

            DeinstallSettingsModel*  m_model;

    private:

            /// \name install options
            ///@{

            QCheckBox* m_deps;
            QCheckBox* m_unusedDeps;
            QCheckBox* m_allVersions;
            QCheckBox* m_unsafeUninstall;
            ///@}

    };
}

#endif
