#include "menubareventpasser.h"

#include <QApplication>
#include <QMouseEvent>

#include "macros.hpp"

MenuBarEventPasser::MenuBarEventPasser(QWidget* parent)
    : QWidget(parent)
{
    raise();
}

bool MenuBarEventPasser::event(QEvent* e) {
    switch(e->type()) {
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::HoverMove:
    case QEvent::MouseButtonDblClick: {
        if (!e->isAccepted()) {
            auto me = static_cast<QMouseEvent*>(e);
            me->setLocalPos(me->pos() + QPoint(x(), 0));
            qApp->sendEvent(parent(), me);
        }
    } break;
    default:
        break;
    }
    
    return QWidget::event(e);
}
