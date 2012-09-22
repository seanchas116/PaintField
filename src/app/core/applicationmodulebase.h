#ifndef FSAPPLICATIONMODULEBASE_H
#define FSAPPLICATIONMODULEBASE_H

#include <QObject>
#include <QVector>

namespace PaintField {

class ApplicationModuleBase : public QObject
{
	Q_OBJECT
public:
	
	struct MetaInfo
	{
		MetaInfo() {}
		MetaInfo(const QString &title, const QVector<int> &version = QVector<int>()) :
		    title(title),
		    version(version)
		{}
		
		QString title;
		QVector<int> version;
	};
	
	explicit ApplicationModuleBase(QObject *parent = 0);
	
	virtual void initialize() = 0;
	
	MetaInfo metaInfo() const { return _metaInfo; }
	QString title() const { return _metaInfo.title; }
	QVector<int> version() const { return _metaInfo.version; }
	
	QList<MetaInfo> dependencies() const { return _dependencies; }
	
protected:
	
	void setMetaInfo(const MetaInfo &metaInfo) { _metaInfo = metaInfo; }
	void setTitle(const QString &title) { _metaInfo.title = title; }
	void setVersion(const QVector<int> &version) { _metaInfo.version = version; }
	
	void setDependencies(const QList<MetaInfo> &dependencies) { _dependencies = dependencies; }
	void addDependency(const MetaInfo &dependency) { _dependencies << dependency; }
	
signals:
	
public slots:
	
private:
	
	MetaInfo _metaInfo;
	QList<MetaInfo> _dependencies;
};

}

#endif // FSAPPLICATIONMODULEBASE_H
