#include "clipmonitorframe.h"

#include "monitor/monitor.h"
#include "monitor/glwidget.h"
#include "monitor/monitorproxy.h"

constexpr auto cLRBorderWidth = 20;
constexpr auto cHeaderHeight = 50;

ClipMonitorFrame::ClipMonitorFrame(Monitor* clipMonitor, QWidget* parent)
    : FramelessMovableWidget(parent)
    , m_clipMonitor(clipMonitor)
{
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    
    m_clipMonitor->setParent(this);

    setStyleSheet(R"(
        ClipMonitorFrame {
            background-color: #2D2C39;
            border: 1px solid #5E5F72;
        }
    )");
    connect(m_clipMonitor->m_glMonitor->getControllerProxy(), &MonitorProxy::clipNameChanged, [this] {
        setTitle(m_clipMonitor->m_glMonitor->getControllerProxy()->property("clipName").toString());
    });
    
    resize(900, 720);
    
    hide();
    
    setHeaderHeight(cHeaderHeight);
}

void ClipMonitorFrame::resizeEvent(QResizeEvent* e) {
    FramelessMovableWidget::resizeEvent(e);
    m_clipMonitor->setGeometry(cLRBorderWidth, headerHeight(), width() - 2*cLRBorderWidth, height() - 1 - headerHeight());
}
