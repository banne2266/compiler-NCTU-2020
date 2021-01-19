#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"



class AssignmentNode : public AstNode {
  public:
    AssignmentNode(const uint32_t line, const uint32_t col, 
        AstNode *VRNode_n, AstNode *ENode_n
                   /* TODO: variable reference, expression V */);
    ~AssignmentNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void accept(AstNodeVisitor &p_visitor) override;

  private:
    AstNode *VRNode;
    AstNode *ENode;
    // TODO: variable reference, expression V
};

#endif
