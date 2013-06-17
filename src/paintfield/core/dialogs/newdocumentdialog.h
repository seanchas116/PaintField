#pragma once

#include <QDialog>

namespace PaintField
{

class NewDocumentDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit NewDocumentDialog(QWidget *parent = 0);
	~NewDocumentDialog();
	
	QSize documentSize() const;
	
public slots:
	
	void onWidthChanged(int w);
	void onHeightChanged(int h);
	
private:
	
	void setProportion();
	
	struct Data;
	Data *d;
};

}

