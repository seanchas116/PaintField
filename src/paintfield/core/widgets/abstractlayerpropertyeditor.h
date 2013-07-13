#pragma once

#include <QWidget>
#include "../layer.h"

namespace PaintField {

class LayerScene;

/**
 * The AbstractLayerPropertyEditor class provides a base of layer property editing panes.
 */
class AbstractLayerPropertyEditor : public QWidget
{
	Q_OBJECT
public:
	explicit AbstractLayerPropertyEditor(LayerScene *layerScene, QWidget *parent = 0);
	~AbstractLayerPropertyEditor();
	
	LayerScene *layerScene();
	LayerConstRef current();
	
	QVariant currentProperty(int role) const;
	void setCurrentProperty(const QVariant &data, int role, const QString &description);
	
	template <class T>
	Ref<const T> currentWithType()
	{
		return std::dynamic_pointer_cast<const T>(current());
	}
	
signals:
	
public slots:
	
protected:
	
	/**
	 * This function is called when the current layer is changed.
	 * The default implementation just calls updateForCurrentPropertyChange.
	 * @param current
	 */
	virtual void updateForCurrentChange(const LayerConstRef &current);
	
	/**
	 * This function is called when a property of the current layer is changed.
	 * The default implementation of updateForCurrentChange also calls this function.
	 */
	virtual void updateForCurrentPropertyChange() = 0;
	
private slots:
	
	void onCurrentChanged(const LayerConstRef &current);
	void onCurrentPropertyChanged();
	
private:
	
	struct Data;
	Data *d;
	
};

} // namespace PaintField
