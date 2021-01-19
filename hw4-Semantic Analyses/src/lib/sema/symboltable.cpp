#include "sema/symboltable.hpp"

SymbolEntry::SymbolEntry(std::string p_name, int p_kind, int p_level, PType p_type, std::string p_attribute, int p_pass)
        :name(p_name), kind(p_kind), level(p_level), type(p_type), attribute(p_attribute), pass(p_pass){}


SymbolTable::SymbolTable(int p_kind) : kind(p_kind){}

void SymbolTable::addSymbol(std::string p_name, int p_kind, int p_level, PType p_type, std::string p_attribute, int pass)
{
        SymbolEntry newSymbolEntry(p_name, p_kind, p_level, p_type, p_attribute, pass);
        entries.push_back(newSymbolEntry);
}

int SymbolTable::lookup(std::string name)
{
        for(int i = 0; i < entries.size(); i++){
                if(name == entries[i].name){
                        return entries[i].kind;
                }
        }
        return ENTRY_KIND_NO_FOUND;
}

SymbolEntry SymbolTable::get_symbol_entry(std::string name)
{
        for(int i = 0; i < entries.size(); i++){
                if(name == entries[i].name){
                        return entries[i];
                }
        }
}

void SymbolTable::dumpDemarcation(const char chr)
{
        for (size_t i = 0; i < 110; ++i) {
                printf("%c", chr);
        }
        puts("");
}

void SymbolTable::print_table()
{
        char * kind_str[]= {"program", "function", "parameter", "variable", "loop_var", "constant"};
        dumpDemarcation('=');
        printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type",
                                        "Attribute");
        dumpDemarcation('-');
        for(int i = 0; i < entries.size(); i++){
                printf("%-33s", entries[i].name.c_str());
                printf("%-11s", kind_str[entries[i].kind]);
                printf("%d", entries[i].level);
                if(entries[i].level == 0)
                        printf("%-10s", "(global)");
                else
                        printf("%-10s", "(local)");
                printf("%-17s", entries[i].type.getPTypeCString());
                printf("%-11s", entries[i].attribute.c_str());
                puts("");
        }
        dumpDemarcation('-');
}


void SymbolManager::pushScope(SymbolTable *new_scope)
{
        tables.push_back(new_scope);
}

void SymbolManager::popScope(int op_print)
{
        SymbolTable* to_free = tables.back();
        if(op_print)
                tables.back()->print_table();
        tables.pop_back();
        free(to_free);
}

SymbolEntry SymbolManager::get_symbol_entry(std::string name){
        for(int i = tables.size() - 1; i >= 0; i--){
                if(tables[i]->lookup(name) != ENTRY_KIND_NO_FOUND){
                        return tables[i]->get_symbol_entry(name);
                }
        }
}

void SymbolManager::addSymbol(std::string p_name, int p_kind, PType p_type, std::string p_attribute, int pass)
{
        tables.back()->addSymbol(p_name, p_kind, tables.size()-1, p_type, p_attribute, pass);
}

int SymbolManager::lookup(std::string name, bool global)
{
        if(global){
                for(int i = tables.size() - 1; i >= 0; i--){
                        if(tables[i]->lookup(name) != ENTRY_KIND_NO_FOUND){
                                return tables[i]->lookup(name);
                        }
                }
        }
        else{
                return tables.back()->lookup(name);
        }
        return ENTRY_KIND_NO_FOUND;
}

bool SymbolManager::lookup_for(std::string name)
{
        for(int i = tables.size() - 1; i >= 0; i--){
                if(tables[i]->get_kind() != 3)
                        continue;
                if(tables[i]->lookup(name) != ENTRY_KIND_NO_FOUND)
                        return true;
        }
        return false;
}



FunctionEntry::FunctionEntry(std::string name_n, std::vector<PType> paremeter_type_p, PType return_type_p)
                :name(name_n), paremeter_type(paremeter_type_p), return_type(return_type_p){}


void FunctionManager::addFunction(std::string name_n, std::vector<PType> paremeter_type_p, PType return_type_p)
{
        FunctionEntry new_entry(name_n, paremeter_type_p, return_type_p);
        table.push_back(new_entry);
}

FunctionEntry FunctionManager::lookup(std::string name)
{
        for(int i = 0; i < table.size(); i++){
                if(table[i].name == name)
                        return table[i];
        }
}


