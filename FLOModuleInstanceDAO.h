#ifndef FLOMODULEINSTANCEDAO_H
#define FLOMODULEINSTANCEDAO_H

#include "FLModuleDefs.h"

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
	explicit FLOModuleInstanceDAO(FLCRepositoryModule *module);


public: //methods
	void serialize(oscpkt::Message *message);
	void serialize(QXmlStreamWriter *xmlWriter);
	void deserialize(oscpkt::Message *message);
	void deserialize(QXmlStreamReader *xmlReader);

public: //getter

	QUuid uuid() const;
	QString moduleTypeName() const;
	QString moduleName() const;
	QString description() const;
	flaarlib::MODULE_TYPE moduleType() const;
	flaarlib::DATA_TYPE dataType() const;
	QPoint position();
	QString moduleFunctionalName() const;

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

signals:

public slots:

private: //members
	QUuid m_uuid;
	QString m_sModuleTypeName;
	QString m_sModuleName;
	QString m_sModuleFunctionalName;
	QString m_sDescription;
	flaarlib::MODULE_TYPE m_moduleType;
	flaarlib::DATA_TYPE m_dataType;
	QPoint m_position;

};

#endif // FLOMODULEINSTANCEDAO_H
