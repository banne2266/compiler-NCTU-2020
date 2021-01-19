#include "AST/print.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col,
      AstNode *expression_n)
    : AstNode{line, col} {
        expression = expression_n;
    }

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {}


void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    expression->accept(p_visitor);
}

void PrintNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}