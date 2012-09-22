#ifndef GENERALACTIONCONTROLLER_H
#define GENERALACTIONCONTROLLER_H

#include <QObject>

namespace PaintField
{

class GeneralActionController : public QObject
{
	Q_OBJECT
public:
	explicit GeneralActionController(QObject *parent = 0);
	
signals:
	
public slots:
	
	void minimizeCurrentWindow();
	void zoomCurrentWindow();
};

}

#endif // GENERALACTIONCONTROLLER_H
