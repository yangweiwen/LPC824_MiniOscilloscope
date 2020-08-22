#include "IIC.h"

#define SDA_High 		Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 12, true)
#define SDA_Low  		Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 12, false)
#define SCL_High 		Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 13, true)
#define SCL_Low  		Chip_GPIO_PinSetState(LPC_GPIO_PORT, 0, 13, false)
#define SDA_OUT  		Chip_GPIO_PinSetDIR(LPC_GPIO_PORT, 0, 12, 1)
#define SDA_IN   		Chip_GPIO_PinSetDIR(LPC_GPIO_PORT, 0, 12, 0)
#define SDA_State	 	Chip_GPIO_PinGetState(LPC_GPIO_PORT, 0, 12)

//define the address of devices
#define I2C_MMA8451_ADR  0X1C
#define I2C_MPU3050_ADR  0x68

/*Initialize the IIC BUS*/
void IIC_init(void)
{
	  /*SDA*/
		Chip_GPIO_PinSetDIR(LPC_GPIO_PORT, 0, 12, 1);//set PIO12 output
	  SDA_High;   //set PIO12 high logic 
	  /*SCL*/
	  Chip_GPIO_PinSetDIR(LPC_GPIO_PORT, 0, 13, 1);//set PIO13 output
	  SCL_High;   //set PIO13 high logic 
	  Board_UARTPutSTR("IIC initialize is OK!\r\n");

	  MMA8451_Init();
	  Board_UARTPutSTR("MMA8451 initialize is OK!\r\n");
	  MPU3050_Init();
		Board_UARTPutSTR("MPU3050 initialize is OK!\r\n");

}
/*The start signal of IIC BUS*/
void IIC_Start(void)
{
	  SDA_OUT;
	  
	  IIC_Delay();
	  IIC_Delay();
		/*SDA=1*/SDA_High;
	  /*SCL=1*/SCL_High;
	  IIC_Delay();
	  IIC_Delay();
	  /*SDA=0*/SDA_Low;
		IIC_Delay();
	  IIC_Delay();
	  /*SCL=0*/SCL_Low;
		IIC_Delay();
	  IIC_Delay();
}
/*The stop signal of IIC BUS*/
void IIC_Stop(void)
{
		SDA_Low;
		SCL_Low;
		IIC_Delay();
	  IIC_Delay();
	
	  SCL_High;
	  IIC_Delay();
	  IIC_Delay();
	
	  SDA_High;
    IIC_Delay();
	  IIC_Delay();
    SCL_Low;		
}

void IIC_Ack(void)
{
    SCL_Low;
	  IIC_Delay();
	
	  SDA_Low;
	  IIC_Delay();
	
	  SCL_High;
	  IIC_Delay();
	
	  SCL_Low;
	  IIC_Delay();
}

void IIC_NoAck(void)
{
	
	  SCL_Low;
	  IIC_Delay();
	
	  SDA_High;
		IIC_Delay();
	
	  SCL_High;
	  IIC_Delay();
	  
	  SCL_Low;
		IIC_Delay();
}

void IIC_SendByte(uint8_t data)
{
  
		uint8_t i=8;
		while(i--)
		{
			SCL_Low;
			if(data&0x80)
      SDA_High;
			else
      SDA_Low;
			data<<=1;
			IIC_Delay();
			SCL_High;
			IIC_Delay();
		}
		SCL_Low;
		IIC_Delay();

		SDA_High;
		IIC_Delay();
		
		SCL_High;
		IIC_Delay();
		
		SCL_Low;
}

uint8_t I2C_ReceiveByte(void)
{
  uint8_t i=8;
  uint8_t ReceiveByte=0;

  SDA_High;
	SDA_IN;
  while(i--)
  {
    ReceiveByte<<=1;
		SCL_Low;
    IIC_Delay();
		SCL_High;
		IIC_Delay();
  
    if(SDA_State)
    {
      ReceiveByte|=0x01;
    }

  }
  SDA_OUT;
  SCL_Low;

  return ReceiveByte;
}

/*Delay for 15 us*/
void IIC_Delay(void)
{
	  uint8_t t=15;
		while( t-- );
}

/*
 * I2C_WriteReg
 * 写I2C设备寄存器
 */
void I2C_WriteReg(uint8_t dev_addr,uint8_t reg_addr , uint8_t data)
{
  IIC_Start();
  IIC_SendByte(dev_addr<<1);
  IIC_SendByte(reg_addr );
  IIC_SendByte(data);
  IIC_Stop();
}

//读寄存器
uint8_t IIC_ReadByte(uint8_t dev_addr,uint8_t reg_addr)
{
  uint8_t data;
  IIC_Start();
  IIC_SendByte( dev_addr<<1);
  IIC_SendByte( reg_addr );
  IIC_Start();
  IIC_SendByte((dev_addr<<1)+1);
  data= I2C_ReceiveByte();
  IIC_NoAck();
  IIC_Stop();
  return data;
}

//读寄存器
int16_t I2C_ReadWord(uint8_t dev_addr,uint8_t reg_addr)
{
  char h,l;
  IIC_Start();
  IIC_SendByte( dev_addr<<1);
  IIC_SendByte( reg_addr);
  IIC_Start();
  IIC_SendByte((dev_addr<<1)+1);
  h= I2C_ReceiveByte();
  IIC_Ack();
  l= I2C_ReceiveByte();
  IIC_NoAck();
  IIC_Stop();
  return (h<<8)+l;
}


void I2C_ReadGryo(uint8_t dev_addr,uint8_t reg_addr,int16_t *x,int16_t *y)
{
		char h,l;
		IIC_Start();
		IIC_SendByte( dev_addr<<1);
		IIC_SendByte( reg_addr);
		IIC_Start();
		IIC_SendByte((dev_addr<<1)+1);
		h= I2C_ReceiveByte();
		IIC_Ack();
		l= I2C_ReceiveByte();
		IIC_Ack();
		*x=(h<<8)+l;
		h= I2C_ReceiveByte();
		IIC_Ack();
		l= I2C_ReceiveByte();
		IIC_Ack();
		h= I2C_ReceiveByte();
		IIC_Ack();
		l= I2C_ReceiveByte();
		IIC_NoAck();
		*y=(h<<8)+l;
		IIC_Stop();
}

int16_t Get_X_Acc(void)
{
		int16_t temp;
		temp=I2C_ReadWord(I2C_MMA8451_ADR,0X01);
		return temp;
}

int16_t Get_Y_Acc(void)
{
		int16_t temp;
		temp=I2C_ReadWord(I2C_MMA8451_ADR,0X03);
		return temp;
}
int16_t Get_Z_Acc(void)
{
		int16_t temp;
		temp=I2C_ReadWord(I2C_MMA8451_ADR,0X05);
		return temp;
}
int16_t Get_Y_Gyro(void)
{
		int16_t temp;
		temp=I2C_ReadWord(I2C_MPU3050_ADR,0X1F);
		return temp;
}

int16_t Get_X_Gyro(void)
{
		int16_t temp;
		temp=I2C_ReadWord(I2C_MPU3050_ADR,0X1D);
		return temp;
}
int16_t Get_Z_Gyro(void)
{
		int16_t temp;
		temp=I2C_ReadWord(I2C_MPU3050_ADR,0X21);
		return temp;
}

void MPU3050_Init(void)
{
		I2C_WriteReg(I2C_MPU3050_ADR,0x3E , 0X80); //复位MPU3050
		DelayMs(5);
		I2C_WriteReg(I2C_MPU3050_ADR,0x15,  0);    //采样不分频
		DelayMs(5);
		I2C_WriteReg(I2C_MPU3050_ADR,0x16 , 0x11);  //采样速率8kHz&&采样范围±1000°
		DelayMs(5);
		I2C_WriteReg(I2C_MPU3050_ADR,0x17 , 0);    //不产生中断
		DelayMs(5);
		I2C_WriteReg(I2C_MPU3050_ADR,0x3E , 0X00); //启动MPU3050
}

void MMA8451_Init(void)
{
		I2C_WriteReg(I2C_MMA8451_ADR,0x0E, 1);  //0为+-2g，1为+-4g，2为+-8g
		DelayMs(5);
		I2C_WriteReg(I2C_MMA8451_ADR,0x0F, 0X10);//使能低通滤波
		DelayMs(5);
		I2C_WriteReg(I2C_MMA8451_ADR,0x2A , 1); //0x2a为控制寄存器1 激活mma8451
		DelayMs(5);
}

void DelayMs(uint16_t ms)
{    
   uint16_t i=0;  
   while(ms--)
   {
      i=5000;
      while(i--) ;    
   }
}




