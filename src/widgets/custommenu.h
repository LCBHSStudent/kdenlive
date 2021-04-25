#ifndef CUSTOMMENU_H
#define CUSTOMMENU_H

#include <QMenu>

/**
 * @annotatedlist check out class QProxyStyle
 */

class CustomMenu: public QMenu {
    Q_OBJECT
public:
    explicit 
        CustomMenu(QWidget* parent = nullptr);
		CustomMenu(const QString& title, QWidget* parent = nullptr);

        inline static const QString menuItemSheet = R"(
            QMenu::item {
                padding-top: 7px;
                padding-left: 30px;
                padding-right: 33px;
                padding-bottom: 7px;
                spacing: 40px;
            }    
            QMenu::item:non-exclusive {
                padding-top: 8px;
                padding-left: 0px;
                padding-right: 33px;
                padding-bottom: 7px;
            }
            QMenu::item:selected {
                background-color: #997781F4;
            }
			QMenu::icon {
                position: absolute;
                left: 15px;
			}
        )";
        inline static const QString menuSheet = R"(
            QMenu::separator { 
                height: 1px; 
                background: #2D2C39; 
                margin-left: 5px; 
                margin-right: 5px; 
            } 
            QMenu {
                font-weight:normal;
                padding: 0px;
            } 
            QMenu::right-arrow {
                image: url(:/classic/controllers/indicator-rightarrow.png); 
                padding: 19px;
            }  
            QMenu{
                font-family: "Microsoft YaHei"; 
                font-size: 12px; 
                background-color: #3E3D4C;
            }
        )";
        inline static const QString indicatorSheet = R"(
            QMenu::indicator {
                padding-left: 5px; 
                padding-right: 5px; 
                width: 20px;
                height: 20px;
            } 
            QMenu::indicator:unchecked { 
                image:url(:/classic/controllers/checkbox_noborder_unchecked.png);
            } 
            QMenu::indicator:checked {
                image:url(:/classic/controllers/checkbox_noborder_checked.png); 
            }
        )"; 
    void setStyleSheet(const QString& qss);
    void setEnableAutoRange(bool value);
    
protected:
    void showEvent(QShowEvent*) override;
    
private:
	void updateAppearance();
	
private:
    bool m_enableAutoRange = false;
	
};


#endif // CUSTOMMENU_H
