#ifndef CUSTOMMENU_H
#define CUSTOMMENU_H

#include <QMenu>

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
                left: 8px;
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
            } 
            QMenu::right-arrow {
                image: url(:/classic/controllers/indicator-rightarrow.png); 
                padding: 19px;
            }  
            QMenu{
                font-family: 'Microsoft yahei'; 
                font-size: 14px; 
                background: #3E3D4C;
            }
        )";
        inline static const QString indicatorSheet = R"(
            QMenu::indicator {
                padding-left: 3px; 
                padding-right: 3px; 
                width: 20px;
                height: 20px;
            } 
            QMenu::indicator:unchecked { 
                image:url(:/icons/menu/unchecked.png);
            } 
            QMenu::indicator:checked {
                image:url(:/icons/menu/checked.png); 
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
