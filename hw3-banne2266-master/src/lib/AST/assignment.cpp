#include "AST/assignment.hpp"


// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col, AstNode *VRNode_n, AstNode *ENode_n)
    : AstNode{line, col} {
        VRNode = VRNode_n;
        ENode = ENode_n;
    }

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    VRNode->accept(p_visitor);
    ENode->accept(p_visitor);
}

void AssignmentNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
}
