#include "projectmonitorframe.h"

#include "monitor/monitor.h"
#include "assets/assetpanel.hpp"

#include "macros.hpp"

#include <QPainter>
#include <QVBoxLayout>
#include <QQuickWidget>
#include <QQuickItem>
#include <mutex>

constexpr auto __assetPanelTopMargin = 20;
constexpr auto __assetPanelBottomMargin = 20;

ProjectMonitorFrame::ProjectMonitorFrame(Monitor* monitor, QWidget* parent)
    : QFrame(parent)
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

void ProjectMonitorFrame::resizeEvent(QResizeEvent* e) { 
    if (s_assetPanel) {
        auto w = s_assetPanel->width();
        s_assetPanel->setGeometry(
            width() - w, __assetPanelTopMargin, 
            w, height() - __assetPanelTopMargin - __assetPanelBottomMargin
        );
    }
    
    QFrame::resizeEvent(e);
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

void ProjectMonitorFrame::setAssetPanel(AssetPanel* rPanel) {
    rPanel->setParent(this);
    rPanel->resize(400, 500);
    rPanel->show();
    
    s_assetPanel = rPanel;
    
    QResizeEvent fake(size(), size());
    resizeEvent(&fake);
}
