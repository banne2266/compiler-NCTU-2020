#include "AST/function.hpp"
#include "visitor/AstNodeVisitor.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col, char *name_n,
        std::vector<DeclNode *> * declarations_n,
        enum p_type *type , AstNode *compound_n)
    : AstNode{line, col} {
        name = name_n;
        declarations = declarations_n;
        return_type = *type;
        compound = compound_n;
    }

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {}

const char *FunctionNode::getNameCString() const { return name.c_str(); }

const char *FunctionNode::getPrototypeCString() const {
    char * temp = new char [512];

    if(return_type == int_)
        strcat(temp, "integer ");
    else if(return_type == real_)
       strcat(temp, "real ");
    else if(return_type == char_)
        strcat(temp, "string ");
    else if(return_type == boolean_)
        strcat(temp, "boolean ");
    else
        strcat(temp, "void ");
    strcat(temp, "(");

    if(declarations != NULL){
        for(int i = 0; i < declarations->size(); i++){
                if(i != 0){
                    strcat(temp, ", ");
                }
                strcat(temp, declarations->at(i)->getPrototypeCString());
            }
    }
    //temp += ")";
    strcat(temp, ")");

    return temp;
}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(declarations != NULL){
        for (auto &decl : *declarations){
            decl->accept(p_visitor);
        }
    }
    if(compound != NULL)
        compound->accept(p_visitor);
}

void FunctionNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}

