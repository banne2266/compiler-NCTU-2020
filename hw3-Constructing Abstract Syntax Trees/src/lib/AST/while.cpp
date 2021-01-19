#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col,
        AstNode* expression_n, AstNode* statement_n)
    : AstNode{line, col} {
        expression = expression_n;
        statement = statement_n;
    }

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {}


void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    expression->accept(p_visitor);
    statement->accept(p_visitor);
}

void WhileNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}