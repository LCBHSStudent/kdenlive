#include "clipmonitorframe.h"

#include "monitor/monitor.h"

ClipMonitorFrame::ClipMonitorFrame(Monitor* clipMonitor, QWidget* parent)
    : QFrame(parent)
    , m_clipMonitor(clipMonitor)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowModality(Qt::NonModal);
    
    m_clipMonitor->setParent(this);
    
    resize(900, 720);
    
    hide();
}

void ClipMonitorFrame::resizeEvent(QResizeEvent*) {
    m_clipMonitor->setGeometry(0, 0, width(), height());
}
