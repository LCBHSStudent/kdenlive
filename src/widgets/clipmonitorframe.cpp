#include "clipmonitorframe.h"

#include "monitor/monitor.h"

ClipMonitorFrame::ClipMonitorFrame(Monitor* clipMonitor, QWidget* parent)
    : FramelessMovableWidget(parent)
    , m_clipMonitor(clipMonitor)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    
    m_clipMonitor->setParent(this);
    
    resize(900, 720);
    
    hide();
}

void ClipMonitorFrame::resizeEvent(QResizeEvent* e) {
    FramelessMovableWidget::resizeEvent(e);
    m_clipMonitor->setGeometry(0, 0, width(), height());
}
