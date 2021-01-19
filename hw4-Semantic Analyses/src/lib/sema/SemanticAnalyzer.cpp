#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"
#define MAX_LINE_NUM        201
#define MAX_LINE_LENG       512

/*
* TODO:
*
* 1. Push a new symbol table if this node forms a scope.
* 2. Insert the symbol into current symbol table if this node is related to
*    declaration (ProgramNode, VariableNode, FunctionNode).
* 3. Travere child nodes of this node.
* 4. Perform semantic analyses of this node.
* 5. Pop the symbol table pushed at the 1st step.
*/

extern int32_t opt_dump_symbol;
extern bool error;
extern char code[MAX_LINE_NUM][MAX_LINE_LENG];
extern int in_func;

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    SymbolTable* newtable = new SymbolTable(1);
    symblo_manager.pushScope(newtable);//1
    
    symblo_manager.addSymbol(p_program.getNameCString(), 
                    ENTRY_KIND_PROGRAM, PType::PrimitiveTypeEnum::kVoidType, "", 0);//2
    std::vector<PType> temp;
    PType temp2(PType::PrimitiveTypeEnum::kVoidType);
    function_manager.addFunction(p_program.getNameCString(),
                    temp, temp2);
    p_program.visitChildNodes(*this);//3
    //4 skip
    symblo_manager.popScope(opt_dump_symbol);//5

    /*for(int i = 0; i < function_manager.table.size(); i++){
        printf("Fname: %s, return %d\n",function_manager.table[i].name.c_str(), function_manager.table[i].return_type);
    }*/

}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    int local_error = 0;
    int put = 0;
    if(symblo_manager.lookup(p_variable.getNameCString(), 0) != ENTRY_KIND_NO_FOUND || symblo_manager.lookup_for(p_variable.getNameCString())){
        print_error(p_variable.getLocation().line, p_variable.getLocation().col);
        fprintf(stderr, "symbol '%s' is redeclared\n", p_variable.getNameCString());
        print_error_code(p_variable.getLocation().line, p_variable.getLocation().col);
        local_error = 1;
        error = 1;
    }
    std::vector<uint64_t> dim_vet = p_variable.getType().get_dim();
    for(int i = 0; i < dim_vet.size(); i++){
        if(dim_vet[i] <= 0){
            print_error(p_variable.getLocation().line, p_variable.getLocation().col);
            fprintf(stderr, "'%s' declared as an array with an index that is not greater than 0\n", p_variable.getNameCString());
            print_error_code(p_variable.getLocation().line, p_variable.getLocation().col);
            put = 1;
            error = 1;
        }
    }

    int kind =  0;
        if(p_variable.is_constant())
            kind = ENTRY_KIND_CONSTANT;
        else if(symblo_manager.get_top_kind() == TABLE_KIND_FUNCTION)//in function declaration
            kind = ENTRY_KIND_PARAMETER;
        else if(symblo_manager.get_top_kind() == TABLE_KIND_FORLOOP)//in for loop varible
            kind = ENTRY_KIND_LOOP_VAR;
        else
            kind = ENTRY_KIND_VARIABLE;
    //1 skip
    if(local_error == 0 && put == 0){
        symblo_manager.addSymbol(p_variable.getNameCString(), 
                        kind, p_variable.getType(), p_variable.getconstantCString(), 0);
    }//2
    else if(local_error == 0 && put == 1) {
        symblo_manager.addSymbol(p_variable.getNameCString(), 
                        kind, p_variable.getType(), p_variable.getconstantCString(), 1);
    }
    
    p_variable.visitChildNodes(*this);//3
    //5 skip
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    //1 skip
    //2 skip
    p_constant_value.visitChildNodes(*this);//3

    p_constant_value.set_type(p_constant_value.getTypePtr());

    //5 skip
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    int local_error = 0;
    if(symblo_manager.lookup(p_function.getNameCString(), 0) != ENTRY_KIND_NO_FOUND){
        print_error(p_function.getLocation().line, p_function.getLocation().col);
        fprintf(stderr, "symbol '%s' is redeclared\n", p_function.getNameCString());
        print_error_code(p_function.getLocation().line, p_function.getLocation().col);
        local_error = 1;
        error = 1;
    }
    
    std::vector<PType> temp;
    function_manager.addFunction(p_function.getNameCString(),
                    p_function.getParatype(), p_function.getReturnType());
    function_manager.add_num();


    if(local_error == 0)
    {    
        symblo_manager.addSymbol(p_function.getNameCString(), 
                        ENTRY_KIND_FUNCTION, p_function.getReturnType(), p_function.getParatypeCString(), 0);
    }//1
    else{}
    SymbolTable* newtable = new SymbolTable(2);
        symblo_manager.pushScope(newtable);

    //2 skip
    p_function.visitChildNodes(*this);//3
    symblo_manager.popScope(opt_dump_symbol);//5
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    int top_kind = symblo_manager.get_top_kind();
    if(top_kind != 2){
        SymbolTable* newtable = new SymbolTable(0);
        symblo_manager.pushScope(newtable);
    }//1
    //2 skip
    symblo_manager.change_state(TABLE_KIND_REST);
    p_compound_statement.visitChildNodes(*this);//3
    //4 skip
    if(top_kind != 2)
        symblo_manager.popScope(opt_dump_symbol);//5

}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    //1 skip
    //2 skip
    p_print.visitChildNodes(*this);//3
    
    int local_error = p_print.get_expr_error();

    if(local_error == 0){
        PType expr_type = p_print.get_expr_type();
        if(expr_type.get_dim().size() != 0 || expr_type.getPrimitiveType() == PType::PrimitiveTypeEnum::kVoidType){
            print_error(p_print.target->getLocation().line, p_print.target->getLocation().col);
            fprintf(stderr, "expression of print statement must be scalar type\n");
            print_error_code(p_print.target->getLocation().line, p_print.target->getLocation().col);
            local_error = 1;
            error = 1;
        }
    }//4

    //5 skip
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    //1 skip
    //2 skip
    p_bin_op.visitChildNodes(*this);//3
    std::string op(p_bin_op.getOpCString());

    PType left = p_bin_op.get_left_node_type();
    PType right = p_bin_op.get_right_node_type();

    if(op == "+" || op =="-" || op == "*" || op == "/"){
        if((left.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType 
        && left.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType) 
        || (right.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType 
        && right.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType)
        || left.get_dim().size() > 0
        || right.get_dim().size() > 0){
            if(op != "+" || left.getPrimitiveType() != PType::PrimitiveTypeEnum::kStringType
            || right.getPrimitiveType() != PType::PrimitiveTypeEnum::kStringType
            || left.get_dim().size() > 0
            || right.get_dim().size() > 0){
                print_error(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
                fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", op.c_str(), left.getPTypeCString(), right.getPTypeCString());
                print_error_code(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
                error = 1;
                p_bin_op.set_error();
                PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kStringType));
                p_bin_op.set_type(temp);
            }
            else{
                PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kStringType));
                p_bin_op.set_type(temp);
            }
        }
        else if(left.getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType
        || right.getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType){
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kRealType));
            p_bin_op.set_type(temp);
        }
        else{
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kIntegerType));
            p_bin_op.set_type(temp);
        }
    }
    else if(op == "mod"){
        if(left.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType
        || right.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType
        || left.get_dim().size() > 0
        || right.get_dim().size() > 0){
            print_error(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", op.c_str(), left.getPTypeCString(), right.getPTypeCString());
            print_error_code(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            error = 1;
            p_bin_op.set_error();
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kIntegerType));
            p_bin_op.set_type(temp);
        }
        else{
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kIntegerType));
            p_bin_op.set_type(temp);
        }
    }
    else if(op == "and" || op == "or" || op == "not"){
        if(left.getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType
        || right.getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType
        || left.get_dim().size() > 0
        || right.get_dim().size() > 0){
            print_error(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", op.c_str(), left.getPTypeCString(), right.getPTypeCString());
            print_error_code(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            error = 1;
            p_bin_op.set_error();
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_bin_op.set_type(temp);
        }
        else{
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_bin_op.set_type(temp);
        }
    }
    else if(op == "<" || op == "<=" || op == "=" || op == ">=" || op == ">" || op == "<>"){
        if((left.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType 
        && left.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType) 
        || (right.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType 
        && right.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType)
        || left.get_dim().size() > 0
        || right.get_dim().size() > 0){
            print_error(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", op.c_str(), left.getPTypeCString(), right.getPTypeCString());
            print_error_code(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            error = 1;
            p_bin_op.set_error();
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_bin_op.set_type(temp);
        }
        else{
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_bin_op.set_type(temp);
        }
    }

    //5 skip
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    //1 skip
    //2 skip
    

    p_un_op.visitChildNodes(*this);//3
    std::string op(p_un_op.getOpCString());
    PType oparand = p_un_op.get_oparand_type();
    if(op == "neg"){
        if(oparand.getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType
        && oparand.get_dim().size() == 0){
            p_un_op.set_error();
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kIntegerType));
            p_un_op.set_type(temp);
        }
        else if(oparand.getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType
        && oparand.get_dim().size() == 0){
            p_un_op.set_error();
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kRealType));
            p_un_op.set_type(temp);
        }
        else{
            print_error(p_un_op.getLocation().line, p_un_op.getLocation().col);
            fprintf(stderr, "invalid operand to unary operator '%s' ('%s')\n", op.c_str(), oparand.getPTypeCString());
            print_error_code(p_un_op.getLocation().line, p_un_op.getLocation().col);
            error = 1;
            p_un_op.set_error();
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kRealType));
            p_un_op.set_type(temp);
        }
    }
    else if(op == "not"){
        if(oparand.getPrimitiveType() == PType::PrimitiveTypeEnum::kBoolType
        && oparand.get_dim().size() == 0){
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_un_op.set_type(temp);
        }
        else{
            print_error(p_un_op.getLocation().line, p_un_op.getLocation().col);
            fprintf(stderr, "invalid operand to unary operator '%s' ('%s')\n", op.c_str(), oparand.getPTypeCString());
            print_error_code(p_un_op.getLocation().line, p_un_op.getLocation().col);
            error = 1;
            p_un_op.set_error();
            PTypeSharedPtr temp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_un_op.set_type(temp);
        }
    }

    //5 skip
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    //1 skip
    //2 skip
    p_func_invocation.visitChildNodes(*this);//3

    int local_error = 0;
    int symbol_kind = symblo_manager.lookup(p_func_invocation.getNameCString(), 1);
    
    //fprintf(stderr, "\nline: %d, name: %s", p_func_invocation.getLocation().line, p_func_invocation.getNameCString());

    if(symbol_kind == ENTRY_KIND_NO_FOUND){
        print_error(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        fprintf(stderr, "use of undeclared symbol '%s'\n", p_func_invocation.getNameCString());
        print_error_code(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        p_func_invocation.set_error();
        local_error = 1;
        error = 1;
    }
    else if(symbol_kind != ENTRY_KIND_FUNCTION){
        print_error(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        fprintf(stderr, "call of non-function symbol '%s'\n", p_func_invocation.getNameCString());
        print_error_code(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        p_func_invocation.set_error();
        local_error = 1;
        error = 1;
    }
    else {
        FunctionEntry func_entry = function_manager.lookup(p_func_invocation.getNameCString());
        std::vector<PType> para_list = p_func_invocation.getParatype();
        if(func_entry.paremeter_type.size() != para_list.size()){
            print_error(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
            fprintf(stderr, "too few/much arguments provided for function '%s'\n", p_func_invocation.getNameCString());
            print_error_code(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
            p_func_invocation.set_error();
            local_error = 1;
            error = 1;
        }
        else{
            for(int i = 0; i < para_list.size(); i++){
                if(func_entry.paremeter_type[i] != para_list[i]){
                    print_error(p_func_invocation.arguments[i]->getLocation().line, p_func_invocation.arguments[i]->getLocation().col);
                    fprintf(stderr, "incompatible type passing '%s' to parameter of type '%s'\n"
                                , para_list[i].getPTypeCString(), func_entry.paremeter_type[i].getPTypeCString());
                    print_error_code(p_func_invocation.arguments[i]->getLocation().line, p_func_invocation.arguments[i]->getLocation().col);
                    p_func_invocation.set_error();
                    local_error = 1;
                    error = 1;
                    break;
                }
            }
        }
    }
    
    if(local_error != 1 && symblo_manager.get_symbol_entry(p_func_invocation.getNameCString()).pass == 0){
        PType* temp = new PType(symblo_manager.get_symbol_entry(p_func_invocation.getNameCString()).type);
        std::vector<uint64_t> new_dim;
        temp->setDimensions(new_dim);
        PTypeSharedPtr tempp = (PTypeSharedPtr) temp;
        p_func_invocation.set_type(tempp);
    }

    //5 skip
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    //1 skip
    //2 skip

    p_variable_ref.visitChildNodes(*this);//3

    int local_error = p_variable_ref.get_error_inner();
    int symbol_kind = symblo_manager.lookup(p_variable_ref.getNameCString(), 1);
    if(symbol_kind == ENTRY_KIND_NO_FOUND){
        print_error(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        fprintf(stderr, "use of undeclared symbol '%s'\n", p_variable_ref.getNameCString());
        print_error_code(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        p_variable_ref.set_error();
        local_error = 1;
        error = 1;
    }
    else if(symbol_kind == ENTRY_KIND_PROGRAM || symbol_kind == ENTRY_KIND_FUNCTION){
        print_error(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        fprintf(stderr, "use of non-variable symbol '%s'\n", p_variable_ref.getNameCString());
        print_error_code(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        p_variable_ref.set_error();
        local_error = 1;
        error = 1;
    }
    else if(symblo_manager.get_symbol_entry(p_variable_ref.getNameCString()).pass == 1)
        p_variable_ref.set_error();

    if(local_error != 1 && symblo_manager.get_symbol_entry(p_variable_ref.getNameCString()).pass == 0){

        std::vector<uint64_t> dim_temp = symblo_manager.get_symbol_entry(p_variable_ref.getNameCString()).type.get_dim();
        std::vector<uint64_t> new_dim;

        for(int i = p_variable_ref.get_dim(); i < dim_temp.size(); i++){
            new_dim.push_back(dim_temp[i]);
        }

        PType* temp = new PType(symblo_manager.get_symbol_entry(p_variable_ref.getNameCString()).type);
        temp->setDimensions(new_dim);
        PTypeSharedPtr tempp = (PTypeSharedPtr) temp;
        p_variable_ref.set_type(tempp);

        std::vector<PType::PrimitiveTypeEnum> variable_ref_expr = p_variable_ref.get_Exprs();
        for(int i = 0; i < variable_ref_expr.size(); i++){
            if(variable_ref_expr[i] != PType::PrimitiveTypeEnum::kIntegerType){
                print_error(p_variable_ref.getLocation().line, p_variable_ref.indices[i]->getLocation().col);
                fprintf(stderr, "index of array reference must be an integer\n");
                print_error_code(p_variable_ref.getLocation().line, p_variable_ref.indices[i]->getLocation().col);
                p_variable_ref.set_error();
                local_error = 1;
                error = 1;
            }
        }
        std::vector<uint64_t> temp2 = symblo_manager.get_symbol_entry(p_variable_ref.getNameCString()).type.get_dim();
        if(p_variable_ref.get_dim() > temp2.size()){
            print_error(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
            fprintf(stderr, "there is an over array subscript on '%s'\n", p_variable_ref.getNameCString());
            print_error_code(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
            p_variable_ref.set_error();
            local_error = 1;
            error = 1;
        }
    }
    
    //5 skip
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    //1 skip
    //2 skip
    p_assignment.visitChildNodes(*this);//3

    int var_symbol_kind = symblo_manager.lookup(p_assignment.lvalue->getNameCString(), 1);

    int local_error = p_assignment.lvalue->get_error();
    int local_error2 = p_assignment.expr->get_error();

    if(local_error == 0){
        PType var_type = p_assignment.lvalue->get_type();
        if(p_assignment.lvalue->get_type().get_dim().size() != 0){
            print_error(p_assignment.lvalue->getLocation().line, p_assignment.lvalue->getLocation().col);
            fprintf(stderr, "array assignment is not allowed\n");
            print_error_code(p_assignment.lvalue->getLocation().line, p_assignment.lvalue->getLocation().col);
            local_error = 1;
            error = 1;
        }
        else if(var_symbol_kind == ENTRY_KIND_CONSTANT){
            print_error(p_assignment.lvalue->getLocation().line, p_assignment.lvalue->getLocation().col);
            fprintf(stderr, "cannot assign to variable '%s' which is a constant\n", p_assignment.lvalue->getNameCString());
            print_error_code(p_assignment.lvalue->getLocation().line, p_assignment.lvalue->getLocation().col);
            local_error = 1;
            error = 1;
        }
        else if(var_symbol_kind == ENTRY_KIND_LOOP_VAR && symblo_manager.get_top_kind() != TABLE_KIND_FORLOOP){
            print_error(p_assignment.lvalue->getLocation().line, p_assignment.lvalue->getLocation().col);
            fprintf(stderr, "the value of loop variable cannot be modified inside the loop body\n");
            print_error_code(p_assignment.lvalue->getLocation().line, p_assignment.lvalue->getLocation().col);
            local_error = 1;
            error = 1;
        }
    }
    if(local_error == 0 && local_error2 == 0){
        PType var_type = p_assignment.lvalue->get_type();
        PType expr_type = p_assignment.expr->get_type();
        if(p_assignment.expr->get_type().get_dim().size() != 0){
            print_error(p_assignment.expr->getLocation().line, p_assignment.expr->getLocation().col);
            fprintf(stderr, "array assignment is not allowed\n");
            print_error_code(p_assignment.expr->getLocation().line, p_assignment.expr->getLocation().col);
            local_error = 1;
            error = 1;
        }
        else if((var_type.getPrimitiveType() == PType::PrimitiveTypeEnum::kStringType && expr_type.getPrimitiveType() != PType::PrimitiveTypeEnum::kStringType)
        || (var_type.getPrimitiveType() == PType::PrimitiveTypeEnum::kBoolType && expr_type.getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType)
        || (var_type.getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType && expr_type.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
        ||(var_type.getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType 
            && (expr_type.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType) && (expr_type.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType))
        ){
            print_error(p_assignment.getLocation().line, p_assignment.getLocation().col);
            fprintf(stderr, "assigning to '%s' from incompatible type '%s'\n", var_type.getPTypeCString(), expr_type.getPTypeCString());
            print_error_code(p_assignment.getLocation().line, p_assignment.getLocation().col);
            local_error = 1;
            error = 1;
        }

    }

    //5 skip
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    //1 skip
    //2 skip
    p_read.visitChildNodes(*this);//3


    int local_error = p_read.get_expr_error();
    int symbol_kind = symblo_manager.lookup(p_read.target->getNameCString(), 1);

    if(local_error == 0){
        PType expr_type = p_read.get_expr_type();
        if(expr_type.get_dim().size() != 0){
            print_error(p_read.target->getLocation().line, p_read.target->getLocation().col);
            fprintf(stderr, "variable reference of read statement must be scalar type\n");
            print_error_code(p_read.target->getLocation().line, p_read.target->getLocation().col);
            local_error = 1;
            error = 1;
        }
        else if(symbol_kind == ENTRY_KIND_LOOP_VAR || symbol_kind == ENTRY_KIND_CONSTANT){
            print_error(p_read.target->getLocation().line, p_read.target->getLocation().col);
            fprintf(stderr, "variable reference of read statement cannot be a constant or loop variable\n");
            print_error_code(p_read.target->getLocation().line, p_read.target->getLocation().col);
            local_error = 1;
            error = 1;
        }
    }

    //5 skip
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    //1 skip
    //2 skip
    p_if.visitChildNodes(*this);//3

    int local_error = p_if.condition->get_error();

    if(local_error == 0){
        PType expr_type = p_if.condition->get_type();
        if(expr_type.getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType
        || expr_type.get_dim().size() != 0){
            print_error(p_if.condition->getLocation().line, p_if.condition->getLocation().col);
            fprintf(stderr, "the expression of condition must be boolean type\n");
            print_error_code(p_if.condition->getLocation().line, p_if.condition->getLocation().col);
            local_error = 1;
            error = 1;
        }
    }

    //5 skip
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    //1 skip
    //2 skip
    p_while.visitChildNodes(*this);//3

    int local_error = p_while.condition->get_error();

    if(local_error == 0){
        PType expr_type = p_while.condition->get_type();
        if(expr_type.getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType
        || expr_type.get_dim().size() != 0){
            print_error(p_while.condition->getLocation().line, p_while.condition->getLocation().col);
            fprintf(stderr, "the expression of condition must be boolean type\n");
            print_error_code(p_while.condition->getLocation().line, p_while.condition->getLocation().col);
            local_error = 1;
            error = 1;
        }
    }

    //5 skip
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    SymbolTable* newtable = new SymbolTable(3);
    symblo_manager.pushScope(newtable);//1
    //2 skip
    p_for.visitChildNodes(*this);//3

    int left_num = p_for.left;
    int right_num = p_for.condition->constant->value.integer;
    if(left_num > right_num){
        print_error(p_for.getLocation().line, p_for.getLocation().col);
        fprintf(stderr, "the lower bound and upper bound of iteration count must be in the incremental order\n");
        print_error_code(p_for.getLocation().line, p_for.getLocation().col);
        error = 1;
    }

    symblo_manager.popScope(opt_dump_symbol);//5
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    //1 skip
    //2 skip
    p_return.visitChildNodes(*this);//3

    int local_error = 0;
    int tt = function_manager.get_func_num();
    if(in_func == 0)
        tt = 0;
    FunctionEntry func_entry = function_manager.lookup(tt);

    std::string namee(func_entry.name);
    std::string name2(symblo_manager.get_symbol_entry(namee).name);

    if(func_entry.return_type.getPrimitiveType() == PType::PrimitiveTypeEnum::kVoidType 
        && p_return.retval->get_type_ptr() != nullptr){
        print_error(p_return.getLocation().line, p_return.getLocation().col);
        fprintf(stderr, "program/procedure should not return a value\n");
        print_error_code(p_return.getLocation().line, p_return.getLocation().col);
        local_error = 1;
        error = 1;
    }
    if(local_error == 0){
        if((func_entry.return_type.getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType
                && (p_return.retval->get_type().getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType
                        &&  p_return.retval->get_type().getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
            )
            || func_entry.return_type != p_return.retval->get_type()){
            print_error(p_return.retval->getLocation().line, p_return.retval->getLocation().col);
            fprintf(stderr, "return '%s' from a function with return type '%s'\n"
                , p_return.retval->get_type().getPTypeCString(), func_entry.return_type.getPTypeCString());
            print_error_code(p_return.retval->getLocation().line, p_return.retval->getLocation().col);
            local_error = 1;
            error = 1;
        }
    }
    //5 skip
    
}

void SemanticAnalyzer::print_error(const uint32_t line, const uint32_t col)
{
    fprintf(stderr, "<Error> Found in line %d, column %d: ", line, col);
}

void SemanticAnalyzer::print_error_code(const uint32_t line, const uint32_t col)
{
    fprintf(stderr, "    %s\n    ", code[line]);
    for(int i = 0; i < col - 1; i++)
        fprintf(stderr, " ");
    fprintf(stderr, "^\n");
}