#include <QFile>
#include <QJsonDocument>
#include "cpplinq-paintfield.h"

#include "json.h"

namespace PaintField
{

namespace Json
{

QVariant read(const QByteArray &data)
{
	auto document = QJsonDocument::fromJson(data);
	return document.toVariant();
}

QVariant readFromFile(const QString &filepath)
{
	QFile file(filepath);
	
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		PAINTFIELD_WARNING << "cannot open file" << filepath;
		return QVariant();
	}
	
	return read(file.readAll());
}

QByteArray write(const QVariant &variant)
{
	auto document = QJsonDocument::fromVariant(variant);
	return document.toJson();
}

bool writeIntoFile(const QString &filepath, const QVariant &variant)
{
	QFile file(filepath);
	
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		PAINTFIELD_WARNING << "cannot open file" << filepath;
		return false;
	}
	
	auto data = write(variant);
	if (file.write(data) != data.size())
	{
		PAINTFIELD_WARNING << "data is not written correctly";
		return false;
	}
	
	return true;
}

}

}

