 #include "cutils.h"
//#include "writfile.h"
CUtils::CUtils()
{

}

void CUtils::WriteLogFile(int logFile, int timeStamp, std::string text)
{
    text+="\r\n";
    write(logFile,text.c_str(),text.size());

}

std::string CUtils::GetAppVersion(std::string path)
{
    return std::string("01");
}

