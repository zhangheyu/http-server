#ifndef _CBASE64_H_
#define _CBASE64_H_

/*****************************************************************/
//                                                                             
//          Copyright (c) 2015,All rights reserved.          
//                                                                                          
//          FileName：cbase64.h                                
//                                                                                                                                         
//          Author:RichieMay                                                      
//          Date:July 27,2015                                                   
//                                                                                            
/*****************************************************************/

#ifndef IN
#define  IN
#endif

#ifndef OUT
#define  OUT
#endif

class CBase64
{
public:
	//编解码后的buff为空时，调用接口则返回所需bufer的大小
	static bool Encoder(IN const unsigned char* data, IN const unsigned int dataLen, IN char* Base64string,OUT unsigned int &outLen);
	static bool Decoder(IN const char* Base64string, IN unsigned char* data, OUT unsigned int &outLen);

	//文件路径必须存在
	//limited_block Base64编码是否分隔为多段
	static bool EncoderFile(IN const char* binaryFileName, IN const char* Base64FileName, IN bool limited_block = false);
	static bool DecoderFile(IN const char* Base64FileName, IN const char* binaryFileName, IN bool limited_block = false);
};

#endif
