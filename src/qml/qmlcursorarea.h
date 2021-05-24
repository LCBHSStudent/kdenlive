#ifndef QML_CURSORAREA_H
#define QML_CURSORAREA_H

#include <QQuickItem>
#include <QQmlComponent>
#include <symbols.h>

class QmlCursorArea: public QQuickItem {
    Q_OBJECT
    
    Q_ENUMS(Cursor)
    
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    
public:
    enum Cursor {
        DefaultCursor,
        IBeamCursor,
        ArrowCursor,
        SizeHorCursor,
        SizeVerCursor,
        SizeAllCursor,
        SplitHCursor,
        SplitVCursor,
        WaitCursor,
        PointingHandCursor
    };
private:    
    REGISTER_PROPERTY(Cursor, cursor)
    REGISTER_PROPERTY(int, hotX)
    REGISTER_PROPERTY(int, hotY)
public:
    explicit 
        QmlCursorArea(QQuickItem* parent = nullptr);
    virtual 
        ~QmlCursorArea();

    QQmlComponent* 
        delegate() const;
    Q_INVOKABLE static void
        setCursor(Cursor);
    void 
        setDelegate(QQmlComponent* delegate);

signals:
    void cursorChanged();
    void delegateChanged();
    void hotXChanged();
    void hotYChanged();
    
protected:
    void hoverEnterEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);
    
private:
    QQmlComponent* 
        m_delegate = nullptr;
    
    QCursor 
        createCustomCursor();
};

#endif // QML_CURSORAREA_H
