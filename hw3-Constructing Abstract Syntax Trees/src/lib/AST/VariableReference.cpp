#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line, const uint32_t col,
      std::string name, std::vector<AstNode *>* expression_n)
    : ExpressionNode{line, col} {
        v_name = name;
        expression = expression_n;
    }

// TODO
// VariableReferenceNode::VariableReferenceNode(const uint32_t line,
//                                              const uint32_t col)
//     : ExpressionNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expression != NULL){
        for(int i = 0; i < expression->size(); i++){
            expression->at(i)->accept(p_visitor);
        }
    }
}

void VariableReferenceNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}