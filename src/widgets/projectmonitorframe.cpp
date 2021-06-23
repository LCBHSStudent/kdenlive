#include "projectmonitorframe.h"

#include "monitor/monitor.h"
#include "widgets/rstoolbar.h"

#include "macros.hpp"

#include <QPainter>
#include <QVBoxLayout>
#include <QQuickWidget>
#include <QQuickItem>
#include <mutex>

ProjectMonitorFrame::ProjectMonitorFrame(Monitor* monitor, QWidget* parent)
    : QFrame(parent)
    , m_rsToolBar(new RSToolBar(this))
{
    auto layout = new QVBoxLayout(this);
    
    m_projectMonitor = monitor;
    m_projectMonitor->setParent(this);
    m_projectMonitor->show();
    
    setStyleSheet("ProjectMonitorFrame { background-color: #292833; }");
    
    layout->addWidget(m_projectMonitor);
    layout->setSpacing(0);
    
    setLayout(layout);
} 

void ProjectMonitorFrame::resizeEvent(QResizeEvent*) {
    m_rsToolBar->resize(m_rsToolBar->width(), height() - __rsToolBar_topMargin - __rsToolBar_bottomMargin);
    m_rsToolBar->move(width() - m_rsToolBar->width(), __rsToolBar_topMargin);
    
    layout()->setContentsMargins(0.191 * width(), __rsToolBar_topMargin - 1, 0.265 * width(), 0.0775 * height());
}

void ProjectMonitorFrame::paintEvent(QPaintEvent*) {
    static QLinearGradient shadowGrad(0, 0, 0, 5);
    static std::once_flag initGrad;
    std::call_once(initGrad, [] {
        shadowGrad.setColorAt(0, QColor(0, 0, 0, 85));
        shadowGrad.setColorAt(1, Qt::transparent);   
    });
    
    QPainter painter(this);
    painter.setBrush(shadowGrad);
    painter.setPen(Qt::transparent);
    painter.drawRect(0, 0, width(), 5);
}
