#include "AST/for.hpp"
#include "visitor/AstNodeVisitor.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
        AstNode* declaration_n, AstNode* assignment_n, AstNode* expression_n, AstNode* statement_n)
    : AstNode{line, col} {
        declaration = declaration_n;
        assignment = assignment_n;
        expression = expression_n;
        statement = statement_n;
    }

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    declaration->accept(p_visitor);
    assignment->accept(p_visitor);
    expression->accept(p_visitor);
    statement->accept(p_visitor);
}

void ForNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}
