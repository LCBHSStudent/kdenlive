#ifndef _SYMBOLS_H
#define _SYMBOLS_H

// 展开变量LOG
#define LOG_EXTEND(__VARIBALE) "\n\t"#__VARIBALE":" << __VARIBALE

// 注册属性系统
#define REGISTER_PROPERTY(__TYPE__, __NAME__)                                                                                                                   \
    Q_PROPERTY(__TYPE__ __NAME__ READ __NAME__ WRITE set_##__NAME__ NOTIFY __NAME__##Changed)                                                                   \
public:                                                                                                                                                         \
    Q_INVOKABLE __TYPE__ __NAME__() const { return m_##__NAME__; }                                                                                              \
    void set_##__NAME__(const __TYPE__& value) {                                                                                                                \
        if (value == m_##__NAME__) {                                                                                                                            \
            return;                                                                                                                                             \
        }                                                                                                                                                       \
        m_##__NAME__ = value;                                                                                                                                   \
        emit __NAME__##Changed();                                                                                                                               \
    }                                                                                                                                                           \
private:                                                                                                                                                        \
    __TYPE__ m_##__NAME__;                                                                                                                                      \
    
// 提供左下 / 右下两种widget缩放策略
constexpr auto borderWidth = 10;
#define REGIST_RESIZEHELPER                                                                                                                                     \
protected:                                                                                                                                                      \
    void mouseMoveEvent(QMouseEvent*) override;                                                                                                                 \
    void mousePressEvent(QMouseEvent*) override;

#define IMPLEMENT_RESIZEHELPER_P1(__CLASS, __SUPER)                                                                                                             \
void __CLASS::mouseMoveEvent(QMouseEvent* e) {                                                                                                                  \
    __SUPER::mouseMoveEvent(e);                                                                                                                                 \
    auto x = e->pos().x();                                                                                                                                      \
    auto y = e->pos().y();                                                                                                                                      \

#define IMPLEMENT_RESIZEHELPER_P2                                                                                                                               \
    bool onBottom = y >= height() - borderWidth;                                                                                           \
                                                                                                                                                                \
    static int dragStarted = 0;                                                                                                                                 \
    if (e->buttons() & Qt::LeftButton) {                                                                                                                        \
        if (!dragStarted) {                                                                                                                                     \
            if (onSide && !onBottom) {                                                                                                                          \
                dragStarted = 1;                                                                                                                                \
            } else if(!onSide && onBottom) {                                                                                                                    \
                dragStarted = 2;                                                                                                                                \
            } else if (onSide && onBottom) {                                                                                                                    \
                dragStarted = 3;                                                                                                                                \
            }                                                                                                                                                   \
        }                                                                                                                                                       \
    } else {                                                                                                                                                    \
        dragStarted = 0;                                                                                                                                        \
    }                                                                                                                                                           \
                                                                                                                                                                \
    if (!dragStarted) {                                                                                                                                         \
        if (onSide && !onBottom) {                                                                                                                              \
            setCursor(Qt::SizeHorCursor);                                                                                                                       \
        } else if(!onSide && onBottom) {                                                                                                                        \
            setCursor(Qt::SizeVerCursor);                                                                                                                       \
        } else if (onSide && onBottom) {                                                                                                                        \
    
#define IMPLEMENT_RESIZEHELPER_P3                                                                                                                               \
        } else {                                                                                                                                                \
            setCursor(Qt::ArrowCursor);                                                                                                                         \
        }                                                                                                                                                       \
    } else {                                                                                                                                                    \
        if (dragStarted == 1) {                                                                                                                                 \
    
#define IMPLEMENT_RESIZEHELPER_P4(__CLASS, __SUPER)                                                                                                             \
        }                                                                                                                                                       \
        else if (dragStarted == 2) {                                                                                                                            \
            resize(this->width(), y);                                                                                                                           \
        }                                                                                                                                                       \
    }                                                                                                                                                           \
    e->accept();                                                                                                                                                \
}                                                                                                                                                               \
                                                                                                                                                                \
void __CLASS::mousePressEvent(QMouseEvent* e) {                                                                                                                 \
    __SUPER::mousePressEvent(e);                                                                                                                                \
	mouseMoveEvent(e);                                                                                                                                          \
}

// 左下缩放
#define IMPLEMENT_RESIZEHELPER_LB(__CLASS, __SUPER, __MAX_X)                                                                                                    \
IMPLEMENT_RESIZEHELPER_P1(__CLASS, __SUPER)                                                                                                                     \
    bool onSide = static_cast<size_t>(x) >= 0 && static_cast<size_t>(x) <= borderWidth;                                                                         \
IMPLEMENT_RESIZEHELPER_P2                                                                                                                                       \
    setCursor(Qt::SizeBDiagCursor);                                                                                                                             \
IMPLEMENT_RESIZEHELPER_P3                                                                                                                                       \
        resize(this->width() - x, this->height());                                                                                                              \
        move(qMin(this->x() + x, __MAX_X), this->y());                                                                                                          \
    } else if (dragStarted == 3) {                                                                                                                              \
        resize(this->width() - x, y);                                                                                                                           \
        move(qMin(this->x() + x, __MAX_X), this->y());                                                                                                          \
IMPLEMENT_RESIZEHELPER_P4(__CLASS, __SUPER)

// 右下缩放
#define IMPLEMENT_RESIZEHELPER_RB(__CLASS, __SUPER)                                                                                                             \
IMPLEMENT_RESIZEHELPER_P1(__CLASS, __SUPER)                                                                                                                     \
    bool onSide = x >= width() - borderWidth && x <= width();                                                                              \
IMPLEMENT_RESIZEHELPER_P2                                                                                                                                       \
    setCursor(Qt::SizeFDiagCursor);                                                                                                                             \
IMPLEMENT_RESIZEHELPER_P3                                                                                                                                       \
    resize(x, this->height());                                                                                                                                  \
} else if (dragStarted == 3) {                                                                                                                                  \
    resize(x, y);                                                                                                                                               \
IMPLEMENT_RESIZEHELPER_P4(__CLASS, __SUPER)
/** end of resize strategy **/

// 记得constchar写法带上参数列表
#define CONNECT_SELF_RAW(__SIGNAL, __SLOT)                                                                                                                          \
    connect(this, SIGNAL(__SIGNAL), this, SLOT(__SLOT))

#define CONNECT_SELF(__CLASS, __FUNC1, __FUNC2)                                                                                                                 \
    connect(this, &__CLASS::__FUNC1, this, &__CLASS::__FUNC2)

#define VAR_SETTER(__VAR_NAME)                                                                                                                                  \
    if (m_##__VAR_NAME == __VAR_NAME) return;                                                                                                                   \
    m_##__VAR_NAME = __VAR_NAME; emit __VAR_NAME##Changed()

#endif // symbols.h
