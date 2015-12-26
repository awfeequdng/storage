 #ifndef CUTILS_H
#define CUTILS_H
#include <unistd.h>
#include <string>
#include "cstring.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <tuple>

template<class Tuple,std::size_t N>
struct TuplePrinter{
    static void print(const Tuple &t)
    {
        TuplePrinter<decltype(t),N-1>::print(t);
        std::cout<<","<<std::get<N-1>(t);
    }
};

template<class Tuple>
struct TuplePrinter<Tuple,1>
{
    static void print(const Tuple &t)
    {
        std::cout<<std::get<0>(t);
    }
};

template<class ...Args>
void PrintTuple(const std::tuple<Args...> &t)
{
    std::cout<<"(";
    TuplePrinter<decltype(t),sizeof...(Args)>::print(t);
    std::cout<<")\n";
}

class CUtils
{
public:
    CUtils();
//    template<class ...>
    static void WriteLogFile(int logFile,int timeStamp,std::string text);

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
