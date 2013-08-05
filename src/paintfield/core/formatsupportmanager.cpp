#include "formatsupport.h"
#include "paintfieldformatsupport.h"

#include "formatsupportmanager.h"

namespace PaintField {

struct FormatSupportManager::Data
{
	QList<FormatSupport *> formatSupports;
	FormatSupport *paintFieldFormatSupport = 0;
};

FormatSupportManager::FormatSupportManager(QObject *parent) :
	QObject(parent),
	d(new Data)
{
	d->paintFieldFormatSupport = new PaintFieldFormatSupport();
	addFormatSupport(d->paintFieldFormatSupport);
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

FormatSupport *FormatSupportManager::paintFieldFormatSupport()
{
	return d->paintFieldFormatSupport;
}

} // namespace PaintField
