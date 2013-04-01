#include <QButtonGroup>
#include "widgets/colorbutton.h"

#include "colorbuttonmanager.h"

namespace PaintField {

struct ColorButtonManager::Data
{
	QButtonGroup *group = 0;
	ColorButton *currentButton = 0;
};

ColorButtonManager::ColorButtonManager(QObject *parent) :
	QObject(parent),
	d(new Data)
{
	d->group = new QButtonGroup(this);
}

ColorButtonManager::~ColorButtonManager()
{
	delete d;
}

void ColorButtonManager::add(ColorButton *button)
{
	d->group->addButton(button);
}

void ColorButtonManager::remove(ColorButton *button)
{
	d->group->removeButton(button);
}

Malachite::Color ColorButtonManager::currentColor() const
{
	if (d->currentButton)
		return d->currentButton->color();
	else
		return Malachite::Color();
}

ColorButton *ColorButtonManager::currentButton()
{
	return d->currentButton;
}

void ColorButtonManager::setCurrentColor(const Malachite::Color &color)
{
	if (d->currentButton)
		d->currentButton->setColor(color);
}

void ColorButtonManager::setCurrentButton(ColorButton *button)
{
	if (d->currentButton == button)
		return;
	
	if (d->currentButton)
		disconnect(d->currentButton, SIGNAL(colorChanged(Malachite::Color)), this, SIGNAL(currentColorChanged(Malachite::Color)));
	
	d->currentButton = button;
	
	if (d->currentButton)
		connect(d->currentButton, SIGNAL(colorChanged(Malachite::Color)), this, SIGNAL(currentColorChanged(Malachite::Color)));
	
	emit currentColorChanged(currentColor());
	emit currentButtonChanged(button);
}

void ColorButtonManager::onCurrentButtonChanged(QAbstractButton *button)
{
	setCurrentButton(qobject_cast<ColorButton *>(button));
}

} // namespace PaintField
