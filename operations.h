#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ASCII_ADD 43
#define ASCII_SUBTRACT 45
#define ASCII_MULT 120
#define ASCII_DIV 47
#define ASCII_POINT 46
#define ASCII_MINUS 45
#define ASCII_E 69
#define ASCII_0 48
#define ASCII_9 57

struct symbol {
   char  operator;
   double  value;
};

void parseInput(char input[20], struct symbol output[20], int calculateFlag);
void doOperation(struct symbol input[20], struct symbol output[20], char operator);
void divide(struct symbol input[20], struct symbol output[20]);
void multiply(struct symbol input[20], struct symbol output[20]);
void add(struct symbol input[20], struct symbol output[20]);
void subtract(struct symbol input[20], struct symbol output[20]);
double processInput(char input[20], double ans, int calculateFlag);
void findAndExecute (struct symbol input[20], struct symbol output[20], char operator);
double strToFloat(char *input, int* count);
double ASCII_Conv(int number, int *decCount, double value);
double strToFloatWithE(char *input, int* count);

#endif