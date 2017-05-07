#ifndef  URL_ENCODE_H
#define  URL_ENCODE_H

#include <string>
#include <iconv.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

class url_encode;

/**************************************************************************
Copyright (c) netposa
Author: rjx
Update: rjx
Last Modify Date:2016-6-21
Description:Provide basic methods or defination for server
**************************************************************************/
class url_encode
{
public:
	url_encode(){}
	~url_encode(){}
	
public:
	/******************************************************************
	*Summary: conver 'prameter' to hex
	*Parameters:
	*     x: your sources pameter. the func would convert it to a hex char.
	*Return : a hex char. 
	*******************************************************************/
	static unsigned char to_hex(unsigned char x);

	/******************************************************************
	*Summary: conver 'hex prameter' to decimal
	*Parameters:
	*     x: your sources pameter. the func would convert it to decimal system.
	*Return : a decimal char.
	*******************************************************************/
	static unsigned char from_hex(unsigned char x);

	/******************************************************************
	*Summary: the fuction would coding your string. then output a url encoding string.
	*Parameters:
	*     str: the string that you want to encode.
	*Return : coded character.
	*******************************************************************/
	static std::string encode(const std::string& str);

	/******************************************************************
	*Summary: Decoding of URL coded characters
	*Parameters:
	*     x: the string that you want to decode.
	*Return : decoded character.
	*******************************************************************/
	static std::string decode(const std::string& str);

	/******************************************************************
	*Summary: Character transcoding
	*Parameters:
	*     from_charset: you source character encoding
	*     to_charset:   your destination character encoding
	*     inbuf:   your input coding character.
	*     inlen:   the len of encoding character 
	*     outbuf:  coded character
	*     outlen:  the len of coded character
	*Return : 0:success   -1:failed
	*******************************************************************/
	static int code_convert(char *from_charset,char *to_charset,char *inbuf, size_t inlen,char *outbuf, size_t outlen);
};


#endif // URL_ENCODE_H 