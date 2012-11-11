#include <QtGui>

#include "splittabareatestwidget.h"

SplitTabAreaTestWidget::SplitTabAreaTestWidget(QWidget *parent) :
	QWidget(parent)
{
	setStyleSheet("QSplitter::handle { background:darkGray; } QSplitter::handle:horizontal { width:1px; } QSplitter::handle:vertical { height:1px; }");
	
	setFocusPolicy(Qt::ClickFocus);
	
	_controller = new SplitTabAreaController(this);
	
	auto mainLayout = new QVBoxLayout;
	
	auto buttonLayout = new QHBoxLayout;
	
	auto addButton = new QPushButton("add");
	auto vsplitButton = new QPushButton("vsplit");
	auto hsplitButton = new QPushButton("hsplit");
	auto closeButton = new QPushButton("close");
	
	connect(addButton, SIGNAL(pressed()), this, SLOT(addTab()));
	connect(vsplitButton, SIGNAL(pressed()), this, SLOT(splitVertical()));
	connect(hsplitButton, SIGNAL(pressed()), this, SLOT(splitHorizontal()));
	connect(closeButton, SIGNAL(pressed()), this, SLOT(close()));
	
	buttonLayout->addWidget(addButton);
	buttonLayout->addWidget(vsplitButton);
	buttonLayout->addWidget(hsplitButton);
	buttonLayout->addWidget(closeButton);
	
	mainLayout->addWidget(_controller->view());
	mainLayout->addLayout(buttonLayout);
	
	setLayout(mainLayout);
	
	connect(_controller, SIGNAL(currentTabChanged(QWidget*)), this, SLOT(onTabChanged(QWidget*)));
}

void SplitTabAreaTestWidget::addTab()
{
	QString name = "tab" + QString::number(_tabCount);
	
	auto label = new QLabel(name);
	label->setObjectName(name);
	
	_controller->addTab(label, name);
	
	_tabCount++;
}

void SplitTabAreaTestWidget::onTabChanged(QWidget *tab)
{
	if (tab)
		qDebug() << "current tab:" << tab->objectName();
}

void SplitTabAreaTestWidget::focusInEvent(QFocusEvent *)
{
	emit focusChanged(true);
}

void SplitTabAreaTestWidget::focusOutEvent(QFocusEvent *)
{
	emit focusChanged(false);
}


