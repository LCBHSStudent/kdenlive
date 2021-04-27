#ifndef CLIPMONITORFRAME_H
#define CLIPMONITORFRAME_H

#include <QFrame>

class Monitor;

class ClipMonitorFrame: public QFrame {
	Q_OBJECT
public:
    explicit ClipMonitorFrame(Monitor* clipMonitor, QWidget* parent = nullptr);
	~ClipMonitorFrame() override = default;
	
protected:
	void resizeEvent(QResizeEvent*) override;
	
private:
	Monitor* const m_clipMonitor;
	
};

#endif // CLIPMONITORFRAME_H
