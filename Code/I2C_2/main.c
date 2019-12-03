#include <MKL25Z4.H>
#include <math.h>
#include <stdio.h>
#include <stdint.h>

//configure I2C control register 1; I2C0->C1 to select master mode 
//(for details, see I2C register mask in mkl25z4.h file)
#define MMA_ADDR 0x3A //Accel. device address 0x3A

#define I2C_M_START 	I2C0->C1 |= I2C_C1_MST_MASK                     // Select Mater Mode (Start) logic '1' indicates START otherwise STOP
#define I2C_M_STOP 		I2C0->C1 &= ~I2C_C1_MST_MASK  	                                          // Select Master Mode (Stop) 
#define I2C_M_RSTART 	I2C0->C1 |= I2C_C1_RSTA_MASK                    // Select Repeat Start

//configure I2C register "I2C_C1_TX_MASK" to allow I2C bus to transmit sensor data; logic '1' to transmit mode, otherwise receiving mode 
#define I2C_TRAN			I2C0->C1 |= I2C_C1_TX_MASK                      // I2C bus to Transmit
#define I2C_REC 			I2C0->C1 &= ~I2C_C1_TX_MASK		                                          //I2C bus to receive

#define BUSY_ACK 	    while(I2C0->S & 0x01)                            //I2C Bus Busy
#define TRANS_COMP		while(!(I2C0->S & 0x80))                         //I2C Bus data transafer Complete
																																			 //Enable Interrupt waiting flag
#define I2C_WAIT			while((I2C0->S & I2C_S_IICIF_MASK)==0) {} \
                                 I2C0->S |= I2C_S_IICIF_MASK;
	
#define NACK 	        I2C0->C1 |= I2C_C1_TXAK_MASK                      //Acknolegement of data NOT received
#define ACK           I2C0->C1 &= ~I2C_C1_TXAK_MASK                     //Acknolegement of data received
	
#define RED_LED_POS (18)		// on port B
#define GREEN_LED_POS (19)	// on port B
#define BLUE_LED_POS (1)		// on port D
	
// basic light switch 
#define LED1_POS (1)	// on port A
#define LED2_POS (2)	// on port A
#define SW1_POS (5)		// on port A

#define MASK(x) (1UL << (x))

// function prototypes
void Init_RGB_LEDs(void);
void Control_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on);


//init i2c0
void i2c_init(void){
	//clock i2c peripheral and port E and port B
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	
	//set pins to I2C function
	//PORTE->PCR[24] |= PORT_PCR_MUX(5);
	//PORTE->PCR[25] |= PORT_PCR_MUX(5);
	//set pins to external I2C to view
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

void interrupt_init(void){
	
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[14] |= PORT_PCR_MUX(1);
	GPIOA->PDDR |= !(1UL << 14); 
	PORTA->PCR[1] |= PORT_PCR_MUX(1);
	GPIOA->PDDR |= (1UL << 1); 
	
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

void Init_RGB_LEDs(void) {
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

int main(){
	
	uint8_t CTRL_REG1_Data;
	uint8_t IntSourceMFF;
	Init_RGB_LEDs();
	Control_RGB_LEDs(0,0,0);
	interrupt_init();
	i2c_init();
	i2c_write_byte(0x00, 0x00, 0x00); //Send out pilot singal
	Delay(10);	
	i2c_write_byte(0x00, 0x00, 0x00); //Send out pilot singal
	Delay(10);	
	i2c_write_byte(MMA_ADDR, 0x2A, 0x18); //Set the device in 100 Hz ODR, Standby
	Delay(10);
	/*i2c_write_byte(MMA_ADDR, 0x15, 0xD8); //Config reg for motion detection
	Delay(10);
	i2c_write_byte(MMA_ADDR, 0x17, 0x30); //Set threshold value of > 3g (step count is 0.063g/count)
	//Set the debouce counter to eliminate false readings for 100Hz sample rate
	Delay(10);
	i2c_write_byte(MMA_ADDR, 0x18, 0x0A); //100ms/10ms (steps) = 10 counts
	Delay(10);
	i2c_write_byte(MMA_ADDR, 0x2D, 0x04); //Enable motion interrupt function
	Delay(10);
	i2c_write_byte(MMA_ADDR, 0x2E, 0x04); //Route interrupt function to INT1 (PTA14)
	//Put the device in active mode
	//Delay(1000);*/
	//i2c_write_byte(MMA_ADDR, 0x2A, 0x19); //Put device in active mode
	//Delay(10);
	CTRL_REG1_Data = i2c_read_byte(MMA_ADDR, 0x2A);
	CTRL_REG1_Data |= 0x01;
	Delay(10);
	i2c_write_byte(MMA_ADDR, 0x2A, CTRL_REG1_Data);
	
	Delay(100);
	IntSourceMFF = i2c_read_byte(MMA_ADDR, 0x0C);
	Delay(100);
	IntSourceMFF = i2c_read_byte(MMA_ADDR, 0x16);
	Delay(100);
	
	
	
	static uint8_t data[3]={0,0,0};
	
	static uint8_t data_x=0;
	static uint8_t data_y=0;
	static uint8_t data_z=0;
	
	while(1){
		
		
		
		//if((PTA->PDIR & (1UL << 14)) > 1) PTA->PSOR |= (1UL << 1);
		//else PTA->PCOR |= (1UL << 1);
		
		/*for(uint8_t x = 0x00; x < 0xFF; x++){
			
			i2c_write_byte(x, 0x2A, 0x19); //Put device in active mode
			
			Delay(500);
			
		}*/
		
		data_x = i2c_read_byte(MMA_ADDR, 0x01);
		Delay(10);
		//data_y = i2c_read_byte(MMA_ADDR, 0x03);
		//Delay(10);
		//data_z = i2c_read_byte(MMA_ADDR, 0x05);
		//Delay(10);
		
		//if((data_x != data[0]) || (data_y != data[1]) || (data_z != data[2])) Control_RGB_LEDs(0,1,0);
		if((data_x > (data[0] + 5) || data_x < (data[0] - 5))) Control_RGB_LEDs(0,1,0);
		else Control_RGB_LEDs (0,0,0);
		
		data[0] = data_x;
		data[1] = data_y;
		data[2] = data_z;
		
	}	
		
}
