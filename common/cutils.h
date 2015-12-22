 #ifndef CUTILS_H
#define CUTILS_H
#include <unistd.h>
#include <string>
#include "cstring.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
class CUtils
{
public:
    CUtils();
    static void WriteLogFile(int logFile,int timeStamp,std::string text);
//    static void WriteLogFile(int logFile, int timeStamp, const char *szFormat... );
    static std::string GetAppVersion(std::string path);

    static void WriteLogFile(int logFile,int timeStamp,CString text)
    {
        WriteLogFile(logFile,timeStamp,text.stdStr());
    }
    static void WriteLogFile(int logFile, int timeStamp, const char *format)
    {
        WriteLogFile(logFile,timeStamp,std::string(format));
    }
    static std::string GetAppVersion(CString path)
    {
        return GetAppVersion(path.stdStr());
    }
    static std::string GetAppPath();
    static std::string GetFilePathWithoutName(CString str);
};

#endif // CUTILS_H
