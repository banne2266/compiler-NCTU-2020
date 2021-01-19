#include "AST/decl.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"

DeclNode::DeclNode(const uint32_t line, const uint32_t col, 
      struct id_list *identifiers_n, struct array_part *type_n)
    : AstNode{line, col} {
        // TODO
        variables = new std::vector<AstNode *>;
        variables_func = new std::vector<VariableNode *>;
        for (int i = 0; i < identifiers_n->name_list.size(); i++){
            AstNode *NewNode = new VariableNode(line, identifiers_n->col_list.at(i), identifiers_n->name_list.at(i), type_n, NULL);
            variables->push_back(NewNode);
            VariableNode *NewNode_func = new VariableNode(line, identifiers_n->col_list.at(i), identifiers_n->name_list.at(i), type_n, NULL);
            variables_func->push_back(NewNode_func);
        }
    }

DeclNode::DeclNode(const uint32_t line, const uint32_t col, 
      struct id_list*identifiers_n, struct const_value *constant)
    : AstNode{line, col} {
        // TODO
        variables = new std::vector<AstNode *>;
        for (int i = 0; i < identifiers_n->name_list.size(); i++){
            AstNode *Newconst = new ConstantValueNode(line, constant->col, constant);
            AstNode *NewNode = new VariableNode(line, identifiers_n->col_list.at(i), identifiers_n->name_list.at(i), constant->type, Newconst);
            
            variables->push_back(NewNode);
        }
    }


const char *DeclNode::getPrototypeCString(){
    /*std::string temp;
    temp.resize(128);*/

    char * temp = new char [100];
    if(variables_func != NULL){
        for(int i = 0; i < variables_func->size(); i++){
                if(i != 0){
                    strcat(temp, ", ");
                }
                strcat(temp, variables_func->at(i)->getTypeCString());
            }
    }
    return temp;
}

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto &id : *variables){
        id->accept(p_visitor);
    }
}

void DeclNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}
