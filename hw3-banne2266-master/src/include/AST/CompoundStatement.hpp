#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class CompoundStatementNode : public AstNode {
  public:
    CompoundStatementNode(const uint32_t line, const uint32_t col,
      std::vector<AstNode *> *declaration_n, std::vector<AstNode *> *statement_n
                          /* TODO: declarations, statements */);
    ~CompoundStatementNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    void accept(AstNodeVisitor &p_visitor)override;

  private:
    // TODO: declarations, statements
    std::vector<AstNode *> *declaration;
    std::vector<AstNode *> *statement;
};

#endif
