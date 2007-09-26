
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of the pertubis frontend for paludis package manager.
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_HTML_FORMATTER_HH
#define _PERTUBIS_ENTRY_PROTECTOR_HTML_FORMATTER_HH

#include <string>
#include <paludis/util/tr1_memory.hh>
#include <paludis/formatter.hh>
#include <paludis/name-fwd.hh>
#include <paludis/dep_spec-fwd.hh>

namespace pertubis
{
    class HtmlFormatter :
        public paludis::CanFormat<paludis::UseFlagName>,
        public paludis::CanFormat<paludis::IUseFlag>,
        public paludis::CanFormat<paludis::KeywordName>,
        public paludis::CanFormat<paludis::UseDepSpec>,
        public paludis::CanFormat<paludis::PackageDepSpec>,
        public paludis::CanFormat<paludis::BlockDepSpec>,
        public paludis::CanFormat<paludis::LabelsDepSpec<paludis::DependencyLabelVisitorTypes> >,
        public paludis::CanFormat<paludis::LabelsDepSpec<paludis::URILabelVisitorTypes> >,
        public paludis::CanFormat<paludis::PlainTextDepSpec>,
        public paludis::CanFormat<paludis::URIDepSpec>,
        public paludis::CanFormat<paludis::tr1::shared_ptr<const paludis::PackageID> >,
        public paludis::CanFormat<std::string>,
        public paludis::CanSpace
    {
        public:
            HtmlFormatter();
            ~HtmlFormatter();

            const paludis::tr1::shared_ptr<const paludis::UseFlagNameSet> seen_new_use_flag_names() const;
            const paludis::tr1::shared_ptr<const paludis::UseFlagNameSet> seen_changed_use_flag_names() const;
            const paludis::tr1::shared_ptr<const paludis::UseFlagNameSet> seen_use_flag_names() const;
            const paludis::tr1::shared_ptr<const paludis::UseFlagNameSet> seen_use_expand_prefixes() const;

            std::string format(const paludis::IUseFlag &, const paludis::format::Plain &) const;
            std::string format(const paludis::IUseFlag &, const paludis::format::Enabled &) const;
            std::string format(const paludis::IUseFlag &, const paludis::format::Disabled &) const;
            std::string format(const paludis::IUseFlag &, const paludis::format::Forced &) const;
            std::string format(const paludis::IUseFlag &, const paludis::format::Masked &) const;
            std::string decorate(const paludis::IUseFlag &, const std::string &, const paludis::format::Added &) const;
            std::string decorate(const paludis::IUseFlag &, const std::string &, const paludis::format::Changed &) const;

            std::string format(const paludis::UseFlagName &, const paludis::format::Plain &) const;
            std::string format(const paludis::UseFlagName &, const paludis::format::Enabled &) const;
            std::string format(const paludis::UseFlagName &, const paludis::format::Disabled &) const;
            std::string format(const paludis::UseFlagName &, const paludis::format::Forced &) const;
            std::string format(const paludis::UseFlagName &, const paludis::format::Masked &) const;

            std::string format(const paludis::UseDepSpec &, const paludis::format::Plain &) const;
            std::string format(const paludis::UseDepSpec &, const paludis::format::Enabled &) const;
            std::string format(const paludis::UseDepSpec &, const paludis::format::Disabled &) const;
            std::string format(const paludis::UseDepSpec &, const paludis::format::Forced &) const;
            std::string format(const paludis::UseDepSpec &, const paludis::format::Masked &) const;

            std::string format(const paludis::PackageDepSpec &, const paludis::format::Plain &) const;
            std::string format(const paludis::PackageDepSpec &, const paludis::format::Installed &) const;
            std::string format(const paludis::PackageDepSpec &, const paludis::format::Installable &) const;

            std::string format(const paludis::PlainTextDepSpec &, const paludis::format::Plain &) const;
            std::string format(const paludis::PlainTextDepSpec &, const paludis::format::Accepted &) const;
            std::string format(const paludis::PlainTextDepSpec &, const paludis::format::Unaccepted &) const;

            std::string format(const paludis::KeywordName &, const paludis::format::Plain &) const;
            std::string format(const paludis::KeywordName &, const paludis::format::Accepted &) const;
            std::string format(const paludis::KeywordName &, const paludis::format::Unaccepted &) const;

            std::string format(const std::string &, const paludis::format::Plain &) const;

            std::string format(const paludis::LabelsDepSpec<paludis::URILabelVisitorTypes> &, const paludis::format::Plain &) const;

            std::string format(const paludis::LabelsDepSpec<paludis::DependencyLabelVisitorTypes> &, const paludis::format::Plain &) const;

            std::string format(const paludis::URIDepSpec &, const paludis::format::Plain &) const;

            std::string format(const paludis::BlockDepSpec &, const paludis::format::Plain &) const;

            std::string format(const paludis::tr1::shared_ptr<const paludis::PackageID> &, const paludis::format::Plain &) const;
            std::string format(const paludis::tr1::shared_ptr<const paludis::PackageID> &, const paludis::format::Installed &) const;
            std::string format(const paludis::tr1::shared_ptr<const paludis::PackageID> &, const paludis::format::Installable &) const;

            std::string newline() const;
            std::string indent(const int) const;

        private:

            mutable std::string active_prefix;
            const paludis::tr1::shared_ptr<paludis::UseFlagNameSet> new_use_flag_names;
            const paludis::tr1::shared_ptr<paludis::UseFlagNameSet> changed_use_flag_names;
            const paludis::tr1::shared_ptr<paludis::UseFlagNameSet> use_flag_names;
            const paludis::tr1::shared_ptr<paludis::UseFlagNameSet> use_expand_prefixes;
    };

}

#endif
