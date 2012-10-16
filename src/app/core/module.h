#ifndef MODULE_H
#define MODULE_H

#include <QObject>

namespace PaintField
{

class Module : public QObject
{
	Q_OBJECT
public:
	
	explicit Module(QObject *parent = 0) : QObject(parent) {}
	
	virtual void initialize() = 0;
	
signals:
	
public slots:
	
private:
};

}

#endif // MODULE_H
