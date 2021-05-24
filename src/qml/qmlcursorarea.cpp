#include "qmlcursorarea.h"

#include <QGuiApplication>
#include <QCursor>
#include <QPainter>
#include <QQuickWindow>

QmlCursorArea::QmlCursorArea(QQuickItem *parent) 
    : QQuickItem(parent)
    , m_hotX(0)
    , m_hotY(0)
{
    setAcceptHoverEvents(true);
}

QmlCursorArea::~QmlCursorArea() {
    if (m_cursor != DefaultCursor) {
        QGuiApplication::setOverrideCursor(QCursor());
    }
}

void QmlCursorArea::hoverEnterEvent(QHoverEvent* e) {
    e->ignore();
    
    setCursor(m_cursor);
}

void QmlCursorArea::hoverLeaveEvent(QHoverEvent* e) {
    e->ignore();

    setCursor(DefaultCursor);
}

void QmlCursorArea::setCursor(Cursor cursor) {
    switch (cursor) {
        case DefaultCursor:
            QGuiApplication::setOverrideCursor(QCursor());
            break;
        case ArrowCursor:
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
            break;
        case IBeamCursor:
            QGuiApplication::setOverrideCursor(Qt::IBeamCursor);
            break;
        case SizeHorCursor:
            QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
            break;
        case SizeVerCursor:
            QGuiApplication::setOverrideCursor(Qt::SizeVerCursor);
            break;
        case SizeAllCursor:
            QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
            break;
        case SplitHCursor:
            QGuiApplication::setOverrideCursor(Qt::SplitHCursor);
            break;
        case SplitVCursor:
            QGuiApplication::setOverrideCursor(Qt::SplitVCursor);
            break;
        case WaitCursor:
            QGuiApplication::setOverrideCursor(Qt::WaitCursor);
            break;
        case PointingHandCursor:
            QGuiApplication::setOverrideCursor(Qt::PointingHandCursor);
            break;
        default:
            return;
    }
}


QCursor QmlCursorArea::createCustomCursor() {
    QCursor result;

    if (m_delegate) {
        QQuickWindow *window = new QQuickWindow();
        QQuickItem *item = qobject_cast<QQuickItem *>(m_delegate->create());

        window->resize(item->width(), item->height());
        item->setParent(window);
        item->setParentItem(window->contentItem());

        qDebug() << window->width() << window->height();

        QImage image = window->grabWindow();
        QPixmap pixmap = QPixmap::fromImage(image);

        result = QCursor(pixmap, m_hotX, m_hotY);

        delete window;
    }
    else {
        result = QQuickItem::cursor();
    }
    
    return result;
}

QQmlComponent *QmlCursorArea::delegate() const {
    return m_delegate;
}

void QmlCursorArea::setDelegate(QQmlComponent *delegate) {
    if (m_delegate != delegate) {
        m_delegate = delegate;
        emit delegateChanged();
    }
}
