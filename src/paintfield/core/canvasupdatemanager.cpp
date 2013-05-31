#include <QTimer>
#include <QThread>
#include <QMetaMethod>
#include <atomic>

#include "canvasupdatemanager.h"

namespace PaintField {

CanvasUpdateTimer::CanvasUpdateTimer(CanvasUpdateManager *manager, QObject *parent) :
    QTimer(parent),
    _manager(manager)
{
	connect(this, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

void CanvasUpdateTimer::onTimeout()
{
	if (_manager->isEnabled() && !_manager->isUpdating())
		emit updateRequested();
}

struct CanvasUpdateManager::Data
{
	std::atomic<bool> updating, enabled;
	QPointSet tiles;
	QTimer *timer = nullptr;
	QThread *thread = nullptr;
};

CanvasUpdateManager::CanvasUpdateManager(QObject *parent) :
    QObject(parent),
    d(new Data)
{
	d->updating = false;
	d->enabled = false;
	
	auto thread = new QThread(this);
	
	auto timer = new CanvasUpdateTimer(this);
	timer->setInterval(16);
	timer->moveToThread(thread);
	
	connect(timer, SIGNAL(updateRequested()), this, SLOT(update()));
	connect(this, SIGNAL(startTimerRequested()), timer, SLOT(start()));
	connect(this, SIGNAL(stopTimerRequested()), timer, SLOT(stop()));
	
	d->thread = thread;
	d->timer = timer;
	
	thread->start();
}

CanvasUpdateManager::~CanvasUpdateManager()
{
	delete d->timer;
	d->thread->quit();
	d->thread->wait();
	delete d;
}

bool CanvasUpdateManager::isEnabled() const
{
	return d->enabled;
}

bool CanvasUpdateManager::isUpdating() const
{
	return d->updating;
}

void CanvasUpdateManager::setEnabled(bool enabled)
{
	if (d->enabled && !enabled)
	{
		stop();
	}
	else if (!d->enabled && enabled)
	{
		start();
	}
}

void CanvasUpdateManager::addTiles(const QPointSet &tiles)
{
	d->tiles |= tiles;
}

void CanvasUpdateManager::update()
{
	if (!d->enabled)
		return;
	
	if (!d->tiles.isEmpty())
	{
		d->updating = true;
		emit updateTilesRequested(d->tiles);
		d->tiles.clear();
		d->updating = false;
	}
}

void CanvasUpdateManager::start()
{
	d->tiles.clear();
	d->enabled = true;
	emit startTimerRequested();
}

void CanvasUpdateManager::stop()
{
	emit stopTimerRequested();
	update();
	d->enabled = false;
}

} // namespace PaintField
