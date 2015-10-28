#ifndef INI_HPP
#define INI_HPP

#include <cassert>
#include <map>
#include <list>
#include <stdexcept>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>

namespace INI
{
struct Level
{
    Level() : parent(NULL), depth(0) {}
    Level(Level* p) : parent(p), depth(0) {}

    typedef std::map<std::string, std::string> value_map_t;
    typedef std::map<std::string, Level> section_map_t;
    typedef std::list<value_map_t::const_iterator> values_t;
    typedef std::list<section_map_t::const_iterator> sections_t;

    value_map_t values;
    section_map_t sections;
    values_t ordered_values; // original order in the ini file
    sections_t ordered_sections;
    Level* parent;
    size_t depth;

    const std::string& operator[](const std::string& name) { return values[name]; }
    Level& operator()(const std::string& name) { return sections[name]; }
};

class Parser
{
  public:
    Parser(const char* fn);
    Parser(std::istream& f);
    Level& top();
    void dump(std::ostream& s);

  private:
    void err(const char* s);
    void parse(Level& l);
    void parseSLine(std::string& sname, size_t& depth);
    void dump(std::ostream& s, const Level& l, const std::string& sname);

  private:
    Level top_;
    std::ifstream f0_;
    std::istream* f_;
    std::string line_;
    size_t ln_;
};

}

#endif // INI_HPP
