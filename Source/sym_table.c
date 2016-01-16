#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sym_table.h"

// The pointer to the symbol table.
symrec *sym_node = NULL;

// Function to add a symbol to the symbol table of a given name.
symrec *putsym(char *name, symEnum type, struct nodeTypeTag *func_tree){
  // First, search if the symbol exists.
  symrec *sym = searchsym(name);

  // if the symbol already exists ...
  if(sym != NULL){
    // ... Don't do anything, and throw an error.
      printf("Symbol already exists!\n");
      return NULL;
  }

  // Create a place in the memory for the symbol.
  sym = (symrec *)malloc(sizeof(symrec));
  if(sym == NULL){
    printf("Not enough memory!\n");
    exit (-1);
  }

  // Create enough place for the name of the symbol.
  sym->name = (char *)malloc(sizeof(strlen(name)+1));
  if(sym->name == NULL){
    printf("Not enough memory for name!\n");
    exit (-1);
  }

  // Set the name of the symbol.
  sym->name = name;
  // Set the type of the symbol (Id, function).
  sym->type = type;
  // If the symbol is a function, add it's body to it.
  if (type == sym_func){
      sym->func_tree = func_tree;
  }
  // Add the symbol to the start of the symbol table.
  sym->next = sym_node;
  // Point at the symbol, making it the first symbol in the table.
  sym_node = sym;
  return sym;
}

// Function to search for a symbol in the symbol table.
symrec *searchsym(char *name){
  symrec *sym;
  // Loop till you find the symbol or reach the end of the symbol table.
  for(sym = sym_node; sym != NULL; sym = sym->next){
    // If the name of the current symbol is the same as the symbol we are looking for, then leave the loop.
    if(strcmp(sym->name,name) == 0){
      break;
    }
  }
  return sym;
}

// Function to get the value of a given symbol in the symbol table.
Value getsym_id (char *name){
  symrec *sym;
  // Search for the symbol in the symbol table.
  sym = searchsym(name);
  // If the symbol doesn't exist ...
  if(sym == NULL){
    // ... Throw an error and exit.
    printf("Variable %s not declared before \n", name);
    exit(1);
  }

  // Check if the symbol is a function.
  if(sym->type == sym_func){
    printf("Symbol %s is a function, not a variable\n", name);
    exit(1);
  }

  // Return the value of the symbol.
  return sym->value;
}

// Function to change the value of a given symbol in the symbol table to a given Value and returns the new value.
Value symchange_id(char *name, Value newValue){
  symrec *sym;
  // Search for the symbol in the symbol table.
  sym = searchsym(name);
  // If the symbol doesn't exist ...
  if(sym == NULL){
    // ... throw an error and exist.
    printf("No variable found with name %s\n",name);
   exit (-1);
  }

  // Check if the symbol is a function.
  if(sym->type == sym_func){
    printf("Symbol %s is a function, not a variable\n", name);
    exit(1);
  }

  // Change the value of the found symbol.
  sym->value = newValue;
  // Return the value of the symbol.
  return sym->value;
}
