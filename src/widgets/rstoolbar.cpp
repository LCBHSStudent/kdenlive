#include "rstoolbar.h"

#include <KLocalizedContext>
#include <KDeclarative/KDeclarative>

#include <QQmlContext>

RSToolBar::RSToolBar(QWidget* parent)
    : QQuickWidget(parent)
{
    // necessary step to use kde framework context variables/functions
    KDeclarative::KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(engine());
    kdeclarative.setupEngine(engine());
    engine()->rootContext()->setContextObject(new KLocalizedContext(this));
    
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setAttribute(Qt::WA_TranslucentBackground);
    
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    
    setWindowFlag(Qt::WindowStaysOnTopHint);
    
    setClearColor(Qt::transparent);
    
    setSource(QUrl("qrc:/qml/RSToolBar.qml"));
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
