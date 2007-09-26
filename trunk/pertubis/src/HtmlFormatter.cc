
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

#include "HtmlFormatter.hh"
#include <paludis/util/stringify.hh>
#include <paludis/util/set.hh>
#include <paludis/name.hh>
#include "FormatterUtils.hh"

pertubis::HtmlFormatter::HtmlFormatter() :
        new_use_flag_names(new paludis::UseFlagNameSet),
        changed_use_flag_names(new paludis::UseFlagNameSet),
        use_flag_names(new paludis::UseFlagNameSet),
        use_expand_prefixes(new paludis::UseFlagNameSet)
{
}

pertubis::HtmlFormatter::~HtmlFormatter()
{
}

const paludis::tr1::shared_ptr<const paludis::UseFlagNameSet>
pertubis::HtmlFormatter::seen_new_use_flag_names() const
{
    return new_use_flag_names;
}

const paludis::tr1::shared_ptr<const paludis::UseFlagNameSet>
pertubis::HtmlFormatter::seen_changed_use_flag_names() const
{
    return changed_use_flag_names;
}

const paludis::tr1::shared_ptr<const paludis::UseFlagNameSet>
pertubis::HtmlFormatter::seen_use_flag_names() const
{
    return use_flag_names;
}

const paludis::tr1::shared_ptr<const paludis::UseFlagNameSet>
pertubis::HtmlFormatter::seen_use_expand_prefixes() const
{
    return use_expand_prefixes;
}

std::string pertubis::HtmlFormatter::format(const paludis::IUseFlag & f, const paludis::format::Plain &) const
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
pertubis::HtmlFormatter::format(const paludis::IUseFlag & f, const paludis::format::Enabled &) const
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
pertubis::HtmlFormatter::format(const paludis::IUseFlag & f, const paludis::format::Disabled &) const
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
pertubis::HtmlFormatter::format(const paludis::IUseFlag & f, const paludis::format::Forced &) const
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
pertubis::HtmlFormatter::format(const paludis::IUseFlag & f, const paludis::format::Masked &) const
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

    return h + color( "(-" + g + ")","red");
}

std::string
pertubis::HtmlFormatter::decorate(const paludis::IUseFlag & i, const std::string & f, const paludis::format::Added &) const
{
    new_use_flag_names->insert(i.flag);
    return f + "+";
}

std::string
pertubis::HtmlFormatter::decorate(const paludis::IUseFlag & i, const std::string & f, const paludis::format::Changed &) const
{
    changed_use_flag_names->insert(i.flag);
    return f + "*";
}

std::string
pertubis::HtmlFormatter::format(const paludis::UseFlagName & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
pertubis::HtmlFormatter::format(const paludis::UseFlagName & f, const paludis::format::Enabled &) const
{
    return color(paludis::stringify(f),"green");
}

std::string
pertubis::HtmlFormatter::format(const paludis::UseFlagName & f, const paludis::format::Disabled &) const
{
    return color( "-" + stringify(f),"red");
}

std::string
pertubis::HtmlFormatter::format(const paludis::UseFlagName & f, const paludis::format::Forced &) const
{
    return color( "(" + stringify(f) + ")","green");
}

std::string
pertubis::HtmlFormatter::format(const paludis::UseFlagName & f, const paludis::format::Masked &) const
{
    return color( "(-" + stringify(f) + ")","red");
}

std::string
pertubis::HtmlFormatter::format(const paludis::UseDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
pertubis::HtmlFormatter::format(const paludis::UseDepSpec & f, const paludis::format::Enabled &) const
{
    return color(paludis::stringify(f),"green");
}

std::string
pertubis::HtmlFormatter::format(const paludis::UseDepSpec & f, const paludis::format::Disabled &) const
{
    return color(paludis::stringify(f),"red");
}

std::string
pertubis::HtmlFormatter::format(const paludis::UseDepSpec & f, const paludis::format::Forced &) const
{
    return color("(" + stringify(f) + ")","green");
}

std::string
pertubis::HtmlFormatter::format(const paludis::UseDepSpec & f, const paludis::format::Masked &) const
{
    return color("(" + stringify(f) + ")","red");
}

std::string
pertubis::HtmlFormatter::format(const paludis::PackageDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
pertubis::HtmlFormatter::format(const paludis::PackageDepSpec & f, const paludis::format::Installed &) const
{
    return color( paludis::stringify(f),"blue");
}

std::string
pertubis::HtmlFormatter::format(const paludis::PackageDepSpec & f, const paludis::format::Installable &) const
{
    return color( bold(paludis::stringify(f)),"blue");
}

std::string
pertubis::HtmlFormatter::format(const paludis::PlainTextDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
pertubis::HtmlFormatter::format(const paludis::PlainTextDepSpec & f, const paludis::format::Accepted &) const
{
    return color(paludis::stringify(f),"green");
}

std::string
pertubis::HtmlFormatter::format(const paludis::PlainTextDepSpec & f, const paludis::format::Unaccepted &) const
{
    return color(paludis::stringify(f),"red");
}

std::string
pertubis::HtmlFormatter::format(const paludis::KeywordName & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
pertubis::HtmlFormatter::format(const paludis::KeywordName & f, const paludis::format::Accepted &) const
{
    return color(paludis::stringify(f),"green");
}

std::string
pertubis::HtmlFormatter::format(const paludis::KeywordName & f, const paludis::format::Unaccepted &) const
{
    return color(paludis::stringify(f),"red");
}

std::string
pertubis::HtmlFormatter::format(const std::string & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
pertubis::HtmlFormatter::format(const paludis::LabelsDepSpec<paludis::URILabelVisitorTypes> & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
pertubis::HtmlFormatter::format(const paludis::LabelsDepSpec<paludis::DependencyLabelVisitorTypes> & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
pertubis::HtmlFormatter::format(const paludis::URIDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
        pertubis::HtmlFormatter::format(const paludis::BlockDepSpec & f, const paludis::format::Plain &) const
{
    return paludis::stringify(f);
}

std::string
pertubis::HtmlFormatter::format(const paludis::tr1::shared_ptr<const paludis::PackageID> & f, const paludis::format::Plain &) const
{
    return paludis::stringify(*f);
}

std::string
pertubis::HtmlFormatter::format(const paludis::tr1::shared_ptr<const paludis::PackageID> & f, const paludis::format::Installed &) const
{
    return color(paludis::stringify(*f),"blue");
}

std::string
pertubis::HtmlFormatter::format(const paludis::tr1::shared_ptr<const paludis::PackageID> & f, const paludis::format::Installable &) const
{
    return color(bold(paludis::stringify(*f)),"blue");
}

std::string
pertubis::HtmlFormatter::newline() const
{
    return "<br>";
}

std::string
pertubis::HtmlFormatter::indent(const int i) const
{
    return std::string(4 * i, ' ');
}
