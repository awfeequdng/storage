#include "cstring.h"
#include <sstream>
#include <vector>
//using namespace std;
#include <string.h>

CString::CString()
{

}

CString::CString(const std::string& str):m_str(str)
{

}

CString::CString(int i)
{
    std::ostringstream ostr;
    ostr<<i;
    m_str = ostr.str();
}

const char *CString::c_str() const
{
    return m_str.c_str();
}

bool CString::Format(const char * szFormat, ...)
{
    va_list arg;
    int done = 0;
    char str[1000];
    va_start(arg,szFormat);
    done =vsprintf(str,szFormat,arg);
    m_str = str;
    va_end(arg);
    return done;
}
//用于比较的函数
int CString::Compare(const char *str)
{
    return m_str.compare(str);
}
int CString::Compare(CString &Str)
{
    return m_str.compare(Str.stdStr());
}
int CString::CompareNoCase(const char *str)
{
    return CompareNoCase(CString(str));
}
int CString::CompareNoCase(const CString &str)  //chi hanshu shi fou zhengque ?
{
    std::string str1 = strToLower(m_str);
    std::string str2 = strToLower(str.stdStr());
    return str1.compare(str2);
}

//字符串截取函数
CString &CString::Right(unsigned int uiLength)
{
    if(uiLength>=GetLength()){
        Mid(0, uiLength);
        return *this;
    }
    Mid(this->GetLength()-uiLength, uiLength);
    return *this;
}
CString &CString::Left(unsigned int uiLength)
{
    Mid(0, uiLength);
    return *this;
}
CString &CString::Mid(unsigned int uiBegin)
{
    Mid(uiBegin, GetLength());
    return *this;
}
CString &CString::Mid(unsigned int uiBegin, unsigned int uiLength)
{
    int iLen = GetLength();

    if(uiBegin>iLen)
        uiBegin = iLen;
    if(uiLength>(iLen-uiBegin))
        uiLength = iLen-uiBegin;

    m_str.erase(0,uiBegin);
    m_str.erase(uiBegin+uiLength,iLen-uiBegin-uiLength);


    return *this;
}
////大小转换函数
//CString &CString::MakeUpper(void)
//{

//}
//CString &CString::MakeLower(void)
//{

//}
//CString &CString::MakeReverse(void)
//{

//}
////字符串修饰(包括置换,删除,添加等)
//CString &CString::Replace(char chOld, char chNew)
//{

//}
//CString &CString::Replace(char *pOld, char *pNew)
//{

//}
//CString &CString::Insert(unsigned int uiIndex, char ch)
//{

//}
//CString &CString::Insert(unsigned int uiIndex, char *str)
//{

//}
//CString &CString::Remove(char ch)
//{

//}
CString &CString::Delete(unsigned int uiIndex, unsigned int uiCount)
{
    unsigned int uiLen = GetLength();

    if(uiIndex>=uiLen){
        return *this;
    }
    if(uiCount==0){
        return *this;
    }

    if(uiCount>uiLen-uiIndex){
        uiCount = uiLen-uiIndex;
    }

    m_str.erase(uiIndex,uiCount);

    return *this;
}
//CString &CString::TrimLeft(void)
//{

//}
//CString &CString::TrimLeft(char ch)
//{

//}
//CString &CString::TrimLeft(char *str)
//{

//}
//CString &CString::TrimRight(void)
//{

//}
//CString &CString::TrimRight(char ch)
//{

//}
//CString &CString::TrimRight(char *str)
//{

//}

//在对象字符串中,从索引uiBegin开始,返回ch第一次出现的位置,省略uiBegin使其为默认的0,未找到返回-1
int CString::Find(char ch, unsigned int uiBegin )
{
    return m_str.find(ch,uiBegin);
}
int CString::Find(char *str, unsigned int uiBegin)
{
    return m_str.find(str,uiBegin);
}
int CString::ReverseFind(char ch)
{
    return m_str.find_last_of(ch);
}
//int CString::FindOneOf(char *str)
//{

//}

unsigned int CString::GetLength(void)
{
    m_str.size();
}
const char *CString::GetBuffer(void)
{
    return m_str.c_str();
}
void CString::Empty(void)
{
    m_str.clear();
}

char CString::GetAt(unsigned int uiIndex)
{
    return m_str.at(uiIndex);
}
void CString::SetAt(unsigned int uiIndex, char ch)
{
    m_str.at(uiIndex) = ch;
}

CString CString::operator +(const CString &str) const
{
    return CString(m_str+str.stdStr());
}

CString &CString::operator +=(const CString &str)
{
    m_str+=str.stdStr();
    return (*this);
}

bool CString::operator !=(const CString &str) const
{
    return (m_str!=str.stdStr());
}

std::string CString::strToLower(const std::string &str)
{
    std::string strTmp = str;
    std::transform(strTmp.begin(),strTmp.end(),strTmp.begin(),tolower);
    return strTmp;
}

static std::vector<std::string> split(const std::string& src, std::string separate_character)
{
    std::vector<std::string> strs;
    int separate_characterLen = separate_character.size();//分割字符串长度，这样就可以支持多字 符串的分隔符
    int lastPosition = 0, index = -1;
    while (-1 != (index = src.find(separate_character, lastPosition)))
    {
        strs.push_back(src.substr(lastPosition, index - lastPosition));
        lastPosition = index + separate_characterLen;
    }
    std::string lastString = src.substr(lastPosition);
    if (!lastString.empty())
    strs.push_back(lastString);
    return strs;
}

std::string CString::replaceAll(const std::string &src, std::string org_str, std::string rep_str)
{
    std::vector<std::string> delimVec = split(src, org_str);
    if (delimVec.size() <= 0)
    {
//        CPX::cpxDebug() << "can not find " << rep_str << "\n";
        return src;
    }
    std::string target("");
    std::vector<std::string>::iterator it = delimVec.begin();
    for (; it!= delimVec.end(); ++it)
    {
        target = target + (*it) + rep_str;
    }
    target = target + (*it);
    return target;
}

//第一种替换字符串的方法用replace()

void CString::string_replace(std::string&s1,const std::string&s2,const std::string&s3)
{
    std::string::size_type pos=0;
    std::string::size_type a=s2.size();
    std::string::size_type b=s3.size();
    while((pos=s1.find(s2,pos))!=std::string::npos)
    {
        s1.replace(pos,a,s3);
        pos+=b;
    }
}

//第二种替换字符串的方法用erase()和insert()
void CString::Replace(CString oriStr,CString repStr)
{
    std::string ori_str = oriStr.stdStr();
    std::string rep_str = repStr.stdStr();
    string_replace_2(m_str,ori_str,rep_str);
//    m_str = replaceAll(m_str,ori_str,rep_str);
}
void CString::string_replace_2(std::string&s1,const std::string&s2,const std::string&s3)
{
    std::string::size_type pos=0;
    std::string::size_type a=s2.size();
    std::string::size_type b=s3.size();
    while((pos=s1.find(s2,pos))!=std::string::npos)
    {
        s1.erase(pos,a);
        s1.insert(pos,s3);
        pos+=b;
    }
}



bool operator==(CString &Str, const char *str){
    return strcmp(Str.c_str(), str)==0;
}

bool operator==(const char *str, CString &Str){
    return strcmp(str, Str.c_str())==0;
}

bool operator==(CString &Str1, CString &Str2){
    return strcmp(Str1.c_str(), Str2.c_str())==0;
}

bool operator!=(const CString &Str, const char *str){
    return strcmp(Str.c_str(), str)!=0;
}

bool operator!=(const char *str, const CString &Str){
    return strcmp(str, Str.c_str())!=0;
}

bool operator!=(const CString &Str1, const CString &Str2){
    return strcmp(Str1.c_str(), Str2.c_str())!=0;
}

bool operator<(const CString &Str, const char *str){
    return strcmp(Str.c_str(), str)<0;
}

bool operator<(const char *str, const CString &Str){
    return strcmp(str, Str.c_str())<0;
}

bool operator<(const CString &Str1, const CString &Str2){
    return strcmp(Str1.c_str(), Str2.c_str())<0;
}

bool operator>(const CString &Str, const char *str){
    return strcmp(Str.c_str(), str)>0;
}

bool operator>(const char *str, const CString &Str){
    return strcmp(str, Str.c_str())>0;
}

bool operator>(const CString &Str1, const CString &Str2) {
    return strcmp(Str1.c_str(), Str2.c_str())>0;
}

bool operator<=(const CString &Str, const char *str) {
    return strcmp(Str.c_str(), str)<=0;
}

bool operator<=(const char *str, const CString &Str){
    return strcmp(str, Str.c_str())<=0;
}

bool operator<=(const CString &Str1, const CString &Str2){
    return strcmp(Str1.c_str(), Str2.c_str())<=0;
}

bool operator>=(const CString &Str, const char *str){
    return strcmp(Str.c_str(), str)>=0;
}

bool operator>=(const char *str, const CString &Str){
    return strcmp(str, Str.c_str())>=0;
}

bool operator>=(const CString &Str1, const CString &Str2){
    return strcmp(Str1.c_str(), Str2.c_str())>=0;
}

std::ostream &operator<<(std::ostream &os, CString str)
{
    os<<str.stdStr();
    return os;
}


std::stringstream &operator<<(std::stringstream &ostr, CString str)
{
    ostr<<str.c_str();
    return ostr;
}

