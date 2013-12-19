#ifndef PAINTFIELD_TEST_PROPERTY_H
#define PAINTFIELD_TEST_PROPERTY_H

#include <QObject>

namespace PaintField {

class Test_Property : public QObject
{
	Q_OBJECT
public:
	explicit Test_Property(QObject *parent = 0);

private slots:
	void test_bind();
	void test_bindTransform();
	void test_bindCustomProperty();

};

} // namespace PaintField

#endif // PAINTFIELD_TEST_PROPERTY_H
