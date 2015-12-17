 #ifndef CUTILS_H
#define CUTILS_H
#include <unistd.h>
#include <string>
#include "cstring.h"
class CUtils
{
public:
    CUtils();
    static void WriteLogFile(int logFile,int timeStamp,std::string text);
    static std::string GetAppVersion(std::string path);

    static void WriteLogFile(int logFile,int timeStamp,CString text)
    {
        WriteLogFile(logFile,timeStamp,text.stdStr());
    }
    static void WriteLogFile(int logFile,int timeStamp,const char *text)
    {
        WriteLogFile(logFile,timeStamp,std::string(text));
    }
    static std::string GetAppVersion(CString path)
    {
        return GetAppVersion(path.stdStr());
    }
};

#endif // CUTILS_H
