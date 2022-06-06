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

// if need to use things somewhere from this file
MODULE module_name

// use things from other file
IMPORT module_name

// function_name return_type : parameter_type parameter_name
MyFunction UINT8 : INT8 i1 INT16 i2 INT32 i3 INT64 i4
	// return value or variable
	RETURN 0
;

// no return type(void printf())
printf : INT8 fmt ...
	// ARGC is the constant for this function. Number of arguments.
	UINT32 argc ARGC
	// or use just ARGC
	ARG[0] = 0 // first parameter
	// ARGC and ARG it's for something like va_list

	UINT32 i 0
	LOOP i < ARGC
		ARG[i]
		++i
	;
;

// comment
ENTRY

	// one valiable in one line
	// `type` `name` `value`
	UINT8  u1 0
	UINT16 u2   // can be without initialization
	UINT32 u3 0
	UINT64 u4 0
	INT8  i1 0
	INT16 i2 0
	INT32 i3 0
	INT64 i4 0
	FLOAT16 f1 0
	FLOAT32 f2 0
	FLOAT64 f3 0
	PTR ptr 0 // void*
	// pointers like in C, *
	UINT8* pu 0

	// array
	UINT8 a1[10]
	UINT8 a2[10] {1 2} // init only first 2

	IF 1 > 0
		// do something
	; // end of IF

	IF u1 == 0
		// do something
	ELSE
		// do something
	;

	IF u1 == 1
		// do something
	ELIF u1 == 2
		// do something
	ELSE
		// do something
	;

	IF u1 == 1
		// do something
	ELIF u1 == 2
		// do something
	;

	// like `while`
	LOOP 1 == 1
		BREAK
	;

	// CALL function_name arguments
	CALL MyFunction arg1 arg2 i1 i2 i3 i4
;

```


