
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#include <Windows.h>

#include "common.h"

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	/*{
		union USC2
		{
			char ch[2];
			wchar_t wch = 0;
		}usc_char;

		_setmode(_fileno(stdout), _O_U16TEXT);

		FILE* f = fopen("C:\\Users\\Admon\\Desktop\\utf8.txt", "rb");
		if (f)
		{
			while (!feof(f))
			{
				wchar_t ch = fgetc(f);
				if (ch == 0xffff)
					break;

				if (ch & 0xC0)
				{
					if (!feof(f))
					{
						auto ch2 = fgetc(f);
						if (ch2 & 0x80)
						{
							usc_char.ch[0] = ch2 ^ 0x80;
							usc_char.ch[1] = (ch ^ 0xC0) >> 2;
						}
					}
				}
				else
				{
					usc_char.wch = ch;
				}

				wprintf(L"%c", usc_char.wch);
			}

			fclose(f);
		}
	}*/

	/*
	* 1. Get file list and other parameters.
	* 2. Get basic information about modules, classes, functions.
	* 3. Do compilation.
	* 4. Linking.
	*/
	SESC_compilation_data cd;
	for (int i = 0; i < argc; ++i)
	{
		//str.clear();
		//str += argv[i];
		if (strcmp(argv[i], "-out") == 0 )
		{
			++i;
			if (i < argc)
				cd.out_file = argv[i];
		}
		else
		{
			FILE* f = fopen(argv[i], "rb");
			if (f)
			{
				fclose(f);
				auto sz = strlen(argv[i]);
				if (sz > 6)
				{
					if ( strcmp(&argv[i][sz - 5], ".sesc") == 0)
					{
						cd.input_files.push_back(argv[i]);
					}
				}
			}
		}
	}

	printf("Process [%i] files\n", cd.input_files.size());

	if (cd.input_files.size())
	{
		cd.m_keyWords.insert("uint8");
		cd.m_keyWords.insert("uint16");
		cd.m_keyWords.insert("uint32");
		cd.m_keyWords.insert("uint64");
		cd.m_keyWords.insert("int8");
		cd.m_keyWords.insert("int16");
		cd.m_keyWords.insert("int32");
		cd.m_keyWords.insert("int64");
		cd.m_keyWords.insert("float16");
		cd.m_keyWords.insert("float32");
		cd.m_keyWords.insert("float64");
		cd.m_keyWords.insert("__module");
		cd.m_keyWords.insert("__main");
		cd.m_keyWords.insert("__globals");
		cd.m_keyWords.insert("__globals_private");
		cd.m_keyWords.insert("___import");
		cd.m_keyWords.insert("const");
		cd.m_keyWords.insert("if");
		cd.m_keyWords.insert("for");
		cd.m_keyWords.insert("while");
		cd.m_keyWords.insert("break");
		cd.m_keyWords.insert("return");

		for (int i = 0; i < cd.input_files.size(); ++i)
		{
			printf("[1/2] [%s]...\n", cd.input_files[i].c_str());

			FILE * file = fopen(cd.input_files[i].c_str(), "rb");
			if (file)
			{
				fseek(file, 0, SEEK_END);
				int file_sz = ftell(file);
				fseek(file, 0, SEEK_SET);
				if (file_sz > 2)
				{
					unsigned char bom[3] = { 0, 0, 0 };
					fread(&bom[0], 3, 1, file);
					//printf("BOM [%#02X %#02X %#02X]\n", bom[0], bom[1], bom[2]);
					fseek(file, 0, SEEK_SET);
					
					if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)
					{
						fseek(file, 3, SEEK_SET);
						file_sz -= 3;
					}

				}
				if (file_sz)
				{
					cd.text_buffer = new unsigned char[file_sz+1];
					fread(cd.text_buffer, file_sz + 1, 1, file);
					cd.text_buffer[file_sz] = 0;
					cd.text_currPosition = cd.text_buffer;
				}
				fclose(file);

				SESC_stage1(&cd);
			}
		}
	}

	return 0;
}