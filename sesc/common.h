#pragma once

#include <vector>
#include <string>
#include <map>

#define SESC_TokenSizeMax 500
class SESC_module 
{
public:
	SESC_module() {}
	~SESC_module() {}
};

struct SESC_compilation_data
{
	SESC_compilation_data() {}
	~SESC_compilation_data() 
	{
	}

	std::string out_file;
	std::vector<std::string> input_files;

	std::map<std::wstring, void(*)(SESC_compilation_data*)> m_keyWords;
	void _initKeyWords();

	std::wstring text_buffer;
	const wchar_t* text_currPosition = 0;
	/*wchar_t getChar()
	{
		m_col++;
		if (*text_currPosition == L'\n')
		{
			m_col = 1;
			m_line++;
		}

		wchar_t c = *text_currPosition;
		text_currPosition++;
		return c;
	}*/
	

	int m_line = 0;
	int m_col = 0;
	int m_tokenLine = 0;
	int m_tokenCol = 0;

	bool m_good = true;

	// error if not found
	bool m_moduleKeyIsFound = false;
};

void SESC_stage1(SESC_compilation_data* cd);
void SESC_printError(SESC_compilation_data* cd, const wchar_t* format, ...);
void SESC_printErrorLn(SESC_compilation_data* cd, const wchar_t* format, ...);
