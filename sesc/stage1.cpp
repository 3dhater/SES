#include "common.h"

#include <cstdarg>

wchar_t token1[SESC_TokenSizeMax];
wchar_t token2[SESC_TokenSizeMax];

void SESC_getToken(SESC_compilation_data* cd, wchar_t* str);


bool SESC_charIsSpace(wchar_t c)
{
	if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n' || c == L'\f' || c == L'\v')
		return true;

	return false;
}


// Если делать всё на подобных стейтах то всё будет чётко, но возможно будет много свитчей
// Главное хорошо продумать и не стесняться добавлять новые.
enum CompilerState
{
	CompilerState_Free, // компилятор ничего не ожидает, или по другому он ждёт хоть что-то
};

bool SESC_charIsSymbol(wchar_t c)
{
	if (c > 0x20 && c < 0x30)
		return true;
	if (c > 0x39 && c < 0x40)
		return true;
	if (c > 0x5A && c < 0x61)
		return true;
	if (c > 0x7A && c < 0x7F)
		return true;
	return false;
}

bool SESC_charIsDigit(wchar_t c)
{
	if (c > 0x2F && c < 0x3A)
		return true;
	return false;
}

bool SESC_charIsAlpha(wchar_t c)
{
	if (c > 0x40 && c < 0x5B)
		return true;
	if (c > 0x60 && c < 0x7B)
		return true;
	return false;
}

void SESC_lineComment(SESC_compilation_data* cd)
{
	while (*cd->text_currPosition)
	{
		cd->text_currPosition++;
		if (*cd->text_currPosition == L'\n')
		{
			cd->text_currPosition++;
			cd->m_line++;
			cd->m_col = 0;
			break;
		}
	}
}
void SESC_multilineComment(SESC_compilation_data* cd)
{
	while (*cd->text_currPosition)
	{
		if (*cd->text_currPosition == L'\n')
		{
			cd->m_line++;
			cd->m_col = 0;
		}
		if (cd->text_currPosition[0] == L'*' && cd->text_currPosition[1] == L'/')
		{
			cd->text_currPosition++;
			cd->m_col++;
			cd->text_currPosition++;
			cd->m_col++;
			break;
		}
		cd->text_currPosition++;
		cd->m_col++;
	}
}

void SESC_printErrorLn(SESC_compilation_data* cd, const wchar_t* format, ...)
{
	static wchar_t m_buffer[1024];
	m_buffer[0] = 0;
	va_list arg;
	va_start(arg, format);
	std::vswprintf(m_buffer, 1024, format, arg);
	va_end(arg);

	fwprintf(stderr, L"%s in Line: %i Col: %i\n", m_buffer, cd->m_tokenLine + 1, cd->m_tokenCol + 1);
}

void SESC_printError(SESC_compilation_data* cd, const wchar_t* format, ...)
{
	static wchar_t m_buffer[1024];
	m_buffer[0] = 0;
	va_list arg;
	va_start(arg, format);
	std::vswprintf(m_buffer, 1024, format, arg);
	va_end(arg);

	fwprintf(stderr, L"%s\n", m_buffer);
}

void SESC_getWord(SESC_compilation_data* cd, std::wstring* out_word)
{
	/*out_word->clear();

	while (cd->text_currPosition)
	{
		auto c = SESC_getChar(cd);
		if (c == L'_' || SESC_charIsAlpha(c))
			out_word->push_back(c);
		else
			break;
	}*/
}

// tokens:
/*
	* ( ) [ ] { } / ; = 
	word _word _word100
*/

const char g_singleSymbolTokens[] =
{
	// skip control
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

//'  '! " #  $ % & '  ( ) * +  , - . /
	0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,

//  0 1 2 3  4 5 6 7  8 9 : ;  < = > ?
	0,0,0,0, 0,0,0,0, 0,0,1,1, 1,1,1,1,

//  @ A B C  D E F G  H I J K  L M N O
	1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

//  P Q R S  T U V W  X Y Z [    ] ^ _
    0,0,0,0, 0,0,0,0, 0,0,0,1, 1,1,1,0,

//  ` a b c  d e f g  h i j k  l m n o
	1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

//  p q r s  t u v w  x y z {  | } ~
    0,0,0,0, 0,0,0,0, 0,0,0,1, 1,1,1
};

void SESC_getToken(SESC_compilation_data* cd, wchar_t* str)
{
	str[0] = 0;
	str[1] = 0;
	str[2] = 0;

	bool ready = false;
	bool isWord = false;
	int token_size = 0;

	while (*cd->text_currPosition)
	{

		if (*cd->text_currPosition == L'_' || SESC_charIsAlpha(*cd->text_currPosition))
		{
			if (!isWord)
			{
				cd->m_tokenCol = cd->m_col - 1;
				cd->m_tokenLine = cd->m_line;
			}

			isWord = true;
			if (token_size < SESC_TokenSizeMax)
			{
				str[token_size] = *cd->text_currPosition;
				str[token_size + 1] = 0;
				token_size++;
			}
		}

		if (!isWord)
		{
			if (*cd->text_currPosition < 127)
			{
				if (g_singleSymbolTokens[*cd->text_currPosition])
				{
					str[0] = cd->text_currPosition[0];
					str[1] = 0;
					str[2] = cd->text_currPosition[1];
					cd->m_tokenCol = cd->m_col;
					cd->m_tokenLine = cd->m_line;
					ready = true;
				}
			}
		}
		else
		{
			if (SESC_charIsDigit(*cd->text_currPosition))
			{
				if (token_size < SESC_TokenSizeMax)
				{
					str[token_size] = *cd->text_currPosition;
					str[token_size + 1] = 0;
					token_size++;
				}
			}
			else if (SESC_charIsSpace(*cd->text_currPosition))
			{
				ready = true;
				cd->text_currPosition--;
				cd->m_col--;
			}
			else
			{
				if (*cd->text_currPosition < 127)
				{
					if (g_singleSymbolTokens[*cd->text_currPosition])
					{
						ready = true;
						cd->text_currPosition--;
					}
				}
			}
		}


		if (*cd->text_currPosition == L'\n')
		{
			cd->m_line++;
			cd->m_col = 0;
		}


		cd->text_currPosition++;
		cd->m_col++;

		if (ready)
			break;
	}
}

// Нужно получить список, модуль-файл.
// Текущий файл может иметь "__module", если нет то значит этот файл не виден для других файлов,
//  а значит его надо проигнорировать.
// Два раза "__module" не может быть.
// "__module" не может находится в { }
void SESC_stage0(SESC_compilation_data* cd)
{
	memset(token1, 0, SESC_TokenSizeMax * sizeof(wchar_t));
	memset(token2, 0, SESC_TokenSizeMax * sizeof(wchar_t));
	
	cd->m_tokenCol = 0;
	cd->m_tokenLine = 0;
	cd->m_moduleKeyIsFound = false;

	CompilerState cs = CompilerState_Free;
	while (*cd->text_currPosition)
	{
		SESC_getToken(cd, token1);
		switch (cs)
		{
		case CompilerState_Free:
		{
			if (token1[0] == L'/')
			{
				if (token1[2] == L'/')
					SESC_lineComment(cd);
				else if (token1[2] == L'*')
				{
					cd->text_currPosition++;
					cd->m_col++;
					SESC_multilineComment(cd);
				}
				else
					SESC_printErrorLn(cd, L"Unexpected token '%s' (expect / or *)", token1);
			}
			else if (token1[0] == L'{')
			{
				cd->m_blockCount++;
			}
			else if (token1[0] == L'}')
			{
				if(cd->m_blockCount)
					cd->m_blockCount--;
			}
			else if (token1[0] == L'_' || SESC_charIsAlpha(token1[0]))
			{
				auto it = cd->m_keyWords.find(token1);
				if (it != cd->m_keyWords.end())
				{
					it->second(cd);
				}
			}
		}break;
		}
	}
	
	//if (cd->m_moduleKeyIsFound)
	//	wprintf(L"MODULE %s\n", cd->module_name.data());
}

void SESC_stage1(SESC_compilation_data* cd)
{
	CompilerState cs = CompilerState_Free;
	std::wstring current_word;
	
	memset(token1, 0, SESC_TokenSizeMax * sizeof(wchar_t));
	memset(token2, 0, SESC_TokenSizeMax * sizeof(wchar_t));

	/*while (*cd->text_currPosition)
	{
		SESC_getToken(cd, token1);

		switch (cs)
		{
		case CompilerState_Free:
		{
			if (token1[0] == L'/')
			{
				if(token1[2] == L'/')
					SESC_lineComment(cd);
				else if (token1[2] == L'*')
				{
					cd->text_currPosition++;
					cd->m_col++;
					SESC_multilineComment(cd);
				}
				else 
					SESC_printErrorLn(cd, L"Unexpected token '%s' (expect / or *)", token1);
			}
			else if (token1[0] == L'_' || SESC_charIsAlpha(token1[0]))
			{
				auto it = cd->m_keyWords.find(token1);
				if (it != cd->m_keyWords.end())
				{
					it->second(cd);
				}
				else
				{
					SESC_printErrorLn(cd, L"Unexpected token '%s'", token1);
				}
			}

		}break;

		}
	}*/
}

void SESC_keywordFunction_uint8(SESC_compilation_data* cd) {}
void SESC_keywordFunction_uint16(SESC_compilation_data* cd) {}
void SESC_keywordFunction_uint32(SESC_compilation_data* cd) {}
void SESC_keywordFunction_uint64(SESC_compilation_data* cd) {}
void SESC_keywordFunction_int8(SESC_compilation_data* cd) {}
void SESC_keywordFunction_int16(SESC_compilation_data* cd) {}
void SESC_keywordFunction_int32(SESC_compilation_data* cd) {}
void SESC_keywordFunction_int64(SESC_compilation_data* cd) {}
void SESC_keywordFunction_float16(SESC_compilation_data* cd) {}
void SESC_keywordFunction_float32(SESC_compilation_data* cd) {}
void SESC_keywordFunction_float64(SESC_compilation_data* cd) {}
// Ожидается имя модуля и ;
void SESC_keywordFunction___module(SESC_compilation_data* cd)
{
	if (cd->m_blockCount)
	{
		SESC_printErrorLn(cd, L"`__module` can not be placed inside {} block");
		cd->m_good = false;
		return;
	}

	if (cd->m_moduleKeyIsFound)
	{
		SESC_printErrorLn(cd, L"`__module` redefinition");
		cd->m_good = false;
		return;
	}

	SESC_getToken(cd, token1);
	if (token1[0] == L'_' || SESC_charIsAlpha(token1[0]))
	{
		SESC_getToken(cd, token2);

		if (token2[0] == L';')
		{
			cd->module_name = token1;
			cd->m_moduleKeyIsFound = true;
		}
		else
		{
			SESC_printErrorLn(cd, L"Unexpected token '%s' after '%s'", token2, token1);
			SESC_printError(cd, L"Need ';' after '%s'", token1);
		}
	}
	else
	{
		SESC_printErrorLn(cd, L"Unexpected token [%s]", token1);
		SESC_printError(cd, L"Need module name");
	}
	return;
}
void SESC_keywordFunction___main(SESC_compilation_data* cd) {}
void SESC_keywordFunction___globals(SESC_compilation_data* cd) {}
void SESC_keywordFunction___globals_private(SESC_compilation_data* cd) {}
void SESC_keywordFunction___import(SESC_compilation_data* cd) {}
void SESC_keywordFunction_const(SESC_compilation_data* cd) {}
void SESC_keywordFunction_if(SESC_compilation_data* cd) {}
void SESC_keywordFunction_for(SESC_compilation_data* cd) {}
void SESC_keywordFunction_while(SESC_compilation_data* cd) {}
void SESC_keywordFunction_break(SESC_compilation_data* cd) {}
void SESC_keywordFunction_return(SESC_compilation_data* cd) {}

void SESC_compilation_data::_initKeyWords()
{
	m_keyWords[L"uint8"] = SESC_keywordFunction_uint8;
	m_keyWords[L"uint16"] = SESC_keywordFunction_uint16;
	m_keyWords[L"uint32"] = SESC_keywordFunction_uint32;
	m_keyWords[L"uint64"] = SESC_keywordFunction_uint64;
	m_keyWords[L"int8"] = SESC_keywordFunction_int8;
	m_keyWords[L"int16"] = SESC_keywordFunction_int16;
	m_keyWords[L"int32"] = SESC_keywordFunction_int32;
	m_keyWords[L"int64"] = SESC_keywordFunction_int64;
	m_keyWords[L"float16"] = SESC_keywordFunction_float16;
	m_keyWords[L"float32"] = SESC_keywordFunction_float32;
	m_keyWords[L"float64"] = SESC_keywordFunction_float64;
	m_keyWords[L"__module"] = SESC_keywordFunction___module;
	m_keyWords[L"__main"] = SESC_keywordFunction___main;
	m_keyWords[L"__globals"] = SESC_keywordFunction___globals;
	m_keyWords[L"__globals_private"] = SESC_keywordFunction___globals_private;
	m_keyWords[L"___import"] = SESC_keywordFunction___import;
	m_keyWords[L"const"] = SESC_keywordFunction_const;
	m_keyWords[L"if"] = SESC_keywordFunction_if;
	m_keyWords[L"for"] = SESC_keywordFunction_for;
	m_keyWords[L"while"] = SESC_keywordFunction_while;
	m_keyWords[L"break"] = SESC_keywordFunction_break;
	m_keyWords[L"return"] = SESC_keywordFunction_return;
}