%{
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern int32_t line_num;   /* declared in scanner.l */
extern char buffer[512];  /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

extern int yylex(void); 
static void yyerror(const char *msg);
%}

%token ',' ';' ':' '(' ')' '[' ']' 
%token '+' '-' '*' '/'  MOD ASSIGN  '<' LE LESSGREAT GE '>' '=' 
%token AND OR NOT ARRAY BEGINN BOOLEAN DEF DO ELSE END FALSE FOR VAR_INTEGER IF OF PRINT READ REAL VAR_STRING THEN TO TRUE RETURN VAR WHILE 
%token ID INTEGER FLOAT STRING

%left AND OR NOT
%left '<' LE LESSGREAT GE '>' '='  
%left '-'
%left '+' 
%left '/' MOD
%left '*' 

%%
    /*  Program  */

program_name: ID ';' zero_or_more_variable_and_constant_declaration zero_or_more_function_declaration_and_definition compound_statement END

zero_or_more_variable_and_constant_declaration: %empty
    | one_or_more_variable_and_constant_declaration

one_or_more_variable_and_constant_declaration: variable_declaration
    | constant_declaration
    | one_or_more_variable_and_constant_declaration variable_declaration
    | one_or_more_variable_and_constant_declaration constant_declaration

zero_or_more_function_declaration_and_definition: %empty
    | one_or_more_function_declaration_and_definition

one_or_more_function_declaration_and_definition: function_declaration
    | function_definition
    | one_or_more_function_declaration_and_definition function_declaration
    | one_or_more_function_declaration_and_definition function_definition




     /*  Function  */

function_declaration: ID '(' zero_or_more_formal_arguments ')' ':' scalar_type ';'
    | ID '(' zero_or_more_formal_arguments ')' ';'

function_definition:  ID '(' zero_or_more_formal_arguments ')' ':' scalar_type compound_statement END
    | ID '(' zero_or_more_formal_arguments ')'  compound_statement END

zero_or_more_formal_arguments: %empty
    | one_or_more_formal_arguments

one_or_more_formal_arguments: formal_arguments
    | one_or_more_formal_arguments formal_arguments

formal_arguments:  identifier_list ':' scalar_type

identifier_list: ID 
    | ID ',' identifier_list




    /*  Variable    */

variable_declaration: VAR identifier_list ':' scalar_type ';'
    | VAR identifier_list ':' ARRAY integer_constant OF type ';'

type: scalar_type
    | ARRAY integer_constant OF type

scalar_type: VAR_INTEGER
    | REAL
    | VAR_STRING
    | BOOLEAN

constant_declaration: VAR identifier_list ':' literal_constant ';'

integer_constant: INTEGER

literal_constant: INTEGER
    | FLOAT
    | STRING
    | TRUE
    | FALSE




    /*  Statement   */

compound_statement: BEGINN zero_or_more_variable_and_constant_declaration zero_or_more_statements END

zero_or_more_statements:  %empty
    | one_or_more_statements

one_or_more_statements: statements
    | one_or_more_statements statements 

statements: compound_statement
    | simple_statement
    | conditional_statement
    | while_statement
    | for_statement
    | return_statement
    | procedure_call_statement

simple_statement: variable_reference ASSIGN expression ';'
    | PRINT variable_reference ';'
    | PRINT expression ';'
    | READ variable_reference ';'

conditional_statement: IF expression THEN compound_statement ELSE compound_statement END IF
    | IF expression THEN compound_statement END IF

while_statement: WHILE expression DO compound_statement END DO

for_statement: FOR ID ASSIGN integer_constant TO integer_constant DO compound_statement END DO

return_statement: RETURN expression ';'

procedure_call_statement: ID '(' expressions_separated_by_zero_or_more_comma ')' ';'

expressions_separated_by_zero_or_more_comma: %empty
    | expressions_separated_by_one_or_more_comma

expressions_separated_by_one_or_more_comma: expression
    | expression ',' expressions_separated_by_one_or_more_comma

variable_reference: ID
    | ID one_or_more_box

one_or_more_box: '[' expression ']'
    | one_or_more_box '[' expression ']'



    /*  Expression  */

expression: expression AND expression
    | expression OR expression
    | NOT expression
    | expression '<' expression
    | expression LE expression
    | expression LESSGREAT expression
    | expression GE expression
    | expression '>' expression
    | expression '=' expression
    | expression '-' expression
    | expression '+' expression
    | expression '/' expression
    | expression MOD expression
    | expression '*' expression
    | '-' expression %prec '*'
    | literal_constant
    | variable_reference
    | function_invocations
    | '(' expression ')'

function_invocations: ID '(' expressions_separated_by_zero_or_more_comma ')'


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
    if (argc != 2) {
        fprintf(stderr, "Usage: ./parser <filename>\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    assert(yyin != NULL && "fopen() fails.");

    yyparse();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
