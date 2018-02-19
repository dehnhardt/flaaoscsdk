#include "FLOModuleInstancesModel.h"
#include "FLOModuleInstanceDAO.h"

#include <QDebug>

FLOModuleInstancesModel::FLOModuleInstancesModel(QObject *parent)
	: QObject(parent)
{

}

void FLOModuleInstancesModel::serialize(QXmlStreamWriter *xmlWriter)
{
	xmlWriter->writeStartElement("Modules");
	xmlWriter->writeAttribute("count", QString::number(m_moduleInstancesMap.size() ));
	for( auto it : m_moduleInstancesMap )
	{
		FLOModuleInstanceDAO *i = it;
		i->serialize(xmlWriter);
	}
	xmlWriter->writeEndElement();
}

void FLOModuleInstancesModel::deserialize(QXmlStreamReader *xmlReader)
{
	while(!xmlReader->atEnd())
	{
		QXmlStreamReader::TokenType t = xmlReader->readNext();
		QStringRef s = xmlReader->name();
		switch( t )
		{
			case QXmlStreamReader::TokenType::StartElement:
				qDebug() << "Model: Element Name: " << s;
				if( s  == "Module")
				{
					FLOModuleInstanceDAO *i = new FLOModuleInstanceDAO();
					i->deserialize(xmlReader);
					addFLOModuleInstance(i);
				}
				break;
			case QXmlStreamReader::TokenType::EndElement:
				if( s == "Modules")
					return;
			default:
				break;
		}
	}
}

void FLOModuleInstancesModel::addFLOModuleInstance(FLOModuleInstanceDAO *moduleInstance)
{
	m_moduleInstancesMap[moduleInstance->uuid()] = moduleInstance;
	emit(moduleAdded(moduleInstance));
}

FLOModuleInstanceDAO *FLOModuleInstancesModel::getFLOModuleInstance(QUuid uuid)
{
	return m_moduleInstancesMap[uuid];
}

QMap<QUuid, FLOModuleInstanceDAO *> FLOModuleInstancesModel::getModuleInstancesMap()
{
	return m_moduleInstancesMap;
}
