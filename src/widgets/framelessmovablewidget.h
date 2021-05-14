#ifndef FRAMELESSMOVABLEWIDGET_H
#define FRAMELESSMOVABLEWIDGET_H

#include <QFrame>

class QPushButton;

class FramelessMovableWidget: public QFrame {
    Q_OBJECT
public:
    explicit FramelessMovableWidget(QWidget* parent = nullptr);
    virtual ~FramelessMovableWidget() = default;
    
    int     
        headerHeight() const;
    void    
        setHeaderHeight(int headerHeight);
    QString 
        title() const;
    void    
        setTitle(const QString&);
    
protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    bool eventFilter(QObject*, QEvent*) override;
    
private:
    int             m_headerHeight      = 0;
    bool            m_pressStarted      = false;
    QPoint          m_pressedPoint      = {};
    QPoint          m_moveStartPoint    = {};
    QPushButton*    m_closeBtn          = nullptr;
    QString         m_title             = {};
};

#endif // FRAMELESSMOVABLEWIDGET_H
