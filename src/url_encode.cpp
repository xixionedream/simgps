#include "url_encode.h"
#include <assert.h>

unsigned char url_encode::to_hex(unsigned char x)   
{   
    return  x > 9 ? x + 55 : x + 48;   
}  
  
unsigned char url_encode::from_hex(unsigned char x)   
{   
   unsigned char y;  
    if (x >= 'A' && x <= 'Z')
	{
		y = x - 'A' + 10;  
	}
    else if (x >= 'a' && x <= 'z') 
	{
		y = x - 'a' + 10;  
	}
    else if (x >= '0' && x <= '9') 
	{
		y = x - '0';  
	}
    else 
	{
		assert(0);  
	}
    return y;  
}  
  
std::string url_encode::encode(const std::string& str)  
{  
    std::string strTemp = "";  
    size_t length = str.length();  
    for (size_t i = 0; i < length; i++)  
    {  
        if (isalnum((unsigned char)str[i]) ||   
            (str[i] == '-') ||  
            (str[i] == '_') ||   
            (str[i] == '.') ||   
            (str[i] == '~'))  
            strTemp += str[i];  
        else if (str[i] == ' ')  
            strTemp += "+";  
        else  
        {  
            //strTemp += '%';  
            strTemp += to_hex((unsigned char)str[i] >> 4);  
            strTemp += to_hex((unsigned char)str[i] % 16);  
        }  
    }  
    return strTemp;  
}  
  
std::string url_encode::decode(const std::string& str)  
{  
    std::string strTemp = "";  
    size_t length = str.length();  
    for (size_t i = 0; i < length; i++)  
    {  
        if (str[i] == '+') strTemp += ' ';  
        else if (str[i] == '%')  
        {  
            assert(i + 2 < length);  
            unsigned char high = from_hex((unsigned char)str[++i]);  
            unsigned char low = from_hex((unsigned char)str[++i]);  
            strTemp += high*16 + low;  
        }  
        else strTemp += str[i];  
    }  
    return strTemp;  
}  

int url_encode::code_convert(char *from_charset,char *to_charset,char *inbuf, size_t inlen,char *outbuf, size_t outlen)
{
         iconv_t cd;
         char **pin = &inbuf;
         char **pout = &outbuf;
  
         cd = iconv_open(to_charset,from_charset);
         if (cd==0) return -1;
         memset(outbuf,0,outlen);
         if(iconv(cd, pin, &inlen, pout, &outlen)<0) 
			 return -1;
         iconv_close(cd);
         return 0;
}

