#include <stdio.h>
#include <string.h>
#include "cbase64.h"

#if defined(_WIN32)
#pragma warning(disable:4996)
#endif
//Base64编码字典
const unsigned char BASE64_ENCODE_ALPHABET[64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

//Base64解码字典
const unsigned char BASE64_DECODE_ALPHABET[128] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x3f,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
	0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00
};

//文件编解码Base64字符串的块大小
const unsigned int base64string_max_line_len = 25 * 4; //4的倍数


bool CBase64::Encoder(const unsigned char* data, const unsigned int dataLen, char* Base64string, unsigned int &outLen)
{
	outLen = (dataLen + 2) / 3 * 4;//字符串结束符 \0
	if (NULL == Base64string)
		return true;

	const unsigned char compare = 0x3f;
	unsigned int unit = 0;
	unsigned char* block = (unsigned char*)(&unit);
	const unsigned char* p = data;
	char *pString = Base64string;

	unsigned int round = dataLen / 3;
	unsigned int mod = dataLen % 3;

	while (round--)
	{	
		block[2] = p[0];
		block[1] = p[1];
		block[0] = p[2];
 
		pString[3] = BASE64_ENCODE_ALPHABET[unit & compare];
		pString[2] = BASE64_ENCODE_ALPHABET[unit >> 6 & compare];
		pString[1] = BASE64_ENCODE_ALPHABET[unit >> 12 & compare];
		pString[0] = BASE64_ENCODE_ALPHABET[unit >> 18 & compare];

		pString += 4;
		p += 3;
	}

	if (mod != 0)
	{
		pString[2] = '\0';
		pString[mod+1] = '=';

		unit = 0;
		do
		{
			block[3 - mod] = p[mod-1];
		} while (--mod);

		pString[0] = BASE64_ENCODE_ALPHABET[unit >> 18 & compare];
		pString[1] = BASE64_ENCODE_ALPHABET[unit >> 12 & compare];
		
		if(pString[2] != '=')
			pString[2] = BASE64_ENCODE_ALPHABET[unit >> 6 & compare];

		pString[3] = '=';		//长度有余时pString[3]必定为=
	}

	return true;
}

bool CBase64::Decoder(const char* Base64string, unsigned char* data, unsigned int &outLen)
{
	//计算长度
	unsigned int length = strlen(Base64string);//必须是带'\0'结束符的字符指针
	if (length < 4 || length % 4 != 0)
	{
		return  false;
	}

	outLen = length / 4 * 3;
	if (Base64string[length - 2] == '=')
	{
		outLen -= 2;
	}
	else if (Base64string[length - 1] == '=')
	{
		outLen--;
	}

	//解码
	if (NULL == data)
	{
		return true;
	}
	else
	{
		unsigned int unit = 0;
		unsigned char* block = (unsigned char*)(&unit);
		unsigned char* p = data;
		const char* pString = Base64string;

		unsigned int round = outLen / 3;
		unsigned int mod = outLen % 3;

		while (round--)
		{
			unit = (BASE64_DECODE_ALPHABET[(short)pString[0]] << 18);	
			unit += (BASE64_DECODE_ALPHABET[(short)pString[1]] << 12);
			unit += (BASE64_DECODE_ALPHABET[(short)pString[2]] << 6);
			unit += (BASE64_DECODE_ALPHABET[(short)pString[3]]);

			p[0] = block[2];
			p[1] = block[1];
			p[2] = block[0];

			pString += 4;
			p += 3;
		}

		if (mod != 0)
		{
			unit = (BASE64_DECODE_ALPHABET[(short)pString[0]] << 18);
			unit += (BASE64_DECODE_ALPHABET[(short)pString[1]] << 12);

			char c = pString[2];
			if (c != '=')
				unit += (BASE64_DECODE_ALPHABET[(short)c] << 6);

			do
			{
				p[mod - 1] = block[3 - mod];
			} while (--mod);
		}

		return true;
	}
}

bool CBase64::EncoderFile(const char* binaryFileName, const char* Base64FileName, bool limited_block/* = false*/)
{
	FILE* fd_read = fopen(binaryFileName, "rb");
	if (NULL == fd_read)
	{
		return false;
	}

	FILE* fd_write = fopen(Base64FileName, "w");
	if (NULL == fd_write)
	{
		fclose(fd_read);
		return false;
	}

	unsigned int block_len = base64string_max_line_len / 4 * 3;
	unsigned char* pBinaryData = new unsigned char[block_len];
	char* pBase64String = new char[base64string_max_line_len+1];
	unsigned int outLen = 0;

	unsigned int read_len = 0;
	while ((read_len = fread(pBinaryData,1, block_len, fd_read)) > 0)
	{
		Encoder(pBinaryData, read_len, pBase64String, outLen);
		fwrite(pBase64String, 1, outLen-1, fd_write);
		if (limited_block && read_len == block_len)
			fwrite("\r\n", 1, 2, fd_write);
	}

	fclose(fd_read);
	fclose(fd_write);

	delete []pBinaryData;
	delete []pBase64String;

	return true;
}

bool CBase64::DecoderFile(const char* Base64FileName, const char* binaryFileName, bool limited_block/* = false*/)
{
	FILE* fd_read = fopen(Base64FileName, "r");
	if (NULL == fd_read)
	{
		return false;
	}

	FILE* fd_write = fopen(binaryFileName, "wb");
	if (NULL == fd_write)
	{
		fclose(fd_read);
		return false;
	}

	unsigned int block_len = base64string_max_line_len / 4 * 3;
	unsigned char* pBinaryData = new unsigned char[block_len];
	char* pBase64String = new char[base64string_max_line_len + 1];
	unsigned int outLen = 0;

	unsigned int read_len = 0;
	while ((read_len = fread(pBase64String, 1, base64string_max_line_len, fd_read)) > 0)
	{
		pBase64String[read_len] = '\0';
		Decoder(pBase64String, pBinaryData,outLen);
		fwrite(pBinaryData, 1, outLen, fd_write);
		if (limited_block)
			read_len = fread(pBase64String, 1, 2, fd_read);
	}

	fclose(fd_read);
	fclose(fd_write);

	delete []pBinaryData;
	delete []pBase64String;

	return true;
}
