#ifndef _CHTTP_SERVER_MGR_
#define _CHTTP_SERVER_MGR_

#include <string>
#include "HttpServer.h"

class CHttpServerMgr:public HttpEvent{
public:
    ~CHttpServerMgr();
    static CHttpServerMgr* GetInstance();
    int Start();
	int HttpsStart();
    int WebRun(int port);
	int HttpsWebRun(int port);
    void Stop();
private:
    CHttpServerMgr();
    std::string PostOnMessage(std::string struri, std::string strmsg);

    std::string PostGetData(std::string strpostdata);
    std::string PostDeleteData(std::string strpostdata);
    std::string PostAddData(std::string strpostdata);
    std::string PostLogin(std::string strpostdata);
    std::string PostBatchAdd(std::string strpostdata);
    std::string GetDataByHid(std::string strhid);
    std::string GetDataByDevid(std::string strdevid);
    std::string GetDataByType(std::string strtype, int pageno);

    std::string GetToken(std::string &strname, std::string &strpwd);
    int CheckToken(std::string &strtoken, std::string & strerr);
    int GetCurrentStamp();
    void RefreshWebtmsp();    

private:    
    static CHttpServerMgr * _httpservermgr;
    std::string _strip;
    unsigned short _port;
    std::string _web_token;
    int _token_tmsp; //token????
    int _web_timeout;
    bool _stop;
    HttpServer _httpserver;
};
#endif /*_CHTTP_SERVER_MGR_*/