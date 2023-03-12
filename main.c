#include "TExaS.h"
#include "operations.h"
#include <stdio.h>

//PORT E - Key PAD COL
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PUR_R        (*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_PDR_R        (*((volatile unsigned long *)0x40024514))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
	
//PORT D - Key PAD ROW
#define GPIO_PORTD_DATA_R       (*((volatile unsigned long *)0x400073FC))
#define GPIO_PORTD_DIR_R        (*((volatile unsigned long *)0x40007400))
#define GPIO_PORTD_AFSEL_R      (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_PUR_R        (*((volatile unsigned long *)0x40007510))
#define GPIO_PORTD_DEN_R        (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_LOCK_R       (*((volatile unsigned long *)0x40007520))
#define GPIO_PORTD_CR_R         (*((volatile unsigned long *)0x40007524))
#define GPIO_PORTD_AMSEL_R      (*((volatile unsigned long *)0x40007528))
#define GPIO_PORTD_PCTL_R       (*((volatile unsigned long *)0x4000752C))

//LCD definitions - portB (display data)
#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_3_0_DATA_R       (*((volatile unsigned long *)0x4000503C))	
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_PUR_R        (*((volatile unsigned long *)0x40005510))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_LOCK_R       (*((volatile unsigned long *)0x40005520))
#define GPIO_PORTB_CR_R         (*((volatile unsigned long *)0x40005524))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
	
//LCD definitions - portA (control)
#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DATA_R_2			(*((volatile unsigned long *)0x40004010))
#define GPIO_PORTA_DATA_R_3			(*((volatile unsigned long *)0x40004020))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_PUR_R        (*((volatile unsigned long *)0x40004510))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_LOCK_R       (*((volatile unsigned long *)0x40004520))
#define GPIO_PORTA_CR_R         (*((volatile unsigned long *)0x40004524))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))


// Function Prototypes
void PLL_Init(unsigned long clockFrequency);
void PortD_Init(void);
void PortE_Init(void);
void PortB_Init(void);
void PortA_Init(void);
void InitDisplayPort(void);

void SysTick_Init(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait_Us(unsigned long us_delay);


//lcd
void SendDisplayNibble(unsigned int reg, char nibble);
void SendDisplayByte(unsigned int reg, char byte);
void Register_Select(unsigned int reg);
void EnPulse_us(unsigned long time);
void clearDisplayScreen();
void moveDisplayCursor(char pos);
void deleteLast();


//keypad
void clearResetInputArray();
void keyPadInit(void);
void KeyPadRead(void);
void decodeKeyPress(char);
void printKeyPad(int i, int j);
void interpretAndAppend(int i, int j);
void printResult(double result, unsigned int cursorPos);
void calculate();
void clearDisplayManual();

//Global Variables
unsigned int calculateFlag = 0;
double ans = 0.0;
unsigned int shift = 0;
float currentValue = 0;
char inputArray[20];
unsigned int inputPos = 0;
char resultBuffer[20];
double resultOut = 0.0;
int convert;

//Function look up tables
unsigned  char functionLookup1[4][4] = {{ '1', '2', '3', '+'},  // +    
																				{ '4', '5', '6', '-'},  // -
																				{'7', '8', '9', '.'},  // .  
																				{ '=', '0', '<', '^'}}; //back space, shift
			
unsigned  char functionLookup2[4][4] = {{ '1', '2', '3', 'x'},  // x    
																				{ '4', '5', '6', '/'},  // /
																				{'7', '8', '9', 'E'},  // E  
																				{ '=', '0', 'D', '^'}}; //back space, deshift
			
//Display look up tables																						
unsigned  char displayLookup1[4][4] = {{0x31, 0x32, 0x33, '+'},  // +    
																				{ 0x34, 0x35, 0x36, '-'},  // -
																				{ 0x37, 0x38, 0x39, 0x2E},  // .  
																				{ 0x00, 0x30, 0x00, 0x00}}; //back space, shift
			
unsigned  char displayLookup2[4][4] = {{ 0x31, 0x32, 0x33, 'x'},  // +    
																				{ 0x34, 0x35, 0x36, '/'},  // -
																				{ 0x37, 0x38, 0x39, 'E'},  // .  
																				{ 0x00, 0x30, 0x00, 0x00}}; //back space, de shift		

 int main(void){    
	PLL_Init(0x00000540);
	PortB_Init();
	PortA_Init();
	keyPadInit();
	SysTick_Init();
	SysTick_Wait_Us(10000);
	InitDisplayPort();
	SysTick_Wait_Us(10000);
	clearResetInputArray();
	KeyPadRead();
}


//Port D, Keypad output
void PortD_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000008;     // 1) D clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTD_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTD_CR_R = 0x0F;           // allow changes to PF3-0       
  GPIO_PORTD_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTD_PCTL_R = 0x00;   // 4) GPIO clear bit PCTL  
  GPIO_PORTD_DIR_R = 0x0F;          // 5) PD3:0, output
  GPIO_PORTD_AFSEL_R = 0x00;        // 6) no alternate function   
	GPIO_PORTD_PUR_R = 0x00; 					//disable pull up
  GPIO_PORTD_DEN_R = 0x0F;          // 7) enable digital pins PF3-PF0        
}

//Port E, Keypad input
void PortE_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;     // 1) E clock
  delay = SYSCTL_RCGC2_R;           // delay    
  GPIO_PORTE_CR_R = 0x0F;           // allow changes to PF4-0       
  GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTE_PCTL_R = 0x00;   			// 4) GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R = 0x00;          // 5) PE3:0, input  
  GPIO_PORTE_AFSEL_R = 0x00;        // 6) no alternate function         // disable pullup     
	GPIO_PORTE_PDR_R = 0x0F;						//enable pulldown
  GPIO_PORTE_DEN_R = 0x0F;          // 7) enable digital pins PF4-PF0        
}

//Port B, LCD Data out
void PortB_Init(void) {
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x02;     		// 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   // 2) unlock PortF PF0  
  GPIO_PORTB_CR_R = 0x0F;           // allow changes to PF4-0       
  GPIO_PORTB_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTB_PCTL_R = 0x00;   // 4) GPIO clear bit PCTL  
  GPIO_PORTB_DIR_R = 0x0F;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTB_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTB_PUR_R = 0x00;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTB_DEN_R = 0x0F;  
}

//Port A, register select & set
void PortA_Init(void) {
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x01;     	// 1) A clock
  delay = SYSCTL_RCGC2_R;           // delay   // 2) unlock PortF PF0  
  GPIO_PORTA_CR_R = 0x0C;           // allow changes to PB2-3       
  GPIO_PORTA_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTA_PCTL_R = 0x00;   // 4) GPIO clear bit PCTL  
  GPIO_PORTA_DIR_R = 0x0C;          // 5) PB2, PB3, output  
  GPIO_PORTA_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTA_PUR_R = 0x00;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTA_DEN_R = 0x0C;  
}

void Register_Select(unsigned int reg) {
	// instructions
	if (reg) {
	GPIO_PORTA_DATA_R_3 = 0x08;
	//data
	} else {
	GPIO_PORTA_DATA_R_3 = 0;
	}
}

void SendDisplayNibble(unsigned int reg, char nibble) {
	Register_Select(reg);
	SysTick_Wait_Us(150);
	GPIO_PORTB_3_0_DATA_R = nibble & 0x0F;
	EnPulse_us(1);
}

void SendDisplayByte(unsigned int reg, char byte) {
	//LHS
	if (byte != 0x00) {
		SendDisplayNibble(reg, byte >> 4);
		//RHS
		SendDisplayNibble(reg, byte);
	}
	SysTick_Wait_Us(150);
}

void clearDisplayScreen() {
  SendDisplayByte(0, 0x01);
  //SysTick_Wait_Us(1000000);
	SysTick_Wait_Us(10000);
}


void InitDisplayPort(void) {
	SysTick_Wait_Us(20000);
	SendDisplayNibble(0, 0x03);
	SysTick_Wait_Us(5000);
	SendDisplayNibble(0, 0x03);
	SysTick_Wait_Us(150);
	SendDisplayNibble(0, 0x03);
	SysTick_Wait_Us(150);
	
	//1.set to 4 bit mode 
	SendDisplayNibble(0, 0x02);
	SysTick_Wait_Us(150);
	
	// 2.set font, function set - dimensons
	SendDisplayByte(0, 0x28);
	SysTick_Wait_Us(150);
	
	//3. Enable cursor & cursor blink.
	SendDisplayByte(0, 0x0E);
	SysTick_Wait_Us(150);

	//4. clear display screen
	clearDisplayScreen();
	
	//5. entry mode set 
	SendDisplayByte(0, 0x06);
	SysTick_Wait_Us(150);
	
	//Move cursor to input
	moveDisplayCursor(16);
	
}

void moveDisplayCursor(char pos) {
		unsigned  char DDRAM[34] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
																0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
																//32 = right shift, 33 = left shift		
																0x14, 0x10};
		
		SendDisplayByte(0, DDRAM[pos]);
	}

void deleteLast() {
	moveDisplayCursor(33);
	SendDisplayByte(0, 0x04); //
	SendDisplayByte(1, 0x10);
	SendDisplayByte(0, 0x06);
	moveDisplayCursor(32);
}

void clearDisplayManual() {
	for (int i = 31; i >= 0; i = i - 1) { 
	moveDisplayCursor(i);
	SendDisplayByte(0, 0x04); 
	SendDisplayByte(1, 0x10);
}
	SendDisplayByte(0, 0x06);
}

void printResult(double resultprint, unsigned int cursorPos) {
	moveDisplayCursor(cursorPos);
	SysTick_Wait_Us(1000000);
	
	//decimal management - remove zeros round to 6 decimal places
	unsigned int numberFlag = 0;
	unsigned int decCounter = 0;
	convert = snprintf(resultBuffer, 20, "%0.6f", resultprint);
	
	for (int i = convert; i >= 0; i = i - 1) {
      if ((resultBuffer[i - 1] != '0') && (numberFlag == 0)) {
        numberFlag = 1;
        decCounter = i;
        }
      }
    if (resultBuffer[decCounter - 1 ] == '.') {
      decCounter = decCounter - 1;
    } 
		
	//screen printing
  for (int i = 0; i < decCounter; i ++) {
		switch (resultBuffer[i]) {
			case '0':
				SendDisplayByte(1, 0x30);
				break;
			case '1':
				SendDisplayByte(1, 0x31);
				break;
			case '2':
				SendDisplayByte(1, 0x32);
				break;
			case '3':
				SendDisplayByte(1, 0x33);
				break;
			case '4':
				SendDisplayByte(1, 0x34);
				break;
			case '5':
				SendDisplayByte(1, 0x35);
				break;
			case '6':
				SendDisplayByte(1, 0x36);
				break;
			case '7':
				SendDisplayByte(1, 0x37);
				break;
			case '8':
				SendDisplayByte(1, 0x38);
				break;
			case '9':
				SendDisplayByte(1, 0x39);
				break;
			case '-':
				SendDisplayByte(1, 0x2D);
				break;
			case '.':
				SendDisplayByte(1, 0x2E);
				break;
			case 'E':
				SendDisplayByte(1, 0x45);
			break;
			case '+':
				SendDisplayByte(1, 0x2B);
			break;
			default:
				break;
			}		
  }
	memset(resultBuffer, 0x00, 20);
	SysTick_Wait_Us(10000);
}


void EnPulse_us(unsigned long time) {
	GPIO_PORTA_DATA_R_2 = 0x04;
	SysTick_Wait_Us(time);
	GPIO_PORTA_DATA_R_2 = 0;
}

void keyPadInit() {
	PortD_Init();
	PortE_Init();
	shift = 0;
};

void KeyPadRead() {
	while (1) {
		//Pulse columns
		for (int i = 0; i < 4; i++) {
			GPIO_PORTD_DATA_R = (1<<i);
			SysTick_Wait_Us(2000);
			
			//Read rows & call functions
			for (int j =0; j <4; j++) {
				if(GPIO_PORTE_DATA_R & (1<<j)) {
					interpretAndAppend(i,j);
					SysTick_Wait_Us(20000);
					printKeyPad(i, j);
					SysTick_Wait_Us(1000000);
				}
			}
		}
		SysTick_Wait_Us(2000);
	}
};

void clearResetInputArray() {
					inputPos = 0;
					memset(inputArray, 0x00, 20);				
}

void calculate() {
					clearDisplayManual();
					resultOut = processInput(inputArray, ans, calculateFlag);
					ans = resultOut;
					printResult(resultOut, 0);
					moveDisplayCursor(16);
					SysTick_Wait_Us(1000);
					clearResetInputArray();
					calculateFlag = 1;
				}

				
void interpretAndAppend(int i, int j) {
													
switch (shift) {
		case 0:
			switch (functionLookup1[j][i]) {
				case '=': 
					calculate();
					break;
				case '<': //delete
				clearDisplayManual();
				clearResetInputArray();
				moveDisplayCursor(16);
				SysTick_Wait_Us(1000);
				ans = 0.0;
				calculateFlag = 0;
					break;
				case '^': //shift
					shift = 1;
					SysTick_Wait_Us(10000);
					break;
				default:
					inputArray[inputPos] = functionLookup1[j][i];
					inputPos = inputPos + 1;
					break;		
			}
			break;

		case 1:
		switch (functionLookup2[j][i]) {
			case '=': 
				calculate();
				break;
			case 'Q': 
				clearDisplayScreen();
				SysTick_Wait_Us(100000);
				break;
			case 'D': //backspace
					deleteLast();
					if (inputPos > 0) {
						inputPos = inputPos - 1;
						inputArray[inputPos] = 0;
					}
				break;
			case '^': // de-shift
				shift = 0;
				SysTick_Wait_Us(10000);
				break;
			default:
					inputArray[inputPos] = functionLookup2[j][i];
					inputPos = inputPos + 1;
					break;		
		}
		break;
}
}

void printKeyPad(int i, int j) {																	
switch (shift) {
	case 0:
		SendDisplayByte(1, displayLookup1[j][i]);
		break;
	case 1:
		SendDisplayByte(1, displayLookup2[j][i]);
		break;
}																					
}


