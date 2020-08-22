
#ifndef __MAIN_H__
#define __MAIN_H__

#define MAX_VOLT_SCALE (4)
#define MAX_TIME_SCALE (9)

#define OLED_WIDTH      (128)
#define OLED_HEIGHT     (64)

#define SCOPE_STATE_M0_MAIN             (0)
#define SCOPE_STATE_M1_SPECTRUM         (1)
#define SCOPE_STATE_M2_MEASURE_VOLT     (2)
#define SCOPE_STATE_M3_MEASURE_TIME     (3)

#define SCOPE_DISPLAY_RUN               (0)
#define SCOPE_DISPLAY_STOP              (1)

typedef struct{
    uint8_t runState;
    uint8_t runStop;//0:run 1:stop
    uint8_t channelSelect;

    uint8_t voltScaleIdx; //电压量程index
    float   voltScale[MAX_VOLT_SCALE]; //电压量程
    char    *pVoltName[MAX_VOLT_SCALE];//电压量程名字，用于显示
    uint32_t voltRawValuePerPixel[MAX_VOLT_SCALE];  //用于0-4095转换成点(pixel)数 adcVal/coef = 实际点位置
    uint32_t voltRawValuePerGrad[MAX_VOLT_SCALE];   //每一大刻度对应的raw值
    
    uint8_t timeScaleIdx; //时间量程index
    uint32_t timeScale[MAX_TIME_SCALE]; //时间量程
    char   *pTimeName[MAX_TIME_SCALE];  //时间量程名字，用于显示
    uint32_t timeDivCoef[MAX_TIME_SCALE]; //采样系数，用于设置采样频率
    uint32_t sampleRate; //采样率
    char pFFTSampRate[8]; //采样率(名字)
    char pFFTDiv[8]; //FFT频率刻度(名字)
    char pFFTMax[8]; //FFT最大量程（名字）
}SCOPE_CTRL, *PSCOPE_CTRL;

extern SCOPE_CTRL g_scopeCtrl;

#endif


