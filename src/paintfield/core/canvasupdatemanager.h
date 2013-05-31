#pragma once

#include <QObject>
#include <QTimer>
#include <Malachite/Surface>

namespace PaintField {

class CanvasUpdateManager;

class CanvasUpdateTimer : public QTimer
{
	Q_OBJECT
	
public:
	
	CanvasUpdateTimer(CanvasUpdateManager *manager, QObject *parent = 0);
	
signals:
	
	void updateRequested();
	
private slots:
	
	void onTimeout();
	
private:
	
	CanvasUpdateManager *_manager = nullptr;
};

class CanvasUpdateManager : public QObject
{
	Q_OBJECT
public:
	explicit CanvasUpdateManager(QObject *parent = 0);
	~CanvasUpdateManager();
	
	bool isEnabled() const;
	bool isUpdating() const;
	
public slots:
	
	void setEnabled(bool enabled);
	void addTiles(const QPointSet &tiles);
	
signals:
	
	void updateTilesRequested(const QPointSet &tiles);
	void startTimerRequested();
	void stopTimerRequested();
	
private slots:
	
	void update();
	
private:
	
	void start();
	void stop();
	
	struct Data;
	Data *d;
};

} // namespace PaintField
