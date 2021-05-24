#include "colorpicker.h"

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <QScreen>
#include <QDesktopWidget>

#ifdef Q_OS_WIN32
#include <Windows.h>
#endif

ColorPicker::ColorPicker(QWidget* parent)
    : QWidget(parent) 
{
    // 设置十字光标
    initCursor();
    // 截获全局屏幕数据
    initGlobalScreen();
    // 全屏窗口
    showFullScreen();
    // 对齐显示器
    setGeometry(getScreenRect());
    move(0, 0);
    // 置顶窗口
    onEgoistic();
    // hover启用事件
    setMouseTracking(true);
    
    colorInfoSize = QSize(
        122, // * Util::getWindowWidthMultiplyingPower(),
        90   // * Util::getWindowHeightMultiplyingPower()
    );
    
    connect(
        this, SIGNAL(posChanged(int,int)),
        this, SLOT(onPosChanged(int,int))
    );
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    
    onPosChanged(cursor().pos().x(), cursor().pos().y());
    
    show();
}

ColorPicker::~ColorPicker() {
    emit cancelled();
}

void ColorPicker::hideEvent(QHideEvent*) {
    
}

void ColorPicker::closeEvent(QCloseEvent*) {
    
}

void ColorPicker::mousePressEvent(QMouseEvent* e) {
    if (e->buttons() & Qt::LeftButton) {
        m_isLeftPressed = true;
    } else {
        m_isLeftPressed = false;
    }
}

void ColorPicker::mouseReleaseEvent(QMouseEvent*) {
    if (!m_isLeftPressed) {
        close();
        return;
    } else {
        QImage&& image        = m_capturedScreen->toImage();
        QColor&& cursorPixel  = image.pixel(cursor().pos());
        
        emit colorPicked(cursorPixel);
        
        metaObject()->invokeMethod(this, &ColorPicker::close, Qt::QueuedConnection);
    }
    
    m_isLeftPressed = false;
}

void ColorPicker::mouseMoveEvent(QMouseEvent* e) {
    // emit posChanged(e->x(), e->y());
    onPosChanged(e->x(), e->y());
}

void ColorPicker::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    // 绘制捕获屏幕
    painter.drawPixmap(
        0, 0,
        m_desktopRect.width(), m_desktopRect.height(), 
        *m_capturedScreen
    );
    
    // 绘制背景
    painter.fillRect(
        QRect(
            m_colorInfoPos.x(), m_colorInfoPos.y(),
            colorInfoSize.width(), colorInfoSize.width()
        ), QColor(0, 0, 0, 160)
    );

    QPixmap endPointImage;
    /** 绘制放大图 */
    const QSize& _size = size();
    auto&& cursorPos = cursor().pos();
    
    if ((cursorPos.x() + 15 < _size.width() && cursorPos.x() - 15 > 0) && 
        (cursorPos.y() + 11 < _size.height() && cursorPos.y() - 11 > 0)
    ) {
        endPointImage = m_capturedScreen->copy(
            QRect(cursorPos.x() - 15, cursorPos.y() - 11, 30, 22)
        ).scaled(colorInfoSize.width(), colorInfoSize.height());
        
        painter.drawPixmap(m_colorInfoPos.x(), m_colorInfoPos.y(), endPointImage);
    }
    else {
        endPointImage = m_capturedScreen->copy(
            QRect(cursorPos.x() - 15, cursorPos.y() - 11, 30, 22)
        );
    }

    /** 绘制十字 */
    painter.setPen(QPen(QColor(0, 180, 255 , 180), 4));
    // 竖线
    painter.drawLine(
        QPoint((colorInfoSize.width() >> 1), 4) + m_colorInfoPos,
        QPoint((colorInfoSize.width() >> 1), colorInfoSize.height() - 4) + m_colorInfoPos
    );
    // 横线
    painter.drawLine(
        QPoint(4, colorInfoSize.height() >> 1) + m_colorInfoPos,
        QPoint(colorInfoSize.width() - 4, colorInfoSize.height() >> 1) + m_colorInfoPos
    );

    // 绘制大图内边框
    painter.setPen(QPen(Qt::white, 2));
    painter.drawRect(
        m_colorInfoPos.x() + 2, m_colorInfoPos.y() + 2, 
        colorInfoSize.width() - 4, colorInfoSize.height() - 4
    );

    // 绘制外边框
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(
        m_colorInfoPos.x(), m_colorInfoPos.y(),
        colorInfoSize.width() - 1, colorInfoSize.height() - 1
    );

    // 当前鼠标像素值的RGB信息
    QImage image        = m_capturedScreen->toImage();
    QColor cursorPixel  = image.pixel(cursorPos);
    
    QString posInfo     = QString(tr("位置:(X:%1,Y:%2)")).arg(cursor().pos().x()).arg(cursor().pos().y());
    QString rgbInfo     = QString(tr("RGB:(%1,%2,%3)"))
                                .arg(cursorPixel.red())
                                .arg(cursorPixel.green())
                                .arg(cursorPixel.blue());

    // 绘制坐标轴相关数据
    painter.setPen(Qt::white);
    painter.drawText(QPoint(6, colorInfoSize.height() + 14) + m_colorInfoPos, rgbInfo);
    painter.drawText(QPoint(6, colorInfoSize.height() + 27) + m_colorInfoPos, posInfo);
}

void ColorPicker::keyPressEvent(QKeyEvent* e) {
    // 使用ECS取消截屏
    switch (e->key()) {
    case Qt::Key_Escape:
        close();
        break;
    case Qt::Key_Up:
        cursor().setPos(cursor().pos().x(), cursor().pos().y() - 1);
        break;
    case Qt::Key_Down:
        cursor().setPos(cursor().pos().x(), cursor().pos().y() + 1);
        break;
    case Qt::Key_Left:
        cursor().setPos(cursor().pos().x() - 1, cursor().pos().y());
        break;
    case Qt::Key_Right:
        cursor().setPos(cursor().pos().x() + 1, cursor().pos().y());
        break;
    default:
        e->ignore();
    }
}

void ColorPicker::initCursor() {
    setCursor(Qt::CrossCursor);
}

void ColorPicker::initGlobalScreen() {
    QScreen* screen = QGuiApplication::primaryScreen();
    auto tempRect = getScreenRect();
    
    m_capturedScreen.reset(
        new QPixmap(screen->grabWindow(0, tempRect.x(), tempRect.y(), tempRect.width(), tempRect.height()))
    );
}

const QRect& ColorPicker::getScreenRect() {
    m_desktopRect = QRect(
        QApplication::desktop()->pos(), QApplication::desktop()->size()
    );
    return m_desktopRect;
}

void ColorPicker::onEgoistic() {
#ifdef Q_OS_WIN32
    SetWindowPos(
        (HWND)this->winId(),
        HWND_TOPMOST,
        this->pos().x(),
        this->pos().y(),
        this->width(),
        this->height(),
        SWP_SHOWWINDOW
    );
#else
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);
#endif
}

void ColorPicker::onPosChanged(int x, int y) {
    m_colorInfoPos = QPoint(x + 4, y + 26);
    
    if (m_colorInfoPos.x() + colorInfoSize.width() > width()) {
        m_colorInfoPos.setX(x - 4 - colorInfoSize.width());
    }
    if (m_colorInfoPos.y() + colorInfoSize.height() > height()) {
        m_colorInfoPos.setY(y - 26 - colorInfoSize.height());
    }
    
    update();
}
