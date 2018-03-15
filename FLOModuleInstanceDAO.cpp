#include "FLOModuleInstanceDAO.h"
#include "../flaaoscsdk/oscpkt.hh"

#include <QDebug>

FLOModuleInstanceDAO::FLOModuleInstanceDAO(QObject *parent)
	: QObject(parent),
	  m_uuid(QUuid::createUuid())
{

}

FLOModuleInstanceDAO::FLOModuleInstanceDAO(flaarlib::MODULE_TYPE moduleType, flaarlib::DATA_TYPE dataType, QString functionalName, QString moduleTypeName ) :
	FLOModuleInstanceDAO()
{
	setModuleType(moduleType);
	setDataType(dataType);
	setModuleFunctionalName(functionalName);
	setModuleTypeName(moduleTypeName);
}

void FLOModuleInstanceDAO::serialize(oscpkt::Message *message)
{
	message->pushStr(uuid().toString().toStdString());
	message->pushStr(moduleFunctionalName().toStdString());
	message->pushStr(moduleName().toStdString());
	message->pushInt32(moduleType());
	message->pushStr(moduleTypeName().toStdString());
	message->pushInt32(dataType());
	message->pushInt32(position().x());
	message->pushInt32(position().y());
}

void FLOModuleInstanceDAO::deserialize(oscpkt::Message *message)
{
	QString uuid;
	int moduleType;
	int dataType;
	int x;
	int y;
	if(message->arg().popStr(uuid).popStr(m_sModuleFunctionalName).popStr(m_sModuleName)
			.popInt32(moduleType).popStr(m_sModuleTypeName).popInt32(dataType).popInt32(x).popInt32(y).isOk() )
	{
		m_moduleType = flaarlib::MODULE_TYPE(moduleType);
		m_dataType = flaarlib::DATA_TYPE(dataType);
		m_position = QPoint(x, y);
		m_uuid = uuid;
	}
}

void FLOModuleInstanceDAO::serialize( QXmlStreamWriter *xmlWriter )
{
	xmlWriter->writeStartElement("Module");
	xmlWriter->writeAttribute("functionalName", moduleFunctionalName());
	xmlWriter->writeAttribute("uuid", uuid().toString());
	xmlWriter->writeAttribute("name", moduleName());
	xmlWriter->writeStartElement("ModuleType");
	xmlWriter->writeAttribute("moduleTypeId", QString::number(static_cast<int>(moduleType())));
	xmlWriter->writeCharacters(moduleTypeName());
	xmlWriter->writeEndElement();
	xmlWriter->writeStartElement("ModuleDataType");
	xmlWriter->writeAttribute("moduleDataTypeId", QString::number(static_cast<int>(dataType())));
	xmlWriter->writeEndElement();
	xmlWriter->writeStartElement("DisplayPosition");
	xmlWriter->writeAttribute("x", QString::number(position().x()));
	xmlWriter->writeAttribute("y", QString::number(position().y()));
	xmlWriter->writeEndElement();
	xmlWriter->writeEndElement();
}

void FLOModuleInstanceDAO::deserialize(QXmlStreamReader *xmlReader)
{
	QXmlStreamReader::TokenType t = xmlReader->tokenType();
	QStringRef s = xmlReader->name();
	QString text;
	while(!xmlReader->atEnd())
	{
		switch( t )
		{
			case QXmlStreamReader::TokenType::StartElement:
				s = xmlReader->name();
				text.clear();
				qDebug() << "Model: Element Name: " << s;
				if( s == "Module")
				{
					QXmlStreamAttributes attributes = xmlReader->attributes();
					for( auto attribute : attributes )
					{
						QStringRef name = attribute.name();
						if( name == "functionalName")
							setModuleFunctionalName(attribute.value().toString());
						if( name == "uuid")
							setUuid(attribute.value().toString());
						if( name == "name")
							setModuleName(attribute.value().toString());
						qDebug() << "\tAttribute Name: " << name << ", value: " << attribute.value();
					}
				}
				if( s == "ModuleType" )
				{
					QXmlStreamAttributes attributes = xmlReader->attributes();
					for( auto attribute : attributes )
					{
						QStringRef name = attribute.name();
						qDebug() << "\tAttribute Name: " << name;
						if(name == "moduleTypeId")
							setModuleType(flaarlib::MODULE_TYPE(attribute.value().toInt()));
					}

				}
				if( s == "ModuleDataType" )
				{
					QXmlStreamAttributes attributes = xmlReader->attributes();
					for( auto attribute : attributes )
					{
						QStringRef name = attribute.name();
						qDebug() << "\tAttribute Name: " << name;
						if( name == "moduleDataTypeId")
							setDataType(flaarlib::DATA_TYPE(attribute.value().toInt()));
					}

				}
				if( s == "DisplayPosition" )
				{
					QXmlStreamAttributes attributes = xmlReader->attributes();
					QPoint p;
					for( auto attribute : attributes )
					{
						QStringRef name = attribute.name();
						qDebug() << "\tAttribute Name: " << name;
						if( name == "x")
							p.setX(attribute.value().toInt());
						if( name == "y")
							p.setY(attribute.value().toInt());
					}
					setPosition(p);
				}
				break;
			case QXmlStreamReader::TokenType::Characters:
				{
					text += xmlReader->text().toString();
					if( s == "ModuleType")
						qDebug() << text;
					break;
				}
			case QXmlStreamReader::TokenType::EndElement:
				s = xmlReader->name();
				if( s == "ModuleType")
					setModuleTypeName( text.trimmed());
				if( s == "Module")
					return;
			default:
				break;
		}
		t = xmlReader->readNext();
	}
}

/*
 *  getter
 */

QUuid FLOModuleInstanceDAO::uuid() const
{
	return m_uuid;
}

QString FLOModuleInstanceDAO::moduleFunctionalName() const
{
	return m_sModuleFunctionalName;
}

QString FLOModuleInstanceDAO::moduleTypeName() const
{
	return m_sModuleTypeName;
}

QString FLOModuleInstanceDAO::moduleName() const
{
	return m_sModuleName;
}

QString FLOModuleInstanceDAO::description() const
{
	return m_sDescription;
}

flaarlib::MODULE_TYPE FLOModuleInstanceDAO::moduleType() const
{
	return m_moduleType;
}

QPoint FLOModuleInstanceDAO::position()
{
	return m_position;
}

flaarlib::DATA_TYPE FLOModuleInstanceDAO::dataType() const
{
	return m_dataType;
}

/*
 * setter
 */

void FLOModuleInstanceDAO::setModuleTypeName(const QString &sModuleTypeName)
{
	m_sModuleTypeName = sModuleTypeName;
}

void FLOModuleInstanceDAO::setModuleName(const QString &sModuleName)
{
	m_sModuleName = sModuleName;
}

void FLOModuleInstanceDAO::setDescription(const QString &sDescription)
{
	m_sDescription = sDescription;
}

void FLOModuleInstanceDAO::setModuleType(const flaarlib::MODULE_TYPE &moduleType)
{
	m_moduleType = moduleType;
}

void FLOModuleInstanceDAO::setUuid(const QUuid &uuid)
{
	m_uuid = uuid;
}

void FLOModuleInstanceDAO::setDataType(const flaarlib::DATA_TYPE &dataType)
{
	m_dataType = dataType;
}

void FLOModuleInstanceDAO::setPosition(const QPoint &position)
{
	this->m_position = position;
}

void FLOModuleInstanceDAO::setPosition(const int x, const int y)
{
	this->m_position.setX(x);
	this->m_position.setY(y);
}

void FLOModuleInstanceDAO::setModuleFunctionalName(const QString &sModuleFunctionalName)
{
	m_sModuleFunctionalName = sModuleFunctionalName;
}
