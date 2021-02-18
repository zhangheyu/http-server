#include "CHttpServerMgr.h"
#include <json/json.h>
#include <json/value.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>

const char *web_get_uri = "/getstate/query";
const char *web_del_uri = "/delete";
const char *web_add_uri = "/add";
const char *web_batchadd_uri = "/batchadd";
const char *web_add_login = "/login";

const char *web_key_devid = "devid";
const char *web_key_hid = "hid";
const char *web_key_pageNo = "pageNo";
const char *web_key_type = "type";
const char *web_token = "token";

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

CHttpServerMgr *CHttpServerMgr::_httpservermgr = NULL;
CHttpServerMgr::CHttpServerMgr()
{
    _port = 8080;
    _stop = true;
    _web_timeout = 600;//s
}

CHttpServerMgr::~CHttpServerMgr()
{
}

CHttpServerMgr* CHttpServerMgr::GetInstance()
{
	if (NULL == _httpservermgr) {
		_httpservermgr = new CHttpServerMgr();
	}
	return _httpservermgr;
}

int CHttpServerMgr::Start()
{
	_httpserver.Init();
    _httpserver.SetHttpEvent(this);
    soap_set_mode(&_httpserver, SOAP_C_UTFSTRING);//SOAP_C_MBSTRING
    //if (!soap_valid_socket(_httpserver.master) && !soap_valid_socket(_httpserver.bind(_strip.c_str(), _port, 100)))
    //{
    //    LogError(("%s failed.errorcode:%d.\r\n", __FUNCTIONINFO__, _httpserver.errnum));        
    //    return -1;
    //}
    printf("%s %d\n", __FUNCTION__, __LINE__);
    return 0;
}

int CHttpServerMgr::HttpsStart() {
	_httpserver.HttpsInit();
    _httpserver.SetHttpEvent(this);
    soap_set_mode(&_httpserver, SOAP_C_UTFSTRING);//SOAP_C_MBSTRING
	return 0;
}

void CHttpServerMgr::Stop()
{
    _stop = true;
    soap_done(&_httpserver);
    _httpserver.destroy();
    return;
}

int CHttpServerMgr::WebRun(int port)
{
  printf("HTTPWebRun RUN:%d\n", _httpserver.master);	
	if (!soap_valid_socket(_httpserver.master) && !soap_valid_socket(_httpserver.bind(NULL, port, 100))) {
    printf("HTTP RUN ERROR\n");	
		return _httpserver.error;
  }
  printf("%s %d\n", __FUNCTION__, __LINE__);
  printf("HTTP RUN:%d\n", _httpserver.master);	
	HttpServer service(_httpserver);
	for (;;)
	{
    if (!soap_valid_socket(service.accept()))
		{
      if (service.errnum == 0) // timeout?
				service.error = SOAP_OK;
      printf("%s %d\n", __FUNCTION__, __LINE__);
			break;
		}
		if (service.serve())
    {
      printf("%s %d\n", __FUNCTION__, __LINE__);
			break;
    }
		service.destroy();
	}
	printf("http server exit\n");
	return _httpserver.error;
}

int CHttpServerMgr::HttpsWebRun(int port)
{
printf("HttpsWebRun RUN:%d\n", _httpserver.master);	
	if (!soap_valid_socket(_httpserver.master)) {
      printf("HTTPS master ok\n");	
    if (!soap_valid_socket(_httpserver.bind(NULL, port, 100))) {
      printf("HTTPS bind ERROR\n");	
  	return _httpserver.error;
  }
  }
  printf("HTTPS RUN:%d\n", _httpserver.master);	
	HttpServer service(_httpserver);
  printf("%s %d\n", __FUNCTION__, __LINE__);
	for (;;)
	{
    if (!soap_valid_socket(service.accept()))
		{
      if (service.errnum == 0) // timeout?
				service.error = SOAP_OK;
      printf("%s %d\n", __FUNCTION__, __LINE__);
			break;
		}

    printf("%s %d\n", __FUNCTION__, __LINE__);
    int ret = service.ssl_accept();
    printf("%s %d ret=%d\n", __FUNCTION__, __LINE__, ret);
		 if (SOAP_OK == ret)
    {
        printf("%s %d\n", __FUNCTION__, __LINE__);
        service.serve();
        printf("%s %d\n", __FUNCTION__, __LINE__);
    }
	}
	printf("http server exit\n");
	service.destroy();
	return _httpserver.error;
}
static bool ivs_again;
std::string CHttpServerMgr::PostOnMessage(std::string struri, std::string strmsg)
{
  ivs_again = false;
  printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
  if (struri.compare(std::string(web_get_uri)) == 0) //查询请求
  {
    return PostGetData(strmsg);
  }
  else if (struri.compare(std::string(web_del_uri)) == 0)
  {
    return PostDeleteData(strmsg);
  }
  else if (struri.compare(std::string(web_add_uri)) == 0)
  {
    return PostAddData(strmsg);
  }
  else if (struri.compare(std::string(web_add_login)) == 0)
  {
    return PostLogin(strmsg);
  }
  else if (struri.compare(std::string(web_batchadd_uri)) == 0)
  {
    return PostBatchAdd(strmsg);
  }
  else if (struri.compare(std::string("/VIID/System/Register")) == 0) // 1400
  {
    printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
    printf("rcv 1400 register, %s\n", strmsg.c_str());
    printf("1400 注册\n");
    static int stage = 1;
    std::string strdata = std::string("HTTP/1.1 200 OK\r\nAuthentication-Info: qop=\"auth\", rspauth=\"3da388c7aad8b8008b73c1375ddf6d04\", cnonce=\"0a4f113b\", nc=\"00000001\"\r\nConnection: close\r\nContent-Type: application/VIID+json\r\nDate: Wed, 24 Jun 2020 17:43:12 GMT\r\nContent-Length: 157\r\n\r\n{\"ResponseStatusObject\":{\"Id\":\"32050500001320000007\",\"LocalTime\":\"20200625014312\",\"RequestURL\":\"/VIID/System/Register\",\"StatusCode\":\"0\",\"StatusString\":\"OK\"}}");
    if ((stage++ % 2) != 0)
    {
      // 注册第一步
      printf("注册第一步\n");
      stage = 0;
      strdata = std::string("HTTP/1.1 401 Unauthorized\r\nConnection: close\r\nContent-Type: text/plain; charset=utf-8\r\nWWW-Authenticate: Digest realm=\"Tallsafe\", nonce=\"9DukP3NBqnsns8L3\", opaque=\"AF1sztyyUsfpS9cI\", algorithm=\"MD5\", qop=\"auth\"\r\nX-Content-Type-Options: nosniff\r\nDate: Wed, 24 Jun 2020 14:52:08 GMT\r\nContent-Length: 13\r\n\r\nUnauthorized");
    }
    else
    {
      //注册第二步
      printf("注册第二步\n");
      // sleep(10);
    }
    printf("注册 resp data:%s--\n", strdata.c_str());
    return strdata;
  }
  else if (struri.compare(std::string("/VIID/System/Keepalive")) == 0)
  {
    printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
    printf("rcv 1400 heartbeat, %s\n", strmsg.c_str());
    return std::string("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: application/VIID+json\r\nDate: Wed, 24 Jun 2020 20:11:21 GMT\r\n\r\nContent-Length: 139\r\n{\"ResponseStatusObject\":{\"Id\":\"32050500001320000007\",\"LocalTime\":\"20200625041121\",\"RequestURL\":\"/VIID/System/Keepalive\",\"StatusCode\":\"0\",\"StatusString\":\"OK\"}}");
  }
  else if (struri.compare(std::string("/VIID/System/UnRegister")) == 0)
  {
    printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
    printf("1400 取消注册\n");
    return std::string("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: application/VIID+json\r\nDate: Wed, 24 Jun 2020 20:11:21 GMT\r\n\r\nContent-Length: 139\r\n{\"ResponseStatusObject\":{\"Id\":\"0\",\"LocalTime\":\"20200625041121\",\"RequestURL\":\"/VIID/System/UnRegister\",\"StatusCode\":\"0\",\"StatusString\":\"OK\"}}");
  }
  else if (struri.compare(std::string("/VIID/MotorVehicles")) == 0)
  {
    printf("收到1400 识别结果数据\n");
    printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
    // printf("rcve 1400 string data:%s\n", strmsg.c_str());

    Json::Value jmsg;
    Json::Reader jreader;
    if (jreader.parse(strmsg, jmsg))
    {
      // printf("rcve 1400 json data:%s\n", jmsg.toStyledString().c_str());
      Json::Value SubImageList;
      unsigned char *big_image_data_decode = NULL;
      SubImageList = jmsg["MotorVehicleListObject"]["MotorVehicleObject"][0u]["SubImageList"];
      // 保存大图
      std::string big_image_data = SubImageList["SubImageInfoObject"][0u]["Data"].asString();
      std::string big_image_name = "1400_ivs_image/" + SubImageList["SubImageInfoObject"][0u]["ShotTime"].asString() + "_big.jpg";
      unsigned int imageSize = big_image_data.size();
      char *imageOutput = NULL;
      if (imageSize > 0)
      {
        imageOutput = (char *)malloc(sizeof(char)*imageSize);
        if (NULL == imageOutput)
        {	 
          printf("\033[31m malloc failed \033[0m \n");
          return "error";
        }
        //大小超过1.8M的图片,json会解析失败
        base64_decode(big_image_data.c_str(), (unsigned char*)imageOutput);
        printf("\033[33m 1400 ivs big image size:%d name:%s \033[0m\n", imageSize, big_image_name.c_str());
        FILE *fp = fopen(big_image_name.c_str(), "wb");   
        if (NULL == fp)
        {
          printf("\033[31m create file failed \033[0m \n");
          free(imageOutput);
          imageOutput = NULL;
          return "error";
        }
        fwrite(imageOutput, 1, imageSize, fp);
        fclose(fp);
        free(imageOutput);
        imageOutput = NULL;
      }
      else 
      {
        printf("\033[31m ivs data no big image data \033[0m \n");
        printf("ivs data:%s\n", jmsg.toStyledString().c_str());
      }
    }
    else 
    {
      // 可能图片太大了，没解析出来
      printf("\033[31m ivs data is not json format \033[0m \n");
      printf("ivs data is :%s\n", strmsg.c_str());
    }
    std::string stresp = std::string("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: application/VIID+json\r\nDate: Wed, 24 Jun 2020 11:55:36 GMT\r\nContent-Length: 214\r\n\r\n{\"ResponseStatusListObject\":{\"ResponseStatusObject\":[{\"Id\":\"320505000013200000070220200624195235527380252739\",\"LocalTime\":\"20200624195536\",\"RequestURL\":\"/VIID/MotorVehicles\",\"StatusCode\":\"0\",\"StatusString\":\"OK\"}]}}");
    printf("resp data:%s--\n", stresp.c_str());
    return stresp;
  }
  else if (struri.compare(std::string("/devicemanagement/php/plateresult.php")) == 0)
  {
    // 车牌识别结果
    printf("%s %d 车牌识别结果 struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
    Json::Value jmsg;
    Json::Reader jreader;
    Json::Value rtvalue;
    std::string rstring;
    if (jreader.parse(strmsg, jmsg))
    {
      // printf("rcve json:%s\n", jmsg.toStyledString().c_str());
      time_t t = time(0);
      char tmpBuf[64];
      strftime(tmpBuf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t)); //format date  and time.
      // printf("recv time is [%s]  type:%s\n",tmpBuf, jmsg["type"].asString().c_str());
      int is_offline = jmsg["AlarmInfoPlate"]["result"]["PlateResult"]["isoffline"].asInt();
      int plateid = jmsg["AlarmInfoPlate"]["result"]["PlateResult"]["plateid"].asInt();
      int hour = jmsg["AlarmInfoPlate"]["result"]["PlateResult"]["timeStamp"]["Timeval"]["dechour"].asInt();
      int min = jmsg["AlarmInfoPlate"]["result"]["PlateResult"]["timeStamp"]["Timeval"]["decmin"].asInt();
      int sec = jmsg["AlarmInfoPlate"]["result"]["PlateResult"]["timeStamp"]["Timeval"]["decsec"].asInt();
      long long time_stamp = jmsg["AlarmInfoPlate"]["result"]["PlateResult"]["timeStamp"]["Timeval"]["sec"].asUInt();
      std::string plate = jmsg["AlarmInfoPlate"]["result"]["PlateResult"]["license"].asString();
      std::string status = is_offline == 1 ? "离线数据" : "在线数据";
      printf("\033[31m [%d:%02d:%02d]收到%s plateid=%d palte=%s sec=%lld \033[0m \n", hour, min, sec, status.c_str(), plateid, plate.c_str(), time_stamp);

      // 识别结果
      rtvalue["Response_AlarmInfoPlate"]["ContinuePushOffline"]["plateid"] = plateid;
      rtvalue["Response_AlarmInfoPlate"]["ContinuePushOffline"]["continue"] = 1;

      // 车牌识别结果响应白名单操作指令
      // rtvalue["Response_AlarmInfoPlate"]["white_list_operate"]["operate_type"] = 0;
      // Json::Value plate_info;
      // plate_info["plate"] = "川A12345";
      // plate_info["enable"] = 1;
      // plate_info["need_alarm"] = 0;
      // plate_info["enable_time"] = "2020-12-11 11:11:11";
      // plate_info["overdue_time"] = "2020-12-21 11:11:11";
      // rtvalue["Response_AlarmInfoPlate"]["white_list_operate"]["white_list_data"].append(plate_info);

      //车牌识别结果响应发送串口数据指令
      // Json::Value serialData;
      // serialData["serialChannel"] = 0;
      // serialData["data"] = "MTIz";
      // serialData["dataLen"] = 3;
      // rtvalue["Response_AlarmInfoPlate"]["serialData"].append(serialData);
      
      rstring = rtvalue.toStyledString();
      // printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
      // printf("resp to dev: %s\n", rstring.c_str());
      return rstring;
    }
  }
  else if ("/devicemanagement/php/receivedeviceinfo.php" == struri)
  {
    // 二次识别
    Json::Value rtvalue;
    printf("IVS Again \n");
    if (!ivs_again)
    {
      printf("only one .....................\n");
      ivs_again = true;
      rtvalue["Response_AlarmInfoPlate"]["ivs_recognition_again"] = "http_ivs_recognition_again";
      rtvalue["Response_AlarmInfoPlate"]["body"]["image_data"] = "image data";
          // "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 + S8aU + TNPsQ5ODtAdjxJiWGKH0 + RS + 43kovhkVhLFHsZvAkdKiJqwOf0EyJdPFnYo0iGn4A9ZtXSIlzsJ3HbuyJz9OL33OrAyNy + mJpjiscfcnAvo1PllrdtsId72KxN712FYD7hYrCwCwsViAdhYgKHYWICKGwsQAFgAAAWAAFhYCIp2KxAANvsGoVsd + wAjyviU8qyUpNL2PUrfk874optx0KwPOrVu7b9z1vhiawSt7nl3OLalE9D4XL / HPU637kHoJkz2Ta7DuNfchNxcX9SAIu4W + Sk00ZxpQ + 5DTil9yAq3ewd7ZNJ / kh0qrUVBJpu1uG734Icow5YvVhy5IC73ptg6XBKzY5OotWV / O4BY7EAUAKmFMaD + wv3CmFMILC2FMNIBb8hYaWKn4KHYci38BT8AFPyG / kKCgCwsKCgCwsKCgCwChWAwCw2AL9x2LYNgDdieOL5SGKxgiXT45c7Gcukj2ZvdjVDBxvpH2ZD6bIjv28hZLB5rxTj2JqS5iz1LXglqL / ABJivN7g3aO + WGEuxjLpVTomDljKVbG2B6nu3Zhfp5NLKUtGRPsMV6MeKYou3uZqeqNo2xKOm2RQ3SIVPyXJqS2FFvwQd88iim2jjxuWbPfKQZsjnkcU9kX06UIak93yddYdLku3Am / BDnHsL1IjUacIVmfrRQevEC7GZevEPmIgaCI + YiHzMF2AsNzN9VHsiX1fhBWwHP8ANewfNewHQFM5vmmuwfNvwB00wpnN82 / AfOP9QOmmFM5vm3 + ovm3 + oHVQqZzfNv8AUPm5eCDpoKOb5t / qL5t / qB1cdh9uDjfVy7RF85L9QronCT4dHPLpMre + VB83L9SX1EmBjl6LI7bmmQsOWKqKZu88herJ96Ax05VzZEnkXdnR6j7uxOa8DEY3OuWDeT3NVKvxGslfiMGLlkik7aQPJlXdmzyRu5Rv2FrXDgtPkiuaWWcvubRGv2bOu473BPwCnBKvTSZcHLCbv6U7N1lzLbUX6kI8Y1 / I / VX62MGfrZv2H62b9ivVX6IPVX6IYan18 / kPXz + S / WX6IXrL9EMNT8xm / Yfr5v2B5U3egaypfgiBfMZlu2HzWUbzWq0IXqr9EAfNZSl1eWifV / 2IPVXGkB / OZA + dl5J1xf4oVx / VFF / OyD52RFx / VBcf1Av51 + A + efgi4 / qguP6hF / OvwP51 + DL6e0RbeANvnX4H88v1MNvAaY + AN / no / qNddF9jm0xDRFgdXzsfAfOx8HLoQaEB2fNwH81j8nF6cfIenHyB3Lqcb7lLPif5HAoxiVFRW9AegtMuGFrwcsc6gqQ / mCwdFhdHP8wHr2B0BaSexz + qP1SI5usgk9XchJyxXXBr1MVOFph0buEoslaR02Wp6Hwzvr6edjy8r059kd + HLrhTM1Y0iq7lrUuWZLmipP3MqedPD0yl3kysbaxc7sx6z1fSSm1VhGatJc0dEsaW0qsTb8iS99hLdsMn / IWhNrgT2dFD2AlWwp1swHsBN9nyO9wDgNXsLUlty + wNpLjcAthbAQDtgKwAYhWAAACAYCAAAQAOwsQAFhYgsB2K0xAAwEAD / sX9hX / A3SV9gEl43Z14OkU4a8stOJdmPBij02P5nqItr8UjslDF1nSrLkei + EgjjePpc9xxTakuL7nNPHLDNxyL + z0cPw7FDLF5Mip / akHUyhPNLpZx3 / FgeU1pdvhhsuODTJjlim4TXHBm9ygsVgBFG4bgAAnsG4AVC3CwAAthSAACkIdhYCoKHYAAtgsAGg3E2NUACsboQDsORAFMAAA2DYAAewrC99 + AbS / gBqgtE3tb2HdvbjyEO0Fi3T34G75W6CnYWJe4XuQOTrGxdK9 / 5Ccn6bXYjpvqTCwdZierXF7B003CSvg2li1Y2r2OVJxe / Zma1j1nKDquRST8E4JRlBbbmra8GRl11 + lzdM4MOd + tb77Hd1G8H4PJnanSdUaK9Vu3tyDTinJo4sXVNKpqq7nt4smCPSRlOnqKw4FajuuRxUracH / B6GaOPL1GOko49N2PqJQ9OGXFUt6bCvO0zf4Pb / 0Chku1jel8s7uqyRjlxRxpVPaR1SSxxePb0338BHjd3GMW2JJ3pju / B6fTQw4Yzc5JtvYUMGOE56KeSrQHnPHljs8TjfAoqTuP5LsduGeeWWD6hUk9jbG8MuqzSSWqJR5dtOns12FY8rvNN + WSgGAm0F2AAHAAACAAAQAMBAAAAAAgAAAAQDAA5AFy / wD0b9HhWTLql / 448nO3Wz57He4Th0WnGr1cvwB2YsmPL0spTjcYukLq8Cz9Io4Px3pGGBJfC3HjfcfU5JYughLFLS758hGfRdLnydQp5FKKhwmeg1hl1NNfX5POwdbnz9Rhi3p3 / wCTqTX / AFP3XKA5usrqo5NK / wAsHSXsedSW3 / J34o5P + p5MmONx4Zy9TjWLPKKd27ZRkIYiKAAAAQwAQAJ7AAyXKK5ZDnvtwBqGxlrFqvuBrt5Fa8mTfuQ37gdOqIaonNq9w1 + 4HTaYHMsldxrKwOirAyjlXdmmpVdlDAVoYAAUAAAAA + f / AOD0p + 7 / AFJ73fBt0sow6qMpq492EdeD4e / S9XqNofqZ / Kxzzaxy9PF2vuz0p5oZMy + pPDRnjwYpT9RZFoi7UAjy8vSZcU9Di5ePcUsOXGt4OK8M7c2bK + rU + EuEadR / mxSllmotce4HlN + QbpbIE0rXJLt7cBpGeVY6L6VVA5 + od5FFHXiWmKRl0ka71SObIqyJM6E9 + THqFxJGWsdXTyUJ15OmTfg87FO5xk + Dulkj + wZqZK4Nex5GZaZM9WEriv4OHq4fW0SVr1HJtzd + UaY8srtzelcIyW2y78i0 / VSZtxehj6p5quTTWyRvqmvslt4PLpp0nTNcXUuOzvSB26pJ6tVt / wDorXkktLyPfk5pdRCMNV34OSfVZZW + LKPSbk5K57IbyTc9UZtNHlrNKKtydnRg6iUnpdUB3epOTueRvwQpOMpNSdvknsFppbcEDuwFYAPkOfYluu5LzVtpsDRJr3C34Mtc3wgvL7FGu / gVmd5fYLy + wGlhZneb2C83sBpaC0Y / 5 / YP8 / sBryBmvW9h / wCb2AsN / BH + b2C8vsBe / ge5neX2D / L7AaUwpmf + Xyg / y + UBcqWz58muLNkh9sm14ZgvVvsL / KnuB6K + JSePRLEkvYvJ1nTdRhWKX00eUvWe0ar3G1PwrA9DHl6XFkjJSbceDSfxGOt5McE5I8prLxSsNOW1TXuB2Zety5k9C9Nvmjmty3u / dmbWZu00kL / K / CA0phTM / wDL5Qf5fKA0phTM / wDL5Qv8vlAaAZ1m9grP7AW9kYzlTKcc1b1RjNTT3oCtSfIr8cEaZPugqVckFWxahU / IqfdhTchNi0ryPSvIC1ewavYen3DT7gK77AmPSKgHZpDJ2ZkGoI7Izi0PdnJGdGj6jTEo6K9xUzilnk3sVDqZR5CuxMGzGOaLNFOLCHymOLuNMSafAPbhAUpy06VJpDUpV9M2iKXILd7FRblK95uwc5v6XJtPyRYJd7CnF6RPdOVjSVnP1M3FaUyCcP8Al6hvweglTOToYaU5vudbZl18ijPPtCjSzHqHtRmumFil9q9zuSi62R5uPk9DBGToRz9TEYZao7diOqja1dzXHGMLVjlFTjQkW15E0kyZUlsa9Tj0zZhJmnKnbBMlsVlRTdIPBPJapIKvHhWR7s7MeGMFaRwxbTtM2j1Ljs + AOxtsI7GMM0Zdy9XgIu2Fk6gtgLI7pFpST + 3YlSqSbVpHq4 / iXw5Y1HJSYHm / V + jYVL9WeqviPwzUqlEp / EPhn7RKPIqX6sKl + rPW / wCofDP2iNdf8Mlw4geRT / Viv / az2Pnvhn + 0Xz3wvzEDyNT / AFYJs9OfxD4ZWyRkut + Ht8IDhYUz1MXV / DGnbijRdR8Lf5RA8en5Dc9r1fhj / KALJ8M / aBNHjWLk9rX8Nv7ojcvhveUBo8T + w / s9rV8M / aAX8M / aI1Xi7PuNUu57NfDfMQr4b5iNHi7Xd7hZ7f8A8b5iH / x3mI1HiXvYXE9v / wCN8xJb + GeYjR4tquRbPuey5fC13iRLP8Ni6UVL3KPJ28ha8noy6r4euMaM31nQrjEiji / sK9zrfW9H / wDkiX1nS / 8A5IDm4E5I6PnemuljQPqOml + CIOVzMMrT7M7nm6b9UiHl6Z + APP8A6Ybnep9N7D9Tpb7EV51SDS + 6Z6kc3RrmjSPUdCnukwPI0 + zDT7M9n5r4f + qH818P / VAeNp8xZLT7RZ7cur6CvtRL6roEuEB4yUl + LD6v1Z7HzPRSW0UC6noV2QHkxi2vqVEt70dvXZcc69NJHntbgi7QfS + SBWRTcSS07RMlQQR2ZpGRlY4so2U5LuaRzSW1mFagqgOv1U + StSfBx6milkaA6g9jGOWzSEkwim0lucU36mVJ7qzfqJ6I13H0OBzbnLgjcjpxRUYqKWyKfJaouo + A6bGTMssZSWyOrSu0S1gm1aRMOnFhwy3bOzp9S / ghwlB02bYm0gxbrs9Xo8j2SROTBgn9k0jmn0uP8JELpJv7clf2aq / VcnXYXGTS3S7nmzTPdzdNKOHeVs8TNHTNpkZuRkub7DFdbdhhndVFCkpJvwOLBvYoj + ytW1PgjuPsBSddzSOeUeNzEAOtdSq35LXUR8nCUgO + OaMtl / ZydXjgp354IUmuC / 8AyYpXyuCxGWOMZPSo7l1HdaeCMU9Ek3ybZFp3Xcmqyen9Qg4xv6RPZ2 + A9 + w0xTcX2JaXgAGh1HshbeBIW9jRpGDlwivTknVbjxt06Kjffk15lrNsidEkCjJb9i6fLYKEnfg3xWeocdKW7LTx / kzNwqreyE46ZKt0yXzYs9a3TwXvIpS6f9mcrScrVAk3tSJlXXTeBczYN4FzNnNovah6Gnuti81Oo6H6F16jD / C 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                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        bGbHVi6p482Wdfc2kdXw6SeHqNTp1Z5jmuECyTi6Uqvk6dRnl2fDepXT51fEnud2b4bj6jI82DJFauUzxW +
          // y / 5KjnyQ + yb / wCSWz + HL2Ornh6Hongxu5vmjyI7rTFpPlsvBGXU5nGUrklbfsa + liyRaxrja / cvn3PP7S + GOOXpZlkv7Xue1m6fF8SwQyRmlJLY8tYcUFCGR / XIcITw65qTjjg9yevcv6WeHpdP02L4bilkyzTm1seQ82rqnlb3TuzaSfUVlnkvHwgl00KcIO5RV0Seoc16WTFj + JdNFqSU48C6TpMfRXlzzWtcHhrPmx3olTR15nPL0iyzyW + 42f6nNZ9Vm9fqHk7J7HqRWP4j0WhSSyLg8vp8UMvTuergvHCUIKXTyqV7lvqYs816HRfDfl5LJnyL6dzg + IdR811T0fbB7FZpynjlGfUbxXF8kLpmo45LZS + 5 + CT1 / aXy9HpJY + r6R4JyWpcEdP8ACJ4s2vNNPHF2jy5ZPRzP0p7ruE + t6qcdLy2vA3 / KnLr + K9THqcyxY / sj3Or4bmx5sMumm6vZWeJqpNrkccrTUovTJGrZmHL118Hm81SkljTsXxnPB449Pie0VvR58 / iPVSjp1mDnLu7kzO61IyycpkNrVaKyce5kk7OdbjS7HHnbciOLJJ7cHbg6XT9T5IpdPh1O2PqsulaIl580cUKjyefPJKbbYC3lOv8AkrI9NRQ4LRHU + 5ndttkA + aAO4AOxp0RYIitllofq2RCKZr6SS2KJ9QXqFemhemggWRMG0xOHgnRJA / RvYh8lJPuGkNbqUjfp8EpNd0KELo9fpOiyaFojafcZpImPTwjBW9yliguDrXw + e2qdGnyGOP39QkXF6scCxY1yWtEeEdno9Hj + / PFh6 / w + H5J0U21y3LtGy4Ys2TiDRcvi3SY19GO2c + T47J7Y4aSamV2Y + gyOLeSWle4ZMvSdFB01KR4 / U / FOqyutdI43JydzbbJa1PLs6rql1OS0qRCajyYw5Vo0acuxm0sxEeWD5HHlg + TLsm2IYhpg0xfKCkuAAaZC / glpeC6HpVF1i + WGy7FqaCUCNLXYsrnfFaJryJ5VH3M377C0p9zWs8qlmb4I1ysWl2DTRdZxcZ3yDk / JnG7YMiwMTBg + AHB0zRLWnXJiNTcXsBX1J1wVDK8T2dlKcZqmTLEq23Kjox9VF / cqN1nxP8zzWmuUa4nH9QOzJLFki2pbxOJ5VqaNZJNdkZSVuqRMULLFB6qu + 5Lxtb0LQVMaeqL1Elx / ZGkel1SWwxXR03VrFkck9mqZv87ixwahzykee4NRpRD06V6SZP6PQXWYMijPJtOIl8Qg4zxzVwmzg0 / 7bE4700TJ / F16PzfT40sUHeO7 / g1yfEOnTk8TuUo1Z5WhJbIHFJ / bQkNayyJXqdNndi6zpn0 / pzlv4PMpvlAoLxYsNehi6rDibh + DKXXYseRrFvF8nmOKYenJ7RVDDXflydM7af1P / wBG8fiGKOOENVpfceVWr8d1yDjfEeeQN8 + TE8jljezJ9SP8GaS + 1ITg09zUv8Rr6kezF6kW7Zk478BpFo1eWPNgssbtcmWnygS32RJaY6un0zk9R0VhX06V / Jw4ou3vRbpK3IqOv1cONcmGTrNvo / 5OWbjJ7BFPwFVN6nd2xwil9Uv + BxgofVdik9QCk9T9uyIaNOwqIIV3uBbWxNASNBQNEGkGaXSOZNopTYVvqDUY62PWwNW0iXP2IXuO / CGrh7yGkwhe9oozrU8rjt / RvD4hngqhkcUc6Ft4Erpy6H1vUz5ysiWbLL7ptmVsLfcunIk2 + W2LSqApIdNTzCikkNPYHsS3RNXmB8g0rQdyu5EsXDd7nZini003ucmP79 + DqjjhykakcvbmjyDj3sE0nu9wlujLrsQ0IpcUKvYjUwnwA / p / sAWFaGrCh0ACWzGBZUxMoRlyZ + lH3NRF1m + Yy0SXHAnF9zYKTGs3xHPx2EzdwQtCLrPDmcWFHRoRDi / BdTis6E0aNPwKn3Gs81mtio5JIqhUX6TFrJL8kh64vlUZptcsq0 + 4Ft47XJV43wZfSu9jtBMa / S1sw0x8oy28itFG + mPlBoj5VGNJhVcMDZR08NDpq6aowt + R2 / IGuj3Goqt6MbfkLfkGNdMVwtx0n2MlKS4YapeQL0b2NR2e25lrl5Gpy8kFqLS3Q1FN2 + CNcvItcvJTGjW / CoepNrZbcGDlLyLVLyBq4Ju7B4lfJlqYamBrpS2tBp90ZN29wtDUa6V3oVJMzTHbCq0rfcnTFbNsLYWNXFLTHhA5N9ibBvYmmDcBag1E0w6YUK2FyGrzVULYT1PYFFjV4o2EWoD0DVnhmkh0uyNNA1EmrwzUWylikzSMUaJImrwyWHyWscUUIa1JBpRLiUhNGWomqHsGkAv7BMihNWFxKLRNFIJhS5Ie72NH7kOlwUNcopL6rIVtlq0mWRm1phTc9jug2lpkji6Lebvk7ZqVWiuNuvN + YjJ1LHXuU1GStZf6OeS + pkvZ + DWJrpUZr7VaBua5iY49erabrwduFS0 / Wr / kY13jm1R / saZ1vHH9UQ8GOXL0k5WfkYoZT6aK4m2T6ORfbuTlqewAtOdcwQapR3nHYnLXYEP1cfex64PgYdIoY9SfgNvYY1sSwK / kWxmqkW5QqYMTXsGnyVuDt8hMTpQtKGKy6nMEIxb3RssGNrgygtzW2kVOYF08EHy8Rxb7mkSs8sflkJ9LXc6KE2NThySwSXcXpSXLOmTTIaoacMfSYekzUBq8MtEg0SNgGnDDRLsrGsc32N4p8lpjU4c / pT50kuMl + J2K6DSNThw / V + ob + Dqyxrgzqhqzwx38BT8G1BQ1eGOlhpZrQUNOGNew69jSgomr8bPSGlmlBQ1fjZ6WGk0oKGnDPSNR3LoEiavCdKDSilwC4C8RNDoase4OSSAe4bgwqYwCgp2HIJDrwECRaJVlKxiCxWPSxWkMNhN + A3BzihepEYbDpgLWhNy7Icp1IoGQ3LwKvMqLPKdm2NMmor8h6kuFZeU7F39u46b5VGmNfTxTMcrmnxsXGezuMHd2T6jk3XBi7bKg64GM267uiSbfk7XKtmzi6VUrN5SbFZjz5JJpLkeXBJR1NbF9PH1c / HB6GSCyQSrbg0y8nE5Y5KWnY7odVCaS4NpYouGhI5JdBO7jIDpUot7NDcb3OeHRzXMjqxxUI09wiFHwh0 + O5VO7CgqHGXdiUU3wm / c00icUF1m4R7xRLw432o2quAryMOq5n02N92S + l / WR1UwoYvVcb6fMvtaJeLMuaO6gomL8ljz2prlMWuX6s9H + kKk + yJjU / JXn6 / KYKaf / APp3vHB8ol4MT5Q5Wfkri1L2C17HU + mxvhEPpIjlfkZLT5Kq + GV8p4YfKzXEhh8kTpa7lRsPl8q4kS8WZdxi9xurFJ + xjWVdhasq5iyZVnuKbfgXJLyT / RiWSXeDJlXuLCifU / 2MPU / 2MZV7i6CifUX6sPUX6sZTuNI / aCVMzWVeCvWj4GU6jSyrMFlj4K9eIyp1FZWZvkcs0ZEvJEZVnqHQqF6iD1F4GVeodBRPqLwHqLwMp1DoKFr8RYtf + 1jmncNIeknU + 0WFy / VjmnyRWkWkX1 / qw0z8Mc1L + SHpCqDTMNEvIyp8kCqgXAlF + Q0vyXE + SGq8j28k6V3FUUMPkXaXDFqJ + kLiMZ + RWteA1 + xOpC1lxPkVr9g1yfBOphbLifJVap + wap + USKmMTurteSXKK8k6ZPsylik + xcTQskeyHr9hxwSfsaRwVyMZtZa34Kcn2TN1jiuxailwi4muJ632YRxTl5O7 + kG / YYa54YF3NljglwV / IwalK0TNXFqjS / YaWwHm5IaWxR5Onq401JcHNFb2RXpdHG4nToTMehV47OhGVeZ0 + ZQm3R348kZx + 7c8x7bJEOcl9smaZezpuNrcS25PKx9Rkhs5M6sfVOXuB1tXwxUzOOV90kilOwKAFuBQgGAE0FFCAVBQwAmgoYATQUUARNAUIKmgooQCChiAVPsOmDD + wCwsKXkP6Bo5FpT5Q79gsGlpj4DTHwOwsYuo9OHgPTh4L / oP6GGs / Sg / xD0ofqWAxNR6UP1D0sX6lhZMNZ + li / UPSh2iXYWXDUelD9Q9KH6l2Aw1Hpw / UPTh + pewbDDU6I9lsGheB2FsYaWhLsFew7DbyMNFA7rgX9j3GGs5QkzP0ZeTo / sNvIw1z + kw9JnQIYaw9FieBnRY7GDmWB2P0DoEMGHoD9A2Cy4MfQGsKXJruD35GDP0oj9KJdIdIYI0rwOkuw9wpgGwBTCggAKAAGIYUCGADj7ia8ClJLkwy9QuIkD6qS0V3OXGm3uO5Tf1GsI8fyRXo9MtOJGu3gWOFY0x37EV5MluZtUzeUaZhPkrJumKn2dC3GmARc4u9TLjnyx4IC2gOiPVz7o1j1Sa3OLdi3WwHpRzxfctZIvhnlXLyPVOO6YHq615DUeZHPNFrqpIo9C2NM4F1jKXVruB2tpoVHJ85BB87DwB2UhWcnzsPBourgwY33E77mSzwf5FLLBfkgLAj1Y / sh + pH9kBQC1R8hsANCoOB2AqAdiCAAAKAAChAAAAABAgAAAAAoAAAAAAAEABAAAABQCsAoOB2D3WwBYC3CwAAteRWvI1TYC1RXLFrj5Gitg2I9SPkn10ho1tA2jH5heA + ZrgmjUZzvqG / AvVk / yQ0dG / kL9zk9Z + RPK / I0x2X5Y9UV3OF5JPhk + pLyNXHc8i8kvPFHHqb7hbGmOl9Ul2J + bvhGG3garsgNX1MmS80mQFkCcpP8mJRsrSXEgUYtI6OnhqkkRFWzt6THTsDqimopDoaYAedmg4tpnLKO9nt9VgjONrk8vLicXpoqOWr3Eayhp2IaoglrYVFPgQCAAKCwsAAQAACZDLJaAgLG0JBVK2DTQ4op1QGepoepvuDJ5AtSfk0T9zKNF6bA3jNr8jWOZLlnFXuPfyB3rPEpZYs85X2Y7l5Go9HXHyPXHyebql5Frl5Gj09cfIaovuebrl5GsklywPR1LyGpeTzvVf7B6r / YD0rXkLXk875iXkPXl5A9EDz11Mkh / NSKY7xHCuqk9hvLLyDHbaC0cDyS8k + pk7PYGPQcorlk + pD9kee5yfLJsGPS9WH7IPUh + yPNTKTBj0dUKvUhOcF + R59vyKm + 4HoerC / uB5ca / JHn6X5DT7k0dzz41 + QPqcaRw + mvIaBquqXVQI + aV7GOhBoiho2fVNkvO2Z6UGlDRfryF6smKkGwB6knyGphsGwCthqY9hAJti38lBVgSk33K0vyFBYBo9w0ILCwGkoiasYBSp + R2wCggGFDogQ6Ch0AJX3KihRRrCNsCsUHJnoY4uMKow6fF3Oh6r24AuNlUSrpDKN6be / Bz9RhUlaR0gEePPHuzCWM9bqcCyK4rc4JpxlTA5XCkTR1SSbvsS0iDmoRu8dk + mBiBo4EuIEiKoKCpFRVCAVWJxK5CgJQclUFAS4kOJrQqAz0sak4l0FALUNNAADuhWCAAsAAoYbdxAA9vAbeBAAUvA6XgQAOk + waUCAgNKCl5AQD2C0IOwAJodCoqkilQqCgBrcK9woNIQLkdioKIK1C1MKAAthv3AAAAAAAAAYCABhQAAqBbDAAFQxgKgoKHQBQAMBAMdAIY6KUbKIGjRRRSj4AUYnRhhb4JxYnJnZjhpXAFQjSoqpXQJ2CApquR0IdAbsljdvgVKXDKgi1xRhn6aE973N4xu / CM5zjF + X2RB52XDLHu19JhKXsem4PLLVN0v1IydFGXGwHmqYajfN0Usau9jktW0nbRBpaYNIzV3vsO6fOwFUhUidXgLCqcUQ4lWLUBLjQqNE0wteAM9LFTNb9hUBlTDc0oHGwMgL0hpAgB0FAICqCgJArSKgEA6EAAAAAAADAQFAAAABYBQDB8CoCAAYAKwsAALAKCgGIKCigAdBRAgHQUAhjoKAQDoekCRj0hQCCrHQJAKh0UGwE0OhgAJD0oEFryAaRisLVWiig37CujTHGUuwCjuzeGNuqKx4O7OiMdKoB44qJoyKKvYBoEKKKSAAsdBTA1pu5J7d0EXa22XccnGFyb / AKMHKWZtQVR7lReTN + OHdkwx / k933vsVHHGCVF / dTezXBAtPjcT3e7G27tisDm61ZcuPRjr + zypdJ1MF9cdl3R63UQ9VVBtM4smTrul4SnH3IPP1uLqpP + UV6u + 53R + JYmv + 5wpP2Q4z + FZ3VSjJlHGnja53K0Jq4vc6n8MxT3wZor + WZS + FdXB2skWvZhWDjk8CtrlDyR6rFs8ba / glZZfnja / lAO63FrBZIt0UoKfCIEnY9QnikhU / AD1Bqsmg4AqxWTYWBdoLRAAUBKY7AYCsLAdCoLCwCgoLCwCgoLCwFQDABDoAsAoKCwsAoKCwsAodCsdgKgoLCwCh0KwsB0KgsLAdAKwsBgKwsBgKwsChWKwAdhYgAdjTonfwFedgKsQcdw1AMDPX7hrvjcqNBakianLiLHGGSTrRL / gB6 / Y2wQWebXAodBlmr1KP8nRh6SWD6pZIv + GFaY8EYcqzZxiqpUNcbAlfIDToLDgdgMBDAY0xIaQDsWq / JVB / QCWNzeqb / o2W0ajsvASaTtc + Cbv2fgqGIe / fkRAA0DJbAKBxTVUKxWQY5emwS++ Fs5cnw2MlcGkejqS / Gydrtf8AAHj5Ph + WG8Zv + mQn1mLZame037Eun3CvNj8R6uG2Skl5QS + J48y05sSfukd08OOf3xswn0mJ7JaQOLJii2pY + H2OrBi0xsWPpnjk3do34VARJatiXjRdbk0yDOWJMzlhrg3ADmeIl4jpaE0BzaCXA6aQmkBz6WhUzdxTFoRRhTGa6ELSgMgNdKE4gZgXpFpAkC9ItIEgVpCgJAqgookLKoNJBNhY9IaQCwsekNIEgVpDSBIF6Q0gSIvSPSBFgaaAUQMwNdKDSgM9I9JrpCgMtI1A1qhpAZqA4wbNEhoDmyOUHUVbHHp + oyq5RaXk3aUd6pmM + qzv6YzpIC49BN / dlUf5K + Sxx + 7PFnM5ZZ / c2wXTzl + LA6lh6aP3SUv4H63SY / txOznXRz90aw6N / lIotfEIL7Mf / omXxDK9oRS / o1h0kEawwY4b6LA41Lqs3N0 / B1YOmWPe2 / 5Z0Knx9IKNAPtQJFVsJbgAwGAhjoKAa4GJDAYAAGtJiVeNxruJdyoBD7CZAgAQATW5QiBWJgACJaTGxBUX2BodbiAlkstoloCeCWXRLQEMRbRNASyaLoVBUUKi6FQEUFFUKgiaJo0omgJoVF0KgJoKKoKAmgoqgoCKCi6FQE0Oh0OgIoaQ6HQE0FFUFASA6HQE0FFUFATQUVQUAqCiqCgEFFUFBU0FFUOgJGkOh0EKgSHQ0gFQ6HQ6AVXzuCjD9EVQUALT2igp9nQ0h0AK / JSBX7FK / YoSVlxtdxIpIB8jq + 4kigBbDQIEA0h0CGgCgoYAADQbAFoApABqu4kV3JfJUAmDYWQIQ7ABCGLkgQmDdBeoCRMuiWgqWIoQEvgTKaJoCSaLoV + 4ENCotteRcgRQqLoVARQqLFQEUKi2hUBFCo0oVARQUVQUBFBRVBQCoVFioCaCiqCgIoC6DQBAFaAqgJoKKoKAmgoqgoBUFFJBQE0FF0FATQ6KoAJoKKGBFDoqh0BFDSKoKAmhpDGAqCih0BNDodBQAhgMAUUNRQUNIoaVFISKQAhiKABoQ0gGhgkOigAYEDXBLjfcpUFICNDuyitkH9AadyXyV3E1uVEtbiosRBNAN8BW1gSS2W1SsjuBPJKTTL5YJbEUEssTQEioqhATQqKEBNE1ErI3FWiYS1K2gJaiCS7FuvAqAmhUVLYK2AzChtJDSAlomi + QoCKCi6QqAigoukJoCKCi6FQEUFF6ULSgJSCiqSB0kBNDCLUhgSFF0KgJoKKoKAVCouhdwEkOiqCgIoKLoVAKhUUNATQJFAAqHQ0NICaCitKHpQEUNIpJDaoCaHRVbAlYE0FFqKYnFWBNDopRQUgBIaQJDSKAY0goAGFAAFIQ6AaY7FQJAUAAAVZSEKgKoP7FbQJ2B //Z";
          rtvalue["state"] = 200;
          rtvalue["msg"] = "ok";
          std::string rstring = rtvalue.toStyledString();
          return rstring;
    }
  }
  else if ("/devicemanage/get_empty_frame.php" == struri)
  {
    // F2 获取空车位
    Json::Value jmsg;
    Json::Reader jreader;
    Json::Value rtvalue;
    std::string rstring;

    printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
    printf("获取空车位\n");
    std::string empty = std::string("[{\"id\":1,\"status\":1,\"remnum\":11},{\"id\":2,\"status\":1,\"remnum\":256},{\"id\":3,\"status\":1,\"remnum\":555},{\"id\":4,\"status\":1,\"remnum\":44},{\"id\":5,\"status\":1,\"remnum\":658},{\"id\":6,\"status\":1,\"remnum\":66}]");
    Json::Value  jempty;
    jreader.parse(empty,jempty);
    rtvalue  = jempty;
    return empty;
  }
  else
  {
    printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
    printf("msg:%s\n", strmsg.c_str());
    Json::Value jmsg;
    // String2Json(strmsg, jmsg);
    Json::Reader jreader;
    Json::Value rtvalue;
    std::string rstring;
    if (jreader.parse(strmsg, jmsg))
    {
      printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
      printf("rcve json:%s\n", jmsg.toStyledString().c_str());
      time_t t = time(0);
      char tmpBuf[64];
      strftime(tmpBuf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t)); //format date  and time.
      // printf("recv time is [%s]  type:%s\n",tmpBuf, jmsg["type"].asString().c_str());

      // printf("return  json:%s-----", rtvalue.toStyledString().c_str());
      // Json2String(rtvalue, rstring);
      // rstring = rtvalue.toStyledString();
      // [{"id":1,"status":1,"remnum":11},
      // {"id":2,"status":1,"remnum":256},
      // {"id":3,"status":1,"remnum":555},
      // {"id":4,"status":1,"remnum":44},
      // {"id":5,"status":1,"remnum":658},
      // {"id":6,"status":1,"remnum":66}]

      if (jmsg["type"].asString() == "http_reg")
      {
        // F2 心跳
        printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
        printf("heartbeat\n");

        // rtvalue["force_trigger"] = 1;//心跳响应强制触发指令
        // rtvalue["screen_shot"]["relative_url"] = "relative_url";
        // rtvalue["screen_shot"]["absolute_url"] = "/absolute_url";    //心跳响应获取当前图像指令

        // std::string serial_data = "test http send serial";
        // int len = serial_data.length();                      //心跳响应发送485数据指令
        // rtvalue["serial_data"]["data"] = serial_data ;
        // rtvalue["serial_data"]["data_len"] = len;

        // 设备状态控制
        // 设备状态
        // typedef enum
        // {
        //   DEVICE_FREE = 0, //空闲
        //   DEVICE_BUSY,     //占用
        //   DEVICE_RESERVE,  //预留
        //   DEVICE_ALARM,    //告警
        // }

        std::string type = jmsg["type"].asString();
        rtvalue["dev_status"]["status"] = 1;
        rtvalue["type"] = type;
        rtvalue["state"] = 200;
        rtvalue["msg"] = "ok";
        rstring = rtvalue.toStyledString();
        printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
        printf("resp to dev: %s\n", rstring.c_str());
        return rstring;
      }

      rtvalue["type"] = jmsg["type"];
      rtvalue["state"] = 200;
      rtvalue["msg"] = "ok";
      rstring = rtvalue.toStyledString();
      printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
      printf("resp to dev: %s\n", rstring.c_str());
      return rstring;
      // return std::string("{\"state\": 200,\"msg\":\"ok\"}");;
    }
  }
  printf("%s %d struri:%s\n", __FUNCTION__, __LINE__, struri.c_str());
  return "  ";
  // printf("go here??? struri=\n", struri.c_str());

  // return std::string("{\"cmd\":\"test\",\"p_version\":\"v1.0\", \"cmd_sn\":123,\"state\": 200,\"msg\":\"ok\"}");
  // return std::string("{\"state\": 200,\"msg\":\"ok\"}");
}

std::string CHttpServerMgr::PostGetData(std::string strpostdata)
{
  return "";

}

std::string CHttpServerMgr::PostDeleteData(std::string strpostdata)
{
  return "";
}

std::string CHttpServerMgr::PostAddData(std::string strpostdata)
{
  return "";
}

std::string CHttpServerMgr::PostLogin(std::string strpostdata)
{
    std::string strerr = std::string("{\"status\":401,\"message\":\"Login failure\"}");
   
    return strerr;
}

std::string CHttpServerMgr::PostBatchAdd(std::string strpostdata)
{

    /*
    ------WebKitFormBoundaryy6Rgf6fHJVqXbBn6
    Content-Disposition: form-data; name="token"

    10b70ceb08ebf98ded0d06ce81a4269a
    ------WebKitFormBoundaryy6Rgf6fHJVqXbBn6
    Content-Disposition: form-data; name="file"; filename="test.txt"
    Content-Type: text/plain

    1,1,1
    2,2,2
    3,3,3
    4,4,4
    5,5,5
    ------WebKitFormBoundaryy6Rgf6fHJVqXbBn6--

    */

     
    return std::string("{\"status\":200,\"message\":\"OK\"}");
}


std::string CHttpServerMgr::GetToken(std::string &strname, std::string &strpwd)
{
    return _web_token;
}

void CHttpServerMgr::RefreshWebtmsp()
{
    _token_tmsp = GetCurrentStamp();
    return;
} 

int CHttpServerMgr::CheckToken(std::string &strtoken, std::string & strerr)
{
  return 0;
}

int CHttpServerMgr::GetCurrentStamp()
{
  return 0;
} 

std::string CHttpServerMgr::GetDataByHid(std::string strhid)
{
    return "";
}

std::string CHttpServerMgr::GetDataByDevid(std::string strdevid)
{
  return "";
}

std::string CHttpServerMgr::GetDataByType(std::string strtype, int pageno)
{
  return "";
}

