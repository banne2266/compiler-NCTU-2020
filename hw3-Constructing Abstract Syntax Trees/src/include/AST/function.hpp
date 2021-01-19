#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "visitor/AstNodeVisitor.hpp"

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col, char *name_n,
        std::vector<DeclNode *> * declarations_n,
        enum p_type *type , AstNode *compound_n
                 /* TODO: name, declarations, return type,
                  *       compound statement (optional) */);
    ~FunctionNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    void accept(AstNodeVisitor &p_visitor)override;
    const char *getNameCString() const; 
    const char *getPrototypeCString() const; 
    

  private:
    // TODO: name, declarations, return type, compound statement
    std::string name;
    std::vector<DeclNode *> *declarations;
    enum p_type return_type;
    AstNode *compound;
};

#endif
