#ifndef SOCKET_CONNECTOR_H_INCLUDED
#define SOCKET_CONNECTOR_H_INCLUDED

#include "platform_dep.h"
#include "connector.h"

#include <string>

class socket_connector final : public connector {
	platform_dep::enable_socket _;
	platform_dep::tcp_socket socket_handler;

public:
	socket_connector(const char host_name[], unsigned short port) {
		if(!socket_handler.valid()) {
			invalidate();
			throw std::runtime_error("Error: Cannot open a socket!");
		}

		hostent* host = gethostbyname(host_name);

		if(!host) {
			invalidate();
			throw std::runtime_error("Error: Cannot find host: " + std::string(host_name));
		}

		sockaddr_in socket_address;
		socket_address.sin_family = AF_INET;
		socket_address.sin_port = htons(port);
		socket_address.sin_addr.s_addr = inet_addr(inet_ntoa(*(in_addr *)host->h_addr_list[0]));

		if(connect(socket_handler.get_handler(), (struct sockaddr*)&socket_address,
			sizeof(socket_address)) != 0) {
			invalidate();
			throw std::runtime_error("Error: Cannot connect to the server: " +
				std::string(host_name) + ":" + std::to_string(port) +
				" error code: " + std::to_string(socketerrno));
		}
	}

	virtual ~socket_connector() = default;

	int send(const char* data, int size) override {
		return ::send(socket_handler.get_handler(), data, size, 0);
	}

	int recv(char* buffer, int size) override {
		return ::recv(socket_handler.get_handler(), buffer, size, 0);
	}

	void invalidate() override {
		connector::invalidate();
		socket_handler.invalidate();
	}

	bool is_valid() const override {
		return connector::is_valid() && socket_handler.valid();
	}
};

#endif // SOCKET_CONNECTOR_H_INCLUDED
