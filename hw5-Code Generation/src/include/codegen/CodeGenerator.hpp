#ifndef __CODE_GENERATOR_H
#define __CODE_GENERATOR_H

#include "sema/SymbolTable.hpp"
#include "visitor/AstNodeVisitor.hpp"

class CodeGenerator : public AstNodeVisitor {
  public:
    CodeGenerator(const char *in_file_name, const char *out_file_name, SymbolManager *symbol_manager);
    ~CodeGenerator();

    void dumpInstrs(const char *format, ...);

    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;

    void function_prologue();
    void function_epilogue(const char*  func_name);

    void offset_add(){offset += 4;};
    void offset_minus(){offset -= 4;};

    void set_in_local(){in_local = 1;};
    void reset_in_local(){in_local = 0;};
    int get_in_local(){return in_local;};

    void set_is_read(){is_read = 1;};
    void reset_is_read(){is_read = 0;};
    int get_is_read(){return is_read;};

    int newlabel(){return label++;};

  private:
    const char *in_file_name;
    FILE *out_fp;
    SymbolManager *symbol_manager;
    int label = 0;
    int offset = 0;
    int in_local = 0;
    int is_read = 0;
};

#endif