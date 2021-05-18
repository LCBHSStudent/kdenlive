#ifndef RSTOOLBAR_H
#define RSTOOLBAR_H

#include <QQuickWidget>

class RSToolBar: public QQuickWidget {
	Q_OBJECT
public:
    explicit RSToolBar(QWidget* parent = nullptr);
	~RSToolBar() override = default;

protected:
	void mousePressEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;
	void mouseMoveEvent(QMouseEvent*) override;
	void keyReleaseEvent(QKeyEvent*) override;
    
private:
	
};

#endif // RSTOOLBAR_H
