#include <QtGui>
#include <QVBoxLayout>
#include <QLabel>
#include "fsborderwidget.h"

#include "fspanel.h"

void fsApplyMacSmallSize(QWidget *widget)
{
	widget->setAttribute(Qt::WA_MacSmallSize);
	foreach (QObject *object, widget->children())
	{
		QWidget *widget = qobject_cast<QWidget *>(object);
		if (widget)
			fsApplyMacSmallSize(widget);
	}
}

FSPanel::FSPanel(QWidget *parent) :
	QWidget(parent),
    _widget(0)
{
	//setStyleSheet("FSPanel { background-color: rgb(250, 250, 250); }");
	
	//setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	setWindowFlags(Qt::Tool);
	
	_layout = new QVBoxLayout;
	_layout->setContentsMargins(8, 8, 8, 8);
	_layout->setSpacing(4);
	
	_label = new QLabel;
	
	//QFont font;
	//font.setBold(true);
	//_label->setFont(font);
	//_label->setAlignment(Qt::AlignHCenter);
	
	_layout->addWidget(_label);
	
	FSBorderWidget *border = new FSBorderWidget;
	border->setColor(QColor(150, 150, 150));
	
	_layout->addWidget(border);
}

void FSPanel::setWidget(QWidget *widget)
{
	if (_widget)
		return;
	
	fsApplyMacSmallSize(widget);
	
	_layout->addWidget(widget);
	_layout->addStretch(1);
	_label->setText(widget->windowTitle());
	_widget = widget;
	
	setLayout(_layout);
}

void FSPanel::closeEvent(QCloseEvent *event)
{
	event->ignore();
	hide();
}

void FSPanel::mousePressEvent(QMouseEvent *event)
{
	_origPanelPos = pos();
	_origMousePos = event->globalPos();
}

void FSPanel::mouseMoveEvent(QMouseEvent *event)
{
	QPoint disp = event->globalPos() - _origMousePos;
	move(_origPanelPos + disp);
}
