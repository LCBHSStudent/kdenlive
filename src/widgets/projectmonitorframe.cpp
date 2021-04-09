#include "projectmonitorframe.h"

#include "monitor/monitor.h"
#include "macros.hpp"

#include <QPainter>

ProjectMonitorFrame::ProjectMonitorFrame(Monitor* monitor, QWidget* parent)
    : QWidget(parent)
{
    m_projectMonitor = monitor;
    m_projectMonitor->setParent(this);
}

void ProjectMonitorFrame::resizeEvent(QResizeEvent*) {
    if (m_projectMonitor != nullptr) {
        m_projectMonitor->resize(width(), height());
    }
    
    LOG_DEBUG() << size();
}

void ProjectMonitorFrame::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::RenderHint::Antialiasing);
}
