
//#include "Utils.h"
#include "PortCommand.h"

PortCommand::PortCommand(void )
{

}


PortCommand::~PortCommand(void)
{
}

void PortCommand::Init( QSerialPort *pSerialPort,int hFile )
{
	m_pSerialPort = pSerialPort;
	m_hLogFile = hFile;
}

void PortCommand::RedLight( int nPort,BOOL bLight )
{
//	WORD wCommand = 0;
//	if (bLight)
//	{
//		wCommand = RED_LIGHT_ON;
//	}
//	else
//	{
//		wCommand = RED_LIGHT_OFF;
//	}

//	// 母盘位置暂时未定
//	wCommand |= (nPort<<8);

//    BYTE buffer[2] = {0};
//	memcpy(buffer,&wCommand,2);

////	m_pSerialPort->WriteBytes(buffer,2);
//    m_pSerialPort->write(buffer,2);
////	CUtils::WriteLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
////	Sleep(SLEEP_TIME);
}

void PortCommand::GreenLight( int nPort,BOOL bLight)
{
//	WORD wCommand = 0;
//	if (bLight)
//	{
//		wCommand = GREEN_LIGHT_ON;
//	}
//	else
//	{
//		wCommand = GREEN_LIGHT_OFF;
//	}

//	// 母盘位置暂时未定
//	wCommand |= (nPort<<8);

//    BYTE buffer[2] = {0};
//	memcpy(buffer,&wCommand,2);

//    m_pSerialPort->write(buffer,2);
////	CUtils::WriteLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
////	Sleep(SLEEP_TIME);
}

void PortCommand::RedGreenLight( int nPort,BOOL bLight )
{
//	WORD wCommand = 0;
//	if (bLight)
//	{
//		wCommand = GREEN_RED_LIGHT_ON;
//	}
//	else
//	{
//		wCommand = GREEN_RED_LIGHT_OFF;
//	}

//	// 母盘位置暂时未定
//	wCommand |= (nPort<<8);

//    BYTE buffer[2] = {0};
//	memcpy(buffer,&wCommand,2);

//    m_pSerialPort->write(buffer,2);
////	CUtils::WriteLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
////	Sleep(SLEEP_TIME);
}

void PortCommand::GreenLightFlash( int nPort )
{
//	WORD wCommand = GREEN_LIGHT_FLASH;

//	// 母盘位置暂时未定
//	wCommand |= (nPort<<8);

//    BYTE buffer[2]= {0};
//	memcpy(buffer,&wCommand,2);

//    m_pSerialPort->write(buffer,2);
////	CUtils::WriteLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
////	Sleep(SLEEP_TIME);
}

void PortCommand::EnableFlashLight()
{
//	WORD wCommand = ENABLE_FLASH_LIGHT;

//    BYTE buffer[2]= {0};
//	memcpy(buffer,&wCommand,2);

//    m_pSerialPort->write(buffer,2);
////	CUtils::WriteLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
////	Sleep(SLEEP_TIME);
}

void PortCommand::AllPass()
{
    WORD wCommand = ALL_PASS;

    BYTE buffer[2]= {0};
	memcpy(buffer,&wCommand,2);

    m_pSerialPort->write((const char *)buffer,2);
//	CUtils::WriteLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
    //	Sleep(SLEEP_TIME);
}

void PortCommand::PassOn(unsigned char nPort)
{
    WORD wCommand = PASS_ON;

    BYTE buffer[2]= {0};

    wCommand |=nPort;
    wCommand |= (nPort<<8)&0xff00;

    memcpy(buffer,&wCommand,2);

    m_pSerialPort->write((const char *)buffer,2);
}

void PortCommand::PassOff(unsigned char nPort)
{
    WORD wCommand = PASS_OFF;

    BYTE buffer[2]= {0};

    wCommand |=nPort;
    wCommand |= (nPort<<8)&0xff00;

    memcpy(buffer,&wCommand,2);

    m_pSerialPort->write((const char *)buffer,2);
}

void PortCommand::AllFail()
{
    WORD wCommand = ALL_FAIL;

    BYTE buffer[2]= {0};
	memcpy(buffer,&wCommand,2);

    m_pSerialPort->write((const char *)buffer,2);
//	CUtils::WriteLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
    //	Sleep(SLEEP_TIME);
}

void PortCommand::FailOn(unsigned char nPort)
{
    WORD wCommand = FAIL_ON;

    BYTE buffer[2]= {0};

    wCommand |=nPort;
    wCommand |= (nPort<<8)&0xff00;

    memcpy(buffer,&wCommand,2);

    m_pSerialPort->write((const char *)buffer,2);
}

void PortCommand::FailOff(unsigned char nPort)
{
    WORD wCommand = FAIL_OFF;

    BYTE buffer[2]= {0};

    wCommand |=nPort;
    wCommand |= (nPort<<8)&0xff00;

    memcpy(buffer,&wCommand,2);

    m_pSerialPort->write((const char *)buffer,2);
}

void PortCommand::ResetPower()
{
	WORD wCommand = POWER_RESET;

    BYTE buffer[2]= {0};
	memcpy(buffer,&wCommand,2);

    m_pSerialPort->write((const char *)buffer,2);
//	CUtils::WriteLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
//	Sleep(SLEEP_TIME);
}

void PortCommand::ResetLight()
{
//	WORD wCommand = LIGHT_RESET;

//    BYTE buffer[2]= {0};
//	memcpy(buffer,&wCommand,2);

//    m_pSerialPort->write(buffer,2);
////	CUtils::WriteLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
////	Sleep(SLEEP_TIME);
}

void PortCommand::Buzzer( BOOL bOn )
{
    WORD wCommand = 0;
    wCommand = BUZZER_ON_OFF;

    BYTE buffer[2]= {0};
	memcpy(buffer,&wCommand,2);

    m_pSerialPort->write((const char *)buffer,2);
//	CUtils::WrieLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
//	Sleep(SLEEP_TIME);
}

void PortCommand::SwitchScreenToHost()
{
    WORD wCommand = SWITCH_SCREEN_TO_HOST;

    BYTE buffer[2]= {0};
	memcpy(buffer,&wCommand,2);

    m_pSerialPort->write((const char *)buffer,2);
//	CUtils::WriteLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
//	Sleep(SLEEP_TIME);
}

void PortCommand::SwitchScreenToUserDefined()
{
    WORD wCommand = SWITCH_SCREEN_TO_USER_DEFINED;

    BYTE buffer[2]= {0};
    memcpy(buffer,&wCommand,2);

    m_pSerialPort->write((const char *)buffer,2);
//	CUtils::WriteLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
//	Sleep(SLEEP_TIME);
}

void PortCommand::Power( int nPort,BOOL bOn )
{
	WORD wCommand = 0;
	if (bOn)
	{
		wCommand = POWER_ON;
	}
	else
	{
		wCommand = POWER_OFF;
	}

    wCommand |= nPort;
    wCommand |= (nPort<<8)&0xFF00;

    BYTE buffer[2]= {0};
	memcpy(buffer,&wCommand,2);

    m_pSerialPort->write((const char *)buffer,2);
//	CUtils::WriteLogFile(m_hLogFile,TRUE,_T("[SerialPort] TX = %02X %02X"),buffer[0],buffer[1]);
//	Sleep(SLEEP_TIME);
}
