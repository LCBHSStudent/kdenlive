#ifndef PROJECTMONITORFRAME_H
#define PROJECTMONITORFRAME_H

#include <QWidget>

class Monitor;

class ProjectMonitorFrame: public QWidget {
	Q_OBJECT
public:
    explicit ProjectMonitorFrame(Monitor* monitor, QWidget* parent = nullptr);
	
protected:
	void resizeEvent(QResizeEvent*) override;
	void paintEvent(QPaintEvent*) override;
	
public slots:
	
private:
	// weak_ptr
	Monitor* m_projectMonitor = nullptr;
	
};

#endif // PROJECTMONITORFRAME_H
