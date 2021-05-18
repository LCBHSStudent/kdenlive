#ifndef CLIPMONITORFRAME_H
#define CLIPMONITORFRAME_H

#include "framelessmovablewidget.h"

class Monitor;

class ClipMonitorFrame: public FramelessMovableWidget {
	Q_OBJECT
public:
    explicit ClipMonitorFrame(Monitor* clipMonitor, QWidget* parent = nullptr);
	~ClipMonitorFrame() override = default;
	
protected:
	void resizeEvent(QResizeEvent*) override;
	void hideEvent(QHideEvent*) override;
private:
	Monitor* const m_clipMonitor;
	
};

#endif // CLIPMONITORFRAME_H
