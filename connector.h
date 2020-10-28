#ifndef CONNECTOR_H_INCLUDED
#define CONNECTOR_H_INCLUDED

class connector {
	bool valid = true;
public:
	virtual int send(const char*, int) = 0;
	virtual int recv(char*, int) = 0;

	virtual void invalidate() {
		valid = false;
	}

	virtual bool is_valid() const {
		return valid;
	}

	virtual ~connector() {};
};

#endif // CONNECTOR_H_INCLUDED
