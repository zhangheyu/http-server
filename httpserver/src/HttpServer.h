#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "./gsoap/httppost.h"
#include "./gsoap/HttpServerSoapService.h"

class HttpEvent 
{
public:
    virtual std::string PostOnMessage(std::string struri, std::string strmsg) = 0;
};


class HttpServer : public HttpServerSoapService
{
public:
	HttpServer();

	virtual ~HttpServer();
	
	int Init();
	
	int HttpsInit();

	void SetHttpEvent(HttpEvent * phttpevent);

	int PostOnMessage(const char * url, const char* body);

private:
	static int HttpGetMethod(struct soap* soap);

	static int HttpPostMethod(struct soap* soap);

	static const char * GetContentByFilename(char* filename);

private:
	HttpEvent* _httpevent;
    int _is_https;
};


#endif /*_HTTP_SERVER_H_*/