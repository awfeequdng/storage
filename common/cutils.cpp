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
//void CUtils::WriteLogFile(int logFile, int timeStamp, const char *szFormat,...)
//{
//    std::string text;

//    va_list arg;
//    int done = 0;
//    char str[1000];
//    va_start(arg,szFormat);
//    done =vsprintf(str,szFormat,arg);
//    text = str;
//    va_end(arg);

//    text += "\r\n";
//    write(logFile,text.c_str(),text.size());
//}

std::string CUtils::GetAppVersion(std::string path)
{
    return std::string("01");
}

