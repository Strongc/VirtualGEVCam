#include "ini.h"


namespace INI
{

inline std::string trim(const std::string& s)
{
    char p[] = " \t\r\n";
    long sp = 0;
    long ep = s.length() - 1;

    for (; sp <= ep; ++sp)
        if (!strchr(p, s[sp])) break;

    for (; ep >= 0; --ep)
        if (!strchr(p, s[ep])) break;

    return s.substr(sp, ep-sp+1);
}

Parser::Parser(const char* fn) : f0_(fn), f_(&f0_), ln_(0)
{
    if (!f0_)
        throw std::runtime_error(std::string("failed to open file: ") + fn);

    parse(top_);
}

Parser::Parser(std::istream& f) : f_(&f), ln_(0)
{
    parse(top_);
}

Level& Parser::top()
{
    return top_;
}

void Parser::dump(std::ostream& s)
{
    dump(s, top(), "");
}

inline void
Parser::err(const char* s)
{
    char buf[256];
    sprintf(buf, "%s on line #%ld", s, ln_);

    throw std::runtime_error(buf);
}

inline void
Parser::parse(Level& l)
{
    while (std::getline(*f_, line_))
    {
        ++ln_;
        if (line_[0] == '#' || line_[0] == ';')
            continue;

        line_ = trim(line_);

        if (line_.empty())
            continue;

        if (line_[0] == '[')
        {
            size_t depth;
            std::string sname;

            parseSLine(sname, depth);

            Level* lp = NULL;
            Level* parent = &l;

            if (depth > l.depth + 1)
                err("section with wrong depth");
            if (l.depth == depth-1)
                lp = &l.sections[sname];
            else
            {
                lp = l.parent;
                size_t n = l.depth - depth;
                for (size_t i = 0; i < n; ++i) lp = lp->parent;
                parent = lp;
                lp = &lp->sections[sname];
            }

            if (lp->depth != 0)
                err("duplicate section name on the same level");
            if (!lp->parent)
            {
                lp->depth = depth;
                lp->parent = parent;
            }

            parent->ordered_sections.push_back(parent->sections.find(sname));
            parse(*lp);
        }
        else
        {
            size_t n = line_.find('=');

            if (n == std::string::npos)
                err("no '=' found");
            std::pair<Level::value_map_t::const_iterator, bool> res =
                    l.values.insert(std::make_pair(trim(line_.substr(0, n)),
                                                   trim(line_.substr(n+1, line_.length()-n-1))));
            if (!res.second)
                err("duplicated key found");

            l.ordered_values.push_back(res.first);
        }
    }
}

inline void
Parser::parseSLine(std::string& sname, size_t& depth)
{
    depth = 0;
    for (; depth < line_.length(); ++depth)
        if (line_[depth] != '[') break;

    sname = line_.substr(depth, line_.length() - 2*depth);
}

inline void
Parser::dump(std::ostream& s, const Level& l, const std::string& sname)
{
    if (!sname.empty())
        s << '\n';
    for (size_t i = 0; i < l.depth; ++i)
        s << '[';
    if (!sname.empty())
        s << sname;
    for (size_t i = 0; i < l.depth; ++i)
        s << ']';
    if (!sname.empty())
        s << std::endl;
    for (Level::values_t::const_iterator it = l.ordered_values.begin(); it != l.ordered_values.end(); ++it)
        s << (*it)->first << '=' << (*it)->second << std::endl;

    for (Level::sections_t::const_iterator it = l.ordered_sections.begin(); it != l.ordered_sections.end(); ++it)
    {
        assert((*it)->second.depth == l.depth+1);
        dump(s, (*it)->second, (*it)->first);
    }
}

}
