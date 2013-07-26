#include "formatsupport.h"

#include "formatsupportmanager.h"

namespace PaintField {

struct FormatSupportManager::Data
{
	QList<FormatSupport *> formatSupports;
};

FormatSupportManager::FormatSupportManager(QObject *parent) :
	QObject(parent),
	d(new Data)
{
}

FormatSupportManager::~FormatSupportManager()
{
	delete d;
}

void FormatSupportManager::addFormatSupport(FormatSupport *support)
{
	support->setParent(this);
	d->formatSupports << support;
}

FormatSupport *FormatSupportManager::formatSupport(const QString &name)
{
	for (auto support : d->formatSupports)
	{
		if (support->name() == name)
			return support;
	}
	return nullptr;
}

QList<FormatSupport *> FormatSupportManager::formatSupports()
{
	return d->formatSupports;
}



} // namespace PaintField
