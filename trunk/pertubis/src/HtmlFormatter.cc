
/* Copyright (C) 2007-2008 Stefan Koegl
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

#include "HtmlFormatter.hh"
#include <paludis/util/stringify.hh>
#include <paludis/util/set.hh>
#include <paludis/name.hh>
#include "FormatterUtils.hh"

using namespace pertubis;

HtmlFormatter::HtmlFormatter() :
        new_use_flag_names(new paludis::UseFlagNameSet),
        changed_use_flag_names(new paludis::UseFlagNameSet),
        use_flag_names(new paludis::UseFlagNameSet),
        use_expand_prefixes(new paludis::UseFlagNameSet)
{
}

HtmlFormatter::~HtmlFormatter()
{
}

const paludis::tr1::shared_ptr<const paludis::UseFlagNameSet>
HtmlFormatter::seen_new_use_flag_names() const
{
    return new_use_flag_names;
}

const paludis::tr1::shared_ptr<const paludis::UseFlagNameSet>
HtmlFormatter::seen_changed_use_flag_names() const
{
    return changed_use_flag_names;
}

const paludis::tr1::shared_ptr<const paludis::UseFlagNameSet>
HtmlFormatter::seen_use_flag_names() const
{
    return use_flag_names;
}

const paludis::tr1::shared_ptr<const paludis::UseFlagNameSet>
HtmlFormatter::seen_use_expand_prefixes() const
{
    return use_expand_prefixes;
}

std::string
HtmlFormatter::format(const paludis::IUseFlag & f, const paludis::format::Plain &) const
{
    use_flag_names->insert(f.flag);
    std::string g(stringify(f.flag)), h;

    if (std::string::npos != f.prefix_delim_pos)
    {
        std::string p(g.substr(0, f.prefix_delim_pos));
        if (active_prefix == p)
            g.erase(0, f.prefix_delim_pos + 1);
        else
        {
            active_prefix = p;
            use_expand_prefixes->insert(paludis::UseFlagName(active_prefix));
            h = active_prefix + ": ";
            g.erase(0, f.prefix_delim_pos + 1);
        }
    }

    return h + g;
}

std::string
HtmlFormatter::format(const paludis::IUseFlag & f, const paludis::format::Enabled &) const
{
    use_flag_names->insert(f.flag);
    std::string g(stringify(f.flag)), h;

    if (std::string::npos != f.prefix_delim_pos)
    {
        std::string p(g.substr(0, f.prefix_delim_pos));
        if (active_prefix == p)
            g.erase(0, f.prefix_delim_pos + 1);
        else
        {
            active_prefix = p;
            use_expand_prefixes->insert(paludis::UseFlagName(active_prefix));
            h = active_prefix + ": ";
            g.erase(0, f.prefix_delim_pos + 1);
        }
    }

    return h + color(g,"green");
}

std::string
HtmlFormatter::format(const paludis::IUseFlag & f, const paludis::format::Disabled &) const
{
    use_flag_names->insert(f.flag);
    std::string g(stringify(f.flag)), h;

    if (std::string::npos != f.prefix_delim_pos)
    {
        std::string p(g.substr(0, f.prefix_delim_pos));
        if (active_prefix == p)
            g.erase(0, f.prefix_delim_pos + 1);
        else
        {
            active_prefix = p;
            use_expand_prefixes->insert(paludis::UseFlagName(active_prefix));
            h = active_prefix + ": ";
            g.erase(0, f.prefix_delim_pos + 1);
        }
    }

    return h + color("-" + g,"red");
}

std::string
HtmlFormatter::format(const paludis::IUseFlag & f, const paludis::format::Forced &) const
{
    use_flag_names->insert(f.flag);
    std::string g(stringify(f.flag)), h;

    if (std::string::npos != f.prefix_delim_pos)
    {
        std::string p(g.substr(0, f.prefix_delim_pos));
        if (active_prefix == p)
            g.erase(0, f.prefix_delim_pos + 1);
        else
        {
            active_prefix = p;
            use_expand_prefixes->insert(paludis::UseFlagName(active_prefix));
            h = active_prefix + ": ";
            g.erase(0, f.prefix_delim_pos + 1);
        }
    }

    return h + color("(" + g + ")","green");
}

std::string
HtmlFormatter::format(const paludis::IUseFlag & f, const paludis::format::Masked &) const
{
    use_flag_names->insert(f.flag);
    std::string g(stringify(f.flag)), h;

    if (std::string::npos != f.prefix_delim_pos)
    {
        std::string p(g.substr(0, f.prefix_delim_pos));
        if (active_prefix == p)
            g.erase(0, f.prefix_delim_pos + 1);
        else
        {
            active_prefix = p;
            use_expand_prefixes->insert(paludis::UseFlagName(active_prefix));
            h = active_prefix + ": ";
            g.erase(0, f.prefix_delim_pos + 1);
        }
    }

    return h + color("(-" + g + ")","red");
}

std::string
HtmlFormatter::decorate(const paludis::IUseFlag & i, const std::string & f, const paludis::format::Added &) const
{
    new_use_flag_names->insert(i.flag);
    return f + "+";
}

std::string
HtmlFormatter::decorate(const paludis::IUseFlag & i, const std::string & f, const paludis::format::Changed &) const
{
    changed_use_flag_names->insert(i.flag);
    return f + "*";
}

std::string
HtmlFormatter::format(const paludis::UseFlagName & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::UseFlagName & f, const paludis::format::Enabled &) const
{
    return color(paludis::stringify(f) +" (on)" ,"green");
}

std::string
HtmlFormatter::format(const paludis::UseFlagName & f, const paludis::format::Disabled &) const
{
    return color("-" + stringify(f)+" (off)" ,"red");
}

std::string
HtmlFormatter::format(const paludis::UseFlagName & f, const paludis::format::Forced &) const
{
    return color("(" + stringify(f) + " (forced) " +")","green");
}

std::string
HtmlFormatter::format(const paludis::UseFlagName & f, const paludis::format::Masked &) const
{
    return color("(-" + stringify(f) + ")","red");
}

std::string
HtmlFormatter::format(const paludis::UseDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::UseDepSpec & f, const paludis::format::Enabled &) const
{
    return color(paludis::stringify(f),"green");
}

std::string
HtmlFormatter::format(const paludis::UseDepSpec & f, const paludis::format::Disabled &) const
{
    return color(paludis::stringify(f),"red");
}

std::string
HtmlFormatter::format(const paludis::UseDepSpec & f, const paludis::format::Forced &) const
{
    return color("(" + stringify(f) + ")","green");
}

std::string
HtmlFormatter::format(const paludis::UseDepSpec & f, const paludis::format::Masked &) const
{
    return color("(" + stringify(f) + ")","red");
}

std::string
HtmlFormatter::format(const paludis::PackageDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::PackageDepSpec & f, const paludis::format::Installed &) const
{
    return color(bold(paludis::stringify(f)),"blue").append(color(std::string(" (installed)"),std::string("green")));
}

std::string
HtmlFormatter::format(const paludis::PackageDepSpec & f, const paludis::format::Installable &) const
{
    return color(paludis::stringify(f),"blue").append(color(std::string(" (installable)"),std::string("green")));
}

std::string
HtmlFormatter::format(const paludis::PlainTextDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::PlainTextDepSpec & f, const paludis::format::Accepted &) const
{
    return color(paludis::stringify(f),"green");
}

std::string
HtmlFormatter::format(const paludis::PlainTextDepSpec & f, const paludis::format::Unaccepted &) const
{
    return color(paludis::stringify(f),"red");
}

std::string
HtmlFormatter::format(const paludis::KeywordName & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::KeywordName & f, const paludis::format::Accepted &) const
{
    return color(paludis::stringify(f),"green");
}

std::string
HtmlFormatter::format(const paludis::KeywordName & f, const paludis::format::Unaccepted &) const
{
    return color(paludis::stringify(f),"red");
}

std::string
HtmlFormatter::format(const paludis::LicenseDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::LicenseDepSpec & f, const paludis::format::Accepted &) const
{
    return color(paludis::stringify(f),"green");
}

std::string
HtmlFormatter::format(const paludis::LicenseDepSpec & f, const paludis::format::Unaccepted &) const
{
    return color(paludis::stringify(f),"red");
}


std::string
HtmlFormatter::format(const std::string & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::URILabelsDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::DependencyLabelsDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::FetchableURIDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::SimpleURIDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::BlockDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::NamedSetDepSpec & f, const paludis::format::Plain &) const
{
    return stringify(f);
}


std::string
HtmlFormatter::format(const paludis::PackageID & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::PackageID & f, const paludis::format::Installed &) const
{
    return color(bold(paludis::stringify(f)+" installed"),"blue");
}

std::string
HtmlFormatter::format(const paludis::PackageID & f, const paludis::format::Installable &) const
{
    return color(paludis::stringify(f)+" installable","blue");
}

std::string
HtmlFormatter::format(const paludis::FSEntry &f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::FSEntry &f, const paludis::format::Installed &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::format(const paludis::FSEntry &f, const paludis::format::Installable &) const
{
    return paludis::stringify(f);
}

std::string
HtmlFormatter::newline() const
{
    return "<br>";
}

std::string
HtmlFormatter::indent(const int i) const
{
    return std::string(4 * i, ' ');
}
