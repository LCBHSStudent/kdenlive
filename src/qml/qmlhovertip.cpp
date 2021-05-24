#include "qmlhovertip.h"

#include <QQuickWidget>
#include <QQmlContext>
#include <QQmlEngine>
#include <mutex>

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
            
            s_toolTip->setResizeMode(QQuickWidget::SizeViewToRootObject);
            s_toolTip->setWindowFlag(Qt::WindowStaysOnTopHint, true);
            s_toolTip->setWindowFlag(Qt::FramelessWindowHint, true);
            
            s_toolTip->rootContext()->setContextProperty("tooltip", s_toolTip);
            
            QStringList path;
            
            qDebug() << s_toolTip->engine()->importPathList();
            
            
            path << "modules" << "DFW" << "Components" << "HoverTipBase.qml";
            
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
    m_hideTimer->start();
}

void QmlHoverTip::onHideTriggered() {
    s_toolTip->setSource(QUrl("qrc:/qml/EmptyItem.qml"));
    s_toolTip->hide();
}
