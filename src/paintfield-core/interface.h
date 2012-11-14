#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>

namespace PaintField
{

class ReproductiveInterface
{
public:
	virtual ~ReproductiveInterface() {}
	
	virtual QObject *createNew() = 0;
	
	template <class T>
	T *createNewAs()
	{
		auto obj = createNew();
		auto as = qobject_cast<T *>(obj);
		Q_ASSERT(as);
		return as;
	}
	
};

}

Q_DECLARE_INTERFACE(PaintField::ReproductiveInterface, "PaintField.ReproductiveInterface")


#endif // INTERFACE_H
