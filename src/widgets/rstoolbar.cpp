#include "rstoolbar.h"

#include <config-kdenlive.h>
#include <KLocalizedContext>
#include <KDeclarative/KDeclarative>

#include <QQmlContext>
#include <QQuickItem>

#include "assetcontroller.hpp"

#ifdef DEBUG_BUILD
    const QUrl qmlPath("file:///" SMARTIP_PWD "/src/widgets/RSToolBar.qml");
#else
    const QUrl qmlPath("qrc:/qml/RSToolBar.qml");
#endif

RSToolBar::RSToolBar(QWidget* parent)
    : QQuickWidget(parent)
{
    // necessary step to use kde framework context variables/functions
    KDeclarative::KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(engine());
    kdeclarative.setupEngine(engine());
    engine()->rootContext()->setContextObject(new KLocalizedContext(this));
    rootContext()->setContextProperty("assetCtrl", AssetController::instance().get());
    rootContext()->setContextProperty("rstoolbar", this);
    
    setMinimumHeight(__qmlTabItemHeight * 8 + __rsToolBar_topMargin + __rsToolBar_bottomMargin);
    resize(__qmlTabBarWidth, parentWidget()->height() - __rsToolBar_topMargin - __rsToolBar_bottomMargin);
    
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setAttribute(Qt::WA_TranslucentBackground);
    
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    
    setClearColor(Qt::transparent);
    
    setSource(qmlPath);

    if (rootObject()) {
        rootObject()->setProperty("tabBarWidth", __qmlTabBarWidth);
        rootObject()->setProperty("tabItemHeight", __qmlTabItemHeight); 
    }   
}

void RSToolBar::mousePressEvent(QMouseEvent* e) {
    this->raise();
    QQuickWidget::mousePressEvent(e);
}

void RSToolBar::mouseMoveEvent(QMouseEvent* e) {
    QQuickWidget::mouseMoveEvent(e);
}

void RSToolBar::mouseReleaseEvent(QMouseEvent* e) {
    QQuickWidget::mouseReleaseEvent(e);
}

void RSToolBar::keyReleaseEvent(QKeyEvent* ke) {
    QQuickWidget::keyReleaseEvent(ke);
    if (!ke->isAccepted()) {
// handle hot reload
#ifdef DEBUG_BUILD
        if (ke->key() == Qt::Key_Q && ke->modifiers() == Qt::AltModifier) {
            auto rootObj = rootObject();
            
            if (rootObj == nullptr) {
                return;
            }
            
            setSource(QUrl());
            engine()->clearComponentCache();
            setSource(qmlPath);
            
            rootObj = rootObject();
            rootObj->setProperty("tabBarWidth", __qmlTabBarWidth);
            rootObj->setProperty("tabItemHeight", __qmlTabItemHeight);     
        }
#endif
        ke->accept();
    }
}


void RSToolBar::slotSwitchPanelState(bool isOpen) {
    constexpr int deltaX = __rsToolBar_fixedWidth - __qmlTabBarWidth;
    if (isOpen) {
        resize(__rsToolBar_fixedWidth, height());
    } else {
        resize(__qmlTabBarWidth, height());
    }
    move(x() + (isOpen? -deltaX: deltaX), y());
}
