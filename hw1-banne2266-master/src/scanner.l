%{
#include <stdint.h>

#define LIST                strCat(yytext)
#define TOKEN(t)            { LIST; if (opt_tok) printf("<%s>\n", #t); }
#define TOKEN_CHAR(t)       { LIST; if (opt_tok) printf("<%c>\n", (t)); }
#define TOKEN_STRING(t, s)  { LIST; if (opt_tok) printf("<%s: %s>\n", #t, (s)); }
#define MAX_LINE_LENG       512

uint32_t line_num = 1;
char buffer[MAX_LINE_LENG];

static int32_t opt_src = 1;
static int32_t opt_tok = 1;
static char *buffer_ptr = buffer;

static void strCat(const char *Text);

%}

digit	[0-9]
non0-digit	[1-9]
letter	[a-zA-Z]

%x COMMENT COMMENT_L COMMENT_P

%%


	/*Tokens That Will Be Passed to the Parser*/
	
	/*Delimiters*/
"," { TOKEN_CHAR(','); }
";" { TOKEN_CHAR(';'); }
":" { TOKEN_CHAR(':'); }
"(" { TOKEN_CHAR('('); }
")" { TOKEN_CHAR(')'); }
"[" { TOKEN_CHAR('['); }
"]" { TOKEN_CHAR(']'); }


	/*Arithmetic, Relational, and Logical Operators*/
"+" { TOKEN_CHAR('+'); }
"-" { TOKEN_CHAR('-'); }
"*" { TOKEN_CHAR('*'); }
"/" { TOKEN_CHAR('/'); }
"mod" { TOKEN(mod); }
":=" { TOKEN(:=); }

"<" { TOKEN(<); }
"<=" { TOKEN(<=); }
"<>" { TOKEN(<>); }
">=" { TOKEN(>=); }
">" { TOKEN(>); }
"=" { TOKEN(=); }

"and" { TOKEN(and); }
"or" { TOKEN(or); }
"not" { TOKEN(not); }


	/*Keywords*/
"array" { TOKEN(KWarray); }
"begin" { TOKEN(KWbegin); }
"boolean" { TOKEN(KWboolean); }
"def" { TOKEN(KWdef); }
"do" { TOKEN(KWdo); }
"else" { TOKEN(KWelse); }
"end" { TOKEN(KWend); }
"false" { TOKEN(KWfalse); }
"for" { TOKEN(KWfor); }
"integer" { TOKEN(KWinteger); }
"if" { TOKEN(KWif); }
"of" { TOKEN(KWof); }
"print" { TOKEN(KWprint); }
"read" { TOKEN(KWread); }
"real" { TOKEN(KWreal); }
"string" { TOKEN(KWstring); }
"then" { TOKEN(KWthen); }
"to" { TOKEN(KWto); }
"true" { TOKEN(KWtrue); }
"return" { TOKEN(KWreturn); }
"var" { TOKEN(KWvar); }
"while" { TOKEN(KWwhile); }


	/*Identifiers*/
{letter}({letter}|{digit})*	{ TOKEN_STRING(id, yytext); }


	/*Integer Constants*/
0[0-7]+	{ TOKEN_STRING(oct_integer, yytext); }
0 |
[1-9][0-9]*		{ TOKEN_STRING(integer, yytext); }


	/*Floating-Point Constants*/
0.0 |
[1-9][0-9]*\.[0-9]*[1-9]	{ TOKEN_STRING(float, yytext); }


	/*Scientific Notations*/
([1-9][0-9]*|[1-9][0-9]*\.[0-9]*)[eE](\+|\-)?([1-9][0-9]*|0)  {
	TOKEN_STRING(scientific, yytext);
}


	/*String Constants*/
\"((\"\")|[^"])*\" {
	LIST;
	char temp[511] = {0};
	int idx = strlen(yytext)-1, i = 1, j = 0;
	for(; i < idx; i++, j++){
		if(yytext[i] == '"' && yytext[i+1] == '"' && yytext[i+2] != 0)
			i++;
		temp[j] = yytext[i];
	}
		
	if(opt_tok) printf("<%s: %s>\n", "string", (temp));
}



	/*Tokens That Will Be Discarded*/
	
	/*Whitespace*/
" " |
\t {LIST; }


	/*Comments*/
<INITIAL>"/*" 	{LIST;	BEGIN COMMENT;}
<COMMENT>\n		{
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}
<COMMENT>.     	LIST;
<COMMENT>"*/"  	{LIST;	BEGIN INITIAL;}

<INITIAL>"//" 	{LIST; 	BEGIN COMMENT_L;}
<COMMENT_L>\n		{
	BEGIN INITIAL;
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}
<COMMENT_L>.	LIST;


	/*Pseudocomments*/
"//&"[ST][+-].*	{
	LIST;
	if(yytext[3] == 'S'){
		if(yytext[4] == '+')
			opt_src = 1;
		else
			opt_src = 0;
	}
	else{
		if(yytext[4] == '+')
			opt_tok = 1;
		else
			opt_tok = 0;
	} 
}


	/*line change*/
\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}


    /* Catch the character which is not accepted by rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}

%%

static void strCat(const char *Text) {
    while (*Text) {
        *buffer_ptr++ = *Text++;
    }
    *buffer_ptr = '\0';
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./scanner [filename]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yylex();

    return 0;
}
