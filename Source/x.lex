%{
  #include <stdlib.h>
  #include "ast.h"
  #include "y.tab.h"

  extern void yyerror(char *);

  int yylineno; // Number of current line.
  int yycolno = 1; // Number of the current column.

  char buf[256]; // Buffer for reading string.
  char *s;
%}

%x STRING
%%

"if"    { yycolno += 2; return IF; }
"else"  { yycolno += 4; return ELSE; }
"while" { yycolno += 5; return WHILE; }
"loop"  { yycolno += 4; return LOOP; }
"print" { yycolno += 5; return PRINT; }
"def"   { yycolno += 3; return DEF; }

"read_i"  { yycolno += 6; return READ_INT; }
"read_r"  { yycolno += 6; return READ_REAL; }
"read_s"  { yycolno += 6; return READ_STR; }

"<"   { yycolno++; return LT; }
">"   { yycolno++; return GT; }
"<="  { yycolno += 2; return LTE; }
">="  { yycolno += 2; return GTE; }
"=="  { yycolno += 2; return EQ; }
"<>"  { yycolno += 2; return NEQ; }
"mod" { yycolno += 3; return MOD; }

\"            { BEGIN STRING; s = buf; yycolno++; }

<STRING>\\n   { *s++ = '\n'; yycolno++;}
<STRING>\\t   { *s++ = '\t'; yycolno++;}
<STRING>\\r   { *s++ = '\r'; yycolno++;}
<STRING>\\\"  { *s++ = '\"'; yycolno++;}
<STRING>\"    { yycolno += 1;
                *s = 0;
                BEGIN 0;
                yylval.string = strdup(buf);
                return STR;
              }
<STRING>\n    { printf("invalid string"); exit(1); }
<STRING>.     { yycolno++; *s++ = *yytext; }

[a-zA-Z_][a-zA-Z_]* {
                    yycolno++;
                    yylval.string = strdup(yytext);
                    return ID;
                    }

[0-9]+\.[0-9]+      {
                    yycolno++;
                    yylval.realval = atof(yytext);
                    return REAL;
                    }

[0-9]+  {
        yycolno++;
        yylval.ival = atoi(yytext);
        return INT;
        }

[-+*/()\[\]{}=;:\^,] {yycolno++; return *yytext; }

\n    { yylineno++; yycolno = 1; }

[ \t]+  yycolno++; /* Ignore Whitespaces! */

. { yycolno++; yyerror("Unrecognised Symbol!"); }

%%
