#include <QFile>
#include <json_spirit/json_spirit.h>
#include "cpplinq-paintfield.h"

#include "json.h"

namespace PaintField
{

namespace Json
{

static QString qStringFromStdStringUtf8(const std::string &str)
{
	return QString::fromUtf8(str.data(), str.size());
}

static std::string stdStringUtf8FromQString(const QString &str)
{
	auto ba = str.toUtf8();
	return std::string(ba.data(), ba.size());
}

static QVariant getVariant(const json_spirit::Value &value)
{
	switch (value.type())
	{
		case json_spirit::obj_type:
		{
			QVariantMap map;
			auto obj = value.get_obj();
			for (auto &pair : obj)
				map[qStringFromStdStringUtf8(pair.name_)] = getVariant(pair.value_);
			
			return map;
		}
		case json_spirit::array_type:
		{
			QVariantList list;
			auto array = value.get_array();
			for (auto &value : array)
				list << getVariant(value);
			
			return list;
		}
		case json_spirit::str_type:
		{
			auto str = value.get_str();
			return QString::fromUtf8(str.data(), str.size());
		}
		case json_spirit::bool_type:
		{
			return value.get_bool();
		}
		case json_spirit::int_type:
		{
			return value.get_int64();
		}
		case json_spirit::real_type:
		{
			return value.get_real();
		}
		default:
		case json_spirit::null_type:
		{
			return QVariant();
		}
	}
}

static json_spirit::Value getValue(const QVariant &variant)
{
	switch (variant.type())
	{
		case QVariant::Map:
		{
			auto map = variant.toMap();
			json_spirit::Object obj;
			obj.reserve(map.size());
			
			for (auto it = map.begin(); it != map.end(); ++it)
				obj.push_back(json_spirit::Pair(stdStringUtf8FromQString(it.key()), getValue(it.value())));
			
			return obj;
		}
		case QVariant::List:
		{
			auto list = variant.toList();
			json_spirit::Array array;
			array.reserve(list.size());
			
			for (auto &item : list)
				array.push_back(getValue(item));
			
			return array;
		}
		case QVariant::String:
			return stdStringUtf8FromQString(variant.toString());
		case QVariant::Bool:
			return variant.toBool();
		case QVariant::Double:
			return variant.toDouble();
		case QVariant::Char:
		case QVariant::Int:
		case QVariant::LongLong:
			return variant.toLongLong();
		case QVariant::UInt:
		case QVariant::ULongLong:
			return variant.toULongLong();
		default:
			return json_spirit::Value();
	}
}

QVariant read(const QByteArray &data)
{
	json_spirit::Value value;
	if ( !json_spirit::read( std::string( data.data(), data.size() ), value ) )
	{
		PAINTFIELD_WARNING << "failed to read";
		return QVariant();
	}
	
	return getVariant(value);
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
	auto value = getValue(variant);
	auto str = json_spirit::write(value, json_spirit::pretty_print);
	return QByteArray(str.data(), str.size());
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

