/***
 *msc_opt.h - MSC compiler options
 *    Copyright (c) 2000-2012, NetPosa Technologies,Ltd. All rights reserved.
 *Purpose:
 **************************************************************************************/

#ifndef _msc_opt_h_
#define _msc_opt_h_

#if _MSC_VER > 1000
	#pragma once
#endif

#ifdef _MSC_VER
	/* 标记代码 */
	#define remind_str0(x)			#x
	#define remind_str(x)			remind_str0(x)
	#define compiling_remind(desc)	message(__FILE__ "(" remind_str(__LINE__) "): " #desc)
	/*	例： */
	/*	#pragma compiling_remind("以后修改此处!") */

	/*********************************/
	/******** 全局编译开关 ***********/
	/*********************************/
	#pragma warning(disable: 4097)	/* typedef-name 'identifier1' used as synonym for class-name 'identifier2' */
	#pragma warning(disable: 4127)	/* conditional expression is constant */
	#pragma warning(disable: 4201)	/* nonstandard extension used : nameless struct/union */
	#pragma warning(disable: 4239)	/* CNPAutoPtr<TYPE> ptr; ptr = CNPAutoPtr<TYPE>(1); */
	#pragma warning(disable: 4251)	/* needs to have dll-interface to be used by clients of class 'XXX' */
	#pragma warning(disable: 4275)	/* non dll-interface class 'A' used as base for dll-interface class 'B' */
	#pragma warning(disable: 4284)	/* return type for 'XXX::operator->' is 'const int*' (ie; not a UDT or reference to a UDT.) */
	#pragma warning(disable: 4355)	/* 'this' : used in base member initializer list */
	#pragma warning(disable: 4503)	/* 'x' : decorated name length exceeded, name was truncated */
	#pragma warning(disable: 4511)	/* copy constructor could not be generated */
	#pragma warning(disable: 4512)	/* assignment operator could not be generated */
	#pragma warning(disable: 4514)	/* unreferenced inline function has been removed */
	#pragma warning(disable: 4663)	/* In order to explicitly specialize a template class, use the new syntax */
	#pragma warning(disable: 4710)	/* function not inlined */
	#pragma warning(disable: 4714)	/* function not inlined */
	#pragma warning(disable: 4786)	/* identifier was truncated to 'number?' characters in the debug information */

	#pragma warning(1: 4100)		/* unreferenced formal parameter */
	#pragma warning(1: 4101)		/* unreferenced local parameter */

	#pragma warning(error: 4002)	/* too many actual parameters for macro 'identifier' */
	#pragma warning(error: 4003)	/* not enough actual parameters for macro 'identifier' */
	#pragma warning(error: 4005)	/* macro redefinition */
	#pragma warning(error: 4013)	/* function 'func' undefined; assuming extern returning int */
	#pragma warning(error: 4016)	/* 'function' : no function return type; using int as default */
	#pragma warning(error: 4018)	/* 'expression' : signed/unsigned mismatch */
	#pragma warning(error: 4033)	/* 'function' : must return a value */
	#pragma warning(error: 4035)	/* 'function' : no return value */
	#pragma warning(error: 4045)	/* 'identifier' : array bounds overflow */
	#pragma warning(error: 4051)	/* type conversion; possible loss of data */
	#pragma warning(error: 4067)	/* unexpected tokens following preprocessor directive ? expected a newline */
	#pragma warning(error: 4091)	/* 'typedef ' : ignored on left of 'struct/class/... xxxx' when no variable is declared */
	#pragma warning(error: 4099)	/* 'identifier' : type name first seen using 'objecttype1' now seen using 'objecttype2' */
	#pragma warning(error: 4102)	/* unreferenced label */
	#pragma warning(error: 4125)	/* decimal digit terminates octal escape sequence */
	#pragma warning(error: 4129)	/* 'x' : unrecognized character escape sequence */
	#pragma warning(error: 4130)	/* '==' : logical operation on address of string constant */
	#pragma warning(error: 4146)	/* unary minus operator applied to unsigned type, result still unsigned */
	#pragma warning(error: 4172)	/* returning address of local variable or temporary */
	#pragma warning(error: 4189)	/* 'xxx' : local variable is initialized but not referenced */
	#pragma warning(error: 4244)	/* '=' : conversion from 'TYPE1' to 'TYPE2', possible loss of data */
	#pragma warning(error: 4305)	/* 'XXX' : truncation from 'const int' to 'bool' */
	#pragma warning(error: 4305)	/* conversion from 'XXX' to 'YYY' of greater size */
	#pragma warning(error: 4307)	/* integral constant overflow */
	#pragma warning(error: 4390)	/* ';' : empty controlled statement found; is this the intent? */
	#pragma warning(error: 4508)	/* 'func_xxx' : function should return a value; 'void' return type assumed */
	#pragma warning(error: 4541)	/* 'dynamic_cast' used on polymorphic type 'class XXX' with /GR-; unpredictable behavior may result */
	#pragma warning(error: 4551)	/* function call missing argument list */
	#pragma warning(error: 4552)	/* 'xx' operator has no effect; expected operator with side-effect */
	#pragma warning(error: 4553)	/* '==' : operator has no effect; did you intend '='? */
	#pragma warning(error: 4554)	/* 'operator x' : check operator precedence for possible error; use parentheses to clarify precedence */
	#pragma warning(error: 4700)	/* local variable used without having been initialized */
	#pragma warning(error: 4701)	/* local variable '?' may be used without having been initialized */
	#pragma warning(error: 4706)	/* assignment within conditional expression */
	#pragma warning(error: 4715)	/* 'function' : not all control paths return a value */
	#pragma warning(error: 4799)	/* function 'xxx' has no EMMS instruction */
	#pragma warning(error: 4800)	/* 'xxx' : forcing value to bool 'true' or 'false' (performance warning) */
	#pragma warning(error: 4804)	/* 'bit oper' : unsafe use of type 'bool' in operation */

	#if (_MSC_VER >= 1400)
		#pragma warning(disable: 4996) /* 'function' was declared deprecated */
	#endif
	#if (_MSC_VER >= 1500)	/* >= VC2008 */
		#pragma warning(error: 4293)	/* '<<' : shift count negative or too big, undefined behavior */
		#pragma warning(error: 4267)	/* from "xxx" convert to "yyy", possible loss of data */
	#endif

	#ifndef __cplusplus		/* C Language */
		#pragma warning(disable: 4115)	/* 'xxx' : named type definition in parentheses */
		#pragma warning(disable: 4211)	/* nonstandard extension used : redefined extern to static */
		#pragma warning(disable: 4204)	/* nonstandard extension used : non-constant aggregate initializer */
		#pragma warning(error: 4020)	/* 'function' : too many actual parameters */
		#pragma warning(error: 4021)	/* 'function' : too few actual parameters */
		#pragma warning(error: 4024)	/* 'function' : different types for formal and actual parameter n */
		#pragma warning(error: 4047)	/* 'function' : 'aaa' differs in levels of indirection from 'bbb' */
		#pragma warning(error: 4133)	/* 'function' : incompatible types - from 'aaa' to 'bbb' */
	#endif

	/* 修正VC6的for语句变量作用域问题，修正后，变量作用域统一只在for循环内部 */
	#if (_MSC_VER <= 1200)
		#undef for
		#define for if(0) ((void)0); else for
	#endif
#endif	/* #ifdef _MSC_VER */

#endif /* #ifndef _msc_opt_h_ */
