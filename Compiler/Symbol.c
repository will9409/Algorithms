/* ====================================================================== */

#include <stdio.h>
#include <stdlib.h>

#define HASH_SIZE	11	/* hash array size */

#define GLOBAL		0	/* global variable */
#define LOCAL		1	/* local variable */
#define ARGUMENT	2	/* formal argument */
#define FUNCTION	3	/* function name */
#define FUNCTIONI	4	/* incomplete function */
				/* symbol's type */
#define INT_TYPE	0	/* int type */
#define INT_ARRAY_TYPE	1	/* int array type */
#define INT_P_TYPE	2	/* int [] type */
#define VOID_TYPE	3	/* void type */


/* ====================================================================== */

struct symbol {
  char *name;			/* symbol's name */
  unsigned char kind;		/* symbol's kind */
  unsigned char type;		/* symbol's type */
  unsigned int size;		/* byte size or no of args */
  unsigned int offset;		/* relative memory offset */
  unsigned int ip[128];		/* ip that calls this function */
  unsigned int ipc;		/* ip count in ip[] */
  struct symbol *next;		/* next symbol pointer */
};

struct table {
  char *name;			/* function name */
  struct symbol *hash[HASH_SIZE];	/* hash array */
};

struct table *global_table;	/* global symbol table */
struct table *local_table;	/* local symbol table */
struct table *current_table;	/* current symbol table */

struct table *create_table(char *name);

/* ====================================================================== */

int hash_function(char *name)// 네임은 글로벌은 글로벌테이블, 로컬은 함수이름
{  // 끝나면 파괴후 다음 funtion에서 재생성
  int hashval;
  for(hashval = 0; *name != '\0';)
    hashval += *name++;

  return hashval % HASH_SIZE;
}

struct table *create_table(char *name) // 글로벌은 시작하자마자, 로컬은 그때마다
{
  struct table *tablep;
  int i;
  if((tablep = (struct table *) malloc(sizeof(*tablep)))==NULL)
    printf("can't malloc in create_table");
  
    tablep->name = name;
  for( i = 0; i < HASH_SIZE; i++)
    tablep->hash[i] = NULL;

  return tablep;
}

struct symbol *add_symbol(struct table *tablep, char *name, unsigned char kind,
                          unsigned char type, unsigned int size, unsigned int offset){
  int hashval;
  struct symbol *symbolp;
  if((symbolp = (struct symbol *) malloc(sizeof(*symbolp))) == NULL)
    printf("can't malloc in add_symbol()");
  

  struct symbol *temp;
  symbolp->name = name;
  symbolp->kind = kind;
  symbolp->type = type;
  symbolp->size = size;
  symbolp->offset = offset;
  symbolp->ipc = 0;
  hashval = hash_function(name);
 
  if(tablep->hash[hashval] != NULL){
  temp = tablep->hash[hashval];
  tablep->hash[hashval] = symbolp;
  symbolp->next = temp;
  }
  else{
  tablep->hash[hashval] = symbolp;
  symbolp->next = NULL;
  }

  return symbolp;
}

struct symbol *find_symbol(struct table *tablep, char *name)
{
  struct symbol *symbolp;
  for(symbolp = tablep->hash[hash_function(name)];symbolp != NULL; symbolp = symbolp->next)
   if(strcmp(name,symbolp->name) == 0) 
     return symbolp;
  
  return NULL;
  
}

struct symbol *lookup_symbol(char *name)
{
  struct symbol *symbolp;
  if((symbolp = find_symbol(local_table, name)) == NULL)
     if((symbolp = find_symbol(global_table, name)) == NULL)
        return NULL;
   return symbolp;
}

char* trans1(unsigned char k){
  if(k == 0)return "global";
  if(k == 1)return "local";
  if(k == 2)return "argument";
  if(k == 3)return "function";
  if(k == 4)return "functioni";
}

char* trans2(unsigned char t){
  if(t == 0) return "int";
  if(t == 1) return "int array";
  if(t == 2) return "int *";
  if(t == 3) return "void";
}

void print_table(struct table *tablep)
{
  struct symbol *symbolp;
  int i;

  for(i = 0; i < HASH_SIZE;i++ ){
    for(symbolp = tablep->hash[i]; symbolp != NULL;symbolp = symbolp->next ){
fprintf(stdout,"%-11s%-11s%-11s%-11s%10d%10d\n",tablep->name,symbolp->name,trans1(symbolp->kind),trans2(symbolp->type),symbolp->size,symbolp->offset);
}
}



}



/* ====================================================================== */
