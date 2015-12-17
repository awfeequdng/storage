//#pragma once
#ifndef __PORT_COMMAND_H_
#define __PORT_COMMAND_H_


#include <QtSerialPort/QSerialPort>
#include "GlobalData/GlobalDef.h"

#define POWER_ON                      0xB0B0
#define POWER_OFF                     0xA0A0

#define FAIL_ON                       0x5050
#define FAIL_OFF                      0x4040
#define PASS_ON                       0x1010
#define PASS_OFF                      0x0000


#define POWER_RESET                   0xD2D2
#define SSD_WORKING_LIGHT_FLASH       0xC9C9
#define BUZZER_ON_OFF                 0xD5D5
#define ALL_PASS                      0xC3C3
#define ALL_FAIL                      0xCECE

#define SWITCH_SCREEN_TO_HOST         0xDFDF
#define SWITCH_SCREEN_TO_USER_DEFINED 0xDCDC



//#define RED_LIGHT_ON                  0x0021
//#define RED_LIGHT_OFF                 0x0020
//#define GREEN_LIGHT_ON                0x0031
//#define GREEN_LIGHT_OFF               0x0030
//#define GREEN_LIGHT_FLASH             0x0041

//#define GREEN_RED_LIGHT_ON            0x0051
//#define GREEN_RED_LIGHT_OFF           0x0050

//#define LIGHT_RESET                   0xD3D3
//#define ENABLE_FLASH_LIGHT            0xD4D4


//#define ALL_POWER_ON                  0xD7D7
//#define ALL_RED_LIGHT_ON              0xD8D8
//#define ALL_RED_LIGHT_OFF             0xD9D9
//#define ALL_GREEN_LIGHT_ON            0xDADA
//#define ALL_GREEN_LIGHT_OFF           0xDBDB



#define SLEEP_TIME         100

class PortCommand
{
public:
    PortCommand(void);
    ~PortCommand(void);

    void Init(QSerialPort *pSerialPort,int hFile);

	void RedLight(int nPort,BOOL bLight);
	void GreenLight(int nPort,BOOL bLight);
	void GreenLightFlash(int nPort);
	void RedGreenLight(int nPort,BOOL bLight);

	void EnableFlashLight();

	void AllPass();
    void PassOn(unsigned char nPort);
    void PassOff(unsigned char nPort);
	void AllFail();
    void FailOn(unsigned char nPort);
    void FailOff(unsigned char nPort);

	void Power(int nPort,BOOL bOn);

	
	void ResetPower();
	void ResetLight();
	void Buzzer(BOOL bOn);
    void SwitchScreenToHost();
    void SwitchScreenToUserDefined();

private:
    QSerialPort  *m_pSerialPort;
    int m_hLogFile;
};


#endif //__PORT_COMMAND_H_
