#ifndef CUSTOMTOOLTIP_H
#define CUSTOMTOOLTIP_H

#include <QLabel>

class QTimer;

class CustomToolTip: public QLabel {
	Q_OBJECT
    explicit CustomToolTip(QWidget* parent = nullptr);
public:
	static void installToolTip(QWidget* parent = nullptr);	
	
	static void showToolTip(const QString&, int x, int y);
	static void hideToolTip();
	
	inline static const char defaultQSS[] = R"(
        QLabel {
            padding-left: 6px;
            padding-top: 2px;
            padding-right: 6px;
            padding-bottom: 3px;
            font-size: 12px; 
            font-family: 'Microsoft YaHei';    
            color: "#CCFFFFFF"
        }
	)";
	
protected:
	void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;
	
private: 
	inline static CustomToolTip* s_instance = nullptr;
	
	QTimer* m_timer;
    int     m_targetX;
    int     m_targetY;
};

#endif // CUSTOMTOOLTIP_H
