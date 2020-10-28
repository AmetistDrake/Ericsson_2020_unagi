#ifndef CONSOLE_CONNECTOR_H_INCLUDED
#define CONSOLE_CONNECTOR_H_INCLUDED

#include "connector.h"
#include <iostream>
#include <cstring>

class console_connector final : public connector {
public:
	console_connector() {
		std::ios::sync_with_stdio(false);
	}

	virtual ~console_connector() {};

	virtual int send(const char* data, int size) override {
		std::cout.write(data, size);
		std::cout.flush();
		return std::cout ? size : 0;
	}

	virtual int recv(char* buffer, int size) override {
		std::memset(buffer, 0, size); // memory set
		int res = std::cin.getline(buffer, size).gcount();
		if (res > 0 && buffer[res-1] == '\0') {
			buffer[res-1] = '\n';
		}
		return res;
	}
};

#endif // CONSOLE_CONNECTOR_H_INCLUDED
