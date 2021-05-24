#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>
#include "utils/util.h"

/** @author DFW */
class ColorPicker: public QWidget {
    Q_OBJECT
public:
    explicit 
        ColorPicker(QWidget* parent = nullptr);
    virtual
        ~ColorPicker();
        
    const QRect& 
        getScreenRect();
    
    inline static QSize colorInfoSize;
    
signals:
    /**
     * @brief 光标位置改变
     * @param x
     * @param y
     */
    void posChanged(int x, int y);
    /**
     * @brief 光标位置改变
     * @param 选中像素的颜色
     */
    void colorPicked(const QColor& color);
    /**
     * @brief 取消选择
     */
    void cancelled();
    
protected:
    void hideEvent(QHideEvent*) override;
    void closeEvent(QCloseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    
    void paintEvent(QPaintEvent*) override;    
    void keyPressEvent(QKeyEvent*) override;
    
private:
    void initCursor();
    void initGlobalScreen();
    
private slots:
    void onPosChanged(int, int);
    void onEgoistic();
    
private:
    QScopedPointer<QPixmap> 
        m_capturedScreen;
    
    QRect
        m_desktopRect = {};
    
    bool 
        m_isLeftPressed = false;
    QPoint
        m_colorInfoPos = {};
};

#endif
