#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col,
      std::vector<AstNode *> *declaration_n, std::vector<AstNode *> *statement_n)
    : AstNode{line, col} {
        declaration = declaration_n;
        statement = statement_n;
    }

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(declaration != NULL){
        for (auto &decl : *declaration){
            decl->accept(p_visitor);
        }
    }
    
    if(statement != NULL){
        for (auto &stat : *statement){
            stat->accept(p_visitor);
        }
    }
}

void CompoundStatementNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}
