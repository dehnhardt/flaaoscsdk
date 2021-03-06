#include "oschandler.h"

OscHandler::OscHandler( std::string prefix)
	: m_sHandlerName("OscHandler"),
	  m_sPrefix(prefix)
{

}

OscHandler::~OscHandler()
{

}

bool OscHandler::handle(UdpSocket *socket, Message *message)
{
	Q_UNUSED(socket);
	Q_UNUSED(message);
	return false;
}

void OscHandler::registerHandler(OscHandler *handler)
{
	registerHandler(handler->prefix(), handler);
}


void OscHandler::registerHandler(std::string prefix, OscHandler *handler)
{
	std::string firstPrefix = prefix.substr(0, prefix.find_first_of('/', 1));
	std::string resultingPrefix = prefix.substr(firstPrefix.size());
	if( resultingPrefix == "")
		m_HandlerMap[prefix] = handler;
	else
	{
		OscHandler *h = handlerFor(firstPrefix);
		if( !h )
			h = new OscHandler(firstPrefix);
		m_HandlerMap[firstPrefix] = h;
		h->registerHandler(resultingPrefix, handler);
	}
}

OscHandler *OscHandler::handlerFor(Message *message)
{
	std::string prefix = message->addressPattern();
	return handlerFor(prefix);
}

OscHandler *OscHandler::handlerFor(std::string prefix)
{
	for( auto handerlIt : m_HandlerMap)
	{
		std::string h = handerlIt.first;
		if( prefix.find(h) == 0)
		{
			if( h == prefix)
				return handerlIt.second;
			std::string resultingPrefix = prefix.substr(h.length());
			OscHandler *handler = handerlIt.second->handlerFor(resultingPrefix);
			if( handler )
				return handler;
			else
				return handerlIt.second;
		}
	}
	return 0;
}

std::string OscHandler::lastPathToken(std::string addressPattern)
{
	unsigned long index = addressPattern.rfind('/');
	if( index < addressPattern.length() && index != std::string::npos )
		++index;
	return addressPattern.substr(index);

}
