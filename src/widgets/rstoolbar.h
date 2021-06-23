#ifndef RSTOOLBAR_H
#define RSTOOLBAR_H

#include <QQuickWidget>

constexpr int __rsToolBar_fixedWidth    = 515;
constexpr int __rsToolBar_topMargin     = 19;
constexpr int __rsToolBar_bottomMargin  = 5;
constexpr int __qmlTabItemHeight        = 46;
constexpr int __qmlTabBarWidth          = 103;

class RSToolBar: public QQuickWidget {
	Q_OBJECT
public:
    explicit RSToolBar(QWidget* parent = nullptr);
	~RSToolBar() override = default;

public slots:
    /**
     * @brief 修改右侧面板开闭状态
     * @param isOpen
     */
    void slotSwitchPanelState(bool isOpen);

protected:
	void mousePressEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;
	void mouseMoveEvent(QMouseEvent*) override;
	void keyReleaseEvent(QKeyEvent*) override;
    
private:

};

#endif // RSTOOLBAR_H
