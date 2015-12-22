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

    inline int SetPathName(CString fileName){
        return SetPathName(fileName.stdStr());
    }
    inline BOOL GetBool(CString key,CString value,BOOL b)
    {
        return GetBool(key.stdStr(),value.stdStr(),b);
    }
    inline UINT GetUInt(CString key,CString value,UINT u)
    {
        return GetUInt(key.stdStr(),value.stdStr(),u);
    }
    inline INT GetInt(CString key,CString value,INT s)
    {
        return GetInt(key.stdStr(),value.stdStr(),s);
    }
    inline double GetDouble(CString key,CString value,double d)
    {
        return GetDouble(key.stdStr(),value.stdStr(),d);
    }
    inline CString GetString(CString key,CString value,CString str)
    {
        std::string strTmp = GetString(key.stdStr(),value.stdStr(),str.stdStr());
        return CString(strTmp);
    }
    inline BOOL IsKeyExist(CString key,CString value)
    {
        return IsKeyExist(key.stdStr(),value.stdStr());
    }
//private:

    inline int SetPathName(const char * fileName)
    {
        return SetPathName(std::string(fileName));
    }
    inline BOOL GetBool(const char * field,const char * key,BOOL b)
    {
        return GetBool(std::string(field),std::string(key),b);
    }
    inline void WriteBool(const char * field,const char *key,BOOL b)
    {
        WriteBool(std::string(field),std::string(key),b);
    }
    inline UINT GetUInt(const char * field,const char * key,UINT u)
    {
        return GetUInt(std::string(field),std::string(key),u);
    }
    inline void WriteUInt(const char * field,const char *key,UINT u)
    {
        WriteUInt(std::string(field),std::string(key),u);
    }
    inline INT GetInt(const char * field,const char * key,INT s)
    {
        return GetInt(std::string(field),std::string(key),s);
    }
    inline void WriteInt(const char * field,const char *key,INT s)
    {
        WriteInt(std::string(field),std::string(key),s);
    }
    inline double GetDouble(const char * field,const char * key,double d)
    {
        return GetDouble(std::string(field),std::string(key),d);
    }
    inline void WriteDouble(const char * field,const char *key,double d)
    {
        WriteDouble(std::string(field),std::string(key),d);
    }
    inline std::string GetString(const char * field,const char * key,const char *str)
    {
        return GetString(std::string(field),std::string(key),std::string(str));
    }
    inline std::string GetString(const char * field,const char * key,std::string str)
    {
        return GetString(std::string(field),std::string(key),str);
    }
    inline void WriteString(const char * field,const char *key,const char *str)
    {
        WriteString(std::string(field),std::string(key),std::string(str));
    }
    inline BOOL IsKeyExist(const char * field,const char * key)
    {
        return IsKeyExist(std::string(field),std::string(key));
    }

    int SetPathName(std::string fileName);

    BOOL GetBool(std::string field,std::string key,BOOL b);
    void WriteBool(std::string field,std::string key, BOOL b);
    UINT GetUInt(std::string field, std::string key, UINT u);
    void WriteUInt(std::string field, std::string key, UINT u);
    INT GetInt(std::string field, std::string key, INT s);
    void WriteInt(std::string field, std::string key,INT s);
    double GetDouble(std::string field, std::string key, double d);
    void WriteDouble(std::string field,std::string key,double d);
    std::string GetString(std::string field,std::string key,std::string str);
    void WriteString(std::string field,std::string key,std::string str);
    BOOL IsKeyExist(std::string field, std::string key);
    void ReadIniFile(std::string filename);
    void SetKey(string field, string key, string value);
private:
    std::string m_configFileName;
    MapFieldKeyValue m_fieldKeyValue;
};

#endif // CINI_H
