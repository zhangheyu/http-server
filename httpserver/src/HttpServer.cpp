#include "HttpServer.h"
#include "./gsoap/soapH.h"
#include <sys/stat.h> 
#include <string.h>
#include <stdio.h>
#include "./gsoap/thread_setup.hpp"


static const struct content_types{
	const char	*extension;
	int		ext_len;
	const char	*mime_type;
} builtin_mime_types[] = {
	{"html",	4,	"text/html; charset=utf-8"			},
	{"htm",		3,	"text/html; charset=utf-8"			},
	{"txt",		3,	"text/plain; charset=utf-8"			},
	{"css",		3,	"text/css"			},
	{"ico",		3,	"image/x-icon"			},
	{"gif",		3,	"image/gif"			},
	{"jpg",		3,	"image/jpeg"			},
	{"jpeg",	4,	"image/jpeg"			},
	{"png",		3,	"image/png"			},
	{"svg",		3,	"image/svg+xml"			},
	{"torrent",	7,	"application/x-bittorrent"	},
	{"wav",		3,	"audio/x-wav"			},
	{"mp3",		3,	"audio/x-mp3"			},
	{"mid",		3,	"audio/mid"			},
	{"m3u",		3,	"audio/x-mpegurl"		},
	{"ram",		3,	"audio/x-pn-realaudio"		},
	{"ra",		2,	"audio/x-pn-realaudio"		},
	{"doc",		3,	"application/msword",		},
	{"exe",		3,	"application/octet-stream"	},
	{"zip",		3,	"application/x-zip-compressed"	},
	{"xls",		3,	"application/excel"		},
	{"tgz",		3,	"application/x-tar-gz"		},
	{"tar.gz",	6,	"application/x-tar-gz"		},
	{"tar",		3,	"application/x-tar"		},
	{"gz",		2,	"application/x-gunzip"		},
	{"arj",		3,	"application/x-arj-compressed"	},
	{"rar",		3,	"application/x-arj-compressed"	},
	{"rtf",		3,	"application/rtf"		},
	{"pdf",		3,	"application/pdf"		},
	{"swf",		3,	"application/x-shockwave-flash"	},
	{"mpg",		3,	"video/mpeg"			},
	{"mpeg",	4,	"video/mpeg"			},
	{"asf",		3,	"video/x-ms-asf"		},
	{"avi",		3,	"video/x-msvideo"		},
	{"bmp",		3,	"image/bmp"			},
	{"js",		2,	"application/x-javascript;charset=UTF-8"			},	
	{NULL,		0,	NULL				}
};

HttpServer::HttpServer()
{
	_httpevent = NULL;
    this-> fget = HttpGetMethod;
	_is_https = false;
}

HttpServer::~HttpServer()
{
 if (_is_https)
	 CRYPTO_thread_cleanup();

}

int HttpServer::Init() {

    static struct http_post_handlers post_handler[] = 
    {
        {"POST", HttpPostMethod},
        { NULL, NULL }
    };

    soap_register_plugin_arg(this, http_post, post_handler);
	return 0;
}

int HttpServer::HttpsInit(){

printf("%s %d \n", __FUNCTION__, __LINE__);
	if (CRYPTO_thread_setup())
	{ 
		fprintf(stderr, "Cannot setup thread mutex for OpenSSL\n");
		return -1;
	}
	soap_ssl_init();
	Init();
	int err = soap_ssl_server_context(this, SOAP_SSL_DEFAULT, "./pem/httpserver.pem", NULL, "./pem/httpservercrt.pem", NULL, NULL, NULL, NULL);
	if (SOAP_OK != err)
	{
		fprintf(stderr, "soap_ssl_server_context failed.errorcode:%d.\r\n", err);
		return -1;
	}
	_is_https = true;
	return 0;
}

void HttpServer::SetHttpEvent(HttpEvent * phttpevent)
{
    _httpevent = phttpevent;
    return;
}

int HttpServer::PostOnMessage(const char * url, const char* body)
{
    if ((!url) || (!body))
        return -1;
    if (!_httpevent)
        return 0;

    std::string strurl = std::string(url);
    std::string strbody = std::string(body);
    std::string strdata = _httpevent->PostOnMessage(url, body);
    if (strdata.empty())
        return -1;

    const char* pdata = strdata.c_str();
    http_content = "application/json;charset=UTF-8";
    soap_response(this, SOAP_FILE);
    soap_send(this, pdata);
    soap_end_send(this);
    return 0;
}

int HttpServer::HttpGetMethod(struct soap* soap)
{
    HttpServer* pServer = (HttpServer*)soap;
    if (!pServer->path)
        return 404;

    char filename[1024];
    memset(filename, 0, 1024);
    const char *path_root = "./web";
    std::string strpath(pServer->path);
    if (strpath.compare(std::string("/")) != 0)
        snprintf(filename, 1023, "%s%s", path_root, strpath.c_str());
    else
        snprintf(filename, 1023, "%s%s", path_root, "/index.html");
        
        
    FILE *fp = ::fopen(filename, "r");
    if (fp == NULL) 
    {
        printf("%s not find\r\n", filename);
        return 404;
    }
    
    soap_response(pServer, SOAP_FILE);
    pServer->http_content = GetContentByFilename(filename);
    int ret = SOAP_OK;
    size_t count = 0;
    while (0 < (count = ::fread(pServer->tmpbuf,1, sizeof(pServer->tmpbuf), fp)))
    {
        if ((ret = soap_send_raw(pServer, pServer->tmpbuf, count)) != SOAP_OK)
            break;
    }
    ret = soap_end_send(pServer);
    ::fclose(fp);

    return ret;
}

int HttpServer::HttpPostMethod(struct soap* soap)
{
    HttpServer* pServer = (HttpServer*)soap;

    if (!pServer->path)
        return 404;

    size_t bodylen = 0;
    char* body = NULL;
    int r = soap_http_body(pServer, &body, &bodylen);
    if (r != SOAP_OK)
        return r;
    //test
    // printf("rcve size:%u\n-----", bodylen);
    //static int cnt = 0;
    //cnt++;
   // printf("cnt:%d---\n", cnt);
   // return SOAP_OK;

    if ((r = pServer->PostOnMessage(pServer->path, body)))
        return 400;

    return SOAP_OK;
}

const char * HttpServer::GetContentByFilename(char* filename)
{
    if (!filename)
        return NULL;
    char *ext = strrchr(filename,'.');//获取后缀名
    int count = sizeof(builtin_mime_types)/sizeof(content_types);
    for (int i = 0; i < count - 1; i++)
    {
        if (strcmp(ext + 1, builtin_mime_types[i].extension) == 0)
        {
            return builtin_mime_types[i].mime_type;
        }
    }
    return NULL;
}
