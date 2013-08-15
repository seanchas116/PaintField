#include "paintfield/core/appcontroller.h"
#include "paintfield/core/formatsupportmanager.h"
#include "malachiteformatsupport.h"
#include "openrasterformatsupport.h"
#include "psdformatsupport.h"

#include "formatsupportsextension.h"

namespace PaintField {

FormatSupportsExtension::FormatSupportsExtension(AppController *appC, QObject *parent) :
	AppExtension(appC, parent)
{
	auto manager = appC->formatSupportManager();
	manager->addFormatSupport(new JpegFormatSupport());
	manager->addFormatSupport(new PngFormatSupport());
	manager->addFormatSupport(new OpenRasterFormatSupport());
	manager->addFormatSupport(new PsdFormatSupport());
}

FormatSupportsExtensionFactory::FormatSupportsExtensionFactory(QObject *parent) :
	ExtensionFactory(parent)
{
}

void FormatSupportsExtensionFactory::initialize(AppController *app)
{
	
}

AppExtensionList FormatSupportsExtensionFactory::createAppExtensions(AppController *app, QObject *parent)
{
	return { new FormatSupportsExtension(app, parent) };
}

} // namespace PaintField
