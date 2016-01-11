#ifndef SYM_HEADER_FILE
#define SYM_HEADER_FILE

#include "value.h"

// A structure for the sym_table nodes.
struct symtable {
  char *name; // Name of the symbol.
  Value value; // The value the symbol holds.
  struct symtable *next; // A pointer to the next symbol.
};

// defining the struct as a symrec type (for ease of programming).
typedef struct symtable symrec;

/* Prototypes. */

// Function to add a symbol to the symbol table of a given name.
symrec *putsym(char *);
// Function to search for a symbol in the symbol table.
symrec *searchsym(char *);
// Function to get the value of a given symbol in the symbol table.
Value getsym(char *);
// Function to change the value of a given symbol in the symbol table to a given Value and returns the new value.
Value symchange(char *, Value);

#endif
