#ifndef __AST_PROGRAM_NODE_H
#define __AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ProgramNode : public AstNode {
  public:
    ProgramNode(const uint32_t line, const uint32_t col,
                const char *p_name, const char *return_type_n, std::vector<AstNode *> * declarations_n,
                std::vector<AstNode *> * functions_n, AstNode *compound_n
                /* TODO: return type, declarations, functions,
                 *       compound statement */);
    ~ProgramNode() = default;

    // visitor pattern version: 
    const char *getNameCString() const; 

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    void accept(AstNodeVisitor &p_visitor)override;

  private:
    const std::string name;
    const std::string return_t;
    std::vector<AstNode *> *declarations;
    std::vector<AstNode *> *functions;
    AstNode *compound;
    // TODO: return type, declarations, functions, compound statement
};

#endif
