#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col, 
      const char * name_n, struct array_part *type_n, AstNode* constant_n)
    : AstNode{line, col}, name(name_n) {
        type = type_n;
        constant = constant_n;
    }

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col, 
      const char * name_n, enum p_type type_n, AstNode* constant_n)
    : AstNode{line, col}, name(name_n) {
        type = new struct array_part ;
        type->type = type_n;
        constant = constant_n;
    }

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {}

const char *VariableNode::getNameCString(){
    std::string temp(&name[0]);
     return name.c_str(); 
}

const char *VariableNode::getTypeCString(){ 
    std::string temp;
    if(type->type == int_)
        temp = "integer";
    else if(type->type == real_)
        temp = "real";
    else if(type->type == char_)
        temp = "string";
    else if(type->type == boolean_)
        temp = "boolean";
    else if(type->type == void_)
        temp = "";
    
    for(int i = 0; i < type->structed_part_list.size(); i++){
        if(i == 0)
            temp += " ";
        temp += "[";
        temp += std::to_string( type->structed_part_list[i] );
        temp += "]";
    }
    return temp.c_str();
}


void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(constant != NULL)
        constant->accept(p_visitor);
}

void VariableNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}