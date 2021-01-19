#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col, 
      AstNode *variable_n)
    : AstNode{line, col} {
        variable = variable_n;
    }

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {}


void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    variable->accept(p_visitor);
}

void ReadNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}

