#include "HttpClient.h"
#include "Particle.h"

HttpClient::HttpClient(IPAddress address, int p)
{
	TCPClient tcpClient;
	client = tcpClient;

	isBusy = false;
	ip = address;
	port = p;
}

void HttpClient::setIP(IPAddress address)
{
	ip = address;
}

void HttpClient::setPath(String p)
{
	path = p;
}

void HttpClient::setPort(int p)
{
	port = p;
}

bool HttpClient::post(std::string path, std::string body)
{
	if (isBusy)
	{
		Serial.println("HttpClient: busy. Aborting");
		return false;
	}

	if (!WiFi.ready())
	{
		Serial.println("HttpClient: WiFi not ready. Aborting");
		return false;
	}

	isBusy = true;

	if (!client.connected())
	{
		while (!client.connect(ip, port))
		{
			Serial.println("HttpClient: connection failed. Retrying...");
			delay(200);
		}
	}

	std::string request;

	Serial.println("HttpClient: creating request");

	request.append("POST " + path + " HTTP/1.1\r\n");
	request.append("Accept: */*\r\n");
	request.append("Connection: close\r\n");
	request.append("Content-Length: " + std::to_string(body.length()) + "\r\n");
	request.append("Content-Type: text/plain; charset=utf-8\r\n");
	request.append("Host: " + ip.toString() + ":" + port + "\r\n");
	request.append("User-Agent: device-wiring/1.0\r\n");
	request.append("\r\n");
	request.append(body);
	request.append("\r\n");

	Serial.println(request.c_str());

	Serial.println("HttpClient: transmit state");
	client.print(request.c_str());

	// read response and discard it
	Serial.println("HttpClient: reading response...");
	while (client.available())
	{
		Serial.println("HttpClient: reading response to buffer...");
		client.readBytes(dataBuffer, sizeof(dataBuffer));
	}

	Serial.println("HttpClient: discard response buffer");
	memset(&dataBuffer[0], 0, sizeof(dataBuffer));

	Serial.println("HttpClient: closing connection");
	client.stop();

	isBusy = false;

	return true;
}

bool HttpClient::isSending()
{
	return isBusy;
}
