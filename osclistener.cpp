#include "osclistener.h"
#include "oscpkt.hh"
#include "udp.hh"

#include <QDebug>
#include <QString>


OscListener::OscListener( int iPortNum) :
	QObject( ),
	OscHandler ("/"),
	m_iPortNum( iPortNum)
{
}

OscListener::~OscListener()
{
	delete m_pUdpSocket;
	m_pUdpSocket = 0;
}

void OscListener::init()
{
	qDebug() << "Listener slot init called";
	m_bRunning = true;
	m_pUdpSocket = new oscpkt::UdpSocket();
	emit( started());
	runListener();
}

void OscListener::exit()
{
	qDebug() << "Listener slot exit called";
	setBRunning(false);
	emit(finished());
}

void OscListener::runListener()
{

	using std::cout;
	using std::cerr;

	m_pUdpSocket->bindTo(m_iPortNum);
	if (!m_pUdpSocket->isOk())
		cerr << "Error opening listener port " << m_iPortNum << ": " << m_pUdpSocket->errorMessage() << "\n";
	else
	{
		cout << "Listener started, will listen to packets on port " << m_iPortNum << std::endl;
		PacketReader pr;
		while (m_pUdpSocket->isOk())
		{
			if (m_pUdpSocket->receiveNextPacket(30))
			{
				SockAddr addr = m_pUdpSocket->remote_addr;
				qDebug("Recieve from host %s", addr.asString().c_str());
				pr.init(m_pUdpSocket->remote_addr.getHost(), m_pUdpSocket->packetData(), m_pUdpSocket->packetSize());
				oscpkt::Message *message;
				while (pr.isOk() && (message = pr.popMessage()) != 0)
				{
					OscHandler *handler = handlerFor(message);
					if( handler)
						handler->handle(m_pUdpSocket, message);
					else
					{
						qDebug() << "Listener: unhandled message: ";
						qDebug() << message->prettyPrint().c_str();
					}
				}
			}
			if( !m_bRunning )
				break;
		}
		qDebug() << "close listener socket";
		m_pUdpSocket->close();
	}
}

void OscListener::setBRunning(bool bRunning)
{
	m_bRunning = bRunning;
}
