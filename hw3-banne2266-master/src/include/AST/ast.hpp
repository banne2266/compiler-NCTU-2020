#ifndef __AST_H
#define __AST_H

#include <cstdint>
#include <cstring>
#include <string>
#include<vector>

#include "visitor/AstNodeVisitor.hpp"

struct Location {
    Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}

    uint32_t line;
    uint32_t col;
};

enum p_type{
    int_, 
    real_, 
    char_,
    boolean_,
    void_
};

union value{
  int value;
  double real_value;
  std::string string_value;
  bool boolean_value;
};

struct array_part{
    std::vector<int> structed_part_list;
    enum p_type type;
};

struct const_value{
  int value;
  double real_value;
  std::string string_value;
  bool boolean_value;
  enum p_type type;
  int col;
};

struct id_list{
    std::vector<char *> name_list;
    std::vector<int> col_list;
    enum p_type type;
};



class AstNode {
  public:
    AstNode(const uint32_t line, const uint32_t col);
    virtual ~AstNode() = 0;

    const Location &getLocation() const;
    virtual void print() = 0;
    virtual void visitChildNodes(AstNodeVisitor &p_visitor) = 0;
    virtual void accept(AstNodeVisitor &p_visitor) = 0;

  protected:
    const Location location;
};

#endif
