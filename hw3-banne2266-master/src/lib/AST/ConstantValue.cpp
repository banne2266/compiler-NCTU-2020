#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <sstream>

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, struct const_value *c_value): ExpressionNode{line, col} {
    type = c_value->type;
    
    if(c_value->type == int_)
        value = c_value->value;
    else if(c_value->type == real_)
        real_value = c_value->real_value;
    else if(c_value->type == char_)
        string_value = c_value->string_value;
    else
        boolean_value = c_value->boolean_value;
}



const char *ConstantValueNode::getValueCString(){
    if(type == int_)
        return (std::to_string(value).c_str());
    else if(type == real_){
        return (std::to_string(real_value).c_str());
    }
    else if(type == char_){
        return string_value.c_str();
    }
    else{
        if(boolean_value == true)
            return "true";
        else
            return "false";            
    }
}

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {}

void ConstantValueNode::visitChildNodes(AstNodeVisitor &p_visitor) {}

void ConstantValueNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}