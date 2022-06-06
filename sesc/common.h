#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>

#define SESC_TokenSizeMax 500
class SESC_module 
{
public:
	SESC_module() {}
	~SESC_module() {}
};

struct SESC_moduleFileListNode
{
	//bool m_fromLibFile = false;
	std::vector<std::string> m_files;
};

struct SESC_compilation_data
{
	SESC_compilation_data() {}
	~SESC_compilation_data() 
	{
	}

	std::string out_file;
	std::vector<std::string> input_files;

	std::string m_currFilePath;

	std::map<std::wstring, void(*)(SESC_compilation_data*)> m_keyWords;
	void _initKeyWords();
	
	std::map<std::wstring, SESC_moduleFileListNode> m_moduleFileListMap;

	std::wstring text_buffer;
	const wchar_t* text_currPosition = 0;
	
	std::wstring module_name;

	void OnNewFile() {
		module_name.clear();
		text_buffer.clear();
		text_currPosition = 0;
		m_line = 0;
		m_col = 0;
		m_tokenLine = 0;
		m_tokenCol = 0;
		m_good = true;
		m_moduleKeyIsFound = false;
	}
	int m_line = 0;
	int m_col = 0;
	int m_tokenLine = 0;
	int m_tokenCol = 0;
	bool m_good = true;

	// error if not found
	bool m_moduleKeyIsFound = false;

	int m_blockCount = 0; // { }

	std::vector<std::string> m_importModuleList;
};

void SESC_doWork(SESC_compilation_data* cd);
void SESC_printError(SESC_compilation_data* cd, const wchar_t* format, ...);
void SESC_printErrorLn(SESC_compilation_data* cd, const wchar_t* format, ...);
