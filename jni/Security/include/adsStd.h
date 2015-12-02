
#ifndef __I386_TYPES_H_
#define __I386_TYPES_H_

#pragma  warning(disable:4786)

//数据类型定义
#ifndef CHAR
typedef char CHAR;
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif


#ifndef W_INT8
typedef char W_INT8;
#endif

#ifndef W_UINT8
typedef unsigned char W_UINT8;
#endif


#ifndef W_INT16
typedef short W_INT16;
#endif

#ifndef W_UINT16
typedef unsigned short W_UINT16;
#endif


#ifndef W_INT
typedef int W_INT;
#endif

#ifndef W_UINT
typedef unsigned int W_UINT;
#endif



#ifndef W_INT32
typedef int W_INT32;
#endif

#ifndef W_UINT32
typedef unsigned int W_UINT32;
#endif


#ifndef W_LONG
typedef long W_LONG;
#endif

#ifndef W_FLOAT
typedef double W_FLOAT;
#endif

#ifndef W_DOUBLE
typedef double W_DOUBLE;
#endif

#ifndef NULL
#define NULL	0
#endif

#ifndef FALSE
#define FALSE	false
#endif

#ifndef TRUE
#define TRUE	true
#endif

#ifndef BOOL
typedef bool BOOL;
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef max
#define max(a,b) (((a)>(b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a)<(b)) ? (a) : (b))
#endif

#define MAX_PATH	260
#endif	//	__I386_TYPES_H_
