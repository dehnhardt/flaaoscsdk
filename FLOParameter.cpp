#include "FLOParameter.h"

#include <QDebug>

void FLOParameter::serialize(oscpkt::Message *message)
{

	message->pushStr(parameterName());
	message->pushInt32(parameterType());
	message->pushBool(editable());
	if( parameterType() == FLOParameter::BYTES)
	{
		QByteArray a = value().toByteArray();
		message->pushBlob(a.data_ptr(),static_cast<unsigned long>(a.size()));
	}
	else
		message->pushStr(value().toString());
}

void FLOParameter::deserialize(oscpkt::Message::ArgReader &reader)
{
	QString parameterName;
	int type;
	FLOParameter::FLO_PARAMETER_TYPE pType;
	bool editable;
	std::vector<char> bValue;
	QString sValue;
	reader.popStr(parameterName).popInt32(type).popBool(editable);
	setParameterName(parameterName);
	pType = FLOParameter::FLO_PARAMETER_TYPE(type);
	if( pType == FLOParameter::BYTES )
	{
		reader.popBlob(bValue);
		QByteArray b(bValue.data());
		setValue(QVariant(b));
	}
	else
	{
		reader.popStr(sValue);
		switch( pType )
		{
			case FLOParameter::BYTES:
				break;
			case FLOParameter::BOOLEAN:
				setValue(QVariant(sValue.toInt() == 1));
				break;
			case FLOParameter::STRING:
				setValue(QVariant(sValue));
				break;
			case FLOParameter::INTEGER:
				setValue(QVariant(sValue.toInt()));
				break;
			case FLOParameter::LONG:
				setValue(QVariant(sValue.toLongLong()));
				break;
		}
	}
}

void FLOParameter::serialize(QXmlStreamWriter *xmlWriter)
{
	xmlWriter->writeStartElement("FLOParameter");
	xmlWriter->writeAttribute("parameterName", parameterName());
	xmlWriter->writeAttribute("parameterType", QString::number(parameterType()));
	xmlWriter->writeAttribute("editable", QString::number(editable()));
	xmlWriter->writeStartElement("Value");
	if( parameterType() == FLOParameter::BYTES)
		xmlWriter->writeCDATA(value().toString());
	else
		xmlWriter->writeCharacters(value().toString());
	xmlWriter->writeEndElement();
	xmlWriter->writeEndElement();
}


void FLOParameter::deserialize(QXmlStreamReader *xmlReader)
{
	QXmlStreamReader::TokenType t = xmlReader->tokenType();
	QStringRef s = xmlReader->name();
	QString text;
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
							setParameterName(attribute.value().toString());
						if( name == "parameterType")
							setParameterType(FLOParameter::FLO_PARAMETER_TYPE(attribute.value().toInt()));
						if( name == "editable")
							setEditable(attribute.value().toInt());
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
				{
					switch(parameterType())
					{
						case FLOParameter::BYTES:
							break;
						case FLOParameter::BOOLEAN:
							setValue(QVariant(text.toInt()).convert(QVariant::Bool));
							break;
						case FLOParameter::STRING:
							setValue(QVariant(text));
							break;
						case FLOParameter::INTEGER:
							setValue(QVariant(text.toInt()));
							break;
						case FLOParameter::LONG:
							setValue(QVariant(text.toLongLong()));
							break;
					}
				}
				if( s == "FLOParameter")
					return;
				break;
			default:
				break;
		}
		t = xmlReader->readNext();
	}
}
