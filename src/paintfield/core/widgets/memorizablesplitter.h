#pragma once

#include <QSplitter>

namespace PaintField {

class MemorizableSplitter : public QSplitter
{
	Q_OBJECT
public:
	explicit MemorizableSplitter(QWidget *parent = 0) : QSplitter(parent) {}
	explicit MemorizableSplitter(Qt::Orientation orientation, QWidget *parent = 0) : QSplitter(orientation, parent) {}
	
	void memorizeSizes();
	void restoreSizes();
	
signals:
	
public slots:
	
private:
	
	QList<int> _memorizedSizes;
};

} // namespace PaintField

