#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <string>

CodeGenerator::CodeGenerator(const char *in_file_name, const char *out_file_name, SymbolManager *symbol_manager) 
    : in_file_name(in_file_name), symbol_manager(symbol_manager) {
    this->out_fp = fopen(out_file_name, "w");
    assert(this->out_fp != NULL && "fopen() fails");
}

CodeGenerator::~CodeGenerator() {
    fclose(this->out_fp);
}

void CodeGenerator::function_prologue()
{
    offset = 0;
    offset_minus();
    offset_minus();
    dumpInstrs("    addi sp, sp, -128\n");   //move stack pointer to lower address to allocate a new stack
    dumpInstrs("    sw ra, 124(sp)\n");        //save return address of the caller function in the current stack
    dumpInstrs("    sw s0, 120(sp)\n");       //save frame pointer of the last stack in the current stack
    dumpInstrs("    addi s0, sp, 128\n");    //move frame pointer to the bottom of the current stack
}
void CodeGenerator::function_epilogue(const char* func_name)
{
    dumpInstrs("    lw ra, 124(sp)\n");        //load return address saved in the current stack
    dumpInstrs("    lw s0, 120(sp)\n");       //move frame pointer back to the bottom of the last stack
    dumpInstrs("    addi sp, sp, 128\n");   //move stack pointer back to the top of the last stack
    dumpInstrs("    jr ra\n");                          //jump back to the caller function
    dumpInstrs("    .size %s, .-%s\n", func_name, func_name);
    offset_add();
    offset_add();
}



void CodeGenerator::dumpInstrs(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(this->out_fp, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // Reconstruct the hash table for looking up the symbol entry
    // Hint: Use symbol_manager->lookup(symbol_name) to get the symbol entry.
    symbol_manager->reconstructHashTableFromSymbolTable(p_program.getSymbolTable());

    // Generate RISC-V instructions for program header
    dumpInstrs("%s%s%s%s", 
        "   .file \"", this->in_file_name, "\"\n",
        "   .option nopic\n"
    );

    //p_program.visitChildNodes(*this);

    for (auto &decl : p_program.var_decls) {
        decl->accept(*this);
    }
    for (auto &func : p_program.funcs) {
        func->accept(*this);
    }

    dumpInstrs("\n.text\n");
    dumpInstrs("  .align  2\n");
    dumpInstrs("  .global main\n");
    dumpInstrs("  .type   main, @function\n");
    dumpInstrs("\nmain:\n");

    function_prologue();
    p_program.body->accept(*this);
    function_epilogue("main");




    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_program.getSymbolTable());
}

void CodeGenerator::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void CodeGenerator::visit(VariableNode &p_variable) {
    if(get_in_local() == 0){//global
        if(symbol_manager->lookup(p_variable.getName())->getKind() == SymbolEntry::KindEnum::kVariableKind){//global variable
            dumpInstrs("\n.comm %s, 4, 4\n", p_variable.getNameCString());
        }
        else if(symbol_manager->lookup(p_variable.getName())->getKind() == SymbolEntry::KindEnum::kConstantKind){//global constant
            dumpInstrs("\n.section    .rodata \n");
            dumpInstrs("    .align 2\n");
            dumpInstrs("    .globl %s\n", p_variable.getNameCString());
            dumpInstrs("    .type %s, @object\n", p_variable.getNameCString());
            dumpInstrs("%s:\n", p_variable.getNameCString());
            dumpInstrs("    .word %s \n", p_variable.getConstantPtr()->getConstantValueCString() );
        }
    }
    else{//local
        SymbolEntry *DeclEntry = (SymbolEntry *) symbol_manager->lookup(p_variable.getName());
        if(DeclEntry->getKind() == SymbolEntry::KindEnum::kVariableKind){//local variable
            offset_minus();
            DeclEntry->set_offset(offset);
            dumpInstrs("#Declare local const variable:%s, offset: %d\n", DeclEntry->getNameCString(), DeclEntry->get_offset());
        }
        else if(DeclEntry->getKind() == SymbolEntry::KindEnum::kConstantKind){//local constant
            offset_minus();
            DeclEntry->set_offset(offset);
            dumpInstrs("#Declare local const variable:%s, offset: %d\n", DeclEntry->getNameCString(), DeclEntry->get_offset());
            dumpInstrs("    addi t0, s0, %d\n", offset);
            dumpInstrs("    addi sp, sp, -4\n");
            dumpInstrs("    sw t0, 0(sp)\n");
            dumpInstrs("    li t0, %s\n", p_variable.getConstantPtr()->getConstantValueCString());
            dumpInstrs("    addi sp, sp, -4\n");
            dumpInstrs("    sw t0, 0(sp)\n");
            dumpInstrs("    lw t0, 0(sp)\n");
            dumpInstrs("    addi sp, sp, 4\n");
            dumpInstrs("    lw t1, 0(sp)\n");
            dumpInstrs("    addi sp, sp, 4\n");
            dumpInstrs("    sw t0, 0(t1)\n");
        }
        else if(DeclEntry->getKind() == SymbolEntry::KindEnum::kParameterKind){
            offset_minus();
            DeclEntry->set_offset(offset);
        }
        else if(DeclEntry->getKind() == SymbolEntry::KindEnum::kLoopVarKind){
            offset_minus();
            DeclEntry->set_offset(offset);
        }
    }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    if(p_constant_value.getTypePtr()->isInteger()){
        dumpInstrs("    li t0, %s\n", p_constant_value.getConstantValueCString());
        dumpInstrs("    addi sp, sp, -4\n");
        dumpInstrs("    sw t0, 0(sp)\n");
    }
    else if(p_constant_value.getTypePtr()->isBool()){
        if(p_constant_value.getConstantValueCString() == std::string("true"))
            dumpInstrs("    li t0, 1\n");
        else
            dumpInstrs("    li t0, 0\n");
        dumpInstrs("    addi sp, sp, -4\n");
        dumpInstrs("    sw t0, 0(sp)\n");
    }
    else if(p_constant_value.getTypePtr()->isString()){
        
    }
}

void CodeGenerator::visit(FunctionNode &p_function) {
    // Reconstruct the hash table for looking up the symbol entry
    symbol_manager->reconstructHashTableFromSymbolTable(p_function.getSymbolTable());
    set_in_local();
    

    dumpInstrs("\n.text\n");
    dumpInstrs("  .align  2\n");
    dumpInstrs("  .global %s\n", p_function.getNameCString());
    dumpInstrs("  .type   %s, @function\n", p_function.getNameCString());
    dumpInstrs("\n%s:\n", p_function.getNameCString());

    function_prologue();
    dumpInstrs("#Parameter num: %d\n", p_function.getParametersNum_t());
    for (auto &parameter : p_function.parameters) {
        parameter->accept(*this);
    }
    if(p_function.getParametersNum_t() <= 8){
        for(int i = 0; i < p_function.getParametersNum_t(); i++){
            std::string para_name = p_function.getSymbolTable()->getEntries().at(i)->getName();
            SymbolEntry *ParaEntry = (SymbolEntry *) symbol_manager->lookup(para_name);
            dumpInstrs("    sw a%d, %d(s0)\n", i, ParaEntry->get_offset());
        }
    }
    else{//TODO
        int super_8_size = 4 * p_function.getParametersNum_t();
        for(int i = 0; i < p_function.getParametersNum_t(); i++){
            std::string para_name = p_function.getSymbolTable()->getEntries().at(i)->getName();
            SymbolEntry *ParaEntry = (SymbolEntry *) symbol_manager->lookup(para_name);
            dumpInstrs("    lw t0, %d(s0)\n", super_8_size - 4);//load parameter from FP
            dumpInstrs("    sw t0, %d(s0)\n", ParaEntry->get_offset());
            super_8_size -= 4;
        }
    }
    if (p_function.body) {
        p_function.body->accept(*this);
    }
    //p_function.visitChildNodes(*this);
    function_epilogue(p_function.getNameCString());

    // Remove the entries in the hash table
    reset_in_local();
    symbol_manager->removeSymbolsFromHashTable(p_function.getSymbolTable());
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    // Reconstruct the hash table for looking up the symbol entry
    symbol_manager->reconstructHashTableFromSymbolTable(p_compound_statement.getSymbolTable());
    set_in_local();


    p_compound_statement.visitChildNodes(*this);
    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_compound_statement.getSymbolTable());
}

void CodeGenerator::visit(PrintNode &p_print) {
    dumpInstrs("#Print start\n");
    p_print.visitChildNodes(*this);
    
    dumpInstrs("    lw a0, 0(sp)\n");
    dumpInstrs("    addi sp, sp, 4\n");
    dumpInstrs("    jal ra, printInt\n");
    dumpInstrs("#Print end\n");
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    p_bin_op.visitChildNodes(*this);
    std::string op(p_bin_op.getOpCString());
    dumpInstrs("    lw t0, 0(sp)\n");//RHS
    dumpInstrs("    addi sp, sp, 4\n");
    dumpInstrs("    lw t1, 0(sp)\n");//LHS
    dumpInstrs("    addi sp, sp, 4\n");
    if(op == "+"){
        dumpInstrs("    add t0, t1, t0 \n");
    }
    else if(op == "-"){
        dumpInstrs("    sub t0, t1, t0 \n");
    }
    else if(op == "*"){
        dumpInstrs("    mul t0, t1, t0 \n");
    }
    else if(op == "/"){
        dumpInstrs("    div t0, t1, t0 \n");
    }
    else if(op == "mod"){
        dumpInstrs("    rem t0, t1, t0 \n");
    }
    else if(op == "<"){//t0 = (t0 < t1) ? 1 : 0
        int label_out = newlabel();
        int label_true = newlabel();
        dumpInstrs("    blt t1, t0, L%d\n", label_true);
        dumpInstrs("    li t0, 0\n");
        dumpInstrs("    j L%d\n", label_out);
        dumpInstrs("L%d:\n", label_true);
        dumpInstrs("    li t0, 1\n");
        dumpInstrs("L%d:\n", label_out);
    }
    else if(op == "<="){//t0 = (t0 < =t1) ? 1 : 0
        int label_out = newlabel();
        int label_true = newlabel();
        dumpInstrs("    ble t1, t0, L%d\n", label_true);
        dumpInstrs("    li t0, 0\n");
        dumpInstrs("    j L%d\n", label_out);
        dumpInstrs("L%d:\n", label_true);
        dumpInstrs("    li t0, 1\n");
        dumpInstrs("L%d:\n", label_out);
    }
    else if(op == "="){//t0 = (t0 = t1) ? 1 : 0
        int label_out = newlabel();
        int label_true = newlabel();
        dumpInstrs("    beq t0, t1, L%d\n", label_true);
        dumpInstrs("    li t0, 0\n");
        dumpInstrs("    j L%d\n", label_out);
        dumpInstrs("L%d:\n", label_true);
        dumpInstrs("    li t0, 1\n");
        dumpInstrs("L%d:\n", label_out);
    }
    else if(op == ">="){//t0 = (t0 >= t1) ? 1 : 0
        int label_out = newlabel();
        int label_true = newlabel();
        dumpInstrs("    bge t1, t0, L%d\n", label_true);
        dumpInstrs("    li t0, 0\n");
        dumpInstrs("    j L%d\n", label_out);
        dumpInstrs("L%d:\n", label_true);
        dumpInstrs("    li t0, 1\n");
        dumpInstrs("L%d:\n", label_out);
    }
    else if(op == ">"){//t0 = (t0 > t1) ? 1 : 0
        int label_out = newlabel();
        int label_true = newlabel();
        dumpInstrs("    bgt t1, t0, L%d\n", label_true);
        dumpInstrs("    li t0, 0\n");
        dumpInstrs("    j L%d\n", label_out);
        dumpInstrs("L%d:\n", label_true);
        dumpInstrs("    li t0, 1\n");
        dumpInstrs("L%d:\n", label_out);
    }
    else if(op == "<>"){//t0 = (t0 <> t1) ? 1 : 0
        int label_out = newlabel();
        int label_true = newlabel();
        dumpInstrs("    bne t0, t1, L%d\n", label_true);
        dumpInstrs("    li t0, 0\n");
        dumpInstrs("    j L%d\n", label_out);
        dumpInstrs("L%d:\n", label_true);
        dumpInstrs("    li t0, 1\n");
        dumpInstrs("L%d:\n", label_out);
    } 
    else if(op == "and"){
        int label_out = newlabel();
        int label_false = newlabel();
        dumpInstrs("    beqz t1, L%d\n", label_false);//LHS is false

        dumpInstrs("    beqz t0, L%d\n", label_false);//RHS is false

        dumpInstrs("    li t0, 1\n");
        dumpInstrs("    j L%d\n", label_out);
//label_false:
        dumpInstrs("L%d:\n", label_false);
        dumpInstrs("    li t0, 0\n");
//label_out:
        dumpInstrs("L%d:\n", label_out);
    }
    else if(op == "or"){
        int label_out = newlabel();
        int label_true = newlabel();
        dumpInstrs("    bneqz t1, L%d\n", label_true);//LHS is true

        dumpInstrs("    bneqz t0, L%d\n", label_true);//RHS is true

        dumpInstrs("    li t0, 0\n");
        dumpInstrs("    j L%d\n", label_out);
//label_true:
        dumpInstrs("L%d:\n", label_true);
        dumpInstrs("    li t0, 1\n");
//label_out:
        dumpInstrs("L%d:\n", label_out);
    }
    dumpInstrs("    addi sp, sp, -4\n");
    dumpInstrs("    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);
    std::string op(p_un_op.getOpCString());
    dumpInstrs("    lw t0, 0(sp)\n");
    dumpInstrs("    addi sp, sp, 4\n");
    if(op == "neg"){
        dumpInstrs("    li t1, 0\n");
        dumpInstrs("    sub t0, t1, t0 \n");
    }
    else if(op == "not"){
        dumpInstrs("    li t1, 1\n");
        dumpInstrs("    sub t0, t1, t0 \n");
    }
    dumpInstrs("    addi sp, sp, -4\n");
    dumpInstrs("    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    dumpInstrs("#Function call start\n");
    p_func_invocation.visitChildNodes(*this);
    int argc = p_func_invocation.get_argument_num();
    int super_8_size = 0;
    if(argc <= 8){
        for(int i = argc-1; i >= 0; i--){
            dumpInstrs("    lw a%d, 0(sp)\n", i);
            dumpInstrs("    addi sp, sp, 4\n");
        }
    }
    else{//TODO
        super_8_size = argc * 4;
    }
    dumpInstrs("    jal ra, %s\n", p_func_invocation.getNameCString());

    if(super_8_size > 0) {
        dumpInstrs("    addi sp, sp, %d\n", super_8_size);
    }
    dumpInstrs("    mv t0, a0\n");
    dumpInstrs("    addi sp, sp, -4\n");
    dumpInstrs("    sw t0, 0(sp)\n");
    dumpInstrs("#Function call end\n");
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    p_variable_ref.visitChildNodes(*this);
    SymbolEntry* Variable_entry = (SymbolEntry*)symbol_manager->lookup(p_variable_ref.getName());
    dumpInstrs("#Variable reference:%s, offset: %d\n", Variable_entry->getNameCString(), Variable_entry->get_offset());
    if(get_is_read() == 1){//write something, load address
        if(Variable_entry->getLevel() == 0){//global
            dumpInstrs("    la t0, %s\n", p_variable_ref.getNameCString());
            dumpInstrs("    addi sp, sp, -4\n");
            dumpInstrs("    sw t0, 0(sp)\n");
        }
        else{//local
            dumpInstrs("    addi t0, s0, %d\n", Variable_entry->get_offset() );
            dumpInstrs("    addi sp, sp, -4\n", p_variable_ref.getNameCString());
            dumpInstrs("    sw t0, 0(sp)\n");
        }
    }
    else{//read something, load value
        if(Variable_entry->getLevel() == 0){//global
            dumpInstrs("    la t0, %s\n", p_variable_ref.getNameCString());
            dumpInstrs("    lw t1, 0(t0)\n");
            dumpInstrs("    mv t0, t1\n");
            dumpInstrs("    addi sp, sp, -4\n");
            dumpInstrs("    sw t0, 0(sp)\n");
        }
        else{//local
            dumpInstrs("    lw t0, %d(s0)\n", Variable_entry->get_offset());
            dumpInstrs("    addi sp, sp, -4 \n");
            dumpInstrs("    sw t0, 0(sp)\n");
        }
    }
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    dumpInstrs("#Assignment start\n");
    set_is_read();
    p_assignment.lvalue->accept(*this);
    reset_is_read();
    p_assignment.expr->accept(*this);

    dumpInstrs("    lw t0, 0(sp)\n");
    dumpInstrs("    addi sp, sp, 4\n");
    dumpInstrs("    lw t1, 0(sp)\n");
    dumpInstrs("    addi sp, sp, 4\n");
    dumpInstrs("    sw t0, 0(t1)\n");
    dumpInstrs("#Assignment end\n");
}

void CodeGenerator::visit(ReadNode &p_read) {
    dumpInstrs("#Read start\n");
    set_is_read();
    p_read.visitChildNodes(*this);
    reset_is_read();
    dumpInstrs("    jal ra, readInt\n");
    dumpInstrs("    lw t0, 0(sp)\n");
    dumpInstrs("    addi sp, sp, 4\n");
    dumpInstrs("    sw a0, 0(t0)\n");
    dumpInstrs("#Read end\n");
}

void CodeGenerator::visit(IfNode &p_if) {  
    int if_label = newlabel();
    int else_label = newlabel();
    int out_label = newlabel();

    p_if.condition->accept(*this);

    dumpInstrs("    lw t0, 0(sp)\n");
    dumpInstrs("    addi sp, sp, 4\n");
    dumpInstrs("    beqz t0, L%d\n", else_label);

    dumpInstrs("#Label for if\n");
    dumpInstrs("L%d:\n", if_label);
    p_if.body->accept(*this);
    dumpInstrs("    j L%d\n", out_label);

    dumpInstrs("#Label for else\n");
    dumpInstrs("L%d:\n", else_label);
    if (p_if.else_body) {
        p_if.else_body->accept(*this);
    }
    dumpInstrs("#Label for out\n");
    dumpInstrs("L%d:\n", out_label);
}

void CodeGenerator::visit(WhileNode &p_while) {
    int loop_label = newlabel();
    int out_label = newlabel();
//loop_label:
    dumpInstrs("L%d:\n", loop_label);
    p_while.condition->accept(*this);
    dumpInstrs("    lw t0, 0(sp)\n");
    dumpInstrs("    addi sp, sp, 4\n");
    dumpInstrs("    beqz t0, L%d\n", out_label);
    p_while.body->accept(*this);
    dumpInstrs("    j L%d\n", loop_label);
//out_label:
    dumpInstrs("L%d:\n", out_label);
}

void CodeGenerator::visit(ForNode &p_for) {
    // Reconstruct the hash table for looking up the symbol entry
    symbol_manager->reconstructHashTableFromSymbolTable(p_for.getSymbolTable());

    SymbolEntry* loop_var_entry = (SymbolEntry *) symbol_manager->lookup(p_for.var_decl->get_first_name_Cstr());
    dumpInstrs("#For start\n");
    dumpInstrs("#For_var name: %s\n", loop_var_entry->getNameCString());
    int loop_label = newlabel();
    int out_label = newlabel();

    p_for.var_decl->accept(*this);
    p_for.initial_statement->accept(*this);

    dumpInstrs("L%d:\n", loop_label);
    p_for.condition->accept(*this);
    dumpInstrs("    lw t0, 0(sp)\n");//load upper bound
    dumpInstrs("    addi sp, sp, 4\n");

    dumpInstrs("    lw t1, %d(s0)\n", loop_var_entry->get_offset());
    dumpInstrs("    bge t1, t0, L%d\n", out_label);

    p_for.body->accept(*this);

    dumpInstrs("    lw t1, %d(s0)\n", loop_var_entry->get_offset());
    dumpInstrs("    addi t1, t1, 1\n");
    dumpInstrs("    sw t1, %d(s0)\n", loop_var_entry->get_offset());
    dumpInstrs("    j L%d\n", loop_label);

    dumpInstrs("#For end\n");
    dumpInstrs("L%d:\n", out_label);
    
    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_for.getSymbolTable());
}

void CodeGenerator::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);
    dumpInstrs("#Return start\n");
    dumpInstrs("    lw t0, 0(sp)\n");
    dumpInstrs("    addi sp, sp, 4\n");
    dumpInstrs("    mv a0, t0\n");
    dumpInstrs("#Return end\n");
}
