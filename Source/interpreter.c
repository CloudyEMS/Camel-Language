#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "ast.h"
#include "value.h"
#include "y.tab.h"

// Prototype for the ex_arith function that executes arithmetic operations.
Value ex_arith(int, Value, Value);

// The string buffer. This means that the longest string in the language is only 256 bytes long.
// TODO: Change this and use malloc to create strings when needed.
char str_buf[256];

// Function to return an empty value.
Value empty_value(){
  Value v;
  return v;
}

// Function to multiply string s, n times.
char * mul_string (char *s, int n){
  if(--n <= 0){
    return s;
  }
  int i;
  char *b = strdup(s);
  for (i = 0; i < n; i++){
    strcat(s,b);
  }
  return s;
}

// Function to execute a node in the AST and returns the value of the executed node.
Value ex(nodeType *p){
  // If the node is empty, return an empty value (null).
  if (!p) return empty_value();

  // Depending on the type of the node, do different actions.
  switch(p->type){
    case typeCon: // If it was a constant.
      return p->con.val; // Return its value.
    case typeId: { // If it was an id (variable).
      Value v;
      v = getsym(p->id.name); // Get the value of the symbol in the symbol table.
      return v; // Return the value of hte symbol.
    }

    case typeOpr: // If it was an operation.
    // Check for the type of operation.
    switch(p->opr.oper){
      case WHILE: { // If it was a while loop.
        LOOPLABEL: { // Define a start label.
        Value v = ex(p->opr.op[0]); // Execute the expression of the loop.
        // Depending on the type of the variable ...
        switch(v.type){
          case conInt: // If the value is integer.
            // Check if it is true.
            if(v.ival){
              // Execute the statements in the body of the while loop.
              ex(p->opr.op[1]);
              // Return to start label.
              goto LOOPLABEL;
            }
            break;
          case conReal: // If the value is real (float).
            // Check if it is true.
            if(v.realval){
              // Execute the statements in the body of the while loop.
              ex(p->opr.op[1]);
              // Return to start label.
              goto LOOPLABEL;
            }
            break;
          default: // If it was a string then just ignore.
            break;
        }
      }
        return empty_value(); // Return an empty value (null).
      }
      case LOOP: { // If it was a loop.
        int end = ex(p->opr.op[0]).ival; //here we know it is for sure an integer.
        int i;
        for (i = 0; i < end; i++){
          ex(p->opr.op[1]); // Execute the body of the loop statement.
        }
        return empty_value(); // Return an empty value.
      }
      case IF:{ // If it was an if statement.
        Value v = ex(p->opr.op[0]); // Execute the expression in the if statement.
        // Execute the if statement depending on the type of the value.
        switch(v.type){
          case conInt:
            if (v.ival)
              ex(p->opr.op[1]);
            else if (p->opr.nops > 2)
              ex(p->opr.op[2]);
            break;
          case conReal:
            if (v.realval)
              ex(p->opr.op[1]);
            else if (p->opr.nops > 2)
              ex(p->opr.op[2]);
            break;
          default:
            break;
        }

        return empty_value();
      }
      case PRINT:{ // If it is a print statement.
        Value v = ex(p->opr.op[0]); // Take the value of the expression.
        // Format the output depending on the type of the value.
        switch (v.type){
          case conInt:
            printf("%d\n", v.ival);
            break;
          case conReal:
            printf("%f\n", v.realval);
            break;
          case conString:
            printf("%s\n", v.str);
            break;
        }
        return empty_value();
      }
      case READ_INT: { // If it was a read integer statement.
        //TODO: IMPLEMENT This later.
        // Value v = getsym(p->opr.op[0]->id.name);
        // Value tmp;
        // switch (v.type) {
        //   case conInt:{
        //     tmp.type = conInt;
        //     scanf("%d", &tmp.ival);
        //     symchange(p->opr.op[0]->id.name, tmp);
        //     return empty_value();
        //   }
        //   case conReal:{
        //     tmp.type = conReal;
        //     scanf("%f", &tmp.realval);
        //     symchange(p->opr.op[0]->id.name, tmp);
        //     return empty_value();
        //   }
        //   case conString:{
        //     tmp.type = conString;
        //     scanf("\n%s", tmp.str);
        //     symchange(p->opr.op[0]->id.name, tmp);
        //     return empty_value();
        //   }
        // }
        // Define a new integer value.
        Value v;
        v.type = conInt;
        // Prompt the user to enter an integer value.
        printf("> ");
        scanf("%d", &v.ival);
        // Change the value of the id (variable) to the newly read integer value.
        symchange(p->opr.op[0]->id.name, v);
        return empty_value();
      }
      case READ_REAL:{ // If it was a read real number (float) statement.
        // Define a new real number (float) value.
        Value v;
        v.type = conReal;
        // Prompt the user to enter a real number (float) value.
        printf("> ");
        scanf("%f", &v.realval);
        // Change the value of the id (variable) to the newly read real number (float) value.
        symchange(p->opr.op[0]->id.name, v);
        return empty_value();
      }
      case READ_STR:{ // If it was a read string statement.
        // Define a new string value.
        Value v;
        v.type = conString;
        // Prompt the user to enter a string value
        printf("> ");
        scanf("\n%s", str_buf);
        // Point at the string buffer.
        v.str = str_buf;
        // Change the value of the id (variable) to the newly read string value.
        symchange(p->opr.op[0]->id.name, v);
        return empty_value();
      }
      case ';': // If it was a semicolon.
        // Execute the operations.
        ex(p->opr.op[0]);
        return ex(p->opr.op[1]);
      case '=':{ // If it was an assignment statement.
        // Change the value of the variable to the right expression.
        return symchange(p->opr.op[0]->id.name, ex(p->opr.op[1]));
      }
      case UMINUS: { // If it was the unary minus (negative) operator.
        Value v = ex(p->opr.op[0]); // Get the value of the expression to be negated.
        // Negate the value depening on its type.
        switch (v.type) {
          case conInt:
            v.ival = -v.ival;
            break;
          case conReal:
            v.realval = -v.realval;
            break;
          default: // TODO: Reverse string when negated.
            return empty_value();
        }
        return v;
      }
      // The rest are arithmetic function. They all call the ex_arith function to get executed.
      case '+':
        return ex_arith('+',ex(p->opr.op[0]),ex(p->opr.op[1]));
      case '-':
        return ex_arith('-',ex(p->opr.op[0]),ex(p->opr.op[1]));
      case '*':
        return ex_arith('*',ex(p->opr.op[0]),ex(p->opr.op[1]));
      case '/':
        return ex_arith('/',ex(p->opr.op[0]),ex(p->opr.op[1]));
      case '^':
        return ex_arith('^',ex(p->opr.op[0]),ex(p->opr.op[1]));
      case MOD:
        return ex_arith(MOD ,ex(p->opr.op[0]),ex(p->opr.op[1]));
      case LT:
        return ex_arith(LT ,ex(p->opr.op[0]),ex(p->opr.op[1]));
      case GT:
        return ex_arith(GT ,ex(p->opr.op[0]),ex(p->opr.op[1]));
      case LTE:
        return ex_arith(LTE ,ex(p->opr.op[0]),ex(p->opr.op[1]));
      case GTE:
        return ex_arith(GTE ,ex(p->opr.op[0]),ex(p->opr.op[1]));
      case EQ:
        return ex_arith(EQ ,ex(p->opr.op[0]),ex(p->opr.op[1]));
      case NEQ:
        return ex_arith(NEQ ,ex(p->opr.op[0]),ex(p->opr.op[1]));
    }
  }
  return empty_value();
}

Value ex_arith(int op, Value a, Value b) {
  Value v; // Create a new value to return.

  // Determine the type of both operands.
  int intFlagA = 0;
  int intFlagB = 0;

  int realFlagA = 0;
  int realFlagB = 0;
  int realState = 0;
  int strFlagA = 0;
  int strFlagB = 0;
  int strState = 0;

  if (a.type == conString){
    strFlagA = 1;
  } else if (a.type == conReal) {
    realFlagA = 1;
  } else if (a.type == conInt) {
    intFlagA = 1;
  }

  if (b.type == conString){
    strFlagB = 1;
  } else if (b.type == conReal) {
    realFlagB = 1;
  } else if (b.type == conInt) {
    intFlagB = 1;
  }

  if (strFlagA || strFlagB){
    strState = 1;
  } else if(realFlagA || realFlagB) {
    realState = 1;
  }

  // Depending on the type of operation...
  switch (op) {
    case '+': { // If it is a plus.
      if (strState) {
        // Add strings!
        v.type = conString;
        v.str = str_buf; // Point at the buffer.
        // Concatenate strings while typecasting the operands to strings.
        if(strFlagA && strFlagB){
          if(v.str != a.str){
            strcpy(v.str, a.str);
          }
          if (v.str != b.str){
            strcat(v.str, b.str);
          }
          break;
        } else if (realFlagA){
          char buf[265];
          sprintf(buf, "%f", a.realval); // copy the value of the float operand to the string buffer.
          strcat(buf, b.str); // concatenate the second string to the buffer.
          strcpy(v.str, buf); // copy the value of the buffer to the value.
          break;
        } else if (realFlagB) {
          char buf[265];
          sprintf(buf, "%f", b.realval); // copy the value of the float operand to the string buffer.
          // If the string is not pointing at itself.
          if(v.str != a.str){
            strcpy(v.str, a.str); // copy the value of the first string to the new value.
          }
          // Concatenate the buffer to the new string.
          strcat(v.str, buf);
          break;
        } else if (intFlagA) {
          char buf[265];
          sprintf(buf, "%d", a.ival); // Copy the value of the first string to the buffer.
          strcat(buf, b.str); // Concatenate the second string to the buffer.
          strcpy(v.str, buf); // copy the value of the buffer to the new string.
          break;
        } else if (intFlagB) {
          char buf[265];
          sprintf(buf, "%d", b.ival); // Copy the value of the second integer to the buffer.
          // If the string is not pointing at itself.
          if(v.str != a.str){
            strcpy(v.str, a.str); // Copy the value of the first string to the new value.
          }
          strcat(v.str, buf); // Concatenate the buffer to the string.
          break;
        }
      }
      else if(realState){
        // Adding real numbers.
        v.type = conReal;
        if (realFlagA && realFlagB){
          v.realval = a.realval + b.realval;
          break;
        } else if (intFlagA) {
          v.realval = (float)a.ival + b.realval;
          break;
        }
        else if (intFlagB) {
          v.realval = a.realval + (float)b.ival;
          break;
        }
      }
      else {
        // Adding integer values.
        v.type = conInt;
        v.ival = a.ival + b.ival;
        break;
      }
    }
    case '-': { // If it is a minus.
      if (strState) {
        // Can't substract strings!
        printf("Can't substract strings!");
        return empty_value();
      }
      else if(realState){
        // Substract real numbers.
        v.type = conReal;
        if (realFlagA && realFlagB){
          v.realval = a.realval - b.realval;
          break;
        } else if (intFlagA) {
          v.realval = (float)a.ival - b.realval;
          break;
        }
        else if (intFlagB) {
          v.realval = a.realval - (float)b.ival;
          break;
        }
      }
      else {
        // Substract integer values.
        v.type = conInt;
        v.ival = a.ival - b.ival;
        break;
      }
    }
    case '*': { // If it is a multiplication.
      if (strState) {
        // Multiply a string n times.
        v.type = conString;
        v.str = str_buf;
        if(intFlagA){
          strcpy(v.str, b.str);
          mul_string(b.str, a.ival);
          break;
        } else if (intFlagB){
          strcpy(v.str, a.str);
          mul_string(v.str, b.ival);
          break;
        } else {
          // Coulnd't multiply string!
          printf("One of the operands must be an integer!\n");
          return empty_value();
        }
      }
      else if(realState){
        // Multiply real numbers.
        v.type = conReal;
        if (realFlagA && realFlagB){
          v.realval = a.realval * b.realval;
          break;
        } else if (intFlagA) {
          v.realval = (float)a.ival * b.realval;
          break;
        }
        else if (intFlagB) {
          v.realval = a.realval * (float)b.ival;
          break;
        }
      }
      else {
        // Multiply integer values.
        v.type = conInt;
        v.ival = a.ival * b.ival;
        break;
      }
    }
    case '/': { // If it is a division.
      //TODO: check for 0!
      if (strState) {
        // Coulnd't divide string!
        printf("Can't divide strings!");
        return empty_value();
      }
      else if(realState){
        //TODO: check for 0!
        // Divide real numbers.
        v.type = conReal;
        if (realFlagA && realFlagB){
          v.realval = a.realval / b.realval;
          break;
        } else if (intFlagA) {
          v.realval = (float)a.ival / b.realval;
          break;
        }
        else if (intFlagB) {
          v.realval = a.realval / (float)b.ival;
          break;
        }
      }
      else {

        // Divide integer values.
        v.type = conInt;
        v.ival = a.ival / b.ival;
        break;
      }
    }
    case '^': { // If it is an exponent.
      if (strState) {
        // Coulnd't exonentiate string!
        printf("Can't exponantiate strings!");
        return empty_value();
      }
      else if(realState){
        // Exponentiate real numbers.
        v.type = conReal;
        if (realFlagA && realFlagB){
          v.realval = pow(a.realval, b.realval);
          break;
        } else if (intFlagA) {
          v.realval = pow((float)a.ival, b.realval);
          break;
        }
        else if (intFlagB) {
          v.realval = pow(a.realval, (float)b.ival);
          break;
        }
      }
      else {
        // Exponentiate integer values.
        v.type = conInt;
        v.ival = (int)pow(a.ival, b.ival);
        break;
      }
    }
    case MOD: { // If it is a MOD
      v.type = conInt; // Mod always returns an integer!
      if (strState) {
        // Coulnd't mod string!
        printf("Can't mod strings!");
        return empty_value();
      }
      else if(realState){
        // make sure to return an integer!
        if (realFlagA && realFlagB){
          v.ival = (int)a.realval % (int)b.realval;
          break;
        } else if (intFlagA) {
          v.ival = a.ival % (int)b.realval;
          break;
        }
        else if (intFlagB) {
          v.ival = (int)a.realval % b.ival;
          break;
        }
      }
      else {
        // Mod integer values.
        v.ival = a.ival % b.ival;
        break;
      }
    }
    case LT: { // If it is <
      // LT always returns an integer!
      v.type = conInt;
      if (strState) {
        // If it is a string, return whether the first string contains less characters than the seccond one.
        if(strFlagA && strFlagB){
          if(strcmp(a.str,b.str) < 0){
            v.ival = 1;
          } else {
            v.ival = 0;
          }
          break;
        } else {
          printf("Both expressions should be strings!\n");
          return empty_value();
        }
      }
      else if(realState){
        // make sure to return an integer!
        if (realFlagA && realFlagB){
          v.ival = a.realval < b.realval;
          break;
        } else if (intFlagA) {
          v.ival = (float)a.ival < b.realval;
          break;
        }
        else if (intFlagB) {
          v.ival = a.realval < (float)b.ival;
          break;
        }
      }
      else {
        v.ival = a.ival < b.ival;
        break;
      }
    }
    case GT: {
      // GT always returns an integer!
      v.type = conInt;
      if (strState) {
        // If it is a string, return whether the first string contains more characters than the seccond one.
        if(strFlagA && strFlagB){
          if(strcmp(a.str,b.str) > 0){
            v.ival = 1;
          } else {
            v.ival = 0;
          }
          break;
        } else {
          printf("Both expressions must be strings!\n");
          return empty_value();
        }
      }
      else if(realState){
        // Make sure to return an int.
        if (realFlagA && realFlagB){
          v.ival = a.realval > b.realval;
          break;
        } else if (intFlagA) {
          v.ival = (float)a.ival > b.realval;
          break;
        }
        else if (intFlagB) {
          v.ival = a.realval > (float)b.ival;
          break;
        }
      }
      else {
        v.ival = a.ival > b.ival;
        break;
      }
    }
    case LTE: {
      // LTE always returns an integer!
      v.type = conInt;
      if (strState) {
        // If it is a string, return whether the first string contains less or equal number of characters than the seccond one.
        if(strFlagA && strFlagB){
          if(strcmp(a.str,b.str) <= 0){
            v.ival = 1;
          } else {
            v.ival = 0;
          }
          break;
        } else {
          printf("Both expressions should be strings!\n");
          return empty_value();
        }
      }
      else if(realState){
        // make sure to return an integer!
        if (realFlagA && realFlagB){
          v.ival = a.realval <= b.realval;
          break;
        } else if (intFlagA) {
          v.ival = (float)a.ival <= b.realval;
          break;
        }
        else if (intFlagB) {
          v.ival = a.realval <= (float)b.ival;
          break;
        }
      }
      else {
        v.ival = a.ival <= b.ival;
        break;
      }
    }
    case GTE: {
      // GTE always returns an integer!
      v.type = conInt;
      if (strState) {
        // If it is a string, return whether the first string contains more or equal number of characters than the seccond one.
        if(strFlagA && strFlagB){
          if(strcmp(a.str,b.str) >= 0){
            v.ival = 1;
          } else {
            v.ival = 0;
          }
          break;
        } else {
          printf("Both expressions should be strings!\n");
          return empty_value();
        }
      }
      else if(realState){
        // make sure to return an integer!
        if (realFlagA && realFlagB){
          v.ival = a.realval >= b.realval;
          break;
        } else if (intFlagA) {
          v.ival = (float)a.ival >= b.realval;
          break;
        }
        else if (intFlagB) {
          v.ival = a.realval >= (float)b.ival;
          break;
        }
      }
      else {
        v.ival = a.ival >= b.ival;
        break;
      }
    }
    case EQ: {
      // EQ always returns an integer!
      v.type = conInt;
      if (strState) {
        // If it is a string, return whether the first string contains equal number of characters than the seccond one.
        if(strFlagA && strFlagB){
          if(strcmp(a.str,b.str) == 0){
            v.ival = 1;
          } else {
            v.ival = 0;
          }
          break;
        } else {
          printf("Both expressions should be strings!\n");
          return empty_value();
        }
      }
      else if(realState){
        // make sure to return an integer!
        if (realFlagA && realFlagB){
          v.ival = a.realval == b.realval;
          break;
        } else if (intFlagA) {
          v.ival = (float)a.ival == b.realval;
          break;
        }
        else if (intFlagB) {
          v.ival = a.realval == (float)b.ival;
          break;
        }
      }
      else {
        v.ival = a.ival == b.ival;
        break;
      }
    }
    case NEQ: {
      // NEQ always returns an integer!
      v.type = conInt;
      if (strState) {
        // If it is a string, return whether the first string contains UNequal number of characters than the seccond one.
        if(strFlagA && strFlagB){
          if(strcmp(a.str,b.str) != 0){
            v.ival = 1;
          } else {
            v.ival = 0;
          }
          break;
        } else {
          printf("Both expressions should be strings!\n");
          return empty_value();
        }
      }
      else if(realState){
        // make sure to return an integer!
        if (realFlagA && realFlagB){
          v.ival = a.realval != b.realval;
          break;
        } else if (intFlagA) {
          v.ival = (float)a.ival != b.realval;
          break;
        }
        else if (intFlagB) {
          v.ival = a.realval != (float)b.ival;
          break;
        }
      }
      else {
        v.ival = a.ival != b.ival;
        break;
      }
    }
  }
  return v;
}
