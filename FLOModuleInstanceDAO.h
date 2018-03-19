#ifndef FLOMODULEINSTANCEDAO_H
#define FLOMODULEINSTANCEDAO_H

#include "FLModuleDefs.h"
#include "FLOParameter.h"
#include "oscpkt.hh"

#include <QObject>
#include <QUuid>
#include <QPoint>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

class FLCRepositoryModule;
namespace oscpkt
{
class Message;
}

class FLOModuleInstanceDAO : public QObject
{
	Q_OBJECT
public:
	explicit FLOModuleInstanceDAO(QObject *parent = nullptr);
	explicit FLOModuleInstanceDAO(flaarlib::MODULE_TYPE moduleType, flaarlib::DATA_TYPE dataType, QString functionalName, QString moduleTypeName);


public:
	//methods serialize to message
	void serialize(oscpkt::Message *message);
	void serializeParameter(oscpkt::Message *message, FLOParameter *parameter);

	//methods serialize to xml
	void serialize(QXmlStreamWriter *xmlWriter);
	void serializeParameter(QXmlStreamWriter *xmlWriter, FLOParameter *parameter);

	//methods deserialize from message
	void deserialize(oscpkt::Message *message);
	oscpkt::Message::ArgReader &deserializeParameter(oscpkt::Message::ArgReader &message, FLOParameter *parameter);

	//methods deserialize from xml
	void deserialize(QXmlStreamReader *xmlReader);
	void deserializeParameter(QXmlStreamReader *xmlReader);

public: //getter

	QUuid uuid() const;
	QString moduleTypeName() const;
	QString moduleName() const;
	QString description() const;
	flaarlib::MODULE_TYPE moduleType() const;
	flaarlib::DATA_TYPE dataType() const;
	QPoint position();
	QString moduleFunctionalName() const;
	QString group() const;

public: //setter

	void setUuid(const QUuid &uuid);
	void setModuleTypeName(const QString &moduleTypeName);
	void setModuleName(const QString &moduleName);
	void setDescription(const QString &description);
	void setModuleType(const flaarlib::MODULE_TYPE &moduleType);
	void setDataType(const flaarlib::DATA_TYPE &dataType);
	void setPosition(const QPoint &m_position);
	void setPosition(const int x, const int y);
	void setModuleFunctionalName(const QString &moduleFunctionalName);
	void setGroup(const QString &group);

signals:

public slots:

private: //data members
	QUuid m_uuid;
	QString m_sModuleTypeName;
	QString m_sModuleName;
	QString m_sModuleFunctionalName;
	QString m_sDescription;
	flaarlib::MODULE_TYPE m_moduleType;
	flaarlib::DATA_TYPE m_dataType;
	FLOParameter *inputs = 0;
	FLOParameter *inputChannels = 0;
	FLOParameter *outputs = 0;
	FLOParameter *outputChannels = 0;

private: //graphical representation
	QPoint m_position;
	QString m_sGroup;

};

#endif // FLOMODULEINSTANCEDAO_H
