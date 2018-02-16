#ifndef OSCHANDLER_H
#define OSCHANDLER_H

#include "oscpkt.hh"

#include <string>
#include <map>

namespace oscpkt
{
class UdpSocket;
}

using namespace oscpkt;

class OscHandler
{
public:
	OscHandler(std::string prefix);
	virtual ~OscHandler();

	std::string prefix() const
	{
		return m_sPrefix;
	}

public: //methods
	virtual bool handle(UdpSocket *socket, Message *message);
	void registerHandler( OscHandler *handler);
	void registerHandler( std::string prefix, OscHandler *handler);
	OscHandler *handlerFor( Message *message);
	OscHandler *handlerFor( std::string prefix);

private: //members
	std::string m_sPrefix;
	std::map<std::string, OscHandler *> m_HandlerMap;

};

#endif // OSCHANDLER_H
