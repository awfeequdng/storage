 #ifndef CSTRING_H
#define CSTRING_H
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


class CString
{
public:
    CString();
    CString(const std::string &str);
    CString(const char*str):m_str(str){}
    CString(int i);
    const char *c_str() const;
    bool Format(const char *szFormat...);
    std::string stdStr()const{return m_str;}

    /*������Ա���� Begin*/
    unsigned int GetLength(void);
    const char *GetBuffer(void);
    void Empty(void);
    bool IsEmpty(){return m_str.empty();}
    char GetAt(unsigned int uiIndex);
    void SetAt(unsigned int uiIndex, char ch);

    //���ڱȽϵĺ���
    int Compare(const char *str);
    int Compare(CString &Str);
    int CompareNoCase(const char *str);
    int CompareNoCase(const CString &str);

    void Replace(CString oriStr, CString repStr);

    //�ַ�����ȡ����
    CString &Right(unsigned int uiLength);
    CString &Left(unsigned int uiLength);
    CString &Mid(unsigned int uiBegin);
    CString &Mid(unsigned int uiBegin, unsigned int uiLength);
    //��Сת������
    CString &MakeUpper(void);
    CString &MakeLower(void);
    CString &MakeReverse(void);
    //�ַ�������(�����û�,ɾ��,��ӵ�)
    CString &Replace(char chOld, char chNew);
    CString &Replace(char *pOld, char *pNew);
    CString &Insert(unsigned int uiIndex, char ch);
    CString &Insert(unsigned int uiIndex, char *str);
    CString &Remove(char ch);
    CString &Delete(unsigned int uiIndex, unsigned int uiCount = 1);
    CString &TrimLeft(void);
    CString &TrimLeft(char ch);
    CString &TrimLeft(char *str);
    CString &TrimRight(void);
    CString &TrimRight(char ch);
    CString &TrimRight(char *str);
    //���Һ���
    int Find(char ch, unsigned int uiBegin = 0);
    int Find(const char *str, unsigned int uiBegin = 0);
    int ReverseFind(char ch);
//    int FindOneOf(char *str);
    /*������Ա���� End*/

    CString operator +(const CString &str)const;
    CString &operator +=(const CString &str);
    CString &operator +=(const char* str);
    bool operator !=(const CString &str)const ;
//    /*�������صĲ����� Begin*/
//    //��Ϊ��Ա����
//    CString& operator= (const char ch);
//    CString& 	  operator= (const char *str);
//    CString& 	  operator= (CString &Str);
//    CString& 	  	 operator+=(const char ch);
//    CString& 	  operator+=(const char *str);
//    CString& 	  operator+=(CString &Str);
//    const char& operator[](unsigned int n);
//    //��Ϊ��Ԫ

//    friend CString&operator+ (CString &Str1, CString &Str2);
//    friend CString&operator+ (CString &Str, const char *str);
//    friend CString&operator+ (const char *str, CString &Str);
//    friend CString&operator+ (CString &Str, char ch);
//    friend CString&operator+ (char ch, CString &Str);
//    friend bool operator==(CString &Str1, CString &Str2);
//    friend bool operator==(CString &Str, const char *str);
//    friend bool operator==(const char *str, CString &Str);
//    friend bool operator!=(const CString &Str1, const CString &Str2);
//    friend bool operator!=(const CString &Str, const char *str);
//    friend bool operator!=(const char *str, const CString &Str);
//    friend bool operator< (const CString &Str1, const CString &Str2);
//    friend bool operator< (const CString &Str, const char *str);
//    friend bool operator< (const char *str, const CString &Str);
//    friend bool operator> (const CString &Str1, const CString &Str2);
//    friend bool operator> (const CString &Str, const char *str);
//    friend bool operator> (const char *str, const CString &Str);
//    friend bool operator<=(const CString &Str1, const CString &Str2);
//    friend bool operator<=(const CString &Str, const char *str);
//    friend bool operator<=(const char *str, const CString &Str);
//    friend bool operator>=(const CString &Str1, const CString &Str2);
//    friend bool operator>=(const CString &Str, const char *str);
//    friend bool operator>=(const char *str, const CString &Str);
//   /*�������صĲ����� End*/
private:
    std::string replaceAll(const std::string &src, std::string org_str, std::string rep_str);
    void string_replace(std::string &s1, const std::string &s2, const std::string &s3);
    void string_replace_2(std::string &s1, const std::string &s2, const std::string &s3);

private:
    std::string strToLower(const std::string &str);

private:
    std::string m_str;
};

std::ostream &operator<<(std::ostream &os,CString str);
std::stringstream &operator<<(std::stringstream &ostr,CString str);
#endif // CSTRING_H
