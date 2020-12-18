#include "HttpServerSoapService.h"

HttpServerSoapService::HttpServerSoapService() : soap(SOAP_IO_DEFAULT)
{	HttpServerSoapService_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

HttpServerSoapService::HttpServerSoapService(const HttpServerSoapService& rhs)
{	soap_copy_context(this, &rhs);
}

HttpServerSoapService::HttpServerSoapService(const struct soap &_soap) : soap(_soap)
{ }

HttpServerSoapService::HttpServerSoapService(soap_mode iomode) : soap(iomode)
{	HttpServerSoapService_init(iomode, iomode);
}

HttpServerSoapService::HttpServerSoapService(soap_mode imode, soap_mode omode) : soap(imode, omode)
{	HttpServerSoapService_init(imode, omode);
}

HttpServerSoapService::~HttpServerSoapService()
{
	this->destroy();
}

void HttpServerSoapService::HttpServerSoapService_init(soap_mode imode, soap_mode omode)
{	soap_imode(this, imode);
	soap_omode(this, omode);
	static const struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL},
	{"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{NULL, NULL, NULL, NULL}
};
	soap_set_namespaces(this, namespaces);
}

void HttpServerSoapService::destroy()
{	soap_destroy(this);
	soap_end(this);
}

void HttpServerSoapService::reset()
{	this->destroy();
	soap_done(this);
	soap_initialize(this);
	HttpServerSoapService_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

#ifndef WITH_PURE_VIRTUAL
HttpServerSoapService *HttpServerSoapService::copy()
{	HttpServerSoapService *dup = SOAP_NEW_COPY(HttpServerSoapService(*(struct soap*)this));
	return dup;
}
#endif

HttpServerSoapService& HttpServerSoapService::operator=(const HttpServerSoapService& rhs)
{	soap_copy_context(this, &rhs);
	return *this;
}

int HttpServerSoapService::soap_close_socket()
{	return soap_closesock(this);
}

int HttpServerSoapService::soap_force_close_socket()
{	return soap_force_closesock(this);
}

int HttpServerSoapService::soap_senderfault(const char *string, const char *detailXML)
{	return ::soap_sender_fault(this, string, detailXML);
}

int HttpServerSoapService::soap_senderfault(const char *subcodeQName, const char *string, const char *detailXML)
{	return ::soap_sender_fault_subcode(this, subcodeQName, string, detailXML);
}

int HttpServerSoapService::soap_receiverfault(const char *string, const char *detailXML)
{	return ::soap_receiver_fault(this, string, detailXML);
}

int HttpServerSoapService::soap_receiverfault(const char *subcodeQName, const char *string, const char *detailXML)
{	return ::soap_receiver_fault_subcode(this, subcodeQName, string, detailXML);
}

void HttpServerSoapService::soap_print_fault(FILE *fd)
{	::soap_print_fault(this, fd);
}

#ifndef WITH_LEAN
#ifndef WITH_COMPAT
void HttpServerSoapService::soap_stream_fault(std::ostream& os)
{	::soap_stream_fault(this, os);
}
#endif

char *HttpServerSoapService::soap_sprint_fault(char *buf, size_t len)
{	return ::soap_sprint_fault(this, buf, len);
}
#endif

void HttpServerSoapService::soap_noheader()
{	this->header = NULL;
}

const SOAP_ENV__Header *HttpServerSoapService::soap_header()
{	return this->header;
}

int HttpServerSoapService::run(int port)
{	if (!soap_valid_socket(this->master) && !soap_valid_socket(this->bind(NULL, port, 100)))
		return this->error;
	for (;;)
	{	if (!soap_valid_socket(this->accept()))
		{	if (this->errnum == 0) // timeout?
				this->error = SOAP_OK;
			break;
		}
		if (this->serve())
			break;
		this->destroy();
	}
	return this->error;
}

#if defined(WITH_OPENSSL) || defined(WITH_GNUTLS)
int HttpServerSoapService::ssl_run(int port)
{	if (!soap_valid_socket(this->master) && !soap_valid_socket(this->bind(NULL, port, 100)))
		return this->error;
	for (;;)
	{	if (!soap_valid_socket(this->accept()))
		{	if (this->errnum == 0) // timeout?
				this->error = SOAP_OK;
			break;
		}

		if (this->ssl_accept() || this->serve())
			break;

		this->destroy();
	}

	return this->error;
}
#endif

SOAP_SOCKET HttpServerSoapService::bind(const char *host, int port, int backlog)
{	return soap_bind(this, host, port, backlog);
}

SOAP_SOCKET HttpServerSoapService::accept()
{	
	// printf("%s %d\n", __FUNCTION__, __LINE__);
	return soap_accept(this);
}

#if defined(WITH_OPENSSL) || defined(WITH_GNUTLS)
int HttpServerSoapService::ssl_accept()
{	return soap_ssl_accept(this);
}
#endif

int HttpServerSoapService::serve()
{
	// printf("%s %d max_keep_alive:%d\n", __FUNCTION__, __LINE__, this->max_keep_alive);
#ifndef WITH_FASTCGI
	unsigned int k = this->max_keep_alive;
#endif
	do
	{

#ifndef WITH_FASTCGI
		if (this->max_keep_alive > 0 && !--k)
			this->keep_alive = 0;
#endif
// printf("%s %d keep_alive:%d\n", __FUNCTION__, __LINE__, this->keep_alive);
		if (soap_begin_serve(this))
		{	if (this->error >= SOAP_STOP)
				continue;
			printf("%s %d error:%d SOAP_STOP=%d\n", __FUNCTION__, __LINE__, this->error, SOAP_STOP);
			perror("soap_begin_serve");
			return this->error;
		}
		if (dispatch() || (this->fserveloop && this->fserveloop(this)))
		{
#ifdef WITH_FASTCGI
			soap_send_fault(this);
#else
			printf("%s %d\n", __FUNCTION__, __LINE__);
			return soap_send_fault(this);
#endif
		}

#ifdef WITH_FASTCGI
		soap_destroy(this);
		soap_end(this);
	} while (1);
#else
	} while (this->keep_alive);
#endif
	return SOAP_OK;
}

int HttpServerSoapService::dispatch()
{	soap_peek_element(this);
	return this->error;
}

/* End of server object code */
