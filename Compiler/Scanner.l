/* ====================================================================== */

%{

typedef struct l_type_struct {	/* lex for var and num */
  char *lex;
} l_type;

typedef struct t_type_struct {	/* type for type_specifier */
  unsigned char type;
} t_type;

typedef struct p_type_struct {
  unsigned char ip;
} p_type;
typedef struct r_type_struct {
  unsigned char regi;
} r_type;
/* ====================================================================== */

#include "parser.h"
#include <string.h>

/* ====================================================================== */

#define	ACCEPT(x) return(x)
#define	ACCEPT_LEX(x) \
{ \
  yytext[yyleng] = '\0'; \
  lex = malloc(yyleng + 1); \
  strcpy(lex, yytext); \
  return(x); \
}
#define LINEPLUS(){source_line_no += 1;}
/* ====================================================================== */

char *lex;			/* current lexeme of ID or NUM */
int source_line_no = 1;		/* source line number */

/* ====================================================================== */

%}

digit				[0-9]
letter				[a-zA-Z]
ids                             ([a-zA-Z]|["_"])([a-zA-Z]|[0-9]|["_"])*
ubar                            ("_")
commentsend2                    ([^\n]*"*/")
commentswitchline               ("/*"[^\n]*)
comslash                        ("//"[^\n]*)
space                           (" "+)
tap                             ("\t"|"\r")
undefined                       (["`"]|["~"]|["!"]|["@"]|["#"]|["$"]|["%"]|["^"]|["&"]|["|"]|["\\"]|["."]|[":"]|["'"]|["\""])
nline                           ("\n")

%%

"void"                          ACCEPT_LEX(VOID);
"int"                           ACCEPT_LEX(INT);
"if"                            ACCEPT_LEX(IF);
"else"                          ACCEPT_LEX(ELSE);
"while"                         ACCEPT_LEX(WHILE);
"input"                         ACCEPT_LEX(INPUT);
"output"                        ACCEPT_LEX(OUTPUT);
"return"                        ACCEPT_LEX(RETURN);
"+"                             ACCEPT_LEX(PLUS);
"-"                             ACCEPT_LEX(MINUS);
"*"                             ACCEPT_LEX(MULTIPLY);
"/"                             ACCEPT_LEX(DIVIDE);
"<"                             ACCEPT_LEX(LT);
"<="                            ACCEPT_LEX(LE);
"=<"				ACCEPT_LEX(LE);
">"                             ACCEPT_LEX(GT);
">="                            ACCEPT_LEX(GE);
"=>"				ACCEPT_LEX(GE);
"=="                            ACCEPT_LEX(EQ);
"!="                            ACCEPT_LEX(NE);
"="                             ACCEPT_LEX(ASSIGN);
","                             ACCEPT_LEX(COMMA);
";"                             ACCEPT_LEX(SEMICOLON);
"("                             ACCEPT_LEX(LPAR);
")"                             ACCEPT_LEX(RPAR);
"{"                             ACCEPT_LEX(LBRACE);
"}"                             ACCEPT_LEX(RBRACE);
"["                             ACCEPT_LEX(LBRACKET);
"]"                             ACCEPT_LEX(RBRACKET);
"\""                            ACCEPT_LEX(UNDEFINED);
{ids}                           ACCEPT_LEX(ID);
{digit}{digit}*                 ACCEPT_LEX(NUM);
{commentswitchline}             {}
{commentsend2}                  {}
{comslash}                      {}
{space}                         {}
{undefined}                     ACCEPT(UNDEFINED);
{nline}                         {LINEPLUS();}

%%

/* ====================================================================== */
