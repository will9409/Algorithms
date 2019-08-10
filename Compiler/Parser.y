/* ====================================================================== */
%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "symbol.c"

/* ====================================================================== */

#define MAX_NO_CODES	1024
#define MAX_LINE	256
#define NO_TMP_REGIS	27
#define REGI_RETURN	27

/* ===================================================================== */

extern FILE *yyin;		/* FILE * for input file */
extern char *yytext;		/* current lexeme is stored here */

extern char *lex;		/* lexeme of ID and NUM from scanner */
extern int source_line_no;	/* souce line number */

/* ====================================================================== */

int position = 0;			/* current symbol's kind */
char fno[5];
int global_offset;		/* global variable offset */
int local_offset;		/* local variable offset */
int *current_offset;		/* current offset pointer */
int temp;
int farg_count;			/* no of formal args in function declaration */
int aarg_count;			/* no of actual args in function call */

/* ====================================================================== */

int error(char *fmt, char *s1, char *s2, char *s3, char *s4, char *s5);
int yyerror(char *message);
int yylex(void);
void regi_init(void);
void regi_print(void);
void regi_free(int i);
int regi_new(void);
void backpatch(unsigned int ip1, unsigned int ip2);
void backpatch_funcalls(void);
void print_code(void);
int generate(char *fmt, int i1, int i2, int i3, int i4);

/* ====================================================================== */

char *prog_name;		/* program file name from argv[] */
char option_symbol;		/* -s option means print symbol table*/
int mainvar;
int position;			/* current symbol's kind*/
int arraytype;
int global_offset;		/* global variable offset */
int local_offset;		/* local variable offset */
int *current_offset;		/* current offset pointer */

int farg_count;			/* no of formal args in function declaration*/
int aarg_count;			/* no of actual args in function call */

char *current_fun_name = 0;	/* current function name */

unsigned int ip = 0;		/* instruction pointer */

char *code[MAX_NO_CODES];	/* generated codes */
char regi_used[NO_TMP_REGIS];	/* 1 if register is used */
/* ====================================================================== */

typedef struct l_type_struct {	/* lex attribute for var and num */
  char *lex;
} l_type;

typedef struct p_type_struct {
  unsigned char ip;
} p_type;

typedef struct t_type_struct {	/* type attribute for type_specifier */
  unsigned char type;
} t_type;

typedef struct r_type_struct {
  unsigned char regi;
} r_type;
%}

/* ====================================================================== */

%start program

%union {
  l_type lval;
  t_type tval;
  r_type rval;
  p_type pval;
}

%token VOID INT
%token IF ELSE WHILE RETURN
%token INPUT OUTPUT
%token PLUS MINUS MULTIPLY DIVIDE
%token LT LE GT GE EQ NE
%token ASSIGN
%token SEMICOLON COMMA
%token LPAR RPAR LBRACKET RBRACKET LBRACE RBRACE
%token ID NUM
%token UNDEFINED

%type <lval> var num
%type <tval> type_specifier 
%type <rval> simple_expression additive_expression term factor call /* reg */
%type <pval> if_rpar if_else while_lpar while_rpar /* branch code */

%%

/* ====================================================================== */

program : 
  {
    struct symbol *symbolp;
    position=GLOBAL;
    current_table=global_table=create_table("_global");
    current_offset=&global_offset;
    *current_offset=0;
    local_table=create_table(""); //local table 탐색 segmentation fault 방지
    fprintf(stdout,
        "---------- ---------- ---------- ---------- ---------- ----------\n");
    fprintf(stdout,
        "table      symbol     kind       type             size     offset\n");
    fprintf(stdout,
        "---------- ---------- ---------- ---------- ---------- ----------\n");
    generate("%d: ld gp, 0(0)",ip++,0,0,0);
    generate("%d: st 0, 0(0)",ip++,0,0,0);
    generate("%d: lda fp, -%%d(gp)",ip++,0,0,0);
    generate("%d: lda sp, -%%d(gp)",ip++,0,0,0);
    generate("%d: push fp",ip++,0,0,0);
    generate("%d: lda 0, 2(pc)",ip++,0,0,0);
    generate("%d: push 0",ip++,0,0,0);
    symbolp = add_symbol(global_table, "main", FUNCTIONI, VOID_TYPE, 0 , 0);
    generate("%d: ldc pc, %%d", ip++,0,0,0);
    symbolp->ip[0] = ip - 1;
    symbolp->ipc = 1;
    generate("%d: halt",ip++,0,0,0);  
  }
var_declaration_list fun_declaration_list
  {                                         /*     */
    backpatch(2, global_offset);
    backpatch(3, global_offset);
    backpatch_funcalls();
    struct symbol *symbolp;
    symbolp = lookup_symbol("main");
    backpatch(7,symbolp->offset);
    print_table(global_table);
    fprintf(stdout,
	"---------- ---------- ---------- ---------- ---------- ----------\n");
    print_code();
  }
;

var_declaration_list
  : var_declaration_list var_declaration
  | empty
;

fun_declaration_list
  : fun_declaration_list fun_declaration
  | fun_declaration
;

var_declaration : type_specifier var SEMICOLON
  {
   if($<tval>1.type == VOID_TYPE){
     error("error %s: %s %s%s%s",fno ,"wrong void","variable \"" ,$<lval>2.lex,"\"");
   }   
   else if(($<tval>1.type != VOID_TYPE) && ($<tval>1.type != INT_TYPE)){
     error("error %s: %s %s%s%s",fno ,"type error","variable \"" ,$<lval>2.lex,"\"");
   }
   else {
     struct symbol *symbolp;
     symbolp = lookup_symbol($<lval>2.lex);
     if(symbolp == NULL ){
       add_symbol(current_table, $<lval>2.lex, position, $<tval>1.type,1, // 일반변수심볼
       *current_offset);
       *current_offset = *current_offset + 1;
     }
     else if(symbolp->type != $<tval>1.type){
       add_symbol(current_table, $<lval>2.lex, position, $<tval>1.type,1, // 일반변수심볼
       *current_offset);
       *current_offset = *current_offset + 1;
     }
     else{
      error("error %s: %s %s%s%s",fno ,"redefined","variable \"" ,$<lval>2.lex,"\"");}
  }
  }
  | type_specifier var LBRACKET num RBRACKET SEMICOLON
  {
    if($<tval>1.type == VOID_TYPE){
      error("error %s: %s %s%s%s",fno ,"wrong void","array \"" ,$<lval>2.lex,"\"");
    }   
    else if($<tval>1.type != VOID_TYPE && $<tval>1.type != INT_TYPE){
      error("error %s: %s %s%s%s",fno ,"type error","array \"" ,$<lval>2.lex,"\"");
    } else {
        struct symbol *symbolp;
        symbolp = lookup_symbol($<lval>2.lex);
        if(symbolp == NULL){
          int n = atoi($<lval>4.lex);
          add_symbol(current_table, $<lval>2.lex, position, INT_ARRAY_TYPE,
          n, *current_offset + n - 1); // 배열변수심볼입력. offset에 주의
          *current_offset = *current_offset + n;
        } else if(symbolp->type != $<tval>1.type ){
            int n = atoi($<lval>4.lex);
            add_symbol(current_table, $<lval>2.lex, position, INT_ARRAY_TYPE,
            n, *current_offset + n - 1); // 배열변수심볼입력. offset에 주의
            *current_offset = *current_offset + n;
        } else{
            error("error %s: %s %s%s%s",fno ,"redefined","array \"" ,$<lval>2.lex,"\"");
        }
     }
  }
;

type_specifier
  : INT
  {
    $<tval>$.type=INT_TYPE;
  }
  | VOID
  {
    $<tval>$.type=VOID_TYPE;
  }
;

var
  : ID
  {
    $<lval>$.lex=lex;
  }
;

num
  : NUM
  {
    $<lval>$.lex = lex;
  }
;


fun_declaration  
  : type_specifier var 
  {  
    if(($<tval>1.type != VOID_TYPE) &&( $<tval>1.type != INT_TYPE)){
      error("error %s: %s %s%s%s",fno ,"type error","function \"" ,$<lval>2.lex,"\"");
    }
    farg_count = 0;
    position = ARGUMENT;
    current_table= local_table = create_table($<lval>2.lex); // 지역테이블을 생성
    current_offset = &local_offset;
    *current_offset = 0;
  }
    LPAR params RPAR
  {
    struct symbol *symbolp;
    symbolp = lookup_symbol($<lval>2.lex);
    if(symbolp == NULL){
      add_symbol(global_table, $<lval>2.lex, FUNCTION, $<tval>1.type, farg_count, ip); //함수심볼->전역테이블입력
    } else if ((strcmp(symbolp->name,"main") == 0) &&( symbolp->kind == FUNCTIONI)) {
        symbolp->kind = FUNCTION;
        symbolp->type = $<tval>1.type;
        symbolp->size = farg_count;
        symbolp->offset = ip;
    } else{
      error("error %s: %s %s%s%s",fno,"redefined","function \"",$<lval>2.lex,"\"");
      }
    generate("%d: lda sp, -%%d(sp)",ip++,0,0,0);
    temp = ip -1;
    position=LOCAL; 
  }
    LBRACE local_declarations statement_list 
  {
    struct symbol *symbolp;
    symbolp = lookup_symbol($<lval>2.lex);
    if((strcmp(symbolp->name,"main") == 0) && mainvar >= 1){
      mainvar = 1;
    } else if((strcmp(symbolp->name,"main") != 0) && mainvar >= 1){
        mainvar = 2;
      }
    else
      mainvar = 0;
  }
    RBRACE
  {
    print_table(current_table);  // 지역 테이블을 출력함
    fprintf(stdout,
	"---------- ---------- ---------- ---------- ---------- ----------\n");
    struct symbol *symbolp;
    symbolp = lookup_symbol($<lval>2.lex);
    backpatch(temp,*current_offset - farg_count);
    if((strcmp(symbolp->name,"main")==0) && mainvar == 0 ){
      generate("%d: ldc %d, 0", ip++, REGI_RETURN,0,0);
      generate("%d: lda sp, 0(fp)", ip++,0,0,0);
      generate("%d: ld fp, 0(fp)", ip++,0,0,0);
      generate("%d: ld pc, -1(sp)", ip++,0,0,0);
    } else if((strcmp(symbolp->name,"main") != 0) && mainvar == 0){
      generate("%d: ldc %d, 0", ip++, REGI_RETURN,0,0);
      generate("%d: lda sp, 0(fp)", ip++,0,0,0);
      generate("%d: ld fp, 0(fp)", ip++,0,0,0);
      generate("%d: ld pc, -1(sp)", ip++,0,0,0);
    }
    mainvar = 0;
  }
;

params
  : param_list
  | VOID
;

param_list
  : param_list COMMA param
  {
    farg_count++; // 형식인수의 개수를 계산하는변수
  }
  | param
  {
    farg_count = 1;
  }
;

param : type_specifier var
  {
     if($<tval>1.type == VOID_TYPE){
       error("error %s: %s %s%s%s",fno ,"wrong void","argument \"" ,$<lval>2.lex,"\"");
     } else{
         struct symbol *symbolp;
         symbolp = lookup_symbol($<lval>2.lex);
         if(symbolp == NULL){
           add_symbol(current_table, $<lval>2.lex, position, $<tval>1.type, 1, *current_offset);
           *current_offset = *current_offset + 1;
         } else{
             error("error %s: %s %s%s%s",fno ,"redefined","argument \"" ,$<lval>2.lex,"\"");
         }
     }
  }
  | type_specifier var LBRACKET RBRACKET
  {                  // 배열변수심볼입력
    if($<tval>1.type == VOID_TYPE){
      error("error %s: %s %s%s%s",fno ,"wrong void","array argument \"" ,$<lval>2.lex,"\"");
    } else{
        struct symbol *symbolp;
        symbolp = lookup_symbol($<lval>2.lex);
        if(symbolp == NULL){
          add_symbol(current_table, $<lval>2.lex, position, INT_P_TYPE, 1, *current_offset);
          *current_offset = *current_offset + 1;
        } else{
            error("error %s: %s %s%s%s",fno ,"redefined","array argument \"" ,$<lval>2.lex,"\"");
        }
    }
  }
;

local_declarations
  : local_declarations var_declaration
  | empty
;

statement_list
  : statement_list statement
  | empty
;

statement
  : compound_stmt
  | expression_stmt
  | selection_stmt
  | iteration_stmt
  | funcall_stmt
  | return_stmt
  | input_stmt
  | output_stmt
;

compound_stmt
  : LBRACE statement_list RBRACE
;

expression_stmt
  : expression SEMICOLON
  | SEMICOLON
;

expression 
  : var ASSIGN expression
  {
    struct symbol *symbolp;
    symbolp = lookup_symbol($<lval>1.lex);
      if(symbolp == NULL){
        error("error %s: %s %s%s%s", fno,"undefined", "variable \"",$<lval>1.lex,"\"");
      } else if(symbolp->kind == FUNCTION) {
          error("error %s: %s %s%s%s", fno,"type error", "variable \"",$<lval>1.lex,"\"");
      }
    if(symbolp->kind == GLOBAL) {  /* var's kind == GLOBAL */
      int offset = symbolp->offset;
      generate("%d: st %d, -%d(gp)", ip++, $<rval>3.regi, offset,0);
      $<rval>$.regi = $<rval>3.regi;
      regi_free($<rval>3.regi);
    } else {    /* var's kind != GLOBAL */
        int offset = symbolp->offset;
        generate("%d: st %d, -%d(fp)", ip++, $<rval>3.regi, offset + 2,0);
        $<rval>$.regi = $<rval>3.regi;
        regi_free($<rval>3.regi);
    }

  }
  | var LBRACKET expression RBRACKET ASSIGN expression
  {
    struct symbol *symbolp;
    symbolp = lookup_symbol($<lval>1.lex);
      if(symbolp == NULL){
        error("error %s: %s %s%s%s", fno,"undefined", "array \"",$<lval>1.lex,"\"");
      } else if(symbolp->kind == FUNCTION || symbolp->kind == FUNCTIONI) {
          error("error %s: %s %s%s%s", fno,"type error", "array \"",$<lval>1.lex,"\"");
      }

    if(symbolp->kind == GLOBAL) {
      int regi1 = regi_new();
      int offset = symbolp->offset;
      generate("%d: add %d, gp, %d", ip++,regi1,$<rval>3.regi,0); 
      generate("%d: st %d, -%d(%d)", ip++,$<rval>6.regi, offset ,regi1);
      $<rval>$.regi = $<rval>3.regi;
      regi_free(regi1);
      regi_free($<rval>3.regi);
      regi_free($<rval>6.regi);
    } else {
        if(arraytype >= 1 || symbolp->kind == ARGUMENT){
          int regi1 = regi_new();
          int regi2 = regi_new();
          int offset = symbolp->offset;
          generate("%d: ld %d, -%d(fp)", ip++, regi1,offset+2,0);
          generate("%d: add %d, %d, %d", ip++, regi2 , regi1, $<rval>3.regi); 
          generate("%d: st %d, %d(%d)", ip++, $<rval>6.regi, $<rval>3.regi ,regi2);
          $<rval>$.regi = $<rval>3.regi;
          regi_free(regi1);
          regi_free(regi2);
          regi_free($<rval>3.regi);
          regi_free($<rval>6.regi);
        } else {
          int regi1 = regi_new();
          int offset = symbolp->offset;
          generate("%d: add %d, fp, %d", ip++, regi1 ,$<rval>3.regi,0);
          generate("%d: st %d, -%d(%d)", ip++, $<rval>6.regi, offset+2 ,regi1);
          $<rval>$.regi = $<rval>3.regi;
          regi_free(regi1);
          regi_free($<rval>3.regi);
          regi_free($<rval>6.regi);
        }
  
   }
    arraytype = 0;
  }
  | simple_expression
;

simple_expression
  : additive_expression LT additive_expression
  {
    int regi = regi_new();
    generate("%d: lt %d, %d, %d", ip++, regi, $<rval>1.regi, $<rval>3.regi);
    regi_free($<rval>1.regi);
    regi_free($<rval>3.regi);
    $<rval>$.regi = regi;
  }
  | additive_expression LE additive_expression
  {
    int regi = regi_new();
    generate("%d: le %d, %d, %d", ip++, regi, $<rval>1.regi, $<rval>3.regi);
    regi_free($<rval>1.regi);
    regi_free($<rval>3.regi);
    $<rval>$.regi = regi;
  }
  | additive_expression GT additive_expression
  {
    int regi = regi_new();
    generate("%d: gt %d, %d, %d", ip++, regi, $<rval>1.regi, $<rval>3.regi);
    regi_free($<rval>1.regi);
    regi_free($<rval>3.regi);
    $<rval>$.regi = regi;
  }

  | additive_expression GE additive_expression
  {
    int regi = regi_new();
    generate("%d: ge %d, %d, %d", ip++, regi, $<rval>1.regi, $<rval>3.regi);
    regi_free($<rval>1.regi);
    regi_free($<rval>3.regi);
    $<rval>$.regi = regi;
  }

  | additive_expression EQ additive_expression
  {
    int regi = regi_new();
    generate("%d: eq %d, %d, %d", ip++, regi, $<rval>1.regi, $<rval>3.regi);
    regi_free($<rval>1.regi);
    regi_free($<rval>3.regi);
    $<rval>$.regi = regi;
  }
  | additive_expression NE additive_expression
  {
    int regi = regi_new();
    generate("%d: ne %d, %d, %d", ip++, regi, $<rval>1.regi, $<rval>3.regi);
    regi_free($<rval>1.regi);
    regi_free($<rval>3.regi);
    $<rval>$.regi = regi;
  }
  | additive_expression
;

/*relop
  : LT | LE | GT | GE | EQ | NE
;*/

additive_expression
  : additive_expression PLUS term   /*     */
  {
    int regi = regi_new();
    generate("%d: add %d, %d, %d",ip++, regi, $<rval>1.regi, $<rval>3.regi);
    regi_free($<rval>1.regi);
    regi_free($<rval>3.regi);
    $<rval>$.regi = regi;
  }
  | additive_expression MINUS term  /*     */
  {
    int regi = regi_new();
    generate("%d: sub %d, %d, %d",ip++, regi, $<rval>1.regi, $<rval>3.regi);
    regi_free($<rval>1.regi);
    regi_free($<rval>3.regi);
    $<rval>$.regi = regi;
  }
  | term
;

/*addop
  : PLUS | MINUS
;*/

term
  : term MULTIPLY factor     /*  */
  {
    int regi = regi_new();
    generate("%d: mul %d, %d, %d", ip++, regi, $<rval>1.regi, $<rval>3.regi);
    regi_free($<rval>1.regi);
    regi_free($<rval>3.regi);
    $<rval>$.regi = regi;
  }
  | term DIVIDE factor      /*  */
  {
    int regi = regi_new();
    generate("%d: div %d, %d, %d", ip++, regi, $<rval>1.regi, $<rval>3.regi);
    regi_free($<rval>1.regi);
    regi_free($<rval>3.regi);
    $<rval>$.regi = regi;
  }
  | factor
;

/*mulop
  : MULTIPLY | DIVIDE
;*/

factor
  : LPAR expression RPAR
  {
    $<rval>$.regi = $<rval>2.regi; // no code gen if factor == ( expression )
  }
  | var
  {
    struct symbol *symbolp;
    symbolp = lookup_symbol($<lval>1.lex);
    if(symbolp == NULL)
      error("error %s: %s %s%s%s", fno,"undefined", "variable \"", $<lval>1.lex,"\"");
   
    if(symbolp->kind == GLOBAL) { // this case : var's kind == GLOBAL
      int regi = regi_new();
      int offset = symbolp->offset;
      if(symbolp->type == INT_ARRAY_TYPE)
        generate("%d: lda %d, -%d(gp)", ip++, regi, offset,0);
      else
        generate("%d: ld %d, -%d(gp)", ip++, regi, offset,0);
      $<rval>$.regi = regi;
   } else { // var's kind != GLOBAL
       int regi = regi_new();
       int offset = symbolp->offset;
       if(symbolp->type == INT_ARRAY_TYPE)
         generate("%d: lda %d, -%d(fp)", ip++, regi, offset+2,0);
       else
         generate("%d: ld %d, -%d(fp)", ip++, regi, offset+2,0);
       $<rval>$.regi = regi;
       arraytype++;
   }
  }  
  | var LBRACKET expression RBRACKET
  {
    struct symbol *symbolp;
    symbolp = lookup_symbol($<lval>1.lex);
    if(symbolp == NULL)
      error("error %s: %s %s%s%s", fno,"undefined", "array \"", $<lval>1.lex,"\"");
    if(symbolp->kind == GLOBAL){ /* array var->kind == GLOBAL */
      int regi1 = regi_new();
      int regi2 = regi_new();
      int offset = symbolp->offset;
      generate("%d: add %d, gp, %d", ip++, regi1, $<rval>3.regi,0);
      generate("%d: ld %d, -%d(%d)", ip++, regi2, offset, regi1);
      regi_free(regi1);
      regi_free($<rval>3.regi);
      $<rval>$.regi = regi2;
      regi_free(regi2);
    } else { /* array var->kind != GLOBAL */
        if(symbolp->kind == ARGUMENT){
          int regi1 = regi_new();
          int regi2 = regi_new();
          int offset = symbolp->offset;
          generate("%d: ld %d, -%d(fp)",ip++, regi2, offset+2,0);
          generate("%d: add %d, %d, %d",ip++, regi1, regi2, $<rval>3.regi);
          generate("%d: ld %d, %d(%d)", ip++, regi2, offset, regi1);
          regi_free(regi1);
          regi_free($<rval>3.regi);
          $<rval>$.regi = regi2;
          regi_free(regi2);
        } else {
            int regi1 = regi_new();
            int regi2 = regi_new();
            int offset = symbolp->offset;
            generate("%d: add %d, fp, %d",ip++, regi1, $<rval>3.regi,0);
            generate("%d: ld %d, -%d(%d)", ip++, regi2, offset+2, regi1);
            regi_free(regi1);
            regi_free($<rval>3.regi);
            $<rval>$.regi = regi2;
            regi_free(regi2);
        }
    }

  }
  | NUM
  {
    int regi = regi_new();
    int num = atoi(lex);
    generate("%d: ldc %d, %d", ip++, regi, num, 0);
    $<rval>$.regi = regi;
  }
;

selection_stmt
  : IF LPAR expression         
  {
    generate("%d: jle %d, %%d(pc)",ip++,$<rval>3.regi,0,0);
    regi_free($<rval>3.regi);
  } 
    if_rpar statement
  {
    generate("%d: ldc pc, %%d", ip++,0,0,0);
    backpatch($<pval>5.ip-1, ip-$<pval>5.ip );
  } 
    if_else statement
  {
    backpatch($<pval>8 .ip-1,ip);
  }
;

if_rpar
  : RPAR
  {
    $<pval>$.ip = ip;
  }
;

if_else
  : ELSE
  {
    $<pval>$.ip = ip;
  }
;

iteration_stmt    /*   */
  : WHILE while_lpar expression 
  {
    generate("%d: jle %d, %%d(pc)",ip++,$<rval>3.regi,0,0);
    regi_free($<rval>3.regi);
  } 
    while_rpar statement
  {
    generate("%d: ldc pc, %d", ip++,$<pval>2.ip,0,0);
    backpatch($<pval>5.ip-1,ip-$<pval>5.ip );
  }
;

while_lpar    /*   */
  : LPAR
  {
    $<pval>$.ip = ip;
  }
;

while_rpar   /*    */
  : RPAR
  {
    $<pval>$.ip = ip;
  }
;

funcall_stmt           /*       */
  : var ASSIGN call
  {
    char *var = $<lval>1.lex;
    struct symbol *symbolp;
    symbolp = lookup_symbol(var);
    if(symbolp==NULL){
      error("error %s: %s %s%s%s", fno,"undefined", "variable \"",$<lval>1.lex,"\"");
    }

    if(symbolp->kind == GLOBAL) {
      int offset = symbolp->offset;
      generate("%d: st %d, -%d(gp)", ip++, $<rval>3.regi, offset,0);
      regi_free($<rval>3.regi);
    } else {
        int offset = symbolp->offset;
        generate("%d: st %d, -%d(fp)", ip++, $<rval>3.regi, offset+2,0);
        regi_free($<rval>3.regi);   
    }

  }
  | var LBRACKET expression RBRACKET ASSIGN call
  {
    struct symbol *symbolp;
    symbolp = lookup_symbol($<lval>1.lex);
    if(symbolp->kind == GLOBAL) {
      int offset = symbolp->offset;
      generate("%d: st %d, -%d(gp)", ip++,$<rval>6.regi, $<rval>3.regi, 0);
      $<rval>$.regi = $<rval>3.regi;
      regi_free($<rval>3.regi);
    } else {
        int offset = symbolp->offset;
        generate("%d: st %d, -%d(fp)", ip++,$<rval>6.regi, $<rval>3.regi+2, 0);
        $<rval>$.regi = $<rval>3.regi;
        regi_free($<rval>3.regi);
    }
  }
  | call
;

call         /* slide 14 caller code part */
  : var
  {
    struct symbol *symbolp;
    symbolp = lookup_symbol($<lval>1.lex); // 함수심볼을 사용함
    if(symbolp == NULL){
      error("error %s: %s %s%s%s", fno,"undefined function", "call \"", $<lval>1.lex,"\"");
    } else if(symbolp->kind != FUNCTION){
        error("error %s: %s %s%s%s", fno,"type error", "function \"",$<lval>1.lex,"\"");
    } 
    aarg_count = 0;
    generate("%d: lda sp, -2(sp)", ip++,0,0,0);
  }
    LPAR args RPAR
  {
    struct symbol *symbolp;
    symbolp = lookup_symbol($<lval>1.lex); // 함수심볼을 사용함
    if(symbolp->size != aarg_count)
      error("error %s: %s %s%s%s", fno,"wrong no argument","function \"", $<lval>1.lex,"\"");

    int regi;
    generate("%d: st fp, -%d(fp)", ip++, *current_offset+2,0,0);
    generate("%d: lda fp, -%d(fp)", ip++, *current_offset+2,0,0);
    regi = regi_new();
    generate("%d: lda %d, 2(pc)", ip++, regi,0,0);
    generate("%d: st %d, -1(fp)", ip++, regi,0,0);
    regi_free(regi);
    if( symbolp->offset != 0) {
      generate("%d: ldc pc, %d", ip++, symbolp->offset,0,0);
    } else { /* save ip which called this function */
        generate("%d: ldc pc, %%d", ip++, 0,0,0);
        symbolp->ip[symbolp->ipc] = ip - 1;
        symbolp->ipc = symbolp->ipc+1;
    }
    regi = regi_new();
    generate("%d: lda %d, 0(%d)", ip++, regi, REGI_RETURN,0);
    $<rval>$.regi = regi;
    regi_free(regi);
  } 
;

args
  : arg_list
  | empty
;

arg_list
  : arg_list COMMA expression 
  {
    aarg_count++;
    generate("%d: push %d", ip++, $<rval>3.regi,0,0);
    regi_free($<rval>3.regi);
  }
  | expression 
  {
    aarg_count++;
    generate("%d: push %d", ip++, $<rval>1.regi,0,0);
    regi_free($<rval>1.regi);
  }
;

return_stmt
  : RETURN SEMICOLON  /* assign return to REGI_RETURN(=27) */
  {
    mainvar += 1;
    generate("%d: ldc %d, 0", ip++, REGI_RETURN,0,0);
    generate("%d: lda sp, 0(fp)", ip++,0,0,0);
    generate("%d: ld fp, 0(fp)", ip++,0,0,0);
    generate("%d: ld pc, -1(sp)", ip++,0,0,0);
    /* save 0 to REGI_RETURN, then return */
  }
  | RETURN expression SEMICOLON /* save return to REGI_RETURN, then return  */
  {
    mainvar += 1;
    generate("%d: lda %d, 0(%d)", ip++, REGI_RETURN, $<rval>2.regi,0);
    generate("%d: lda sp, 0(fp)", ip++,0,0,0);
    generate("%d: ld fp, 0(fp)", ip++,0,0,0);
    generate("%d: ld pc, -1(sp)", ip++,0,0,0);
    regi_free($<rval>2.regi);
  }
;

input_stmt
  : INPUT var SEMICOLON   /*     */
  {

    struct symbol *symbolp;
    symbolp = lookup_symbol($<lval>2.lex);
    if(symbolp->kind == GLOBAL) {
      int regi = regi_new();
      int offset = symbolp->offset;
      generate("%d: in %d", ip++, regi,0,0);
      generate("%d: st %d, -%d(gp)", ip++, regi, offset,0);
      regi_free(regi);
    } else {
      int regi = regi_new();
      int offset = symbolp->offset;
      generate("%d: in %d", ip++, regi,0,0);
      generate("%d: st %d, -%d(fp)", ip++, regi, offset+2,0);
      regi_free(regi);
    }
  }
  | INPUT var LBRACKET expression RBRACKET SEMICOLON
  {
    struct symbol *symbolp;
    symbolp = lookup_symbol($<lval>2.lex);
    if(symbolp->kind == GLOBAL) {
      int regi = regi_new();
      int regi1 = regi_new();
      int offset = symbolp->offset;
      generate("%d: in %d", ip++, regi,0,0);
      generate("%d: add %d, gp, %d", ip++, regi1,$<rval>4.regi,0);
      generate("%d: st %d, -%d(%d)", ip++, regi, offset, regi1);
      regi_free(regi);
      regi_free(regi1);
      regi_free($<rval>4.regi);
    } else {
      int regi = regi_new();
      int regi1 = regi_new();
      int offset = symbolp->offset;
      generate("%d: in %d", ip++, regi,0,0);
      generate("%d: add %d, fp, %d", ip++, regi1,$<rval>4.regi,0);
      generate("%d: st %d, -%d(%d)", ip++, regi,offset+2,regi1);
      regi_free(regi);
      regi_free(regi1);
      regi_free($<rval>4.regi);
    }
  }
;

output_stmt
  : OUTPUT expression SEMICOLON
  {
    generate("%d: out %d", ip++, $<rval>2.regi,0,0);
    regi_free($<rval>2.regi);
  }
;

empty
  :
;


%%


/* ====================================================================== */

void regi_init(void)
{
  int i;
  for (i = 0; i < NO_TMP_REGIS; i++)
    regi_used[i] = 0;
}

/* ====================================================================== */

int regi_new(void)
{
  int i;

  for (i = 0; i < NO_TMP_REGIS; i++)
    if (regi_used[i] == 0) {
      regi_used[i] = 1;
      return i;
    }
  
  //errror("error 50: all registers are used!");
  printf("error 50: all registers are used!");
  exit(-1);
}

/* ====================================================================== */

void regi_free(int i)
{
  regi_used[i] = 0;
}

/* ====================================================================== */

int generate(char *fmt, int i1, int i2, int i3, int i4)
{
  char tmp[MAX_LINE];
  char *p;

  sprintf(tmp, fmt, i1, i2, i3, i4);
  p = (char *) malloc(strlen(tmp) + 1);
  strcpy(p, tmp);
  code[ip - 1] = p;
}

/* ====================================================================== */

void backpatch(unsigned int ip1, unsigned int ip2)
{
  char tmp[MAX_LINE];
  char *p;

  sprintf(tmp, code[ip1], ip2);
  p= (char *) malloc(strlen(tmp) + 1);
  strcpy(p, tmp);
  free(code[ip1]); // Free incomplete instruction
  code[ip1] = p;

}

/* ====================================================================== */

void backpatch_funcalls(void)
{
  int i, j;
  struct symbol *symbolp;
  int k = 0;
  for(i = 0; i < HASH_SIZE; i++)
    for( symbolp = global_table->hash[i]; symbolp != NULL;
         symbolp = symbolp->next){
      for(j = 0; j < symbolp->ipc; j++){
        backpatch(symbolp->ip[j], symbolp->offset);
      }
}
}

/* ====================================================================== */

void print_code(void)
{
  int i;
  int j;
  char file[MAX_LINE];
  FILE *fp;

  i = strlen(prog_name);
  if ((i > 2) && (prog_name[i-2] == '.') && (prog_name[i - 1] == 'c'))
    prog_name[i - 2] = '\0';
  else if ((i > 2) && (prog_name[i - 2] == '.') && (prog_name[i - 1] == 'C'))
    prog_name[i - 2] = '\0';

  sprintf(file, "%s.tm", prog_name);
  if ((fp = fopen(file, "w")) == NULL) {
    fprintf(stderr, "%s: %s\n", file, strerror(errno));
    exit(1);
  }

  struct symbol *symbolp;
  
  fprintf(fp, "// ====================\n");
  fprintf(fp, "// c startup\n");
  fprintf(fp, "// ====================\n");
  for (i = 0; i < ip; i++){
     for(j = 0; j < HASH_SIZE; j++){
      for( symbolp = global_table->hash[j]; symbolp != NULL;
           symbolp = symbolp->next){
            if(symbolp->offset == i && symbolp->kind == FUNCTION){
              fprintf(fp, "// ====================\n");
              fprintf(fp, "// %s()\n",symbolp->name);
              fprintf(fp, "// ====================\n");
              j++;
            }
      }
    }
    fprintf(fp, "%s\n", code[i]);
  }
  fprintf(fp, "// ====================\n");
  fclose(fp);
}

/* ====================================================================== */
int yyerror(char *message)
{
  print_table(current_table);
  fprintf(stdout,
	"---------- ---------- ---------- ---------- ---------- ----------\n");
  print_table(global_table);
  fprintf(stdout,
	"---------- ---------- ---------- ---------- ---------- ----------\n");
  fprintf(stderr, "line %d: %s at \"%s\"\n", source_line_no, message,
	  yytext);
}

/* ====================================================================== */

int error(char *fmt, char *s1, char *s2, char *s3, char *s4,char *s5)
{
  print_table(current_table);
  fprintf(stdout,
	"---------- ---------- ---------- ---------- ---------- ----------\n");
  print_table(global_table);
  fprintf(stdout,
	"---------- ---------- ---------- ---------- ---------- ----------\n");
  fprintf(stdout, "line %d: ", source_line_no);
  fprintf(stdout, fmt, s1, s2, s3, s4, s5);
  fprintf(stdout, "\n");
  fflush(stdout);
  exit(1);
}

/* ====================================================================== */

int main(int argc, char *argv[])
{
  
  if(argc == 2){
    option_symbol = 0;
    prog_name = argv[1];
  } else if (argc == 3){
    if (strcmp(argv[1], "-s") != 0) {
      fprintf(stderr, "usage: cm [-s] file\n");
      exit(1);
    }
    option_symbol = 1;
    prog_name = argv[2];
  } else {
    fprintf(stderr, "usage: cm [-s] file\n");
    exit(1);
  }
  
  yyin = fopen(prog_name, "r");
  fno[0] =argv[1][6];
  fno[1] =argv[1][7];

  if(yyin == NULL) {
    fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
    exit(1);
  }
  yyparse();

  return 0;
}

/* ====================================================================== */
