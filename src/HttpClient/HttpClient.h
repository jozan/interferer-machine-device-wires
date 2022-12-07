#ifndef __HTTP_CLIENT_H_
#define __HTTP_CLIENT_H_

#include "spark_wiring_tcpclient.h"
#include "spark_wiring_usbserial.h"

class HttpClient
{
public:
	HttpClient(IPAddress address, int port);

	void setIP(IPAddress ip);
	void setPath(String path);
	void setPort(int port);

	bool post(std::string path, std::string body);

	bool isSending();

private:
	IPAddress ip;
	String path;
	int port;

	TCPClient client;
	char dataBuffer[128];
	bool isBusy;
};

#endif /* __HTTP_CLIENT_H_ */
