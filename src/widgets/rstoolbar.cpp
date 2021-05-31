#include "rstoolbar.h"

#include <KLocalizedContext>
#include <KDeclarative/KDeclarative>

#include <QQmlContext>
#include <QQuickItem>

#include "assetcontroller.hpp"

#ifdef DEBUG_BUILD
    const QUrl qmlPath("file:///A:/CraftRoot/build/kde/kdemultimedia/kdenlive/work/kde_based_editor/src/widgets/RSToolBar.qml");
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

    setAttribute(Qt::WA_AlwaysStackOnTop);
    setAttribute(Qt::WA_TranslucentBackground);
    
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    
    setClearColor(Qt::transparent);
    
    setSource(qmlPath);
}

void RSToolBar::mousePressEvent(QMouseEvent* e) {
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
            int tabBarWidth = 0;
            int tabItemHeight = 0;
            
            if (rootObj != nullptr) {
                tabBarWidth = rootObj->property("tabBarWidth").toInt();
                tabItemHeight = rootObj->property("tabItemHeight").toInt();           
            }
            setSource(QUrl());
            engine()->clearComponentCache();
            setSource(qmlPath);
            
            rootObj = rootObject();
            rootObj->setProperty("tabBarWidth", tabBarWidth);
            rootObj->setProperty("tabItemHeight", tabItemHeight);     
        }
#endif
        ke->accept();
    }
}
