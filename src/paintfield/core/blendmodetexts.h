#pragma once

#include <QObject>
#include <Malachite/BlendMode>

namespace PaintField {

class BlendModeTexts : public QObject
{
	Q_OBJECT
public:
	explicit BlendModeTexts(QObject *parent = 0);
	~BlendModeTexts();
	
	QString text(Malachite::BlendMode mode);
	
signals:
	
public slots:
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField
