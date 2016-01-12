#include <p18cxxx.h>
#include<delays.h>

char COUNT;             // Counter available as local to functions
char RPGCNT;            // Used to display RPG changes

/*******************************
 * Constant strings
 ********************************/

// For stability reasons, create an EVEN number of elements in any given array
const char LCDstr[]  = {0x33,0x32,0x28,0x01,0x0c,0x06,0x00,0x00};// LCD Initialization string
const char Clear1[]  = {0x80,' ',' ',' ',' ',' ',' ',' ',' ',0};// Clear line 1
const char Clear2[]  = {0xC0,' ',' ',' ',' ',' ',' ',' ',' ',0};// Clear line 2


/*******************************
 * Function prototypes
 ********************************/

void Initial(void);
void InitLCD(void);
void DisplayC(const char *);
void Delay();
void arithmetic(unsigned char * displayString, unsigned char result);
unsigned short calcDenominator(unsigned short denom, unsigned char numBits);
void DisplayFloat(float ADReading, unsigned char * displayString);

void main()
{

	unsigned char displayString1[] = {0x80,' ',' ',' ',' ',' ',' ',' ',' ',0x00}; //1st line
	unsigned char displayString2[] = {0xC0,' ',' ',' ',' ',' ',' ',' ',' ',0x00}; //2nd line
	unsigned char u;
	unsigned char l;
/***************************************************************************/
	unsigned short denominator=1;
	//unsigned char i=0;
	unsigned char numBits=10; //register size of AD converter is 10 bits
	//unsigned char upper = 3; //upper is 00000011
	//unsigned char lower = 200; //lower is 11001000
	float ADReading; //combination of upper and lower bytes 10 bit wide max //may need p change to short
	unsigned char myString[8]={0}; //used for printing
	unsigned char range=5; //0V to 5V input
	float sigOutVolt; //actual decimal reading in volts


/**************************************************************************/
    Initial();  
    
	//TRISC=0;			
  	TRISD=0; //may have to change to 1			
  	TRISEbits.TRISE2=1;	//AN7 or RE2
  	ADCON0 = 0xB9; 	
  	ADCON1 = 0xCE; 		
					
  	while(1)
    	{
      Delay();
	
      ADCON0bits.GO = 1; 	
      while(ADCON0bits.DONE == 1);

	  Delay();
/************************************************************************/
		ADReading = (ADRESH << 8) + ADRESL; //combinind upper and lower bits
		denominator= calcDenominator(denominator,numBits); //calculate denominator 2^10	
		ADReading = ((ADReading * range)/denominator) - (range/2);
		DisplayFloat(ADReading,displayString1);
		
		/////printing result
		Delay();
		DisplayC(displayString1);
		Delay();
/************************************************************************/
      //PORTC=ADRESL;		
      //PORTD=ADRESH;
	  //DisplayC(Clear1);
	  Delay(); //Delay for clearing
     // DisplayC(Clear2);



      Delay();		
    	}

}
/*******************************
 * Initial()
 *
 * This function performs all initializations of variables and registers.
 ********************************/

void Initial()
{
    ADCON1 = 0b10001110;            // Enable PORTA & PORTE digital I/O pins
    TRISA  = 0b11100001;            // Set I/O for PORTA
    TRISB  = 0b11011100;            // Set I/O for PORTB
    TRISC  = 0b11010000;            // Set I/0 for PORTC
    TRISD  = 0b00001111;            // Set I/O for PORTD
    TRISE  = 0b00000000;            // Set I/O for PORTE
    PORTA  = 0b00010000;            // Turn off all four LEDs driven from PORTA
    RPGCNT   = 0;                   // Clear counter to be displayed
    InitLCD();                      // Initialize LCD
}

/*******************************
 * InitLCD()
 *
 * Initialize the Optrex 8x2 character LCD.
 * First wait for 0.1 second, to get past display's power-on reset time.
 ********************************/
void InitLCD()
{
    char currentChar;
    char *tempPtr;
    COUNT = 10;
    while (COUNT)
    {
        Delay();
        COUNT--;
    }
    PORTEbits.RE0 = 0;              // RS=0 for command
    tempPtr = LCDstr;
    while (*tempPtr)                // if the byte is not zero
    {
        currentChar = *tempPtr;
        PORTEbits.RE1 = 1;            // Drive E pin high
        PORTD = currentChar;          // Send upper nibble
        PORTEbits.RE1 = 0;            // Drive E pin low so LCD will accept nibble
        Delay();
        currentChar <<= 4;            // Shift lower nibble to upper nibble
        PORTEbits.RE1 = 1;            // Drive E pin high again
        PORTD = currentChar;          // Write lower nibble
        PORTEbits.RE1 = 0;            // Drive E pin low so LCD will process byte
        Delay();
        tempPtr++;                    // Increment pointerto next character
    }
}

/*******************************
 * DisplayC(const char *)
 *
 * This function is called with the passing in of an array of a constant
 * display string.  It sends the bytes of the string to the LCD.  The first
 * byte sets the cursor position.  The remaining bytes are displayed, beginning
 * at that position.
 * This function expects a normal one-byte cursor-positioning code, 0xhh, or
 * an occasionally used two-byte cursor-positioning code of the form 0x00hh.
 ********************************/

void DisplayC(const char * tempPtr)
{
    char currentChar;
    PORTEbits.RE0 = 0;              // Drive RS pin low for cursor-positioning code
    while (*tempPtr)                // if the byte is not zero
    {
        currentChar = *tempPtr;
        PORTEbits.RE1 = 1;            // Drive E pin high
        PORTD = currentChar;          // Send upper nibble
        PORTEbits.RE1 = 0;            // Drive E pin low so LCD will accept nibble
        currentChar <<= 4;            // Shift lower nibble to upper nibble
        PORTEbits.RE1 = 1;            // Drive E pin high again
        PORTD = currentChar;          // Write lower nibble
        PORTEbits.RE1 = 0;            // Drive E pin low so LCD will process byte
        Delay();
        PORTEbits.RE0 = 1;            // Drive RS pin high for displayable characters
        tempPtr++;                    // Increment pointerto next character
    }
}

/******************************************************************************
	int Delay()
	This function is called to create a .05 sec delay multiplied
	by the integer given to it to make a specific delay time
 ******************************************************************************/
void Delay()
{
    int delay = 8; //changed from 2 to 8
    int index = 0;
    int delayTimeInt = 0;
    double delayTimeDouble = 0.0;
    
    for( index = 0; index < delay; index++ )
    {
        Delay100TCYx(1250);		// each delay cost .05 of a second
        delayTimeDouble += .05;
    }
    
    
}

/******************************************************************************
	void arithmetic()
	sets up string for arithmetic operation to be displayed
 ******************************************************************************/
void arithmetic(unsigned char * displayString, unsigned char result){
    unsigned char i= 3;
    while(result> 0){
        displayString[i] = result % 10;
        displayString[i] += 0x30; //asdd 30 for ascii
        result /= 10;
        i = i -1;
    }
	return;
}

/******************************************************************************
	void calcDenominator()
	calculates denominator the reading of A/D Converter is to be borken up into denominator of parts
 ******************************************************************************/
unsigned short calcDenominator(unsigned short denom, unsigned char numBits){
	char i;
	for(i=0;i<numBits;i++){
		denom= denom *2;
	}
	return denom;
}

void DisplayFloat(float ADReading, unsigned char * myString){

myString[1] = 0;
myString[2] = '.';
myString[3] = 0;
myString[4] = 0;
//myString[4] = 0;
 
while ( ADReading > 1.0 ) {
  myString[1]++;
  ADReading -= 1.0;
}

while ( ADReading > 0.1 ) {
    myString[3]++;
    ADReading -= 0.1;
}

while ( ADReading > 0.01 ) {
   myString[4]++;
   ADReading -= 0.01;
}
 
myString[1] += 0x30;
myString[3] += 0x30;
myString[4] += 0x30;


}
