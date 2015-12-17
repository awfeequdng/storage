 #ifndef READFILE_H
#define READFILE_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

class ReadFile
{
public:
    ReadFile();
    ~ReadFile();

    static std::vector<std::vector<double> >read(std::string pathname);
};


#endif // READFILE_H
