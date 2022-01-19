
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
		cd._initKeyWords();

		for (uint32_t i = 0; i < cd.input_files.size(); ++i)
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
					unsigned char * text_buffer = new unsigned char[file_sz+1];
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
								unsigned char ch2 = text_buffer[i+1];

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
						cd.text_buffer.push_back(wch);
					}
					cd.text_currPosition = cd.text_buffer.data();
					//wprintf(L"\n%s\n", cd.text_buffer.c_str());
					delete text_buffer;
					//cd.text_currPosition = cd.text_buffer;
				}
				fclose(file);

				SESC_stage1(&cd);

				if (!cd.m_moduleKeyIsFound)
				{
					SESC_printError(&cd, L"`__module` not found or there's some error");
					continue;
				}
			}
		}
	}

	return 0;
}