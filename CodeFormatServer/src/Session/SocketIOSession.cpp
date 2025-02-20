#include "CodeFormatServer/Session/SocketIOSession.h"

#include <iostream>

#include "CodeFormatServer/Protocol/ProtocolParser.h"
#include "CodeFormatServer/Protocol/ProtocolBuffer.h"
using namespace asio::ip;

SocketIOSession::SocketIOSession(asio::ip::tcp::socket&& socket)
	: _socket(std::move(socket))
{
}

void SocketIOSession::Run()
{
	ProtocolBuffer protocolBuffer(1024);
	while (true)
	{
		asio::error_code code;
		std::size_t readSize = 0;


		do
		{
			char* writableCursor = protocolBuffer.GetWritableCursor();
			std::size_t capacity = protocolBuffer.GetRestCapacity();

			readSize += _socket.read_some(asio::buffer(writableCursor, capacity), code);
			if (code == asio::error::eof || code)
			{
				goto endLoop;
			}

			protocolBuffer.SetReadableSize(readSize);

			if (protocolBuffer.CanReadOneProtocol())
			{
				break;
			}
		}
		while (true);

		do {
			auto content = protocolBuffer.ReadOneProtocol();
			// std::cout << content << std::endl;
			std::string result = Handle(content);

			protocolBuffer.Reset();
			if (!result.empty()) {
				Send(result);
			}
		} while (protocolBuffer.CanReadOneProtocol());
	}
endLoop:
	return;
}

void SocketIOSession::Send(std::string_view content)
{
	asio::write(_socket, asio::buffer(content));
}
