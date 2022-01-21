# SES
Simulated Entertainment System - virtual console

# sesc 
SES Compiler

```
Translation unit must have .sesc extension: file.sesc
File format: UTF-8
Command line should be like this:
	sesc.exe file1.sesc file2.sesc file3.sesc -out output_file
```

Language
```cpp
// comment
/*
   multiline comment
   
   You can use any name (same C language rules) except next reserved names:
   
   Types:
	   uint8
	   uint16
	   uint32
	   uint64
	   int8
	   int16
	   int32
	   int64
	   float16
	   float32
	   float64
	
	keywords:
		__main{ return 0; } - C main, return integer. Only 1 per comp. unit.
		__globals{} - put all global variables inside this block. Only 1 per comp. unit.
		__globals_private{} - same as __globals but other modules will not see this variables. Only 1 per comp. unit.
		__module name; - add this if other files need to see this file.
		__import name; - use other module.
		
		const
		if
		for
		while
		break
		return
		
	stdlib:
		__sin
		__cos
*/

__module app;
__import ses;

__globals
{
	uint8 g_a = 0;
}

__main()
{
	free();
	return error_code;
}
```

asm
```

```