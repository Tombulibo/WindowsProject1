#pragma once
#include <Windows.h>
#include <FSTREAM>
#include <VECTOR>
#include <STRING>
#include <sstream>
using namespace std;

typedef vector<wstring> vecLinesContainer;

class SimpleFile
{
public:
	SimpleFile();

	BOOLEAN ReadFile(WCHAR* pathName);
	BOOLEAN WriteFile(WCHAR* pathName, WCHAR* fileLines);

	vecLinesContainer getFileLines(WCHAR* pathName);

private:
	vecLinesContainer FileContent;
};

