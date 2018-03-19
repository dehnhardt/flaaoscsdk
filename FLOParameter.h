#ifndef FLOPARAMETER_H
#define FLOPARAMETER_H

#include <QString>
#include <QVariant>

class FLOParameter
{
public:
	enum FLO_PARAMETER_TYPE
	{
		STRING = 1,
		BOOLEAN = 2,
		INTEGER = 3,
		LONG = 4,
		BYTES = 5
	};
public:
	FLOParameter() :
		m_sParameterName(""),
		m_bEditable(true),
		m_parameterType(FLOParameter::STRING),
		m_vValue(QVariant(""))
	{}
	FLOParameter(QString parameterName, QVariant value, FLO_PARAMETER_TYPE parameterType = STRING, bool editable = false) :
		m_sParameterName(parameterName),
		m_bEditable(editable),
		m_parameterType(parameterType),
		m_vValue(value)
	{}
	~FLOParameter() {}

public: //getter
	QString parameterName() const
	{
		return m_sParameterName;
	}
	bool editable() const
	{
		return m_bEditable;
	}
	FLO_PARAMETER_TYPE parameterType() const
	{
		return m_parameterType;
	}
	QVariant value() const
	{
		return m_vValue;
	}

public: //setter
	void setParameterName(const QString &parameterName)
	{
		m_sParameterName = parameterName;
	}
	void setEditable(bool editable)
	{
		m_bEditable = editable;
	}
	void setParameterType(const FLO_PARAMETER_TYPE &parameterType)
	{
		m_parameterType = parameterType;
	}
	void setValue(const QVariant &value)
	{
		m_vValue = value;
	}

private:
	QString m_sParameterName;
	bool m_bEditable = false;
	FLO_PARAMETER_TYPE m_parameterType = STRING;
	QVariant m_vValue;

};

#endif // FLOPARAMETER_H

