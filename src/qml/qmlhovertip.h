#ifndef QMLHOVERTIP_H
#define QMLHOVERTIP_H

#include <symbols.h>
#include <QQuickItem>
#include <QTimer>

class QQuickWidget;

class QmlHoverTip: public QQuickItem {
    Q_OBJECT
    
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(int delay READ delay WRITE setDelay NOTIFY delayChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    
public:
    explicit 
        QmlHoverTip(QQuickItem* parent = nullptr);
    virtual
        ~QmlHoverTip();
    
    // getters
    int 
        delay() const { return m_delay; }
    int 
        timeout() const { return m_timeout; }
    bool
        enabled() const { return m_enabled; }
    QString
        text() const { return m_text; }
    
    // setters
    void
        setDelay(int delay) { VAR_SETTER(delay); m_displayTimer->setInterval(delay); }
    void
        setTimeout(int timeout) { VAR_SETTER(timeout); m_hideTimer->setInterval(timeout); }
    void
        setEnabled(int enabled) { VAR_SETTER(enabled); }
    void    // sepical cond for text
        setText(const QString& text) { VAR_SETTER(text); }
    
signals:
    void textChanged();
    void delayChanged();
    void timeoutChanged();
    void enabledChanged();
    
protected:
    void hoverEnterEvent(QHoverEvent*) override;
//    void hoverMoveEvent(QHoverEvent*) override;
    void hoverLeaveEvent(QHoverEvent*) override;
    
private slots:
    void onHideTriggered();
    void onDisplayTriggered();

private:
    inline static QQuickWidget* 
        s_toolTip = nullptr;
    inline static QUrl
        s_toolTipQmlUrl = {};
    
    QTimer* 
        m_hideTimer = nullptr;
    QTimer* 
        m_displayTimer = nullptr;
    
    int 
        m_delay     = 800;
    int 
        m_timeout   = 5000;
    bool 
        m_enabled   = true;
    QString
        m_text      = {};

};

#endif // QMLHOVERTIP_H
