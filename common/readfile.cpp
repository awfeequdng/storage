 #include "readfile.h"


using namespace std;

vector<string> split(const string& src, string separate_character);
string subString(const string &src, int begin_pos, int end_pos);
string replaceAll(const string &src, vector<string> &org_strVec, string rep_str);
string replaceAll(const string &src, string org_str, string rep_str);

// trim_right() family.
std::string trim_right (const std::string & source , const string &t = " " );
// trim_left() family.
std::string trim_left ( const std::string & source ,const std::string  &t = " " );
// trim() family.
std::string trim ( const std::string & source ,const std::string  &t = " " );


ReadFile::ReadFile()
{

}

ReadFile::~ReadFile()
{

}

std::vector<std::vector<double> > ReadFile::read(string pathname)
{
    std::vector<std::vector<double> > values;

//    for(std::vector<std::vector<double> >::size_type i = 0; i < values.size(); i++) {
//        for (std::vector<double>::size_type j = 0; j < values[i].size(); j++) {
//            values[i][j] = 0.0;
//        }
//    }

//    std::ifstream fr;
//    fr.open(pathname.c_str());
//    if(!fr.is_open())
//    {
//	CPX::cpxDebug()<<"open file "<<pathname<<" failed!"<<"\n";
//    }
//    std::string line ;
//    for (int i = 0; i < Card::POINTNUM; i++) {
//        std::getline(fr,line);
//        if(line.empty())
//        {
//            CPX::cpxDebug()<<"read file <<pathname<<end!"<<"\n";
//            return values;
//        }
//        line = trim(line," \n\t");
//        std::vector<std::string> strings = split(line,"\t");
//        for (std::vector<std::string>::size_type j = 0; j < strings.size(); j++) {
//            if(strings[j].empty()) {
//                break;
//            }
//            values[i][j+Card::POINTNUM-strings.size()] = atof(strings[j].c_str());
//            CPX::cpxDebug()<<values[i][j+Card::POINTNUM-strings.size()]<<" ";
//        }
//    }
//    fr.close();
    return values;
}

//几个常用的字符串拆分函数，封装好后使用及其方便~~
//split()
vector<string> split(const string& src, string separate_character)
{
    vector<string> strs;
    int separate_characterLen = separate_character.size();//分割字符串长度，这样就可以支持多字 符串的分隔符
    int lastPosition = 0, index = -1;
    while (-1 != (index = src.find(separate_character, lastPosition)))
    {
        strs.push_back(src.substr(lastPosition, index - lastPosition));
        lastPosition = index + separate_characterLen;
    }
    string lastString = src.substr(lastPosition);
    if (!lastString.empty())
    strs.push_back(lastString);
    return strs;
}

////subString()
//string subString(const string &src, int begin_pos, int end_pos)
//{
//    string sub = src.substr(begin_pos, end_pos - begin_pos);
//    return sub;
//}

//relaceAll()
string replaceAll(const string &src, string org_str, string rep_str)
{
    vector<string> delimVec = split(src, org_str);
    if (delimVec.size() <= 0)
    {
//        CPX::cpxDebug() << "can not find " << rep_str << "\n";
        return src;
    }
    string target("");
    vector<string>::iterator it = delimVec.begin();
    for (; it!= delimVec.end(); ++it)
    {
        target = target + (*it) + rep_str;
    }
    target = target + (*it);
    return target;
}

////relaceAll()
//string replaceAll(const string &src, vector<string> &org_strVec, string rep_str)
//{
//    vector<string>::iterator pOrg = org_strVec.begin();
//    string org_str(""), target = src;
//    for (; pOrg != org_strVec.end(); ++pOrg)
//    {
//        org_str = *pOrg;
//        vector<string> delimVec = split(target, org_str);
//        target = "";
//        if (delimVec.size() > 0)
//        {
//            vector<string>::iterator it = delimVec.begin();
//            for (; it != delimVec.end() - 1; ++it)
//            {
//                target = target + (*it) + rep_str;
//            }
//            target = target + *(it);
//        }
//    }
//    return target;
//}



// trim_right() family.
std::string trim_right ( const std::string & source ,const std::string  &t )
{
       std::string str = source;
       return str.erase (str.find_last_not_of ( t ) + 1 ) ;
}



//inline std::wstring trim_right ( const std::wstring & source ,
//       const std::wstring & t = L" " )
//{
//       std::wstring str = source;
//       return str.erase ( str.find_last_not_of ( t ) + 1 ) ;
//}



// trim_left() family.
std::string trim_left ( const std::string & source ,const std::string  &t)
{
       std::string str = source;
       return str.erase(0,source.find_first_not_of (t)) ;
}


//inline std::wstring trim_left ( const std::wstring & source ,
//       const std::wstring & t = L" " )
//{
//       std::wstring str = source;
//       return str.erase ( 0 , source.find_first_not_of ( t ) ) ;
//}



// trim() family.
std::string trim ( const std::string & source ,const std::string  &t)
{
       std::string str = source;
       return trim_left (trim_right (str,t),t) ;
}



//inline std::wstring trim ( const std::wstring & source ,
//       const std::wstring & t = L" " )
//{
//       std::wstring str = source;
//       return trim_left ( trim_right ( str , t ) , t ) ;
//}
