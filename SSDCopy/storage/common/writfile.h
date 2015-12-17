  #ifndef WRITFILE_H
#define WRITFILE_H


#ifdef  __unix
#ifndef __UNIX_LIKE_PLATFORM__
    #define __UNIX_LIKE_PLATFORM__
#endif  //__UNIX_LIKE_PLATFORM__
#endif  //__unix


#ifdef  __unix__
#ifndef __UNIX_LIKE_PLATFORM__
    #define __UNIX_LIKE_PLATFORM__
#endif  //__UNIX_LIKE_PLATFORM__
#endif  //__unix__

//linux
#ifdef  __linux
#ifndef __UNIX_LIKE_PLATFORM__
    #define __UNIX_LIKE_PLATFORM__
#endif  //__UNIX_LIKE_PLATFORM__
#endif  //__linux

//linux
#ifdef  __linux__
#ifndef __UNIX_LIKE_PLATFORM__
    #define __UNIX_LIKE_PLATFORM__
#endif  //__UNIX_LIKE_PLATFORM__
#endif  //__linux_

//FreeBSD
#ifdef  __FreeBSD__
#ifndef __UNIX_LIKE_PLATFORM__
    #define __UNIX_LIKE_PLATFORM__
#endif  //__UNIX_LIKE_PLATFORM__
#endif  //__FreeBSD

#ifdef  _WIN32
#define __WIN32_PLATFORM__
#endif

#ifdef  _WIN64
#define __WIN64_PLATFORM__
#endif

#ifdef __UNIX_LIKE_PLATFORM__
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif //__UNIX_LIKE_PLATFORM__

#include <string>
#include <iostream>
#include <fstream>

#include<stdio.h>
//#include <dir.h>
//#include <direct.h>

class WritFile
{
public:
    WritFile();
    ~WritFile();

    WritFile(std::string dirPath) {
#ifdef __UNIX_LIKE_PLATFORM__
        DIR *dir = opendir(dirPath.c_str());
        if(!dir)
        {
            int status;
            status = mkdir(dirPath.c_str(),S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
        }
#else
        std::cout<<"WriterFile can't create the directory :"<<dirPath<<"\n";
#endif
    }

    static void write(std::string dir, std::string filename, std::string str) {
        std::ofstream fw;
        fw.open((dir+filename).c_str());
        fw<<str;
        fw.close();
    }
};

#endif // WRITFILE_H
