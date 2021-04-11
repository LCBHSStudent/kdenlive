#include "rstoolbar.h"

RSToolBar::RSToolBar(QWidget* parent)
    : QQuickWidget(parent)
{
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setAttribute(Qt::WA_TranslucentBackground);
    
    setResizeMode(QQuickWidget::SizeRootObjectToView);
    
    setWindowFlag(Qt::WindowStaysOnTopHint);
    
    setClearColor(Qt::transparent);
    
    setSource(QUrl("qrc:/qml/RSToolBar.qml"));
}

void RSToolBar::mousePressEvent(QMouseEvent *) {
    
}

void RSToolBar::mouseMoveEvent(QMouseEvent *) {
    
}

void RSToolBar::mouseReleaseEvent(QMouseEvent *) {
    
}
