
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_INSTALL_SETTINGS_H
#define _PERTUBIS_ENTRY_PROTECTOR_INSTALL_SETTINGS_H 1

#include <QWidget>
#include <QString>
#include <paludis/environment-fwd.hh>

namespace paludis
{
    class InstallTask;
}

class QCheckBox;
class QLineEdit;
class QComboBox;

namespace pertubis
{
    class InstallSettingsModel : public QObject
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            InstallSettingsModel(QObject *parent);
            ///@}

            ~InstallSettingsModel();

            ///@name Content modification
            ///@{

            /// loads all settings for the gui
            void loadSettings();

            /// saves all settings for the gui
            void saveSettings();

            ///@}

            /// \name install options
            ///@{
            int         m_debug;
            int         m_config;
            int         m_continueOnFailure;
            bool        m_fetch;
            bool        m_pretend;
            bool        m_preserve;
            bool        m_noSafeResume;
            bool        m_checks;
            QString     m_worldSpec;

            ///@}

        signals:

            void checksChanged(int state);
            void continueChanged(int state);
            void debugChanged(int state);
            void fetchChanged(bool checked);
            void preserveWorldChanged(bool checked);
            void noConfigChanged(bool checked);
            void noSafeChanged(bool checked);
            void pretendChanged(bool checked);
            void worldSpecChanged(const QString & spec);

        public slots:

            void populate_install_task(const paludis::Environment *env, paludis::InstallTask &task) const;

            void changeDebug(int state)
            {
                if (m_debug != state)
                    emit debugChanged(state);
                m_debug = state;
            }

            void changeFetch(bool checked)
            {
                if (checked != m_fetch)
                    emit fetchChanged(checked);
                m_fetch = checked;
            }

            void changeNoConfig(bool checked)
            {
                if (checked != m_config)
                    emit noConfigChanged(checked);
                m_config = checked;
            }

            void changeNoSafe(bool checked)
            {
                if (checked != m_noSafeResume)
                    emit noSafeChanged(checked);
                m_noSafeResume = checked;
            }

            void changePreserveWorld(bool checked)
            {
                if (checked != m_preserve)
                    emit preserveWorldChanged(checked);
                m_preserve = checked;
            }

            void changePretend(bool checked)
            {
                if (checked != m_pretend)
                    emit pretendChanged(checked);
                m_pretend = checked;
            }

            void changeContinue(int state)
            {
                if (m_continueOnFailure != state)
                    emit continueChanged(state);
                m_continueOnFailure = state;
            }

            void changeChecks(int state)
            {
                if (state != m_checks )
                    emit checksChanged(state);
                m_checks = state;
            }

            void changeWorldSpec(const QString spec)
            {
                if (spec != m_worldSpec )
                    emit worldSpecChanged(spec);
                m_worldSpec = spec;
            }
    };

    class InstallSettingsView : public QWidget
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            InstallSettingsView(QWidget *parent,InstallSettingsModel* model);
            ///@}

            ~InstallSettingsView();

            ///@name Content modification
            ///@{

            ///@}

            InstallSettingsModel* m_model;

        private:

            /// \name install options
            ///@{


            ///@}

    };
}

#endif
