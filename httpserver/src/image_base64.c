#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// bindata待编码数据buffer  base64 编码后数据buffer  binlength 待编码数据大小
char *base64_encode( const unsigned char * bindata, char * base64, int binlength);
// base64编码字符串 bindata 解码后buffer
int base64_decode( const char * base64, unsigned char * bindata);

int main()
{	
	FILE *fp = NULL;
	unsigned int imageSize;        //图片字节数
	char *imageBin;               
    char *imageBase64;
    char *imageOutput;
	size_t result;
	char *ret; 
	unsigned int base64StrLength;

	fp = fopen("test.jpg","rb");   //待编码图片
	if (NULL == fp)
    {
	    printf("file open file failed\n");
		return -1;
    }
	//获取图片大小
	fseek(fp, 0L, SEEK_END);
	imageSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
 
	//分配内存存储整个图片
	imageBin = (char *)malloc(sizeof(char)*imageSize);
	if (NULL == imageBin)
    {
	    printf("malloc failed\n");
	 	return -1;
    }
 
	//读取图片
	result = fread(imageBin, 1, imageSize, fp);
	 if (result != imageSize)  
    {  
        printf("file read failed\n");  
   		return -1;
    }  
    fclose(fp);
    
    //分配编码后图片所在buffer
	imageBase64 = (char *)malloc(sizeof(char)*imageSize*2);//因为编码一版会比源数据大1/3的样子，这里直接申请源文件一倍的空间
	if (NULL == imageBase64)
    {
	    printf("malloc failed\n");
	   	return -1;
    }

    //base64编码
    base64_encode(imageBin, imageBase64, imageSize);
    base64StrLength = strlen(imageBase64);
    printf("base64 str length:%d\n", base64StrLength);
    printf("%s\n", imageBase64);
 
    //分配存储解码数据buffer
	imageOutput = (char *)malloc(sizeof(char)*imageSize);//解码后应该和源图片大小一致
	if (NULL == imageBase64)
    {
	    printf("malloc failed");
	   	return -1;
    }
    base64_decode(imageBase64, imageOutput);

	fp = fopen("output.jpg","wb");   
	if (NULL == fp)
    {
	    printf("file open file");
		return -1;
    }
    fwrite(imageOutput, 1, imageSize, fp);
    fclose(fp);

	free(imageBin);
	free(imageBase64);
    free(imageOutput);

	return 0;
}

const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
char *base64_encode( const unsigned char * bindata, char * base64, int binlength )
{
    int i, j;
    unsigned char current;
 
    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;
        base64[j++] = base64char[(int)current];
 
        current = ( (unsigned char)(bindata[i] << 4 ) ) & ( (unsigned char)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+1] >> 4) ) & ( (unsigned char) 0x0F );
        base64[j++] = base64char[(int)current];
 
        current = ( (unsigned char)(bindata[i+1] << 2) ) & ( (unsigned char)0x3C ) ;
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+2] >> 6) ) & ( (unsigned char) 0x03 );
        base64[j++] = base64char[(int)current];
 
        current = ( (unsigned char)bindata[i+2] ) & ( (unsigned char)0x3F ) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = '\0';
    return 0;
}
 
int base64_decode( const char * base64, unsigned char * bindata )
{
    int i, j;
    unsigned char k;
    unsigned char temp[4];
    for ( i = 0, j = 0; base64[i] != '\0' ; i += 4 )
    {
        memset( temp, 0xFF, sizeof(temp) );
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i] )
                temp[0]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+1] )
                temp[1]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+2] )
                temp[2]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+3] )
                temp[3]= k;
        }
 
        bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2))&0xFC)) |
                ((unsigned char)((unsigned char)(temp[1]>>4)&0x03));
        if ( base64[i+2] == '=' )
            break;
 
        bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4))&0xF0)) |
                ((unsigned char)((unsigned char)(temp[2]>>2)&0x0F));
        if ( base64[i+3] == '=' )
            break;
 
        bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6))&0xF0)) |
                ((unsigned char)(temp[3]&0x3F));
    }
    return j;
}