#include "FLOModuleInstanceDAO.h"
#include "../flaaoscsdk/oscpkt.hh"

#include <QDebug>
#include <vector>

FLOModuleInstanceDAO::FLOModuleInstanceDAO(QObject *parent)
	: QObject(parent),
	  m_uuid(QUuid::createUuid()),
	  inputs(new FLOParameter("inputs", 1, FLOParameter::INTEGER, true)),
	  inputChannels(new FLOParameter("inputChannels", 2, FLOParameter::INTEGER, true)),
	  outputs(new FLOParameter("outputs", 1, FLOParameter::INTEGER, true)),
	  outputChannels(new FLOParameter("outputChannels", 2, FLOParameter::INTEGER, true))
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
	message->pushStr(m_sGroup);
	serializeParameter(message, inputs);
	serializeParameter(message, inputChannels);
	serializeParameter(message, outputs);
	serializeParameter(message, outputChannels);
}

void FLOModuleInstanceDAO::serializeParameter(oscpkt::Message *message, FLOParameter *parameter)
{
	if( !parameter )
		parameter = new FLOParameter();
	message->pushStr(parameter->parameterName());
	message->pushInt32(parameter->parameterType());
	message->pushBool(parameter->editable());
	if( parameter->parameterType() == FLOParameter::BYTES)
	{
		QByteArray a = parameter->value().toByteArray();
		message->pushBlob(a.data_ptr(),static_cast<unsigned long>(a.size()));
	}
	else
		message->pushStr(parameter->value().toString());
}

void FLOModuleInstanceDAO::deserialize(oscpkt::Message *message)
{
	QString uuid;
	int moduleType;
	int dataType;
	int x;
	int y;
	oscpkt::Message::ArgReader &argReader = message->arg().popStr(uuid).popStr(m_sModuleFunctionalName).popStr(m_sModuleName)
											.popInt32(moduleType).popStr(m_sModuleTypeName).popInt32(dataType).popInt32(x).popInt32(y).popStr(m_sGroup);
	argReader = deserializeParameter(argReader, inputs);
	argReader = deserializeParameter(argReader, inputChannels);
	argReader = deserializeParameter(argReader, outputs);
	argReader = deserializeParameter(argReader, outputChannels);
	if( argReader.isOk() )
	{
		m_moduleType = flaarlib::MODULE_TYPE(moduleType);
		m_dataType = flaarlib::DATA_TYPE(dataType);
		m_position = QPoint(x, y);
		m_uuid = uuid;
	}
}

oscpkt::Message::ArgReader &FLOModuleInstanceDAO::deserializeParameter(oscpkt::Message::ArgReader &reader, FLOParameter *parameter)
{
	QString parameterName;
	int type;
	FLOParameter::FLO_PARAMETER_TYPE pType;
	bool editable;
	std::vector<char> bValue;
	QString sValue;
	parameter = new FLOParameter();
	reader = reader.popStr(parameterName).popInt32(type).popBool(editable);
	pType = FLOParameter::FLO_PARAMETER_TYPE(type);
	if( pType == FLOParameter::BYTES )
	{
		reader = reader.popBlob(bValue);
		QByteArray b(bValue.data());
		parameter->setValue(QVariant(b));
	}
	else
	{
		reader = reader.popStr(sValue);
		switch( pType )
		{
			case FLOParameter::BYTES:
				break;
			case FLOParameter::BOOLEAN:
				parameter->setValue(QVariant(sValue).convert(QVariant::Bool));
				break;
			case FLOParameter::STRING:
				parameter->setValue(QVariant(sValue));
				break;
			case FLOParameter::INTEGER:
				parameter->setValue(QVariant(sValue).convert(QVariant::Int));
				break;
			case FLOParameter::LONG:
				parameter->setValue(QVariant(sValue).convert(QVariant::LongLong));
				break;
		}
	}
	return reader;
}

void FLOModuleInstanceDAO::serialize( QXmlStreamWriter *xmlWriter )
{
	xmlWriter->writeStartElement("Module");
	xmlWriter->writeAttribute("functionalName", moduleFunctionalName());
	xmlWriter->writeAttribute("uuid", uuid().toString());
	xmlWriter->writeAttribute("name", moduleName());
	xmlWriter->writeAttribute("moduleGroup", group());
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
	serializeParameter(xmlWriter, inputs);
	serializeParameter(xmlWriter, inputChannels);
	serializeParameter(xmlWriter, outputs);
	serializeParameter(xmlWriter, outputChannels);
	xmlWriter->writeEndElement();
}

void FLOModuleInstanceDAO::serializeParameter(QXmlStreamWriter *xmlWriter, FLOParameter *parameter)
{
	if(!parameter)
		parameter = new FLOParameter();
	xmlWriter->writeStartElement("FLOParameter");
	xmlWriter->writeAttribute("parameterName", parameter->parameterName());
	xmlWriter->writeAttribute("parameterType", QString(parameter->parameterType()));
	xmlWriter->writeAttribute("editable", QString(parameter->editable()));
	xmlWriter->writeStartElement("Value");
	if( parameter->parameterType() == FLOParameter::BYTES)
		xmlWriter->writeCDATA(parameter->value().toString());
	else
		xmlWriter->writeCharacters(parameter->value().toString());
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
						if( name == "moduleGroup")
							setGroup(attribute.value().toString());
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
				if( s == "FLOParameter" )
					deserializeParameter(xmlReader);
				break;
			case QXmlStreamReader::TokenType::Characters:
				{
					text += xmlReader->text().toString();
					break;
				}
			case QXmlStreamReader::TokenType::EndElement:
				s = xmlReader->name();
				if( s == "ModuleType")
					setModuleTypeName( text.trimmed());
				if( s == "Module")
					return;
				break;
			default:
				break;
		}
		t = xmlReader->readNext();
	}
}

void FLOModuleInstanceDAO::deserializeParameter(QXmlStreamReader *xmlReader)
{
	QXmlStreamReader::TokenType t = xmlReader->tokenType();
	QStringRef s = xmlReader->name();
	QString text;
	FLOParameter *p = new FLOParameter();
	while(!xmlReader->atEnd())
	{
		switch( t )
		{
			case QXmlStreamReader::TokenType::StartElement:
				if( s == "FLOParameter" )
				{
					QXmlStreamAttributes attributes = xmlReader->attributes();
					for( auto attribute : attributes )
					{
						QStringRef name = attribute.name();
						qDebug() << "\tAttribute Name: " << name;
						if( name == "parameterName")
							p->setParameterName(attribute.value().toString());
						if( name == "parameterType")
							p->setParameterType(FLOParameter::FLO_PARAMETER_TYPE(attribute.value().toInt()));
						if( name == "editable")
							p->setEditable(attribute.value().toInt());
					}
				}
				if( s == "Value" )
				{

				}
				break;
			case QXmlStreamReader::TokenType::Characters:
				{
					text += xmlReader->text().toString();
					break;
				}
			case QXmlStreamReader::TokenType::EndElement:
				s = xmlReader->name();
				if( s == "Value")
					switch(p->parameterType())
					{
						case FLOParameter::BYTES:
							break;
						case FLOParameter::BOOLEAN:
							p->setValue(QVariant(text).convert(QVariant::Bool));
							break;
						case FLOParameter::STRING:
							p->setValue(QVariant(text));
							break;
						case FLOParameter::INTEGER:
							p->setValue(QVariant(text).convert(QVariant::Int));
							break;
						case FLOParameter::LONG:
							p->setValue(QVariant(text).convert(QVariant::LongLong));
							break;
					}
				setModuleTypeName( text.trimmed());
				if( s == "FLOParameter")
					return;
				break;
			default:
				break;
		}
		t = xmlReader->readNext();
	}
	if( p->parameterName() == "inputs")
		inputs = p;
	else if( p->parameterName() == "inputChannels")
		inputChannels = p;
	else if( p->parameterName() == "outputs")
		outputs = p;
	else if( p->parameterName() == "outputChannels")
		outputChannels = p;
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

QString FLOModuleInstanceDAO::group() const
{
	return m_sGroup;
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

void FLOModuleInstanceDAO::setGroup(const QString &group)
{
	m_sGroup = group;
}
