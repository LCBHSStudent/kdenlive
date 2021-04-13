#include "projectmonitorframe.h"

#include "monitor/monitor.h"
#include "widgets/rstoolbar.h"
#include "macros.hpp"

#include <QPainter>
#include <QVBoxLayout>
#include <QQuickWidget>
#include <QQuickItem>

constexpr int __rsToolBar_fixedWidth    = 560;
constexpr int __rsToolBar_topMargin     = 19;
constexpr int __rsToolBar_bottomMargin  = 5;
constexpr int __qmlTabItemHeight        = 46;
constexpr int __qmlTabBarWidth          = 106;

ProjectMonitorFrame::ProjectMonitorFrame(Monitor* monitor, QWidget* parent)
    : QFrame(parent)
    , m_rsToolBar(new RSToolBar(this))
{
    auto layout = new QVBoxLayout(this);
    
    m_projectMonitor = monitor;
    m_projectMonitor->setParent(this);
    
    setStyleSheet("ProjectMonitorFrame { background-color: #292833; }");
    
    layout->addWidget(m_projectMonitor);
    layout->setContentsMargins(0, 18, 0, 51);
    
    setLayout(layout);
    m_projectMonitor->hide();
    m_rsToolBar->rootObject()->setProperty("tabBarWidth", __qmlTabBarWidth);
    m_rsToolBar->rootObject()->setProperty("tabItemHeight", __qmlTabItemHeight);
    
    setMinimumHeight(__qmlTabItemHeight * 8 + __rsToolBar_topMargin + __rsToolBar_bottomMargin);
} 

void ProjectMonitorFrame::resizeEvent(QResizeEvent*) {
    m_rsToolBar->resize(__rsToolBar_fixedWidth, height() - __rsToolBar_topMargin - __rsToolBar_bottomMargin);
    m_rsToolBar->move(width() - __rsToolBar_fixedWidth, __rsToolBar_topMargin);
}

void ProjectMonitorFrame::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::RenderHint::Antialiasing);
}
