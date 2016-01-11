#ifndef AST_HEADER_FILE
#define AST_HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "value.h"
#include "sym_table.h"

// Enum, holding the types of all nodes for the AST.
typedef enum { typeCon, typeId, typeOpr } nodeEnum;

// Constant node.
typedef struct {
  Value val; // The value of the node.
} conNodeType;

// Variables node.
typedef struct {
  char *name; // The name of the variable.
} idNodeType;

// Operations nodes.
typedef struct {
  int oper; // The operation.
  int nops; // How many arguments the opration has.
  struct nodeTypeTag *op[1]; // The arguments (operands).
} oprNodeType;

// The nodes.
typedef struct nodeTypeTag{
  nodeEnum type; // Type of the node.

  union {
    conNodeType con;
    idNodeType id;
    oprNodeType opr;
  };
} nodeType;

/* Prototypes. */

// Function to create an operation node.
nodeType *opr (int oper, int nops, ...);
// Function to create an id (variable) node of a given name.
nodeType *id (char *name);
// Function to creat a constant node.
nodeType *con (void);
// Function to creat a constant integer node of a given integer.
nodeType *conint (int value);
// Function to create a constant string node of a given string.
nodeType *constr (char* str);
// Function to create a constant real number (float) node of a given float value.
nodeType *conreal (float value);
void freeNode (nodeType *p);

// From y.tab.c
extern void yyerror(char *);

#endif
