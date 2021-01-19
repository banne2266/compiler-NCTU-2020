#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col, 
      AstNode *expression_n, AstNode *if_statement_n, AstNode *else_statement_n)
    : AstNode{line, col} {
        expression = expression_n;
        if_statement = if_statement_n;
        else_statement = else_statement_n;
    }

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {}


void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    expression->accept(p_visitor);
    if_statement->accept(p_visitor);
    if(else_statement != NULL)
        else_statement->accept(p_visitor);
}

void IfNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}