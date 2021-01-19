#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line, const uint32_t col,
      std::string name, std::vector<AstNode *>* expression_n)
    : ExpressionNode{line, col} {
        f_name = name;
        expression = expression_n;
    }

// TODO: You may use code snippets in AstDumper.cpp
void FunctionInvocationNode::print() {}


void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expression != NULL){
        for(int i = 0; i < expression->size(); i++){
            expression->at(i)->accept(p_visitor);
        }
    }
}

void FunctionInvocationNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}
