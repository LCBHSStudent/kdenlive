#include "framelessmovablewidget.h"

#include "core.h"
#include "macros.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QGraphicsOpacityEffect>

FramelessMovableWidget::FramelessMovableWidget(QWidget* parent)
    : QFrame(parent)
{
    m_closeBtn = new QPushButton(this);
    m_closeBtn->setStyleSheet("QPushButton { background: transparent; }");
    m_closeBtn->installEventFilter(this);
    m_closeBtn->setGraphicsEffect(new QGraphicsOpacityEffect(m_closeBtn));
    static_cast<QGraphicsOpacityEffect*>(m_closeBtn->graphicsEffect())->setOpacity(1.0f);
    connect(m_closeBtn, &QPushButton::clicked, this, &FramelessMovableWidget::close);
    
    m_closeBtn->setFixedSize(14, 14);
    m_closeBtn->setIcon(QIcon(":/classic/controllers/window_close.png"));
    m_closeBtn->setIconSize(m_closeBtn->size());
}

bool FramelessMovableWidget::eventFilter(QObject* obj, QEvent* e) {
    if (obj == m_closeBtn) {
        if (e->type() == QEvent::MouseButtonPress) {
            static_cast<QGraphicsOpacityEffect*>(m_closeBtn->graphicsEffect())->setOpacity(0.6f);
        } else if (e->type() == QEvent::MouseButtonRelease) {
            static_cast<QGraphicsOpacityEffect*>(m_closeBtn->graphicsEffect())->setOpacity(1.0f);
        }
    }
    return false;
}

void FramelessMovableWidget::mousePressEvent(QMouseEvent* e) {
    if (m_closeBtn->geometry().contains(e->pos()) || e->pos().y() >= m_headerHeight) {
        m_pressStarted = false;
        return;
    }
    m_pressStarted = true;
    
    setCursor(Qt::SizeAllCursor);
    m_pressedPoint = e->globalPos();
    m_moveStartPoint = this->pos();
}

void FramelessMovableWidget::mouseMoveEvent(QMouseEvent* e) {
    if (m_pressStarted) {
        auto targetPos = e->globalPos() - m_pressedPoint + m_moveStartPoint;
        auto windowSize = reinterpret_cast<QWidget*>(pCore->window())->size();
        auto mx = qBound(0, targetPos.x(), windowSize.width() - width());
        auto my = qBound(0, targetPos.y(), windowSize.height() - height());
        move(mx, my);                
    }
}

void FramelessMovableWidget::mouseReleaseEvent(QMouseEvent*) {
    m_pressStarted = false;
    setCursor(Qt::ArrowCursor);
}

void FramelessMovableWidget::resizeEvent(QResizeEvent* e) {
    QFrame::resizeEvent(e);
    m_closeBtn->move(width() - 32, 15);
}

void FramelessMovableWidget::paintEvent(QPaintEvent *) {
    QFont YaHei("Microsoft YaHei");
    YaHei.setPixelSize(14);
    
    QPainter p(this);
    p.setFont(YaHei);
    p.setBrush(Qt::white);
    
    p.drawText(0, 0, this->width(), headerHeight(), Qt::AlignCenter, m_title);    
}

int FramelessMovableWidget::headerHeight() const {
    return m_headerHeight;
}

void FramelessMovableWidget::setHeaderHeight(int headerHeight) {
    m_headerHeight = headerHeight;
}

QString FramelessMovableWidget::title() const {
    return m_title;
}

void FramelessMovableWidget::setTitle(const QString& title) {
    m_title = title;
    update();
}
