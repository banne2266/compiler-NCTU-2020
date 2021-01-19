%{
#include "AST/ast.hpp"
#include "AST/program.hpp"
#include "AST/decl.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/print.hpp"
#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/read.hpp"
#include "AST/if.hpp"
#include "AST/while.hpp"
#include "AST/for.hpp"
#include "AST/return.hpp"
#include "AST/AstDumper.hpp"

#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
using namespace std;

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

/* Declared by scanner.l */
extern uint32_t line_num;
extern char buffer[512];
extern FILE *yyin;
extern char *yytext;
/* End */

static AstNode *root;

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);

%}

%code requires {
    #include <vector>
    #include "AST/ast.hpp"
    #include "AST/variable.hpp"
    #include "AST/decl.hpp"
    class vector;
    class AstNode;
    class DeclNode;
    class VariableNode;
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *identifier;
    int  int_val;
    double float_val;
    bool bolean;

    struct array_part *array_p;
    AstNode *node;
    std::vector<AstNode *> *list;

    DeclNode *node_func;
    std::vector<DeclNode *> *list_func;

    struct id_list  *id_list_;
    
    struct const_value *c_val;
};

%type <identifier> ProgramName ID FunctionName STRING_LITERAL TRUE FALSE
%type <int_val> INT_LITERAL
%type <float_val> REAL_LITERAL
%type <bolean> NegOrNot

%type <list>  DeclarationList Declarations FunctionList Functions
%type <list>  ExpressionList Expressions StatementList Statements 
%type <list>  ArrRefs ArrRefList 

%type <id_list_>  IdList
%type <node_func> FormalArg
%type <list_func>  FormalArgs FormalArgList
%type <array_p> ReturnType Type ScalarType ArrType ArrDecl
%type <c_val>  LiteralConstant  StringAndBoolean IntegerAndReal
%type <node>  Function FunctionDeclaration FunctionDefinition
%type <node>  Declaration 
%type <node> Statement CompoundStatement Simple Condition ElseOrNot While For Return FunctionCall FunctionInvocation
%type <node> Expression VariableReference

    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
%left OR
%left AND
%right NOT
%left LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UNARY_MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN_ /* Use BEGIN_ since BEGIN is a keyword in lex */
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token FALSE TRUE
%token PRINT READ

    /* Identifier */
%token ID

    /* Literal */
%token INT_LITERAL
%token REAL_LITERAL
%token STRING_LITERAL

%%
    /*
       Program Units
                     */

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
        root = new ProgramNode(@1.first_line, @1.first_column,
                               $1,  "void",$3, $4, $5);

        free($1);
    }
;

ProgramName:
    ID {$$ = $1;}
;

DeclarationList:
    Epsilon {$$ = NULL;}
    |
    Declarations {$$ = $1;}
;

Declarations:
    Declaration {
        $$ = new std::vector<AstNode *>;
        $$->push_back($1);
    }
    |
    Declarations Declaration{
        $$ = $1;
        $$->push_back($2);
    }
;

FunctionList:
    Epsilon {$$ = NULL;}
    |
    Functions  {$$ = $1;}
;

Functions:
    Function {
        $$ = new std::vector<AstNode *>;
        $$->push_back($1);
    }
    |
    Functions Function{
        $$ = $1;
        $$->push_back($2);
    }
;

Function:
    FunctionDeclaration {$$ = $1;}
    |
    FunctionDefinition {$$ = $1;}
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON{
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, &($5->type), NULL);
    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END{
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, &($5->type), $6);
    }
;

FunctionName:
    ID {$$ = $1;}
;

FormalArgList:
    Epsilon {$$ = NULL;}
    |
    FormalArgs {$$ = $1;}
;

FormalArgs:
    FormalArg{
         $$ = new std::vector<DeclNode  *>;
         $$->push_back($1);
    }
    |
    FormalArgs SEMICOLON FormalArg{
        $$ = $1;
        $$->push_back($3);
    }
;

FormalArg:
    IdList COLON Type {
        $$ = new DeclNode(@1.first_line, @1.first_column, $1, $3);
    }
;

IdList:
    ID{
        $$ = new struct id_list;
        $$->name_list.push_back($1);
        $$->col_list.push_back(@1.first_column);
    }
    |
    IdList COMMA ID{
        $$ = $1;
        $$->name_list.push_back($3);
        $$->col_list.push_back(@3.first_column);
    }
;

ReturnType:
    COLON ScalarType {$$ = $2;}
    |
    Epsilon{
        $$ = new struct array_part;
        $$->type = void_;
    }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR  IdList COLON Type SEMICOLON {
        $$ = new DeclNode(@1.first_line, @1.first_column, $2, $4);
    }
    |
    VAR IdList COLON LiteralConstant SEMICOLON{
        $$ = new DeclNode(@1.first_line, @1.first_column, $2, $4);
    }
;

Type:
    ScalarType {
        $$ = $1;
        }
    |
    ArrType {$$ = $1;}
;

ScalarType:
    INTEGER {
        $$ = new struct array_part;
        $$->type = int_;
        }
    |
    REAL {
        $$ = new struct array_part;
        $$->type = real_;
        }
    |
    STRING {
        $$ = new struct array_part;
        $$->type = char_;
        }
    |
    BOOLEAN {
        $$ = new struct array_part;
        $$->type = boolean_; 
        }
;

ArrType:
    ArrDecl ScalarType {
        $$ = $1;
        $$->type = $2->type;
    }
;

ArrDecl:
    ARRAY INT_LITERAL OF{
        $$ = new struct array_part;
        $$->structed_part_list.push_back($2);
    }
    |
    ArrDecl ARRAY INT_LITERAL OF{
        $$ = $1;
        $$->structed_part_list.push_back($3);
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL {
            $$ = new struct const_value;
            $$->type = int_;
            $$->value = $2;
            $$->col = @2.first_column;
            if($1 == true){
                $$->value = -$2;
                $$->col = @1.first_column;
            }
        }
    |
    NegOrNot REAL_LITERAL {
            $$ = new struct const_value;
            $$->type = real_;
            $$->real_value = $2;
            $$->col = @2.first_column;
            if($1 == true){
                $$->real_value = -$2;
                $$->col = @1.first_column;
        }
        }
    |
    StringAndBoolean {$$ = $1;}
;

NegOrNot:
    Epsilon {$$ = false;}
    |
    MINUS %prec UNARY_MINUS {$$ = true;}
;

StringAndBoolean:
    STRING_LITERAL  {
        $$ = new struct const_value;
        $$->type = char_;
        $$->string_value = $1;
        $$->col = @1.first_column;
        }
    |
    TRUE  {
        $$ = new struct const_value;
        $$->type = boolean_;
        $$->boolean_value = true;
        $$->col = @1.first_column;
        }
    |
    FALSE  {
        $$ = new struct const_value;
        $$->type = boolean_;
        $$->boolean_value = false;
        $$->col = @1.first_column;
        }
;

IntegerAndReal:
    INT_LITERAL{
        $$ = new struct const_value;
        $$->type = int_;
        $$->value = $1;
        }
    |
    REAL_LITERAL{
        $$ = new struct const_value;
        $$->type = real_;
        $$->real_value = $1;
        }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement {$$ = $1;}
    |
    Simple {$$ = $1;}
    |
    Condition {$$ = $1;}
    |
    While {$$ = $1;}
    |
    For {$$ = $1;}
    |
    Return {$$ = $1;}
    |
    FunctionCall {$$ = $1;}
;

CompoundStatement:
    BEGIN_
    DeclarationList
    StatementList
    END{
        $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3);
    }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON{
        $$ = new AssignmentNode(@2.first_line, @2.first_column, $1, $3);
    }
    |
    PRINT Expression SEMICOLON{
        $$ = new PrintNode(@1.first_line, @1.first_column, $2);
    }
    |
    READ VariableReference SEMICOLON{
        $$ = new ReadNode(@1.first_line, @1.first_column, $2);
    }
;

VariableReference:
    ID ArrRefList{
        $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2);
    }
;

ArrRefList:
    Epsilon {$$ = NULL;}
    |
    ArrRefs {$$ = $1;}
;

ArrRefs:
    L_BRACKET Expression R_BRACKET {
        $$ = new std::vector<AstNode *>;
        $$->push_back($2);
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET {
        $$ = $1;
        $$->push_back($3);
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF{
        $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, $5);
    }
;

ElseOrNot:
    ELSE
    CompoundStatement {$$ = $2;}
    |
    Epsilon {$$ = NULL;}
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO{
        $$ = new WhileNode(@1.first_line, @1.first_column, $2, $4);
    }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO{
        struct id_list *id = new (struct id_list);
        id->name_list.push_back($2);
        id->col_list.push_back(@2.first_column);
        struct array_part *type = new struct array_part;
        type->type = int_;
        AstNode *decl = new DeclNode(@2.first_line, @2.first_column, id, type);

        struct const_value *c_value1 = new struct const_value;
        c_value1->value = $4;
        c_value1->type = int_;
        AstNode *expre_0 =  new  ConstantValueNode(@4.first_line, @4.first_column, c_value1);
        AstNode *variable_0 = new VariableReferenceNode(@2.first_line, @2.first_column, $2, NULL);
        
        AstNode *assign = new  AssignmentNode(@3.first_line, @3.first_column, variable_0, expre_0);

        struct const_value *c_value2 = new struct const_value;
        c_value2->value = $6;
        c_value2->type = int_;
        AstNode *expre = new  ConstantValueNode(@6.first_line, @6.first_column, c_value2);

        
        $$ = new ForNode(@1.first_line, @1.first_column, decl, assign, expre, $8);
    }
;

Return:
    RETURN Expression SEMICOLON{
        $$ = new ReturnNode(@1.first_line, @1.first_column, $2);
    }
;

FunctionCall:
    FunctionInvocation SEMICOLON    {$$ = $1;}
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS{
        $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3);
    }
;

ExpressionList:
    Epsilon {$$ = NULL;}
    |
    Expressions {$$ = $1;}
;

Expressions:
    Expression {
        $$ = new std::vector<AstNode *>;
        $$->push_back($1);
    }
    |
    Expressions COMMA Expression{
        $$ = $1;
        $$->push_back($3);
    }
;

StatementList:
    Epsilon {$$ = NULL;}
    |
    Statements  {$$ = $1;}
;

Statements:
    Statement{
        $$ = new  std::vector<AstNode *>;
        $$->push_back($1);
    }
    |
    Statements Statement{
        $$ = $1;
        $$->push_back($2);
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS  {$$ = $2;}
    |
    MINUS Expression %prec UNARY_MINUS { $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "neg", $2); }
    |
    Expression MULTIPLY Expression  { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "*", $1, $3); }
    |
    Expression DIVIDE Expression    { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "/", $1, $3); }
    |
    Expression MOD Expression   { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "mod", $1, $3); }
    |
    Expression PLUS Expression  { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "+", $1, $3); }
    |
    Expression MINUS Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "-", $1, $3); }
    |
    Expression LESS Expression  { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<", $1, $3); }
    |
    Expression LESS_OR_EQUAL Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<=", $1, $3); }
    |
    Expression GREATER Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, ">", $1, $3); }
    |
    Expression GREATER_OR_EQUAL Expression  { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, ">=", $1, $3); }
    |
    Expression EQUAL Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "=", $1, $3); }
    |
    Expression NOT_EQUAL Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<>", $1, $3); }
    |
    NOT Expression  { $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "not", $2); }
    |
    Expression AND Expression   { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "and", $1, $3); }
    |
    Expression OR Expression    { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "or", $1, $3); }
    |
    IntegerAndReal  {$$ = new ConstantValueNode(@1.first_line, @1.first_column, $1);}
    |
    StringAndBoolean  {$$ = new ConstantValueNode(@1.first_line, @1.first_column, $1);  free($1);}
    |
    VariableReference {$$ = $1;}
    |
    FunctionInvocation {$$ = $1;}
;

    /*
       misc
            */
Epsilon:
;
%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, buffer, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./parser <filename> [--dump-ast]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    assert(yyin != NULL && "fopen() fails.");
    
    yyparse();
    
    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        AstDumper *p_visitor = new AstDumper;
        ProgramNode *PNode = (ProgramNode*)root;
        p_visitor->visit(*PNode);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
