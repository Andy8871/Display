#ifndef __SYSTEM__STRING__H___
#define __SYSTEM__STRING__H___


#define STRID_INFORMATION					(CBinary("\x00\x00\x00\x00\x00\x01",6))         
#define STRID_ERROR							(CBinary("\x00\x00\x00\x00\x00\x02",6))         
                                                                                            
#define STRID_COMMUNICATING					(CBinary("\x00\x00\x00\x00\x00\x03",6))         
#define STRID_COMMUNICATION_FAILED			(CBinary("\x00\x00\x00\x00\x00\x04",6))         
                                                                                            
#define STRID_CONNECTING_ECU				(CBinary("\x00\x00\x00\x00\x00\x05",6))         
#define STRID_CONNECT_ECU_FAILED			(CBinary("\x00\x00\x00\x00\x00\x06",6))         
                                                                                            
#define STRID_READINGCONTROLUNITVERSION		(CBinary("\x00\x00\x00\x00\x00\x07",6))         
#define STRID_READCONTROLUNITVERSION_FAILED	(CBinary("\x00\x00\x00\x00\x00\x08",6))         
                                                                                                
#define STRID_READINGFAULTCODE				(CBinary("\x00\x00\x00\x00\x00\x09",6))             
#define STRID_READFAULTCODE_FAILED			(CBinary("\x00\x00\x00\x00\x00\x0A",6))             
#define STRID_NOFAULTCODEPRESENT			(CBinary("\x00\x00\x00\x00\x00\x0B",6))             
                                                                                                
#define STRID_ASK_CLEARFAULTCODE			(CBinary("\x00\x00\x00\x00\x00\x0C",6))                                                                                                                                                                                                                                                                                        
#define STRID_CLEARINGFAULTCODE				(CBinary("\x00\x00\x00\x00\x00\x0D",6))
#define STRID_CLEARFAULTCODE_FAILED			(CBinary("\x00\x00\x00\x00\x00\x0E",6))
#define STRID_CLEARFAULTCODE_OK				(CBinary("\x00\x00\x00\x00\x00\x0F",6))

#define STRID_ASK_END_DIAGNOSIS				(CBinary("\x00\x00\x00\x00\x00\x10",6))
#define STRID_QUITING_SYSTEM				(CBinary("\x00\x00\x00\x00\x00\x11",6))

#define STRID_ASK_START_SHORTTEST			(CBinary("\x00\x00\x00\x00\x00\x12",6))
#define STRID_ASK_STOP_SHORTTEST			(CBinary("\x00\x00\x00\x00\x00\x13",6))

#endif