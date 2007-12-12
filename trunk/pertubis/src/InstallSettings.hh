
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_INSTALL_SETTINGS_H
#define _PERTUBIS_ENTRY_PROTECTOR_INSTALL_SETTINGS_H 1

#include <QWidget>
#include <QString>
#include <paludis/args/args.hh>
#include <paludis/args/args_handler.hh>
#include <paludis/args/install_args_group.hh>

class QCheckBox;
class QComboBox;

namespace pertubis
{
    class InstallSettingsModel : public QObject, public paludis::args::ArgsHandler
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

            std::string app_name() const;
            std::string app_synopsis() const;
            std::string app_description() const;

            QString m_tooltip;

            /// \name install options
            ///@{
            int     m_debug;
            int     m_continueOnFailure;
            ///@}

            paludis::args::InstallArgsGroup install_args;

        signals:

            void debugChanged(int state);
            void fetchChanged(bool checked);
            void noConfigChanged(bool checked);
            void noSafeChanged(bool checked);
            void pretendChanged(bool checked);
            void continueChanged(int state);
            void checksChanged(const QString& argument);

        public slots:

            void changeDebug(int state)
            {
                if (m_debug != state)
                    emit debugChanged(state);
                m_debug = state;
            }

            void changeFetch(bool checked)
            {
                if (checked != install_args.a_fetch.specified())
                    emit fetchChanged(checked);
                install_args.a_fetch.set_specified(checked);
            }

            void changeNoConfig(bool checked)
            {
                if (checked != install_args.a_no_config_protection.specified())
                    emit noConfigChanged(checked);
                install_args.a_no_config_protection.set_specified(checked);
            }

            void changeNoSafe(bool checked)
            {
                if (checked != install_args.a_no_safe_resume.specified())
                    emit noSafeChanged(checked);
                install_args.a_no_safe_resume.set_specified(checked);
            }

            void changePretend(bool checked)
            {
                if (checked != install_args.a_pretend.specified())
                    emit pretendChanged(checked);
                install_args.a_pretend.set_specified(checked);
            }

            void changeContinue(int state)
            {
                if (m_continueOnFailure != state)
                    emit continueChanged(state);
                m_continueOnFailure = state;
            }

            void changeChecks(const QString& argument)
            {
                if (argument != QString::fromStdString(install_args.a_checks.argument()) )
                    emit checksChanged(argument);
                install_args.a_checks.set_argument(argument.toStdString());
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

            void createView();
            void createNormal(QWidget* pobj);

            QComboBox* m_debug;
            QCheckBox* m_fetch;
            QCheckBox* m_noConfigProtection;
            QCheckBox* m_noSafeResume;
            QCheckBox* m_pretend;
            QComboBox* m_continueOnFailure;
            QComboBox* m_checks;
            ///@}

    };
}

#endif
