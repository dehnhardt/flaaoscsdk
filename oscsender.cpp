#include "oscsender.h"

#include <iostream>
#include <QDebug>

using std::cout;
using std::cerr;

using namespace oscpkt;


OscSender::OscSender( int iPortNum) :
	m_sHost("localhost"),
	m_iPortNum(iPortNum)
{
}

OscSender::OscSender(string sHost, int iPortNum) :
	m_sHost(sHost),
	m_iPortNum(iPortNum)
{
}

OscSender::~OscSender()
{
	qDebug() << "Closing Socket";
	m_pUdpSocket->close();
	delete m_pMessageQueue;
	delete m_pUdpSocket;
	m_pMessageQueue = 0;
	m_pUdpSocket = 0;
}

void OscSender::start()
{
	run();
}

void OscSender::init()
{
	m_pUdpSocket = new UdpSocket();
	m_pMessageQueue = new QQueue<Message>();
	if( m_pUdpSocket->isOk() )
		qDebug() << "Socket ready, sending to host " << m_sHost.c_str() << " on port " << m_iPortNum;
	else
		cerr << "Socket not ready:" << m_pUdpSocket->errorMessage().c_str() << " (host " << m_sHost.c_str() << ", port " << m_iPortNum << ")";
}

void OscSender::enqueuMessage(Message message, bool multicast __attribute__((unused)) )
{
	if( m_pMessageQueue )
	{
		m_pMessageQueue->enqueue(message);
		if( ! m_bRunning )
			sendQueuedMessages();
	}
}

void OscSender::sendQueuedMessages()
{
	if( !m_pUdpSocket )
		cerr << "Socket is null";
	if( m_pUdpSocket->isOk() )
		m_bRunning = true;
	else
	{
		cerr << "Socket not ready";
		return;
	}
	while( !m_pMessageQueue->isEmpty())
	{
		Message message = m_pMessageQueue->dequeue();
		PacketWriter pw;
		pw.startBundle().addMessage(message).endBundle();
		sendPackage(pw);
	}
	m_bRunning = false;
}

void OscSender::sendPackage(PacketWriter pw)
{
	bool ok = m_pUdpSocket->sendPacket(pw.packetData(), pw.packetSize());
	if (ok && m_pUdpSocket->receiveNextPacket(30 /* timeout, in ms */))
	{
		PacketReader pr(m_pUdpSocket->packetData(), m_pUdpSocket->packetSize());
		Message *incoming_msg;
		while (pr.isOk() && (incoming_msg = pr.popMessage()) != 0)
			qDebug() << "Client: received " << incoming_msg->addressPattern().c_str();
	}
	if( !ok )
		cerr << "Sending of packet failed";

}

void OscSender::run()
{
	init();
	m_pUdpSocket->connectTo(m_sHost, m_iPortNum);
}
