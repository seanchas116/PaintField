#pragma once

#include <QObject>

namespace PaintField {

class FormatSupport;

class FormatSupportManager : public QObject
{
	Q_OBJECT
public:
	explicit FormatSupportManager(QObject *parent = 0);
	~FormatSupportManager();
	
	void addFormatSupport(FormatSupport *support);
	
	FormatSupport *formatSupport(const QString &name);
	QList<FormatSupport *> formatSupports();
	
signals:
	
public slots:
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField
