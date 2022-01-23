
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#include <Windows.h>

#include "common.h"

void SESC_GetTextFromFile(const char* fn, std::wstring* str)
{
	str->clear();
	FILE* file = fopen(fn, "rb");
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
			unsigned char* text_buffer = new unsigned char[file_sz + 1];
			fread(text_buffer, file_sz + 1, 1, file);
			text_buffer[file_sz] = 0;

			for (int i = 0; i < file_sz; ++i)
			{
				wchar_t wch = 0;

				unsigned char ch = text_buffer[i];

				if (ch >= 0 && ch < 0x80)
				{
					wch = ch;
				}
				else if (ch >= 0xC0 && ch < 0xE0)
				{
					if (i + 1 < file_sz)
					{
						unsigned char ch2 = text_buffer[i + 1];

						if (ch2)
						{
							wchar_t char16 = 0;
							char16 = ch;
							char16 ^= 0xC0;
							char16 <<= 6;
							char16 |= (ch2 ^ 0x80);

							wch = char16;
						}
					}
				}
				else if (ch >= 0xE0 && ch < 0xF0) // 3
				{
					if (i + 1 < file_sz)
					{
						unsigned char ch2 = text_buffer[i + 1];

						if (ch2)
						{
							if (i + 1 < file_sz)
							{
								unsigned char ch3 = text_buffer[i + 2];
								if (ch3)
								{
									wchar_t char16 = 0;
									char16 = ch;
									char16 ^= 0xE0;
									char16 <<= 12;
									char16 |= (ch2 ^ 0x80) << 6;
									char16 |= (ch3 ^ 0x80);

									wch = char16;
								}
							}
						}
					}
				}
				str->push_back(wch);
			}
			//cd.text_currPosition = cd.text_buffer.data();
			//wprintf(L"\n%s\n", cd.text_buffer.c_str());
			delete text_buffer;
		}
		fclose(file);
	}
}

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	/*
	* 1. Get file list and other parameters.
	* 2. Текущий файл должен знать, какие модули он использует. Нужно получить список, модуль-файл.
	* 3. Построить список зависимостей. В какой последовательности брать информацию (нужны только объявления).
	* 4. В соответствии со списком, анализирую все необходимые модули.
	* 5. Компиляция.
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
		cd._initKeyWords();

		for (uint32_t i = 0; i < cd.input_files.size(); ++i)
		{
			printf("[1/2] [%s]...\n", cd.input_files[i].c_str());
			
			cd.OnNewFile();
			cd.m_currFilePath = cd.input_files[i];

			SESC_GetTextFromFile(cd.m_currFilePath.c_str(), &cd.text_buffer);
			cd.text_currPosition = cd.text_buffer.data();

			cd.m_compiler_state = SESC_CompilerState_Stage0;
			cd.m_moduleKeyIsFound = false;
			// Нужно получить список, модуль-файл.
			// Текущий файл может иметь "__module", если нет то значит 
			//  этот файл не виден для других файлов,
			//  а значит его надо проигнорировать.
			// Два раза "__module" не может быть.
			// "__module" не может находится в { }
			SESC_doWork(&cd); // module-fileName

			if (cd.m_moduleKeyIsFound)
			{
				auto it = cd.m_moduleFileListMap.find(cd.module_name);
				if (it != cd.m_moduleFileListMap.end())
				{
					it->second.m_files.push_back(cd.input_files[i]);
				}
				else
				{
					SESC_moduleFileListNode n;
					n.m_files.push_back(cd.input_files[i]);
					cd.m_moduleFileListMap[cd.module_name] = n;
				}
			}
		}

		if (cd.m_good)
		{
			for (uint32_t i = 0; i < cd.input_files.size(); ++i)
			{
				printf("[2/2] [%s]...\n", cd.input_files[i].c_str());

				cd.OnNewFile();
				cd.m_currFilePath = cd.input_files[i];

				SESC_GetTextFromFile(cd.input_files[i].c_str(), &cd.text_buffer);
				cd.text_currPosition = cd.text_buffer.data();

				cd.m_compiler_state = SESC_CompilerState_Stage1;
				SESC_doWork(&cd); // read all import
				if (cd.m_importModuleList.size())
				{
					for (size_t o = 0, sz = cd.m_importModuleList.size(); o < sz; ++o)
					{
						cd.OnNewFile();
						cd.m_currFilePath = cd.m_importModuleList[o];
						SESC_GetTextFromFile(cd.m_currFilePath.c_str(), &cd.text_buffer);
						cd.text_currPosition = cd.text_buffer.data();

						SESC_doWork(&cd); // read all import
					}
				}

				//SESC_stage2(&cd); // compile

				// scan all other .sesc (#include)
				if (cd.m_importModuleList.size())
				{
					std::set<std::string> skipThis;
					cd.m_compiler_state = SESC_CompilerState_ScanImport;

					for (size_t i2 = cd.m_importModuleList.size() - 1; i2 >= 0; )
					{
						auto & md = cd.m_importModuleList[i2];
						auto it = skipThis.find(md);
						if (it == skipThis.end())
						{
							skipThis.insert(md);

							cd.OnNewFile();
							cd.m_currFilePath = md;
							SESC_GetTextFromFile(cd.m_currFilePath.c_str(), &cd.text_buffer);
							cd.text_currPosition = cd.text_buffer.data();
							SESC_doWork(&cd);
						}

						if (i2 == 0)
							break;
						i2--;
					}
				}

				cd.m_importModuleList.clear();
			}
		}

		/*for (auto & o : cd.m_moduleFileListMap)
		{
			for (size_t i = 0; i < o.second.m_files.size(); ++i)
			{
				wprintf(L"MODULE %s ", o.first.data());
				printf("FILE %s\n", o.second.m_files[i].c_str());
			}
		}*/
	}

	return 0;
}