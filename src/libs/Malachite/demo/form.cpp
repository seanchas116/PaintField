#include <Malachite/Image>
#include <Malachite/Surface>
#include <Malachite/Painter>
#include <Malachite/ImageIO>
#include <Malachite/ColorGradient>
#include <QScopedPointer>

#include "form.h"
#include "ui_form.h"

using namespace Malachite;

struct Form::Data
{
	Color color;
	QScopedPointer<ArgbGradient> gradient;
	LinearGradientShape linearGradientInfo;
	RadialGradientShape radialGradientInfo;
	Image image;
	Surface surface;
	
	QRectF rect;
	QPainterPath path;
};

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form),
    d(new Data)
{
	ui->setupUi(this);
	
	d->color = Color::fromRgbValue(1.0, 0.5, 0.2);
	
	d->gradient.reset(new ArgbGradient);
	d->gradient->addStop(0.0, Color::fromRgbValue(1.0, 0.5, 0.0, 1.0));
	d->gradient->addStop(0.5, Color::fromRgbValue(0.5, 1.0, 0.0, 1.0));
	d->gradient->addStop(1.0, Color::fromRgbValue(1.0, 1.0, 1.0, 1.0));
	
	d->linearGradientInfo = LinearGradientShape(Vec2D(100, 100), Vec2D(200, 300));
	d->radialGradientInfo = RadialGradientShape(Vec2D(200, 200), 50, Vec2D(200, 210));
	
	{
		ImageImporter importer;
		importer.load(":/demo-texture.jpg");
		d->image = importer.toImage();
		d->surface = importer.toSurface();
	}
	
	d->rect = QRect(50, 50, 400, 300);
	
	{
		QFont font;
		font.setBold(true);
		font.setPixelSize(288);
		d->path.addText(0, 300, font, "Path");
	}
	
	updateDest();
}

Form::~Form()
{
	delete ui;
	delete d;
}

QString Form::currentDestination()
{
	return ui->comboBoxDestination->currentText();
}

QString Form::currentSource()
{
	return ui->comboBoxSource->currentText();
}

QString Form::currentSpreadType()
{
	return ui->comboBoxSpreadType->currentText();
}

void Form::updateDest()
{
	ui->viewport->clearPaintable();
	
	if (currentDestination() == "Image")
		ui->viewport->setMode(Viewport::ModeImage);
	else
		ui->viewport->setMode(Viewport::ModeSurface);
	
	{
		Brush brush;
		
		if (currentSource() == "Color")
			brush = Brush(d->color);
		else if (currentSource() == "Linear Gradient")
			brush = Brush(Brush(*d->gradient.data(), d->linearGradientInfo));
		else if (currentSource() == "Radial Gradient")
			brush = Brush(Brush(*d->gradient.data(), d->radialGradientInfo));
		else if (currentSource() == "Image")
			brush = Brush(d->image);
		else
			brush = Brush(d->surface);
		
		if (currentSpreadType() == "Pad")
			brush.setSpreadType(SpreadTypePad);
		else if (currentSpreadType() == "Repeat")
			brush.setSpreadType(SpreadTypeRepeat);
		else
			brush.setSpreadType(SpreadTypeReflective);
		
		{
			Painter painter(ui->viewport->paintable());
			painter.setBrush(brush);
			painter.drawPath(d->path);
		}
	}
	
	ui->viewport->update();
}
