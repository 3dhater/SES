#include "common.h"

wchar_t SESC_getChar(SESC_compilation_data* cd)
{
	union USC2
	{
		char ch[2];
		wchar_t wch = 0;
	}usc_char;

	wchar_t ch = *cd->text_currPosition;
	cd->text_currPosition++;

	if (ch == 0xffff)
		return 0;

	if (ch >= 0 && ch < 0x80)
	{
		usc_char.wch = ch;
	}
	else if (ch >= 0xC0 && ch < 0xE0)
	{
		if (cd->text_currPosition)
		{
			auto ch2 = *cd->text_currPosition;
			cd->text_currPosition++;

			if (ch2)
			{
				wchar_t char16 = 0;
				char16 = ch;
				char16 ^= 0xC0;
				char16 <<= 6;
				char16 |= (ch2 ^ 0x80);

				usc_char.wch = char16;
			}
		}
	}
	else if (ch >= 0xE0 && ch < 0xF0) // 3
	{
		if (cd->text_currPosition)
		{
			auto ch2 = *cd->text_currPosition;
			cd->text_currPosition++;

			if (ch2)
			{
				auto ch3 = *cd->text_currPosition;
				cd->text_currPosition++;
				if (ch3)
				{
					wchar_t char16 = 0;
					char16 = ch;
					char16 ^= 0xE0;
					char16 <<= 12;
					char16 |= (ch2 ^ 0x80) << 6;
					char16 |= (ch3 ^ 0x80);

					usc_char.wch = char16;
				}
			}
		}
	}

	return usc_char.wch;
}

bool SESC_charIsSpace(wchar_t c)
{
	if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n' || c == L'\f' || c == L'\v')
		return true;

	return false;
}

wchar_t SESC_skipSpaces(SESC_compilation_data* cd)
{
	wchar_t ch = 0;
	while (cd->text_currPosition)
	{
		ch = SESC_getChar(cd);
		if(!SESC_charIsSpace(ch))
			break;
	}
	return ch;
}

enum CompilerState
{
	CompilerState_Free
};

void SESC_stage1(SESC_compilation_data* cd)
{
	CompilerState cs = CompilerState_Free;

	while (cd->text_currPosition)
	{
		wchar_t curr_char = SESC_getChar(cd);
		if (curr_char)
		{
			switch (cs)
			{
			case CompilerState_Free:
			{
				if (SESC_charIsSpace(curr_char))
					curr_char = SESC_skipSpaces(cd);

				if (curr_char)
				{
					wprintf(L"%c", curr_char);
				}

			}break;
			default:
				break;
			}
		}
		else
			break;

	}
}