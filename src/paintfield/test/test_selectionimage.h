#ifndef TEST_SELECTIONIMAGE_H
#define TEST_SELECTIONIMAGE_H

#include <QObject>

namespace PaintField {

class Test_SelectionImage : public QObject
{
	Q_OBJECT
public:
	explicit Test_SelectionImage(QObject *parent = 0);

signals:

public slots:

private slots:

	void test_isBlank();
	void test_iterator();
	void test_toQImageARGBPremult();

};

}

#endif // TEST_SELECTIONIMAGE_H
