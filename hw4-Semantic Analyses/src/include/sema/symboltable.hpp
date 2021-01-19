#include<vector>
#include<string>
#include"AST/PType.hpp"
#include"AST/constant.hpp"
#define TABLE_KIND_PROGRAM 1
#define TABLE_KIND_FUNCTION 2
#define TABLE_KIND_FORLOOP 3
#define TABLE_KIND_REST 0

#define ENTRY_KIND_PROGRAM 0
#define ENTRY_KIND_FUNCTION 1
#define ENTRY_KIND_PARAMETER 2
#define ENTRY_KIND_VARIABLE 3
#define ENTRY_KIND_LOOP_VAR 4
#define ENTRY_KIND_CONSTANT 5
#define ENTRY_KIND_NO_FOUND 6


class SymbolEntry {
public:
    SymbolEntry(std::string p_name, int p_kind, int p_level, PType p_type, std::string p_attribute, int pass);
    char* get_kind_cstr();
public:
    std::string name;
    int kind;
    int level;
    PType type;
    std::string attribute;
    int pass;
};

class SymbolTable {
public:
    SymbolTable(int p_kind);
    void addSymbol(std::string p_name, int p_kind, int p_level, PType p_type, std::string p_attribute, int pass);
    int lookup(std::string name);
    int get_kind(){return kind;};
    SymbolEntry get_symbol_entry(std::string name);
    void change_state(int n){kind = n;};
    void print_table();
    void dumpDemarcation(const char chr);
private:
    std::vector<SymbolEntry> entries;
    int kind;//1 for program, 2 for function, 3 for forloop, 0 for rest
    int dummy;
};

class SymbolManager {
public:
    void addSymbol(std::string p_name, int p_kind, PType p_type, std::string p_attribute, int pass);
    int lookup(std::string name, bool global);//could loop up in global/local scope.
    bool lookup_for(std::string name);
    int get_top_kind(){return tables.back()->get_kind();};
    SymbolEntry get_symbol_entry(std::string name);
    void change_state(int n){tables.back()->change_state(n);};
    void pushScope(SymbolTable *new_scope);
    void popScope(int op_print);
private:
    std::vector <SymbolTable *> tables;
};


class FunctionEntry{
public:
    FunctionEntry(std::string name_n, std::vector<PType> paremeter_type_p, PType return_type_p);
public:
    std::string name;
    std::vector<PType> paremeter_type;
    PType return_type;
};

class FunctionManager{
public:
    FunctionManager() {func_num = 0;};

    void addFunction(std::string name_n, std::vector<PType> paremeter_type_p, PType return_type_p);
    FunctionEntry lookup(std::string name);
    FunctionEntry lookup(int n) {return table[n];};
    void add_num(){func_num++;};
    void minus_num(){func_num--;};
    int get_func_num(){return func_num;};
public:
    std::vector<FunctionEntry> table;
    int func_num;
};
