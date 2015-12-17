 #ifndef CINI_H
#define CINI_H

#include <string>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unistd.h>
#include <stdlib.h>

#include "GlobalData/GlobalDef.h"
#include "cstring.h"


using namespace std;
class CIni
{
public:
    CIni();
    typedef std::map<std::string,std::string> MapFieldKey;
    typedef std::map<MapFieldKey,std::string> MapFieldKeyValue;
    typedef std::map<std::string,std::string> MapKeyValue;
//    typedef std::multimap<std::string,MapKeyValue> MapFieldKeyValue;

    int SetPathName(CString fileName){
        return SetPathName(fileName.stdStr());
    }
    BOOL GetBool(CString key,CString value,BOOL b)
    {
        return GetBool(key.stdStr(),value.stdStr(),b);
    }
    UINT GetUInt(CString key,CString value,UINT u)
    {
        return GetUInt(key.stdStr(),value.stdStr(),u);
    }
    INT GetInt(CString key,CString value,INT s)
    {
        return GetInt(key.stdStr(),value.stdStr(),s);
    }
    double GetDouble(CString key,CString value,double d)
    {
        return GetDouble(key.stdStr(),value.stdStr(),d);
    }
    CString GetString(CString key,CString value,CString str)
    {
        std::string strTmp = GetString(key.stdStr(),value.stdStr(),str.stdStr());
        return CString(strTmp);
    }
    BOOL IsKeyExist(CString key,CString value)
    {
        return IsKeyExist(key.stdStr(),value.stdStr());
    }
//private:

    int SetPathName(const char * fileName)
    {
        return SetPathName(std::string(fileName));
    }
    BOOL GetBool(const char * field,const char * key,BOOL b)
    {
        return GetBool(std::string(field),std::string(key),b);
    }
    UINT GetUInt(const char * field,const char * key,UINT u)
    {
        return GetUInt(std::string(field),std::string(key),u);
    }
    INT GetInt(const char * field,const char * key,INT s)
    {
        return GetInt(std::string(field),std::string(key),s);
    }
    double GetDouble(const char * field,const char * key,double d)
    {
        return GetDouble(std::string(field),std::string(key),d);
    }
    std::string GetString(const char * field,const char * key,const char *str)
    {
        return GetString(std::string(field),std::string(key),std::string(str));
    }
    std::string GetString(const char * field,const char * key,std::string str)
    {
        return GetString(std::string(field),std::string(key),str);
    }
    BOOL IsKeyExist(const char * field,const char * key)
    {
        return IsKeyExist(std::string(field),std::string(key));
    }

    int SetPathName(std::string fileName);

    BOOL GetBool(std::string field,std::string key,BOOL b);
    UINT GetUInt(std::string field, std::string key, UINT u);
    INT GetInt(std::string field, std::string key, INT s);
    double GetDouble(std::string field, std::string key, double d);
    std::string GetString(std::string field,std::string key,std::string str);
    BOOL IsKeyExist(std::string field, std::string key);

    void ReadIniFile(std::string filename);
    void SetKey(string field, string key, string value);
private:
    std::string m_configFileName;
    MapFieldKeyValue m_fieldKeyValue;
};

#endif // CINI_H
