 #include "cutils.h"
//#include "writfile.h"
#include "GlobalData/GlobalDef.h"
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
#include <QApplication>
std::string CUtils::GetAppPath()
{
    char link[256], path[256];
    memset(link,0,sizeof(link));
    memset(path,0,sizeof(path));
    sprintf(link, "/proc/self/exe", getpid());
    readlink(link, path, sizeof(path));
//    std::cout<<getpid()<<std::endl;
    return std::string(path);

    //    QString runPath = QCoreApplication::applicationDirPath();
    //    return runPath.toStdString();
}

std::string CUtils::GetFilePathWithoutName(CString strFilePath)
{
    strFilePath.Left(strFilePath.ReverseFind('/'));
    return strFilePath.stdStr();
}

std::string CUtils::AdjustFileSize(unsigned long long ullFileSize)
{
    double dbSize = 0.0;
    CString strSize;

    if (ullFileSize/1024/1024/1024)
    {
        dbSize = ullFileSize/1024/1024/1024.0;
        strSize.Format(_T("%.1fGB"),dbSize);
    }
    else if (ullFileSize/1024/1024)
    {
        dbSize = ullFileSize/1024/1024.0;
        strSize.Format(_T("%.1fMB"),dbSize);
    }
    else if (ullFileSize/1024)
    {
        dbSize = ullFileSize/1024.;
        strSize.Format(_T("%.1fKB"),dbSize);
    }
    else
    {
        dbSize = ullFileSize/1.;
        strSize.Format(_T("%.1fB"),dbSize);
    }

    return strSize.stdStr();
}


std::string CUtils::AdjustSpeed( unsigned long long ullSpeed )
{
    int speed = 0;
    CString strSpeed;

    if ((speed = int(ullSpeed/1024/1024/1024)))
    {
        strSpeed.Format(_T("%d GB/s"),speed);
    }
    else if ((speed = int(ullSpeed/1024/1024)))
    {
        strSpeed.Format(_T("%d MB/s"),speed);
    }
    else if ((speed = int(ullSpeed/1024)))
    {
        strSpeed.Format(_T("%d KB/s"),speed);
    }
    else
    {
        speed = (int)ullSpeed;
        strSpeed.Format(_T("%d B/s"),speed);
    }

    return strSpeed.stdStr();
}
