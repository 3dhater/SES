#pragma once

#include <vector>
#include <string>
#include <set>

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
		if (text_buffer)
			delete[] text_buffer;
	}

	std::string out_file;
	std::vector<std::string> input_files;

	std::set<std::string> m_keyWords;

	//FILE* file = 0;
	unsigned char* text_buffer = 0;
	unsigned char* text_currPosition = 0;
	//int file_sz = 0;
	//bool m_eof = false;
};

void SESC_stage1(SESC_compilation_data* cd);

