#include "qmlhovertip.h"

#include <QQuickWidget>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDir>
#include <mutex>

#include "core.h"

QmlHoverTip::QmlHoverTip(QQuickItem* parent)
    : QQuickItem(parent)
    , m_hideTimer(new QTimer(this))
    , m_displayTimer(new QTimer(this))
{
    setAcceptHoverEvents(true);
    
    m_hideTimer->setInterval(m_timeout);
    m_hideTimer->setSingleShot(true);
    m_displayTimer->setInterval(m_delay);
    m_displayTimer->setSingleShot(true);
    
    connect(m_hideTimer, SIGNAL(timeout()), this, SLOT(onHideTriggered()));
    connect(m_displayTimer, SIGNAL(timeout()), this, SLOT(onDisplayTriggered()));
    
    static std::once_flag toolTipInit;
    std::call_once(toolTipInit, [this] {
        QTimer::singleShot(1, this, [] {
            s_toolTip = new QQuickWidget(nullptr); // default constructor
            s_toolTip->setAttribute(Qt::WA_AlwaysStackOnTop);
            s_toolTip->setAttribute(Qt::WA_TranslucentBackground);
            s_toolTip->setClearColor(Qt::transparent);
            
            s_toolTip->setResizeMode(QQuickWidget::SizeViewToRootObject);
            s_toolTip->setWindowFlag(Qt::WindowStaysOnTopHint, true);
            s_toolTip->setWindowFlag(Qt::FramelessWindowHint, true);
            
            s_toolTip->rootContext()->setContextProperty("tooltip", s_toolTip);
            
            foreach (auto& importList, s_toolTip->engine()->importPathList()) {
                QDir dir(importList);
                if (dir.cd("DFW")) {
                    if (dir.cd("Components")) {
                        s_toolTip->setSource(QUrl::fromLocalFile(dir.filePath("HoverTipBase.qml")));
                    } else continue;
                } else continue;
            }
            
            s_toolTipQmlUrl = s_toolTip->source();
        });
    });
}

QmlHoverTip::~QmlHoverTip() {
    if (!s_toolTip->isHidden()) {
        onHideTriggered();
    }
    m_hideTimer->stop();
    m_displayTimer->stop();
    
    m_hideTimer->deleteLater();
    m_displayTimer->deleteLater();
}

void QmlHoverTip::hoverEnterEvent(QHoverEvent* e) {
    auto mousearea = property("fallthrough").value<QQuickItem*>();
    if (mousearea) {
        qApp->sendEvent(mousearea, e);        
    }
    
    if (!m_enabled) return;
    
    m_displayTimer->start();
    
    return;
}

void QmlHoverTip::hoverLeaveEvent(QHoverEvent* e) {
    auto mousearea = property("fallthrough").value<QQuickItem*>();
    if (mousearea) {
        qApp->sendEvent(mousearea, e);        
    }
    
    if (!s_toolTip->isHidden()) {
        m_hideTimer->stop();
        onHideTriggered();
    } else {
        m_displayTimer->stop();
    }
    
    return;
}


void QmlHoverTip::onDisplayTriggered() {
    if (!m_enabled) return;
    
    s_toolTip->setSource(s_toolTipQmlUrl);
    s_toolTip->rootObject()->setProperty("text", m_text);
    auto toolTipPos = [](int width, int height) {
        auto&& scrGeo = reinterpret_cast<QWidget*>(pCore->window())->geometry();
        auto&& curPos = QCursor::pos();
        
        // 15: horizontal margin; 4: vertical margin
        auto targetX = curPos.x() + 15;
        auto targetY = curPos.y() + 4;
        
        if (targetX + width >= scrGeo.x() + scrGeo.width()) {
            targetX = scrGeo.x() + scrGeo.width() - width - 2;
        }
        if (targetY + height >= scrGeo.x() + scrGeo.width()) {
            targetY = scrGeo.y() + scrGeo.height() - height - 2;
        }
        
        return QPoint(targetX, targetY);
    };
    s_toolTip->adjustSize();
    s_toolTip->move(toolTipPos(s_toolTip->width(), s_toolTip->height()));
    m_hideTimer->start();
}

void QmlHoverTip::onHideTriggered() {
    s_toolTip->setSource(QUrl("qrc:/qml/EmptyItem.qml"));
    s_toolTip->hide();
}
