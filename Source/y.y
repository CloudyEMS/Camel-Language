%{
  #include <stdio.h>
  #include "ast.h"

  // From lex.yy.c
  extern int yyparse();
  extern int yylex();
  extern FILE *yyin;
  extern int yylineno;
  extern int yycolno;

  // From interpreter.c
  extern int ex(nodeType *);
  // Prototypes.
  void yyerror(char *s);
%}

%union {
  int ival; //integer value;
  float realval;
  char sval; //byte value;
  char *string; //string value;
  nodeType *nPtr; //AST node pointer;
}

%token <ival> INT
%token <realval> REAL
%token <string> ID STR
%token PRINT WHILE LOOP IF DEF FOR
%token READ_INT READ_REAL READ_STR
%token LT GT LTE GTE EQ NEQ MOD
%nonassoc IFX
%nonassoc ELSE

%left LT GT LTE GTE EQ NEQ MOD
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS
%right '^'

%type <nPtr> stmt expr stmt_list id_list declare_id read_stmt
%%

program:
  function { exit (0); }
  ;

function:
  function stmt { ex($2); freeNode($2); }
  | function DEF ID '(' ')' stmt { putsym($3, sym_func, $6); }
  |
  ;

stmt:
  ';'                               { $$ = opr(';', 2 , NULL, NULL); }
  | declare_id ';'                  { $$ = $1; }
  | expr ';'                        { $$ = $1; }
  | PRINT expr ';'                  { $$ = opr(PRINT, 1, $2); }
  | read_stmt ';'                   { $$ = $1; }
  | ID '=' expr ';'                 { $$ = opr('=', 2, id($1), $3); }
  | WHILE '(' expr ')' stmt         { $$ = opr(WHILE, 2, $3, $5); }
  | LOOP INT ':' stmt               { $$ = opr(LOOP, 2, conint($2), $4); }
  | LOOP ID ':' stmt                { $$ = opr(LOOP, 2, id($2), $4); }
  | IF '(' expr ')' stmt %prec IFX  { $$ = opr(IF, 2, $3, $5); }
  | IF '(' expr ')' stmt ELSE stmt  { $$ = opr(IF, 3, $3, $5, $7); }
  | '{' stmt_list '}'               { $$ = $2; }
  ;

stmt_list:
  stmt              { $$ = $1; }
  | stmt_list stmt  { $$ = opr(';', 2, $1, $2); }
  ;

declare_id:
  DEF id_list {$$ = $2;}
  ;

id_list:
  ID                        { if(putsym($1, sym_id, NULL)) $$ = id($1); else yyerror("Symbol already defined!"); }
  | ID '=' expr             { if(putsym($1, sym_id, NULL)) $$ = opr('=', 2, id($1), $3); else yyerror("Symbol already defined!"); }
  | id_list ',' ID          { if(putsym($3, sym_id, NULL)) $$ = opr(';', 2, $1, id($3)); else yyerror("Symbol already defined!"); }
  | id_list ',' ID '=' expr { if(putsym($3, sym_id, NULL)) $$ = opr(';', 2, $1, opr('=', 2, id($3), $5)); else yyerror("Symbol already defined!"); }
  ;

read_stmt:
  READ_INT ID     { $$ = opr(READ_INT, 1 ,id($2));  }
  | READ_REAL ID  { $$ = opr(READ_REAL, 1 ,id($2)); }
  | READ_STR ID   { $$ = opr(READ_STR, 1 ,id($2));  }
  ;

expr:
  INT                     { $$ = conint($1); }
  | STR                   { $$ = constr($1); }
  | REAL                  { $$ = conreal($1); }
  | ID                    { $$ = id($1); }
  | ID '(' ')'            { $$ = func($1); }
  | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2);  }
  | expr '+' expr         { $$ = opr('+', 2, $1, $3); }
  | expr '-' expr         { $$ = opr('-', 2, $1, $3); }
  | expr '*' expr         { $$ = opr('*', 2, $1, $3); }
  | expr '/' expr         { $$ = opr('/', 2, $1, $3); }
  | expr '^' expr         { $$ = opr('^', 2, $1, $3); }
  | expr MOD expr         { $$ = opr(MOD, 2, $1, $3); }
  | expr LT expr          { $$ = opr(LT, 2, $1, $3);  }
  | expr GT expr          { $$ = opr(GT, 2, $1, $3);  }
  | expr LTE expr         { $$ = opr(LTE, 2, $1, $3); }
  | expr GTE expr         { $$ = opr(GTE, 2, $1, $3); }
  | expr EQ expr          { $$ = opr(EQ, 2, $1, $3);  }
  | expr NEQ expr         { $$ = opr(NEQ, 2, $1, $3); }
  | '(' expr ')'          { $$ = $2; }
  ;

%%

int main(int argc, char **argv){
  // If a filename was given ...
  if(argc > 1)
  {
    // Then run the compiler on the file.
    FILE *file = fopen(argv[1], "r");
    if ( file == NULL){
      printf("Couldn't open file %s\n",argv[1]);
      return -1;
    }
    yyin = file;
    do {
      yyparse();
    } while (feof(file));
    fclose(file);
  }
  else // Read from stdin.
  {
    yyparse();
  }

  //TODO: Free symtable when exiting.
  return 0;
}

void yyerror(char *s){
  printf("Error at line %d:%d! Message: %s\n", yylineno, yycolno, s);
}
