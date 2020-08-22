#include <stdio.h>
#include <math.h>
#include "board.h"
#include "oled.h"
#include "bmp.h"
#include "delay.h"
#include "ADC.h"
#include "key_PINT.h"
#include "main.h"
#include "Adafruit_ZeroFFT.h"

#define TICKRATE_HZ (100)	/* 100 ticks per second */
#define SAVE_ITEMS  (128)
#define SCOPE_ABS(x,y) (((x)>(y))?((x)-(y)):((y)-(x)))

extern bool sequenceComplete, thresholdCrossed;

uint32_t ADC_Buf[256];
int16_t ADC_TransBuf[128];
uint8_t keyState = 0xff;
uint8_t DMA_IntA_Flag, DMA_IntB_Flag;
uint16_t ConvData;

SCOPE_CTRL g_scopeCtrl;
char *g_pVolScaleName[MAX_VOLT_SCALE] = {"0.1v", "0.4v", "0.8v", "1.0v"};
char *g_pTimeScaleName[MAX_TIME_SCALE] = {"10us", "20us", "50us", "100u", "200u", "500u", "1.0m", "2.0m", "2.5m"};

const char *g_miniScopeVersion = "v1.0.1";

/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
void SysTick_Handler(void)
{
    #define MAX_KEY_COUNT (10)
    static uint8_t keyDelay[5] = {0, 0, 0, 0, 0};
    uint8_t i;

    for(i = 0; i < 5; i++)
    {
        if(keyIntState[i])
        {
            keyIntState[i] = 0;
            if(keyDelay[i] == 0)
            {
                keyDelay[i] = MAX_KEY_COUNT;
                //printf("key %d interrupt.\r\n", i);
            }
        }
    }    

    //延时，判断是否为低电平，消抖用
    for(i = 0; i < 5; i++)
    {
        if(keyDelay[i] != 0)
        {            
            keyDelay[i]--;
            if(keyDelay[i] == 0)
            {
                if(0 == PinGetButtonState(i))
                {
                    keyState = i;
                    //printf("key %d confirm.\r\n", i);
                }
            }
        }
    }
    
}

void SCOPE_InitVoltParam(void)
{
    uint32_t i;
    float pVolt[MAX_VOLT_SCALE] = {0.1, 0.4, 0.8, 1.0};
    
    for(i = 0; i < MAX_VOLT_SCALE; i++)
    {
        g_scopeCtrl.voltScale[i] = pVolt[i];
        g_scopeCtrl.pVoltName[i] = g_pVolScaleName[i];
        
        //计算转换系数
        g_scopeCtrl.voltRawValuePerPixel[i] =  (uint32_t)(g_scopeCtrl.voltScale[i] * 4096 / (3.3 * 12)); //4096/coef=满偏值
        g_scopeCtrl.voltRawValuePerGrad[i] = (uint32_t)(pVolt[i] * 4096 / 3.3);

        //DEBUGOUT("%d  ",g_scopeCtrl.voltDrawCoef[i]);
    }

    g_scopeCtrl.voltScaleIdx = 3;
}

void SCOPE_UpdateSampRate(void)
{
    g_scopeCtrl.sampleRate = 12000000 / g_scopeCtrl.timeScale[g_scopeCtrl.timeScaleIdx] ;
    
    if(g_scopeCtrl.sampleRate > 60000)
    {
        snprintf(g_scopeCtrl.pFFTSampRate, sizeof(g_scopeCtrl.pFFTSampRate), "%dk", g_scopeCtrl.sampleRate/1000);
        snprintf(g_scopeCtrl.pFFTDiv, sizeof(g_scopeCtrl.pFFTDiv), "%dk", g_scopeCtrl.sampleRate/1000/2/4);
        snprintf(g_scopeCtrl.pFFTMax, sizeof(g_scopeCtrl.pFFTMax), "%dk", g_scopeCtrl.sampleRate/1000/2);
    }
    else
    {
        snprintf(g_scopeCtrl.pFFTSampRate, sizeof(g_scopeCtrl.pFFTSampRate), "%d", g_scopeCtrl.sampleRate);
        snprintf(g_scopeCtrl.pFFTDiv, sizeof(g_scopeCtrl.pFFTDiv), "%d", g_scopeCtrl.sampleRate/2/4);
        snprintf(g_scopeCtrl.pFFTMax, sizeof(g_scopeCtrl.pFFTMax), "%d", g_scopeCtrl.sampleRate/2);
    }
}

void SCOPE_InitTimeParam(void)
{
    uint32_t i;
    uint32_t pTime[MAX_TIME_SCALE] = {10, 20, 50, 100, 200, 500, 1000, 2000, 2500}; //单位:us
    
    for(i = 0; i < MAX_TIME_SCALE; i++)
    {
        g_scopeCtrl.timeScale[i] = pTime[i];
        g_scopeCtrl.pTimeName[i] = g_pTimeScaleName[i];
        g_scopeCtrl.timeDivCoef[i] =  pTime[i] / 10; //ADC采样频率设置的分频系数
    }
    g_scopeCtrl.timeScaleIdx = 0;
    SCOPE_UpdateSampRate();
}

void SCOPE_InitCtrlParam(void)
{
    memset((void*)&g_scopeCtrl, 0, sizeof(SCOPE_CTRL));

    g_scopeCtrl.runState = SCOPE_STATE_M0_MAIN;
    g_scopeCtrl.runStop = SCOPE_DISPLAY_RUN;
    g_scopeCtrl.channelSelect = 0;

    SCOPE_InitVoltParam();
    SCOPE_InitTimeParam();
}

void SCOPE_UIShowWelcomePage(void)
{
    char buf[16];
    
    OLED_ShowPicture(4,3,58,32,(unsigned char*)Mini,1); 
    OLED_ShowPicture(64,4,60,32,(unsigned char*)Scope,1);
    OLED_ShowString(13,44,(unsigned char*)"NXP LPC824 INSIDE",12,1);
    snprintf(buf, sizeof(buf), "%s Jack", g_miniScopeVersion);
    OLED_ShowString(30,56,(unsigned char*)buf,8,1);
    
    OLED_Refresh(); 
}

void SCOPE_UIShowVoltGrad(int16_t startIndex)
{
    int32_t i;
    uint8_t vIndex;
    char buf[8];
    
    vIndex = g_scopeCtrl.voltScaleIdx;
    for(i = 0; i < 5; i++)
    {
        snprintf(buf, sizeof(buf), "%1.1f", g_scopeCtrl.voltScale[vIndex] * (startIndex + i));
        OLED_ShowString(110, (4 - i) * 12, (unsigned char *)buf, 8, 1);
    }
}

void SCOPE_UIShowFFTInfo(int16_t maxMag)
{
    #define X_TITLE     (68)
    #define X_CONTENT   (94)
    #define Y_MAG       (4)
    #define Y_DIV       (20)
    #define Y_MAX       (30)
    #define Y_SMP       (40)

    char frqBuf[16];

    OLED_ShowString(X_TITLE, Y_MAG, "Mag:", 8, 1);
    snprintf(frqBuf, sizeof(frqBuf), "%d", maxMag);
    OLED_ShowString(X_CONTENT, Y_MAG, (unsigned char*)frqBuf, 8, 1);
    
    OLED_ShowString(X_TITLE, Y_DIV, "Div:", 8, 1);
    OLED_ShowString(X_CONTENT, Y_DIV, (unsigned char*)g_scopeCtrl.pFFTDiv, 8, 1);

    OLED_ShowString(X_TITLE, Y_MAX, "Max:", 8, 1);
    OLED_ShowString(X_CONTENT, Y_MAX, (unsigned char*)g_scopeCtrl.pFFTMax, 8, 1);
    
    OLED_ShowString(X_TITLE, Y_SMP, "Smp:", 8, 1); 
    OLED_ShowString(X_CONTENT, Y_SMP, (unsigned char*)g_scopeCtrl.pFFTSampRate, 8, 1);
}

void SCOPE_ADCParamSet(uint8_t *pChannelChangeFlag, uint8_t *pSampRateChangeFlag)
{
    if(1 == *pChannelChangeFlag)
    {
        Board_ADC1_Init(g_scopeCtrl.channelSelect);
        Board_ADC2_Init(g_scopeCtrl.channelSelect);
        *pChannelChangeFlag = 0;
    }
    
    if(1 == *pSampRateChangeFlag)
    {
        Board_ADC1_SetSampleRate(g_scopeCtrl.timeDivCoef[g_scopeCtrl.timeScaleIdx]);
        SCOPE_UpdateSampRate();
        *pSampRateChangeFlag = 0;
    }
}

/**
 * @brief	main routine for template example
 * @return	Function should not exit.
 */
int main(void)
{
    uint16_t i, j, ii;
    uint16_t tempVal;
    int32_t vMax = 0;
    int32_t vMin = 0;
    int32_t trigStartPos, trigEndPos;
    int8_t  autoVoltMeasureRange = 0;
    uint8_t  channelChangeFlag = 0;
    uint8_t  sampRateChangeFlag = 0;
    
	// SystemCoreClockUpdate会更新全局变量的值，因此只能在进入main()后再调用
	SystemCoreClockUpdate();
	Board_Init();
    PinINTConfig();

    OLED_Init();
    OLED_Clear();

    SCOPE_InitCtrlParam();    
    DMA_IntA_Flag = 0;
    DMA_IntB_Flag = 0;
    DMA_ADC_Setup(ADC_Buf,SAVE_ITEMS);  
    Board_ADC1_Init(g_scopeCtrl.channelSelect);   
    Board_ADC2_Init(g_scopeCtrl.channelSelect);
    Board_ADC1_SetSampleRate(g_scopeCtrl.timeDivCoef[g_scopeCtrl.timeScaleIdx]);
    
    SysTick_Config(SystemCoreClock / TICKRATE_HZ);

    DEBUGOUT("LPC824 Mini Scope Start. MCU run @ %d MHz.\r\n", SystemCoreClock / 1000000);
    DEBUGOUT("Firmware version %s. Build @ %s, %s.\r\n", g_miniScopeVersion, __DATE__, __TIME__);
    SCOPE_UIShowWelcomePage();
    delay_ms(1500);
    
    /* Loop forever */
    while(1)
    {
        if(keyState != 0xff)
        {
            switch(keyState)
            {
                case 0:
                    g_scopeCtrl.voltScaleIdx++;
                    if(g_scopeCtrl.voltScaleIdx == MAX_VOLT_SCALE)
                    {
                        g_scopeCtrl.voltScaleIdx = 0;
                    }
                    
                    if(g_scopeCtrl.voltScaleIdx == (MAX_VOLT_SCALE - 1))
                    {
                        //1.0V档显示范围为0.0V~4.0V，不需要自动量程
                        autoVoltMeasureRange = 0;
                    }
                    else
                    {
                        autoVoltMeasureRange = 1;
                    }

                    DEBUGOUT("volt scale: %f V.\r\n", g_scopeCtrl.voltScale[g_scopeCtrl.voltScaleIdx]);
                    break;   
                    
                case 1:
                    if(g_scopeCtrl.runStop == SCOPE_DISPLAY_STOP)
                    {
                        break;
                    }
                    
                    if(g_scopeCtrl.timeScaleIdx == (MAX_TIME_SCALE - 1))
                        g_scopeCtrl.timeScaleIdx = 0;
                    else
                        g_scopeCtrl.timeScaleIdx++;
                    sampRateChangeFlag = 1;
                    DEBUGOUT("time scale: %d us.\r\n", g_scopeCtrl.timeScale[g_scopeCtrl.timeScaleIdx]);
                    break;

                case 2:
                    if(g_scopeCtrl.runStop == SCOPE_DISPLAY_STOP)
                    {
                        break;
                    }
                    g_scopeCtrl.channelSelect = (g_scopeCtrl.channelSelect ? 0 : 1);
                    channelChangeFlag = 1;
                    DEBUGOUT("channel select: %d.\r\n", g_scopeCtrl.channelSelect);
                    break;
                    
                case 3:
                    if(g_scopeCtrl.runState == SCOPE_STATE_M0_MAIN)
                        g_scopeCtrl.runState = SCOPE_STATE_M1_SPECTRUM;
                    else
                        g_scopeCtrl.runState = SCOPE_STATE_M0_MAIN;
                    break;
                    
                case 4:
                    g_scopeCtrl.runStop = (g_scopeCtrl.runStop == SCOPE_DISPLAY_RUN) ? SCOPE_DISPLAY_STOP : SCOPE_DISPLAY_RUN;
                    break;
                    
                default:
                    break;
            }
            keyState = 0xff;
        } 
        

        if (DMA_IntA_Flag) 
        {
            if(g_scopeCtrl.runState == SCOPE_STATE_M0_MAIN)
            {
                uint8_t yPos, yPosLast;
                int16_t autoVMRoffset = 0;
                int16_t autoVMRoffsetIndex = 0;
                static int16_t lastIndex = 0;
                
                OLED_ShowGrid(0);

                vMax = 0x00000000;
                vMin = 0x7FFFFFFF;
                for(i = 0; i < 128; i++)
                {
                    ADC_TransBuf[i] = ((ADC_Buf[i] & 0x0000fff0) >> 4);

                    if(ADC_TransBuf[i] > vMax)
                    {
                        vMax = ADC_TransBuf[i];
                    }

                    if(ADC_TransBuf[i] < vMin)
                    {
                        vMin = ADC_TransBuf[i];
                    }
                }

                //自动量程
                if(autoVoltMeasureRange)
                {
                    autoVMRoffsetIndex = vMin / g_scopeCtrl.voltRawValuePerGrad[g_scopeCtrl.voltScaleIdx];

                    //消除量程抖动
                    if((autoVMRoffsetIndex < lastIndex)&&(lastIndex - autoVMRoffsetIndex == 1))
                    {
                        autoVMRoffsetIndex = lastIndex;
                    }
                    
                    autoVMRoffset = autoVMRoffsetIndex *  g_scopeCtrl.voltRawValuePerGrad[g_scopeCtrl.voltScaleIdx];
                }
                else
                {
                    autoVMRoffsetIndex = 0;
                    autoVMRoffset = 0;
                }

                SCOPE_UIShowVoltGrad(autoVMRoffsetIndex);

                //if((trigStartPos > 6)&&(trigStartPos < 16))
                {
                    trigStartPos = 8;
                    trigEndPos = trigStartPos + 108;
                    
                    for(i = 0, ii = trigStartPos; ii < trigEndPos; i++, ii++)
                    {
                        
                        tempVal = (ADC_TransBuf[ii] - autoVMRoffset) / g_scopeCtrl.voltRawValuePerPixel[g_scopeCtrl.voltScaleIdx];

                        if(tempVal > 48)
                        {
                            tempVal = 48;
                        }

                        //画波形
                        {
                            yPos = 48 - tempVal;

                            //tempVal为48时（即yPos为0）不显示，表示实际值超出显示范围
                            if(yPos > 0)
                            {
                                OLED_DrawPoint(i, yPos, 1);
                            }

                            //y轴插值，使波形看起来是连续的
                            if(i == 0)
                            {
                                yPosLast = yPos;
                            }
                            else
                            {
                                if(yPos > (yPosLast + 1))
                                {
                                    for(j = yPosLast + 1; j < yPos; j++)
                                    {
                                        OLED_DrawPoint(i, j, 1);
                                    }
                                }
                                else if(yPosLast > (yPos + 1))
                                {
                                    for(j = yPosLast - 1; j > yPos; j--)
                                    {
                                        OLED_DrawPoint(i, j, 1);
                                    }
                                }
                                yPosLast = yPos;
                            }
                        }
                        
                    }

                    OLED_Refresh();
                }
            }
            //FFT Display
            else
            {
                uint8_t yPos;

                if(g_scopeCtrl.channelSelect == 0)
                {
                    for(i = 0; i < 128; i++)
                    {
                        //增加计算精度，只右移1位，而非4位
                        ADC_TransBuf[i] = (ADC_Buf[i] & 0x0000fff0) >> 1;
                    }
                }
                else
                {
                    for(i = 0; i < 128; i++)
                    {
                        //audio channel 减去直流偏置3.0V(3273)
                        ADC_TransBuf[i] = (((ADC_Buf[i] & 0x0000fff0) >> 4) - 3723) << 5;
                    }
                }

                //FFT 运算
                ZeroFFT(ADC_TransBuf, 128);

                //适配屏幕显示范围
                int16_t maxFFTVal = -32768;
                if(g_scopeCtrl.channelSelect == 0)
                {
                    //为了显示更多频谱细节，不统计直流分量，i从3开始计算
                    for(i = 3; i < 64; i++)
                    {
                        if(ADC_TransBuf[i] > maxFFTVal)
                            maxFFTVal = ADC_TransBuf[i];
                    }
                }
                else
                {
                    maxFFTVal = 512;
                }
                
                OLED_ShowGrid(1);
                
                for(i = 0; i < 64; i++)
                {
                    //显示范围0~47
                    #define MAX_GRID_VALUE (47)
                    tempVal = ADC_TransBuf[i] * (MAX_GRID_VALUE) / maxFFTVal;
                    if(tempVal > MAX_GRID_VALUE)
                    {
                        tempVal = MAX_GRID_VALUE;
                    }

                    //频谱柱状图显示
                    yPos = MAX_GRID_VALUE - tempVal;
                    for(j = yPos; j < MAX_GRID_VALUE; j++)
                    {
                        //i右移2个像素，适应框图
                        OLED_DrawPoint(i+2, j+2, 1);
                    }
                }

                SCOPE_UIShowFFTInfo(maxFFTVal);
                OLED_Refresh();
            }

            //简单实现ADC通道、采样率切换，此处后续优化
            SCOPE_ADCParamSet(&channelChangeFlag, &sampRateChangeFlag);

            if(SCOPE_DISPLAY_RUN == g_scopeCtrl.runStop)
            {
                DMA_IntA_Flag = 0;
                Chip_ADC_EnableSequencer(LPC_ADC, ADC_SEQA_IDX);
                Chip_ADC_StartSequencer(LPC_ADC, ADC_SEQA_IDX);
            }
        }

        if (DMA_IntB_Flag) 
        {
            DMA_IntB_Flag = 0;
            Chip_ADC_EnableSequencer(LPC_ADC, ADC_SEQA_IDX);
            Chip_ADC_StartSequencer(LPC_ADC, ADC_SEQA_IDX);
        }
    }
}

