#include "framelesswindowhelper.h"

#include <QRect>
#include <QRubberBand>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

#include "macros.hpp"
#include <QDebug>

/*****
 * FramelessHelperPrivate
 * 存储界面对应的数据集合，以及是否可移动、可缩放属性
*****/
class FramelessHelperPrivate {
public:
    QHash<QWidget*, WidgetData*> m_widgetDataHash;
    bool m_bWidgetMovable        : true;
    bool m_bWidgetResizable      : true;
    bool m_bRubberBandOnResize   : true;
    bool m_bRubberBandOnMove     : true;
};
 
/***** CursorPosCalculator *****/
CursorPosCalculator::CursorPosCalculator() {
    reset();
}
 
void CursorPosCalculator::reset() {
    m_bOnEdges              = false;
    m_bOnLeftEdge           = false;
    m_bOnRightEdge          = false;
    m_bOnTopEdge            = false;
    m_bOnBottomEdge         = false;
    m_bOnTopLeftEdge        = false;
    m_bOnBottomLeftEdge     = false;
    m_bOnTopRightEdge       = false;
    m_bOnBottomRightEdge    = false;
}
 
void CursorPosCalculator::recalculate(const QPoint &gMousePos, const QRect &frameRect) {
    int globalMouseX = gMousePos.x();
    int globalMouseY = gMousePos.y();
    
    int frameX = frameRect.x();
    int frameY = frameRect.y();
    
    int frameWidth = frameRect.width();
    int frameHeight = frameRect.height();
    
    void(); // 勿动
    
    m_bOnLeftEdge = (globalMouseX >= frameX &&
                  globalMouseX <= frameX + m_nBorderWidth ) && m_bLeftEnabled;


    m_bOnRightEdge = (globalMouseX >= frameX + frameWidth - m_nBorderWidth &&
                   globalMouseX <= frameX + frameWidth) && m_bRightEnabled;

    m_bOnTopEdge = (globalMouseY >= frameY &&
                 globalMouseY <= frameY + m_nBorderWidth ) && m_bTopEnabled;

    m_bOnBottomEdge = (globalMouseY >= frameY + frameHeight - m_nBorderWidth &&
                    globalMouseY <= frameY + frameHeight) && m_bBottomEnabled;
    
    m_bOnTopLeftEdge = m_bOnTopEdge && m_bOnLeftEdge;
    m_bOnBottomLeftEdge = m_bOnBottomEdge && m_bOnLeftEdge;
    m_bOnTopRightEdge = m_bOnTopEdge && m_bOnRightEdge;
    m_bOnBottomRightEdge = m_bOnBottomEdge && m_bOnRightEdge;

    m_bOnEdges = m_bOnLeftEdge || m_bOnRightEdge || m_bOnTopEdge || m_bOnBottomEdge;  
}
 
 
/***** WidgetData *****/
WidgetData::WidgetData(FramelessHelperPrivate *_d, QWidget *pTopLevelWidget, QObject *parent) 
    : QObject(parent) 
{
    d = _d;
    m_pWidget = pTopLevelWidget;
    m_bLeftButtonPressed = false;
    m_bCursorShapeChanged = false;
    m_bLeftButtonTitlePressed = false;
    m_bMax = false;
    m_rect = m_pWidget->geometry();
    m_pRubberBand = NULL;
    
    m_windowFlags = m_pWidget->windowFlags();
    m_pWidget->setMouseTracking(true);
    m_pWidget->setAttribute(Qt::WA_Hover, true);
    
    updateRubberBandStatus();
}
 
WidgetData::~WidgetData() {
    m_pWidget->setMouseTracking(false);
    m_pWidget->setWindowFlags(m_windowFlags);
    m_pWidget->setAttribute(Qt::WA_Hover, false);
    
    delete m_pRubberBand;
    m_pRubberBand = NULL;
}
 
QWidget* WidgetData::widget() {
    return m_pWidget;
}
 
void WidgetData::handleWidgetEvent(QEvent *event) {
    switch (event->type()) {
    default:
        break;
    case QEvent::MouseButtonPress:
        handleMousePressEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseButtonRelease:
        handleMouseReleaseEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseMove:
        handleMouseMoveEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseButtonDblClick:
        handleMouseDblClickEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::Leave:
        handleLeaveEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::HoverMove:
        handleHoverMoveEvent(static_cast<QHoverEvent*>(event));
        break;
    }
}
 
void WidgetData::updateRubberBandStatus() {
    if (d->m_bRubberBandOnMove || d->m_bRubberBandOnResize) {
        if (NULL == m_pRubberBand)
            m_pRubberBand = new QRubberBand(QRubberBand::Rectangle);
    }
    else {
        delete m_pRubberBand;
        m_pRubberBand = NULL;
    }
}
 
void WidgetData::setMax(bool bMax) {
    if(m_bMax != bMax) {
        if(bMax) {
            m_rect = m_pWidget->geometry();
            m_pWidget->setGeometry(m_pWidget->screen()->availableGeometry());
        }
        else {
            if(m_rect.x() == 0 && m_rect.y() == 0) {
                QSize size = QApplication::desktop()->size();
                int x = (size.width() - m_rect.width())/2;
                int y = (size.height() - m_rect.height())/2;
                m_rect.moveTo(x,y);
            }
            m_pWidget->setGeometry(m_rect);
        }
        m_bMax = bMax;
        emit sigMax(m_bMax);
    }
}
 
void WidgetData::setBorderWidth(uint width) {
    m_moveMousePos.m_nBorderWidth = width;
    m_pressedMousePos.m_nBorderWidth = width;   
}

void WidgetData::setHeaderHeight(uint height) {
    m_moveMousePos.m_nTitleHeight = height;
    m_pressedMousePos.m_nTitleHeight = height;
}

void WidgetData::setDirectionEnabled(bool top, bool left, bool bottom, bool right) {
    m_moveMousePos.m_bTopEnabled = top;
    m_moveMousePos.m_bLeftEnabled = left;
    m_moveMousePos.m_bBottomEnabled = bottom;
    m_moveMousePos.m_bRightEnabled = right;
    
    m_pressedMousePos.m_bTopEnabled = top;
    m_pressedMousePos.m_bLeftEnabled = left;
    m_pressedMousePos.m_bBottomEnabled = bottom;
    m_pressedMousePos.m_bRightEnabled = right;
}

bool WidgetData::isMax() {
    return m_bMax;
}

void WidgetData::updateCursorShape(const QPoint &gMousePos) {
    if (m_pWidget->isFullScreen() || m_pWidget->isMaximized()) {
        if (m_bCursorShapeChanged) {
            m_pWidget->unsetCursor();
        }
        return;
    }
    
    m_moveMousePos.recalculate(gMousePos, m_pWidget->geometry());
    
    if(m_moveMousePos.m_bOnTopLeftEdge || m_moveMousePos.m_bOnBottomRightEdge) {
        m_pWidget->setCursor( Qt::SizeFDiagCursor );
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnTopRightEdge || m_moveMousePos.m_bOnBottomLeftEdge) {
        m_pWidget->setCursor( Qt::SizeBDiagCursor );
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnLeftEdge || m_moveMousePos.m_bOnRightEdge) {
        m_pWidget->setCursor( Qt::SizeHorCursor );
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnTopEdge || m_moveMousePos.m_bOnBottomEdge) {
        m_pWidget->setCursor( Qt::SizeVerCursor );
        m_bCursorShapeChanged = true;
    }
    else {
        if (m_bCursorShapeChanged) {
            m_pWidget->unsetCursor();
            m_bCursorShapeChanged = false;
        }
    }
}
 
void WidgetData::resizeWidget(const QPoint &gMousePos) {
    QRect origRect;
    
    if (d->m_bRubberBandOnResize)
        origRect = m_pRubberBand->geometry();
    else
        origRect = m_pWidget->geometry();

    int left = origRect.left();
    int top = origRect.top();
    int right = origRect.right();
    int bottom = origRect.bottom();
    origRect.getCoords(&left, &top, &right, &bottom);

    int minWidth = m_pWidget->minimumWidth();
    int minHeight = m_pWidget->minimumHeight();

    if (m_pressedMousePos.m_bOnTopLeftEdge)
    {
        left = gMousePos.x();
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomLeftEdge)
    {
        left = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnTopRightEdge)
    {
        right = gMousePos.x();
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomRightEdge)
    {
        right = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnLeftEdge)
    {
        left = gMousePos.x();
    }
    else if (m_pressedMousePos.m_bOnRightEdge)
    {
        right = gMousePos.x();
    }
    else if (m_pressedMousePos.m_bOnTopEdge)
    {
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomEdge)
    {
        bottom = gMousePos.y();
    }

    QRect newRect(QPoint(left, top), QPoint(right, bottom));

    if (newRect.isValid())
    {
        if (minWidth > newRect.width())
        {
            if (left != origRect.left())
                newRect.setLeft(origRect.left());
            else
                newRect.setRight(origRect.right());
        }
        if (minHeight > newRect.height())
        {
            if (top != origRect.top())
                newRect.setTop(origRect.top());
            else
                newRect.setBottom(origRect.bottom());
        }

        if (d->m_bRubberBandOnResize)
        {
            m_pRubberBand->setGeometry(newRect);
        }
        else
        {
            m_pWidget->setGeometry(newRect);
        }
    }  
}
 
void WidgetData::moveWidget(const QPoint& gMousePos) {
    if (d->m_bRubberBandOnMove) {
        m_pRubberBand->move(gMousePos - m_ptDragPos);
    }
    else {
        m_pWidget->move(gMousePos - m_ptDragPos);
    }
}
 
void WidgetData::handleMousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_bLeftButtonPressed = true;
        m_bLeftButtonTitlePressed = event->pos().y() < m_moveMousePos.m_nTitleHeight;
        
        QRect frameRect = m_pWidget->geometry();
        m_pressedMousePos.recalculate(event->globalPos(), frameRect);
        
        m_ptDragPos = event->globalPos() - frameRect.topLeft();
        
        if (m_pressedMousePos.m_bOnEdges) {
            if (d->m_bRubberBandOnResize) {
                m_pRubberBand->setGeometry(frameRect);
                m_pRubberBand->show();
            }
        }
        else if (d->m_bRubberBandOnMove) {
            m_pRubberBand->setGeometry(frameRect);
            m_pRubberBand->show();
        }
    }
}
 
void WidgetData::handleMouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_bLeftButtonPressed = false;
        if (m_bLeftButtonTitlePressed && !isMax()) {
            if (QCursor::pos().y() == 0) {
                setMax(true);
            }
            m_bLeftButtonTitlePressed = false;            
        }
        
        m_pressedMousePos.reset();
        if (m_pRubberBand && m_pRubberBand->isVisible()) {
            m_pRubberBand->hide();
            m_pWidget->setGeometry(m_pRubberBand->geometry());
        }
    }
}
 
void WidgetData::handleMouseMoveEvent(QMouseEvent *event) {
    if (m_bMax) {
        if (m_bLeftButtonTitlePressed) {
            setMax(false);
            moveWidget(event->globalPos());
        }
        return;
    }
    if (m_bLeftButtonPressed) {
        if (d->m_bWidgetResizable && m_pressedMousePos.m_bOnEdges) {
            resizeWidget(event->globalPos());
        }
        else if (d->m_bWidgetMovable && m_bLeftButtonTitlePressed) {
            moveWidget(event->globalPos());
        }
    }
    else if (d->m_bWidgetResizable) {
        updateCursorShape(event->globalPos());
    }
}
 
void WidgetData::handleMouseDblClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if(event->pos().y() < m_moveMousePos.m_nTitleHeight) {
            setMax(!m_bMax);
        }
    }
}
 
void WidgetData::handleLeaveEvent(QEvent *event) {
    Q_UNUSED(event)
    if (!m_bLeftButtonPressed) {
        m_pWidget->unsetCursor();
    }
}
 
void WidgetData::handleHoverMoveEvent(QHoverEvent *event) {
    if(m_bMax) {
        return;
    }
    if (d->m_bWidgetResizable) {
        updateCursorShape(m_pWidget->mapToGlobal(event->pos()));
    }
}
 
/*****FramelessHelper*****/
FramelessHelper::FramelessHelper(QObject *parent)
    : QObject(parent),
      d(new FramelessHelperPrivate())
{
    d->m_bWidgetMovable = true;
    d->m_bWidgetResizable = true;
    d->m_bRubberBandOnResize = false;
    d->m_bRubberBandOnMove = false;
}
 
FramelessHelper::~FramelessHelper() {
    QList<QWidget*> keys = d->m_widgetDataHash.keys();
    int size = keys.size();
    for (int i = 0; i < size; ++i) {
        delete d->m_widgetDataHash.take(keys[i]);
    }
    
    delete d;
}
 
bool FramelessHelper::eventFilter(QObject *obj, QEvent *event) {
    switch (event->type()) {
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Leave: {
        WidgetData *data = d->m_widgetDataHash.value(static_cast<QWidget*>(obj));
        if (data) {
            data->handleWidgetEvent(event);
            return true;
        }
    }
    default: 
        break;
    }
    return QObject::eventFilter(obj, event);
}
 
void FramelessHelper::activateOn(QWidget *topLevelWidget) {
    if (!d->m_widgetDataHash.contains(topLevelWidget)) {
        WidgetData *data = new WidgetData(d, topLevelWidget);
        d->m_widgetDataHash.insert(topLevelWidget, data);
        connect(data, SIGNAL(sigMax(bool)), this, SIGNAL(sigMax(bool)));
        
        topLevelWidget->installEventFilter(this);
    }
}
 
void FramelessHelper::removeFrom(QWidget *topLevelWidget) {
    WidgetData *data = d->m_widgetDataHash.take(topLevelWidget);
    if (data) {
        topLevelWidget->removeEventFilter(this);
        delete data;
    }
}
 
void FramelessHelper::setRubberBandOnMove(bool movable) {
    d->m_bRubberBandOnMove = movable;
    QList<WidgetData*> list = d->m_widgetDataHash.values();
    foreach (WidgetData *data, list) {
        data->updateRubberBandStatus();
    }
}
 
void FramelessHelper::setWidgetMovable(bool movable) {
    d->m_bWidgetMovable = movable;
}
 
void FramelessHelper::setWidgetResizable(bool resizable) {
    d->m_bWidgetResizable = resizable;
}
 
void FramelessHelper::setRubberBandOnResize(bool resizable) {
    d->m_bRubberBandOnResize = resizable;
    QList<WidgetData*> list = d->m_widgetDataHash.values();
    foreach (WidgetData *data, list) {
        data->updateRubberBandStatus();
    }
}
 
void FramelessHelper::setBorderWidth(QWidget* w, uint width) {
    if (d->m_widgetDataHash.find(w) != d->m_widgetDataHash.end() && width > 0) {
        d->m_widgetDataHash[w]->setBorderWidth(width);
    }
}
 
void FramelessHelper::setTitleHeight(QWidget* w, uint height) {
    if (d->m_widgetDataHash.find(w) != d->m_widgetDataHash.end() && height > 0) {
        d->m_widgetDataHash[w]->setHeaderHeight(height);
    }
}
 
bool FramelessHelper::widgetMovable() {
    return d->m_bWidgetMovable;
}
 
bool FramelessHelper::widgetResizable() {
    return d->m_bWidgetResizable;
}
 
bool FramelessHelper::rubberBandOnMove() {
    return d->m_bRubberBandOnMove;
}
 
bool FramelessHelper::rubberBandOnResisze() {
    return d->m_bRubberBandOnResize;
}
 
void FramelessHelper::setMax(QWidget *w,bool bMax) {
    WidgetData* data = d->m_widgetDataHash[w];
    if(data) {
        data->setMax(bMax);
    }
}
 
bool FramelessHelper::isMax(QWidget *w) {
    WidgetData* data = d->m_widgetDataHash[w];
    if(data) {
        return data->isMax();
    }
    return false;
}

void FramelessHelper::exportedEventFilter(QWidget* topLevelWidget, QEvent* e) {
    (void) eventFilter(topLevelWidget, e);
}

void FramelessHelper::setDirectionEnabled(QWidget *w, bool top, bool left, bool bottom, bool right) {
    if (d->m_widgetDataHash.find(w) != d->m_widgetDataHash.end()) {
        d->m_widgetDataHash[w]->setDirectionEnabled(top, left, bottom, right);
    }
}
