#ifndef PROJECTMONITORFRAME_H
#define PROJECTMONITORFRAME_H

#include <QFrame>

class Monitor;
class RSToolBar;

class ProjectMonitorFrame: public QFrame {
	Q_OBJECT
public:
    explicit ProjectMonitorFrame(Monitor* monitor, QWidget* parent = nullptr);
	
protected:
	void resizeEvent(QResizeEvent*) override;
	void paintEvent(QPaintEvent*) override;
	
public slots:
	
private:
	// weak_ptr
	Monitor*
        m_projectMonitor = nullptr;
	RSToolBar*
        m_rsToolBar = nullptr;
};

#endif // PROJECTMONITORFRAME_H
