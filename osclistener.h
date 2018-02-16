#ifndef OSCLISTENER_H
#define OSCLISTENER_H

#define OSCPKT_OSTREAM_OUTPUT

#include "oschandler.h"

#include <QObject>
#include <map>
#include <string>

namespace oscpkt
{
class UdpSocket;
}

class OscListener : public QObject, public OscHandler
{
	Q_OBJECT
public:
	OscListener( int iPortNum);
	~OscListener();

	void setBRunning(bool bRunning);

public slots:
	void init();
	void exit();

signals:
	void started();
	void finished();

private:
	void runListener();

private:
	int m_iPortNum = 0;
	bool m_bRunning = false;
	oscpkt::UdpSocket *m_pUdpSocket =0;
};

#endif // OSCLISTENER_H
