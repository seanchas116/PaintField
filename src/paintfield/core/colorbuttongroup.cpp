#include <QButtonGroup>
#include "widgets/colorbutton.h"

#include "colorbuttongroup.h"

namespace PaintField {

struct ColorButtonGroup::Data
{
	ColorButton *current = 0;
	QButtonGroup *group = 0;
	
	QList<ColorButton *> buttons;
	QList<ColorButton *> buttonHistory;
	
	void addButtonHistory(ColorButton *btn)
	{
		if (buttonHistory.contains(btn))
			buttonHistory.removeAll(btn);
		
		buttonHistory.prepend(btn);
	}
	
	void updateCheckState()
	{
		PAINTFIELD_DEBUG << current;
		
		for (auto button : buttons)
		{
			
			button->setChecked(button == current);
		}
	}
};

ColorButtonGroup::ColorButtonGroup(QObject *parent) :
	QObject(parent),
	d(new Data)
{
	d->group = new QButtonGroup(this);
	connect(d->group, SIGNAL(buttonPressed(QAbstractButton*)), this, SLOT(onCurrentButtonChanged(QAbstractButton*)));
}

ColorButtonGroup::~ColorButtonGroup()
{
	delete d;
}

void ColorButtonGroup::add(ColorButton *button)
{
	d->buttons << button;
	d->group->addButton(button);
	button->installEventFilter(this);
	if (button->isChecked())
		setCurrentButton(button);
}

Malachite::Color ColorButtonGroup::currentColor() const
{
	if (d->current)
		return d->current->color();
	else
		return Malachite::Color();
}

void ColorButtonGroup::setCurrentColor(const Malachite::Color &color)
{
	if (d->current)
		d->current->setColor(color);
}

void ColorButtonGroup::onCurrentButtonChanged(QAbstractButton *button)
{
	setCurrentButton(qobject_cast<ColorButton *>(button));
}

void ColorButtonGroup::setCurrentButton(ColorButton *button)
{
	if (d->current == button)
		return;
	
	if (d->current)
		disconnect(d->current, SIGNAL(colorChanged(Malachite::Color)), this, SIGNAL(currentColorChanged(Malachite::Color)));
	
	d->current = button;
	button->setChecked(true);
	d->addButtonHistory(button);
	
	if (d->current)
		connect(d->current, SIGNAL(colorChanged(Malachite::Color)), this, SIGNAL(currentColorChanged(Malachite::Color)));
	
	emit currentColorChanged(currentColor());
}

void ColorButtonGroup::changeDisabledCurrentButton()
{
	if (d->current->isEnabled())
		return;
	
	for (auto button : d->buttonHistory)
	{
		if (button->isEnabled())
		{
			setCurrentButton(button);
			return;
		}
	}
	
	setCurrentButton(0);
}

bool ColorButtonGroup::eventFilter(QObject *object, QEvent *event)
{
	auto button = qobject_cast<ColorButton *>(object);
	
	if (button && d->buttons.contains(button))
	{
		if (event->type() == QEvent::EnabledChange)
		{
			changeDisabledCurrentButton();
		}
	}
	
	return false;
}

} // namespace PaintField
