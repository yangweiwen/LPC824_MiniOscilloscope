#ifndef __IIC_H_
#define __IIC_H_

#include "chip.h"
#include "board.h"


void IIC_init(void);
void IIC_Delay(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Ack(void);
void IIC_NoAck(void);
void IIC_SendByte(uint8_t data);
uint8_t I2C_ReceiveByte(void);

void I2C_WriteReg(uint8_t dev_addr,uint8_t reg_addr , uint8_t data);
uint8_t IIC_ReadByte(uint8_t dev_addr,uint8_t reg_addr);
int16_t I2C_ReadWord(uint8_t dev_addr,uint8_t reg_addr);

void I2C_ReadGryo(uint8_t dev_addr,uint8_t reg_addr,int16_t *x,int16_t *y);
void DelayMs(uint16_t ms);

void MMA8451_Init(void);
void MPU3050_Init(void);

int16_t Get_X_Gyro(void);
int16_t Get_Y_Gyro(void);
int16_t Get_Z_Gyro(void);
int16_t Get_X_Acc(void);
int16_t Get_Y_Acc(void);
int16_t Get_Z_Acc(void);



#endif

