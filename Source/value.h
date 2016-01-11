#ifndef VALUE_HEADER_FILE
#define VALUE_HEADER_FILE

// Define an enum for constant types.
typedef enum { conInt, conReal, conString } conType;

// The Values supported by the language.
typedef struct value {
  conType type; // The type of the value.
  // Union for the values. forcing it to hold only one value at a time.
  union {
    int ival; // Integer value.
    float realval; // Real number (float) value.
    char *str; // String value.
  };
} Value;

#endif
