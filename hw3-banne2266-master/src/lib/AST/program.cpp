#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                const char *p_name, const char *return_type_n, std::vector<AstNode *> * declarations_n,
                std::vector<AstNode *> * functions_n, AstNode *compound_n)
    : AstNode{line, col}, name(p_name), return_t(return_type_n) {
        declarations = declarations_n;
        functions = functions_n;
        compound = compound_n;
    }

// visitor pattern version: 
const char *ProgramNode::getNameCString() const { return name.c_str(); }

void ProgramNode::print() {
    // TODO
    // outputIndentationSpace();

    /*std::printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), "void");*/
    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();

    
}

void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(declarations != NULL){
        for (auto &decl : *declarations){
            decl->accept(p_visitor);
        }
    }
    if(functions != NULL){
        for (auto &func : *functions){
            func->accept(p_visitor);
        }
    }
    compound->accept(p_visitor);
}

void ProgramNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}
