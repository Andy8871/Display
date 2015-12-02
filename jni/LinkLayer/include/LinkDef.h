#if !defined(_LINK_DEF_H_)
#define _LINK_DEF_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define MAX_PATH_LENGTH	260	//MAX_PATH =260


//RETURN VALUE AND MEANING (long)
#define RET_OK								0x00000001	//Ö´ÐÐ³É¹¦1

#define RET_ERR_UNKNOWN						0x10000000

#define RET_ERR_FILE_OPEN					0x10000001
#define RET_ERR_FILE_READ					0x10000002
#define RET_ERR_FILE_WRITE					0x10000003

#define RET_ERR_MEMORY_ALLOC				0x10000004



#endif	//	!defined(_LINK_DEF_H_)