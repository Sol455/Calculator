#include "operations.h"

struct symbol opTempA[20];
struct symbol opTempB[20];
struct symbol processTempA[20];
struct symbol processTempB[20];

void parseInput (char input[20], struct symbol output[20], int calculateFlag) {
	
  int outputArrayPos = 0;
  memset(output, 0x00, sizeof(struct symbol) * 20);
  int i = 0;

  while(i < 20) {
		
    if ((input[i] >= ASCII_0 && input[i] <= ASCII_9) || ((i == 0) && (input[i] == ASCII_MINUS) && (calculateFlag == 0))) {	
      char* pointer = &input[i];
			int counter = 0;
			output[outputArrayPos].value = strToFloatWithE(pointer, &counter);
			i += counter;
      output[outputArrayPos].operator = 0;	
		
    } else if (input[i] == 0){
      break;
			
    } else { 	
    output[outputArrayPos].operator = input[i];
    output[outputArrayPos].value = 0;			
    i++;	
    }
    outputArrayPos++;
	}
}

void divide(struct symbol input[20], struct symbol output[20]) {
  doOperation(input, output, ASCII_DIV);
}

void multiply(struct symbol input[20], struct symbol output[20]) {
  doOperation(input, output, ASCII_MULT);
}

void add(struct symbol input[20], struct symbol output[20]) {
  doOperation(input, output, ASCII_ADD);
}

void subtract(struct symbol input[20], struct symbol output[20]) {
  doOperation(input, output, ASCII_SUBTRACT);
}

void doOperation(struct symbol input[20], struct symbol output[20], char operator) {

  int operationCount = 0;
  int toggleArrayBool = 0;
	
  //count number of times operator appears
  for (int i = 0; i < 20; i++) {
    //copy input array to tempA
    opTempA[i].operator = input[i].operator;
    opTempA[i].value = input[i].value;
    //check if operator appears
    if (input[i].operator == operator) {
      operationCount ++;
    }
  } 
	//Find and execute operation 
  for (int j = 0; j < operationCount; j = j + 1) {
    switch (toggleArrayBool) {
      case 0:
        findAndExecute(opTempA, opTempB, operator);
        toggleArrayBool = 1;
        break;

      case 1:
        findAndExecute(opTempB, opTempA, operator);        
        toggleArrayBool = 0;
        break;
      }
    }
		
    //Copy to output
    for (int i = 0; i < 20; i++) { 
      switch (toggleArrayBool) {      
        case 0:
              output[i].operator = opTempA[i].operator;
              output[i].value = opTempA[i].value;
              break;
        case 1:
              output[i].operator = opTempB[i].operator;
              output[i].value = opTempB[i].value;
              break;
       } 
      } 
		
}

//FIND operator for an array, do it and return remainding array
void findAndExecute(struct symbol input[20], struct symbol output[20], char operator) {
  
  int outputArrayPos = 0;
  int i = 0;
  int calculatedBool = 0;
	
  memset(output, 0x00, sizeof(struct symbol) * 20);

  while(i < 20) {

    if((calculatedBool == 0) && (input[i].operator == operator)){ 
        double valueOne = input[i -1].value;
        double valueTwo = input[i +1].value;
        double result;

        //Do operation
        switch (operator) {
          case ASCII_DIV:
            result = valueOne / valueTwo;
            break;
          case ASCII_MULT:
            result = valueOne * valueTwo;
           break;
         case ASCII_ADD:
            result = valueOne + valueTwo;
            break;
         case ASCII_SUBTRACT:
            result = valueOne - valueTwo;
            break;
        }
      
        output[outputArrayPos -1].value = result;
        i = i + 2;
        calculatedBool = 1;
      
      } else {
      output[outputArrayPos].operator = input[i].operator;
      output[outputArrayPos].value = input[i].value;
      i++;
      outputArrayPos++;
      }   
    }
}


double processInput(char input[20], double ans, int calculateFlag) {
	//concatenate char array into struct array of values (doubles) and operators (chars)
	  parseInput(input, processTempA, calculateFlag);
	
		//Add previous answer start of array
		if(calculateFlag == 1) {
		for(int i = 0; i < 19; i ++)	{
			processTempA[19 - i].value = processTempA[18 - i].value;
			processTempA[19 - i].operator = processTempA[18 - i].operator;
		}
		processTempA[0].value = ans;
		processTempA[0].operator = 0;
	}
		
	//iterate and execute according to order of operations
  divide(processTempA, processTempB);
  multiply(processTempB, processTempA);
  add(processTempA, processTempB);
  subtract(processTempB, processTempA);
  return (double)processTempA[0].value;
}

double ASCII_Conv(int number, int *decCount, double value) {
  if (*decCount == 0) {
    value = value * 10 + number;
  } else {
    value = value + (number / pow(10, (*decCount)));
    *decCount = *decCount + 1;
  }
  return value;
}

double strToFloatWithE(char *input, int* count) {
  int counter  = 0;
  double value = strToFloat(input, &counter);
  char *next = input + counter;
  if(*next == 69){
    int expCount = 0; 
    double exp = strToFloat(next+1, &expCount);
    *count = counter + expCount + 1;
    return value * pow(10, exp);
    
  } else {
    *count = counter;
    return value;
  }  
}

double strToFloat(char *input, int* count) {
  double value = 0;
  int counter = 0;
  int decCount = 0;
  int neg = 1;
  char *p = input;
  while (1) {
    char v = *p;
    switch (v) {
    case 48:
      value = ASCII_Conv(0, &decCount, value);
      break;
    case 49:
      value = ASCII_Conv(1, &decCount, value);
      break;
    case 50:
      value = ASCII_Conv(2, &decCount, value);
      break;
    case 51:
      value = ASCII_Conv(3, &decCount, value);
      break;
    case 52:
      value = ASCII_Conv(4, &decCount, value);
      break;
    case 53:
      value = ASCII_Conv(5, &decCount, value);
      break;
    case 54:
      value = ASCII_Conv(6, &decCount, value);
      break;
    case 55:
      value = ASCII_Conv(7, &decCount, value);
      break;
    case 56:
      value = ASCII_Conv(8, &decCount, value);
      break;
    case 57:
      value = ASCII_Conv(9, &decCount, value);
      break;
      // decimal
    case 46:
      decCount = 1;
      break;
    case 45:
      if(counter == 0) {
        neg = -1;
      } else {
      *count = counter;
      return value * neg;           
      }
      break;
    default:
      *count = counter;
      return value * neg;
    }
    p++;
    counter++;
  }
}