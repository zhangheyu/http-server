#ifndef _ENTPSMS_SERVICE_H_
#define _ENTPSMS_SERVICE_H_

#include "soapH.h"

class HttpServerSoapService : public soap
{
public:
	/// Constructor
	HttpServerSoapService();
	/// Copy constructor
	HttpServerSoapService(const HttpServerSoapService&);
	/// Construct from another engine state
	HttpServerSoapService(const struct soap&);
	/// Constructor with engine input+output mode control
	HttpServerSoapService(soap_mode iomode);
	/// Constructor with engine input and output mode control
	HttpServerSoapService(soap_mode imode, soap_mode omode);
	/// Destructor deletes deserialized data and engine context
	virtual ~HttpServerSoapService();
	/// Delete all deserialized data (with soap_destroy() and soap_end())
	virtual	void destroy();
	/// Delete all deserialized data and reset to defaults
	virtual	void reset();
	/// Initializer used by constructor
	virtual	void HttpServerSoapService_init(soap_mode imode, soap_mode omode);
	/// Create a new copy
	virtual	HttpServerSoapService *copy() SOAP_PURE_VIRTUAL;
	/// Copy assignment
	HttpServerSoapService& operator=(const HttpServerSoapService&);
	/// Close connection (normally automatic)
	virtual	int soap_close_socket();
	/// Force close connection (can kill a thread blocked on IO)
	virtual	int soap_force_close_socket();
	/// Return sender-related fault to sender
	virtual	int soap_senderfault(const char *string, const char *detailXML);
	/// Return sender-related fault with SOAP 1.2 subcode to sender
	virtual	int soap_senderfault(const char *subcodeQName, const char *string, const char *detailXML);
	/// Return receiver-related fault to sender
	virtual	int soap_receiverfault(const char *string, const char *detailXML);
	/// Return receiver-related fault with SOAP 1.2 subcode to sender
	virtual	int soap_receiverfault(const char *subcodeQName, const char *string, const char *detailXML);
	/// Print fault
	virtual	void soap_print_fault(FILE*);
#ifndef WITH_LEAN
	/// Print fault to stream
#ifndef WITH_COMPAT
	virtual	void soap_stream_fault(std::ostream&);
#endif
	/// Put fault into buffer
	virtual	char *soap_sprint_fault(char *buf, size_t len);
#endif
	/// Disables and removes SOAP Header from message
	virtual	void soap_noheader();
	/// Get SOAP Header structure (NULL when absent)
	virtual	const SOAP_ENV__Header *soap_header();
	/// Run simple single-thread (iterative, non-SSL) service on port until a connection error occurs (returns error code or SOAP_OK), use this->bind_flag = SO_REUSEADDR to rebind for a rerun
	virtual	int run(int port);
#if defined(WITH_OPENSSL) || defined(WITH_GNUTLS)
	/// Run simple single-thread SSL service on port until a connection error occurs (returns error code or SOAP_OK), use this->bind_flag = SO_REUSEADDR to rebind for a rerun
	virtual	int ssl_run(int port);
#endif
	/// Bind service to port (returns master socket or SOAP_INVALID_SOCKET)
	virtual	SOAP_SOCKET bind(const char *host, int port, int backlog);
	/// Accept next request (returns socket or SOAP_INVALID_SOCKET)
	virtual	SOAP_SOCKET accept();
#if defined(WITH_OPENSSL) || defined(WITH_GNUTLS)
	/// Then accept SSL handshake, when SSL is used
	virtual	int ssl_accept();
#endif
	/// Serve this request (returns error code or SOAP_OK)
	virtual	int serve();
	/// Used by serve() to dispatch a request (returns error or SOAP_OK)
	virtual	int dispatch();
};

#endif