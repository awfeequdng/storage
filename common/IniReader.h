
#ifndef INIREADER_H
#define INIREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map> 
#include "GlobalData/GlobalDef.h"
//#include "PrintMacros.h"
using namespace std;

#define DEFINE_UINT_PARAM(name, paramtype) {#name, &name, UINT, paramtype, false}
#define DEFINE_STRING_PARAM(name, paramtype) {#name, &name, STRING, paramtype, false}
#define DEFINE_FLOAT_PARAM(name,paramtype) {#name, &name, FLOAT, paramtype, false}
#define DEFINE_BOOL_PARAM(name, paramtype) {#name, &name, BOOL, paramtype, false}
#define DEFINE_UINT64_PARAM(name, paramtype) {#name, &name, UINT64, paramtype, false}

namespace USB3014
{

typedef enum _variableType {STRING, UINT, UINT64, FLOAT, BOOL} varType;
typedef enum _paramType {SYS_PARAM, DEV_PARAM} paramType;
typedef struct _configMap
{
	string iniKey; //for example "tRCD"

	void *variablePtr;
	varType variableType;
	paramType parameterType;
	bool wasSet;
} ConfigMap;

class IniReader
{

public:
	typedef std::map<string, string> OverrideMap;
	typedef OverrideMap::const_iterator OverrideIterator; 

	static void SetKey(string key, string value, bool isSystemParam = false, size_t lineNumber = 0);
    static void SetKey(string field,string key, string value, bool isSystemParam = false, size_t lineNumber = 0);

	static void OverrideKeys(const OverrideMap *map);
	static void ReadIniFile(string filename, bool isSystemParam);
	static void InitEnumsFromStrings();
	static bool CheckIfAllSet();
	static void WriteValuesOut(std::ofstream &visDataOut);

	static int getBool(const std::string &field, bool *val);
	static int getUint(const std::string &field, unsigned int *val);
	static int getUint64(const std::string &field, uint64_t *val);
	static int getFloat(const std::string &field, float *val);

private:
	static void WriteParams(std::ofstream &visDataOut, paramType t);
	static void Trim(string &str);
};
}


#endif
