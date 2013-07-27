#ifndef FORM_H
#define FORM_H

#include <QWidget>

namespace Ui {
class Form;
}

class Form : public QWidget
{
	Q_OBJECT
	
public:
	explicit Form(QWidget *parent = 0);
	~Form();
	
	QString currentDestination();
	QString currentSource();
	QString currentSpreadType();
	
public slots:
	
	void updateDest();
	
private:
	Ui::Form *ui;
	
	struct Data;
	Data *d;
};

#endif // FORM_H
