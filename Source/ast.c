#include "ast.h"

// Macro for the size of the generic nodetype.
#define SIZEOF_NODETYPE ((char *)&p->con - (char*)p)

// Creat a constant node.
nodeType *con(){
  nodeType *p;
  size_t nodeSize;

  nodeSize = SIZEOF_NODETYPE + sizeof(conNodeType);
  if((p = malloc(nodeSize)) == NULL) {
    yyerror ("Out of memory!");
  }

  p->type = typeCon;
  return p;
}

// Creat a constant integer node.
nodeType *conint (int value) {
  nodeType *p = con();
  p->con.val.type = conInt;
  p->con.val.ival = value;
  return p;
}

// Creat a constant real number (float) node.
nodeType *conreal (float value) {
  nodeType *p = con();
  p->con.val.type = conReal;
  p->con.val.realval = value;
  return p;
}

// Create a constant string node.
nodeType *constr (char* str) {
  nodeType *p = con();
  p->con.val.type = conString;
  p->con.val.str = str;
  return p;
}

// Create a variable node.
nodeType *id (char *name) {
  nodeType *p;
  size_t nodeSize;

  nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
  if((p = malloc(nodeSize)) == NULL) {
    yyerror ("out of memory!");
  }

  p->type = typeId;
  p->id.name = name;
  return p;
}

// Create a variable node.
nodeType *func (char *name) {
  nodeType *p;
  size_t nodeSize;

  nodeSize = SIZEOF_NODETYPE + sizeof(funcNodeType);
  if((p = malloc(nodeSize)) == NULL) {
    yyerror ("out of memory!");
  }

  p->type = typeFunc;
  p->func.name = name;
  return p;
}

// Create an operation node.
nodeType *opr (int oper, int nops, ...) {
  va_list ap; // List for multiple arguemnts.
  nodeType *p;
  size_t nodeSize;
  int i;

  nodeSize = SIZEOF_NODETYPE + sizeof(oprNodeType) + (nops -1) + sizeof(nodeType*);
  if((p = malloc(nodeSize)) == NULL) {
    yyerror ("out of memory!");
  }

  p->type = typeOpr;
  p->opr.oper = oper;
  p->opr.nops = nops;

  // Get multiple arguments
  va_start(ap, nops);
  for (i = 0; i < nops; i++){
    p->opr.op[i] = va_arg(ap, nodeType*);
  }
  va_end(ap);

  return p;
}

// Free a node after it is executed.
void freeNode (nodeType *p){
  int i;
  if (!p)
    return;

  if(p->type == typeOpr){
    for(i = 0; i < p->opr.nops; i++){
      freeNode(p->opr.op[i]);
    }
  }

  free (p);
}
