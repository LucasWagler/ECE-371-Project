#include "accelerometer.h"

void filler(void)
{
	for(int i = 0; i < PRINTLENGTH; i++)
	{
		prepData(printSeq[i], 1);
	}
	for(uint8_t i = 0; i < SENDSize; i++)
	{
		put_q(&espSendQ, Send[i]);
	}																					
	for(uint8_t i = 0; i < MSGSize; i++)
	{	
		put_q(&espMsgQ, message[i]);
	}		
	for(uint8_t i = 0; i < CLOSESize; i++)
	{
		put_q(&espCloseQ, Close[i]);
	}
	filled = true;
}

//init i2c0
void i2c_init(){
	//clock i2c peripheral and port E and port B
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	//set pins to external I2C
	PORTB->PCR[1] |= PORT_PCR_MUX(2);
	PORTB->PCR[2] |= PORT_PCR_MUX(2);
		
	//set to 100k baud
	//baud = bus freq/(scl_div+mul)
 	//~100k = 48M/(104x2); icr=0x27 sets scl_div to 480, mult of 1
 	I2C0->F = (I2C_F_ICR(0x1B) | I2C_F_MULT(0x0));
	
	//enable i2c and set to master mode
	I2C0->C1 |= (I2C_C1_IICEN_MASK );
	
	//select high drive mode
	I2C0->C2 |= (I2C_C2_HDRS_MASK);	
}

//send start sequence
void i2c_start(){
	I2C_TRAN;							/*set to transmit mode */
	I2C_M_START;					/*send start	*/
}

//////////funcs for reading and writing a single byte
//using 7bit addressing reads a byte from dev:address
uint8_t i2c_read_byte(uint8_t dev, uint8_t address){
	uint8_t data;
	i2c_start();					/*set to transmit mode */
												/*send start	*/
	I2C0->D = dev; 				/*1. send dev address	*/
	I2C_WAIT;							/*wait for completion */
	I2C0->D = address;		/*2. send read address	*/
	I2C_WAIT;							/*wait for completion */
		
	I2C_M_RSTART;					/*repeated start */
	I2C0->D = (dev|0x1);	/*3. send dev address (read)	*/
	I2C_WAIT;							/*wait for completion */
	I2C_REC;							/*set to receive mode */
	NACK;									/*set NACK after read	*/
	
	data=I2C0->D;			  	/*dummy read	*/
	I2C_WAIT;							/*wait for completion */
	
	I2C_M_STOP;						/*send stop	*/
	data = I2C0->D;			/*read data	*/

	return data;       /*return data*/
}

//using 7bit addressing writes a byte data to dev:address
void i2c_write_byte(uint8_t dev, uint8_t address, uint8_t data){
	
		i2c_start();									/*set to transmit mode & send start*/
		I2C0->D = dev;								/*send dev address	*/

		I2C_WAIT;											/*wait for ack */	
		I2C0->D = address;						/*send write address	*/

		I2C_WAIT;											/*wait for ack */		
		I2C0->D = data;								/*send data	*/
		
		I2C_WAIT;											/*wait for ack*/
		I2C_M_STOP;										/*send Stop*/	
}

void Delay (uint32_t dly) {
  volatile uint32_t t;

	for (t=dly*10000; t>0; t--)
		;
}

void Init_RGB_LEDs() {
	// Enable clock to ports B and D
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;;
	
	// Make 3 pins GPIO
	PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);          
	PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);          
	
	// Set ports to outputs
	PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PDDR |= MASK(BLUE_LED_POS);
}

void Control_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on) {
	if (red_on) {
			PTB->PCOR = MASK(RED_LED_POS);
	} else {
			PTB->PSOR = MASK(RED_LED_POS); 
	}
	if (green_on) {
			PTB->PCOR = MASK(GREEN_LED_POS);
	}	else {
			PTB->PSOR = MASK(GREEN_LED_POS); 
	} 
	if (blue_on) {
			PTD->PCOR = MASK(BLUE_LED_POS);
	}	else {
			PTD->PSOR = MASK(BLUE_LED_POS); 
	}
}	

void accelerometer_task()
{	
	//disable interrupts
	uint32_t mask;
	mask = __get_PRIMASK();
	__disable_irq();
	
	static uint32_t cmd = 0;
	static bool first = true;	
	static uint8_t data[3]={0,0,0};	
	static uint8_t data_x=0;
	static uint8_t data_y=0;
	static uint8_t data_z=0;
	
	switch (cmd)
	{
		case 0:
			data_x = i2c_read_byte(MMA_ADDR, 0x01);
			Delay(DELAYVALUE);
			data_y = i2c_read_byte(MMA_ADDR, 0x03);
			Delay(DELAYVALUE);
			data_z = i2c_read_byte(MMA_ADDR, 0x05);
			Delay(DELAYVALUE);
			
			if(first)
			{
				data[0] = data_x;
				data[1] = data_y;
				data[2] = data_z;
				first = false;
			}	
			
			if((data_x > (data[0] + SENSITIVITY)) || (data_x < (data[0] - SENSITIVITY)) ||
				(data_y > (data[1] + SENSITIVITY)) || (data_y < (data[1] - SENSITIVITY)) ||
				(data_z > (data[2] + SENSITIVITY)) || (data_z < (data[2] - SENSITIVITY))) 
			{
				Control_RGB_LEDs(0,1,0);
				filler();
				put_q(&speakerQ, (uint8_t)1);
				cmd++;
			}
			else Control_RGB_LEDs (0,0,0);
			
			data[0] = data_x;
			data[1] = data_y;
			data[2] = data_z;
			break;
		case 800000:
			cmd = 0;
			first = true;
			break;	
		default: 
			cmd++;
			break;
	}
	
	//re-enable interrupts
	__set_PRIMASK(mask);
}

/*
int main(){
	
	uint8_t CTRL_REG1_Data;
	Init_RGB_LEDs();
	Control_RGB_LEDs(0,0,0);
	i2c_init();
	i2c_write_byte(0x00, 0x00, 0x00); //Send out pilot signal
	Delay(DELAYVALUE);	
	i2c_write_byte(0x00, 0x00, 0x00); //Send out pilot signal
	Delay(DELAYVALUE);	
	i2c_write_byte(MMA_ADDR, 0x2A, 0x18); //Set the device in 100 Hz ODR, Standby
	Delay(DELAYVALUE);
	CTRL_REG1_Data = i2c_read_byte(MMA_ADDR, 0x2A);
	CTRL_REG1_Data |= 0x01;
	Delay(DELAYVALUE);
	i2c_write_byte(MMA_ADDR, 0x2A, CTRL_REG1_Data);
	Delay(DELAYVALUE);
	uint8_t IntSourceMFF = i2c_read_byte(MMA_ADDR, 0x0C);
	Delay(DELAYVALUE);
	IntSourceMFF = i2c_read_byte(MMA_ADDR, 0x16);
	Delay(DELAYVALUE);	
	static uint8_t data[3]={0,0,0};
	
	static uint8_t data_x=0;
	static uint8_t data_y=0;
	static uint8_t data_z=0;
	
	while(1)
	{
		data_x = i2c_read_byte(MMA_ADDR, 0x01);
		Delay(DELAYVALUE);
		data_y = i2c_read_byte(MMA_ADDR, 0x03);
		Delay(DELAYVALUE);
		data_z = i2c_read_byte(MMA_ADDR, 0x05);
		Delay(DELAYVALUE);
		
		if((data_x > (data[0] + SENSITIVITY)) || (data_x < (data[0] - SENSITIVITY)) ||
			(data_y > (data[1] + SENSITIVITY)) || (data_y < (data[1] - SENSITIVITY)) ||
			(data_z > (data[2] + SENSITIVITY)) || (data_z < (data[2] - SENSITIVITY))) 
		{
			Control_RGB_LEDs(0,1,0);
		}
		else Control_RGB_LEDs (0,0,0);
		
		data[0] = data_x;
		data[1] = data_y;
		data[2] = data_z;		
	}		
}
*/
