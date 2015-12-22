 #include "cini.h"

#include <QSettings>
#include <sstream>
CIni::CIni()
{

}

int CIni::SetPathName(std::string fileName)
{
    m_configFileName = fileName;
    ReadIniFile(fileName);
    return 0;
}

BOOL CIni::GetBool(std::string field, std::string key, BOOL b)
{
    MapFieldKey fieldKey;
    fieldKey[field] = key;
    MapFieldKeyValue::iterator it = m_fieldKeyValue.find(fieldKey);
    if(it != m_fieldKeyValue.end())
    {
        return (BOOL)atol((it->second).c_str());
    }
    else
    {
        return b;
    }

}

void CIni::WriteBool(string field, string key, BOOL b)
{
    std::ostringstream ostr;
    ostr<<(!!b);
    std::string str(ostr.str());
    MapFieldKey fieldKey;
    fieldKey[field] = key;
    m_fieldKeyValue[fieldKey] = str;
    QSettings settings(QString(m_configFileName.c_str()),QSettings::IniFormat);
    settings.setValue(QString((field+"/"+key).c_str()),QString(str.c_str()));
}

UINT CIni::GetUInt(std::string field, std::string key, UINT u)
{
    MapFieldKey fieldKey;
    fieldKey[field] = key;
    MapFieldKeyValue::iterator it = m_fieldKeyValue.find(fieldKey);
    if(it != m_fieldKeyValue.end())
    {
        return (UINT)atol((it->second).c_str());
    }
    else
    {
        return u;
    }
}

void CIni::WriteUInt(string field, string key, UINT u)
{
    std::ostringstream ostr;
    ostr<<u;
    std::string str(ostr.str());
    MapFieldKey fieldKey;
    fieldKey[field] = key;
    m_fieldKeyValue[fieldKey] = str;
    QSettings settings(QString(m_configFileName.c_str()),QSettings::IniFormat);
    settings.setValue(QString((field+"/"+key).c_str()),QString(str.c_str()));
}

INT CIni::GetInt(std::string field, std::string key, INT s)
{
    MapFieldKey fieldKey;
    fieldKey[field] = key;
    MapFieldKeyValue::iterator it = m_fieldKeyValue.find(fieldKey);
    if(it != m_fieldKeyValue.end())
    {
        return (INT)atoi((it->second).c_str());
    }
    else
    {
        return s;
    }
}

void CIni::WriteInt(string field, string key, INT s)
{
    std::ostringstream ostr;
    ostr<<s;
    std::string str(ostr.str());
    MapFieldKey fieldKey;
    fieldKey[field] = key;
    m_fieldKeyValue[fieldKey] = str;
    QSettings settings(QString(m_configFileName.c_str()),QSettings::IniFormat);
    settings.setValue(QString((field+"/"+key).c_str()),QString(str.c_str()));
}

double CIni::GetDouble(std::string field, std::string key, double d)
{
    MapFieldKey fieldKey;
    fieldKey[field] = key;
    MapFieldKeyValue::iterator it = m_fieldKeyValue.find(fieldKey);
    if(it != m_fieldKeyValue.end())
    {
        return (double)atof((it->second).c_str());
    }
    else
    {
        return d;
    }
}

void CIni::WriteDouble(string field, string key, double d)
{
    std::ostringstream ostr;
    ostr<<d;
    std::string str(ostr.str());
    MapFieldKey fieldKey;
    fieldKey[field] = key;
    m_fieldKeyValue[fieldKey] = str;
    QSettings settings(QString(m_configFileName.c_str()),QSettings::IniFormat);
    settings.setValue(QString((field+"/"+key).c_str()),QString(str.c_str()));
}

string CIni::GetString(string field, string key, string str)
{
    MapFieldKey fieldKey;
    fieldKey[field] = key;
    MapFieldKeyValue::iterator it = m_fieldKeyValue.find(fieldKey);
    if(it != m_fieldKeyValue.end())
    {
        return (it->second);
    }
    else
    {
        return str;
    }
}

void CIni::WriteString(string field, string key, string str)
{
    MapFieldKey fieldKey;
    fieldKey[field] = key;
    m_fieldKeyValue[fieldKey] = str;
    QSettings settings(QString(m_configFileName.c_str()),QSettings::IniFormat);
    settings.setValue(QString((field+"/"+key).c_str()),QString(str.c_str()));
}

BOOL CIni::IsKeyExist(std::string field, std::string key)
{
    MapFieldKey fieldKey;
    fieldKey[field] = key;
    MapFieldKeyValue::iterator it = m_fieldKeyValue.find(fieldKey);
    if(it != m_fieldKeyValue.end())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



void CIni::ReadIniFile(std::string filename)
{
    ifstream iniFile;
    string line;
    string field=string();
    string key,valueString;

    size_t commentIndex, equalsIndex,leftBracketIndex,rightBracketIndex;
    size_t lineNumber=0;

    iniFile.open(filename.c_str());
    if (iniFile.is_open())
    {
        while (!iniFile.eof())
        {
            lineNumber++;
            getline(iniFile, line);
            //this can happen if the filename is actually a directory
            if (iniFile.bad())
            {
                std::cerr<<"Cannot read ini file '"<<filename<<"'";
                exit(-1);
            }
            // skip zero-length lines
            if (line.size() == 0)
            {
//					DEBUG("Skipping blank line "<<lineNumber);
                continue;
            }
            //search for a comment char
            if ((commentIndex = line.find_first_of(";")) != string::npos)
            {
                //if the comment char is the first char, ignore the whole line
                if (commentIndex == 0)
                {
//						DEBUG("Skipping comment line "<<lineNumber);
                    continue;
                }
//					DEBUG("Truncating line at comment"<<line[commentIndex-1]);
                //truncate the line at first comment before going on
                line = line.substr(0,commentIndex);
            }
            // trim off the end spaces that might have been between the value and comment char
            size_t whiteSpaceEndIndex;
            if ((whiteSpaceEndIndex = line.find_last_not_of(" \t")) != string::npos)
            {
                line = line.substr(0,whiteSpaceEndIndex+1);
            }

            // at this point line should be a valid, commentless string

            // a line has to have an equals sign or left bracket and right bracket
            if ((equalsIndex = line.find_first_of("=")) == string::npos)
            {
                leftBracketIndex = line.find_first_of("[") ;
                rightBracketIndex = line.find_last_of("]");

                if( leftBracketIndex  == string::npos||
                    rightBracketIndex == string::npos )
                {
                    std::cerr<<"Malformed Line "<<lineNumber<<" (missing equals or '[' and ']')";
                    abort();
                }
                else if( leftBracketIndex  != string::npos&&
                         rightBracketIndex != string::npos )
                {
                    field = line.substr(leftBracketIndex+1,rightBracketIndex-leftBracketIndex-1);
                    continue;
                }
            }

            size_t strlen = line.size();
            // all characters before the equals are the key
            key = line.substr(0, equalsIndex);
            // all characters after the equals are the value
            valueString = line.substr(equalsIndex+1,strlen-equalsIndex);

            if(!field.empty())
            {
                SetKey(field,key, valueString);
            }
//			IniReader::SetKey(key, valueString, isSystemFile, lineNumber);
            // got to the end of the config map without finding the key
        }
    }
    else
    {
        std::cerr<<"Unable to load ini file "<<filename;
        abort();
    }
    /* precompute frequently used values */
}


void CIni::SetKey(string field, string key, string value)
{
    MapFieldKey fieldKey;
    fieldKey[field] = key;
    m_fieldKeyValue[fieldKey] = value;
}
