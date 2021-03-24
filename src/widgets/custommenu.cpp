#include "custommenu.h"

#include <QGraphicsEffect>
#include <QScreen>

CustomMenu::CustomMenu(QWidget* parent)
    : QMenu(parent)
{    
    updateAppearance();
}

CustomMenu::CustomMenu(const QString& title, QWidget* parent) 
    : QMenu(title, parent) 
{
    updateAppearance();
}

void CustomMenu::updateAppearance() {
    auto* se = new QGraphicsDropShadowEffect(this);
    se->setOffset(-3, 0);
    se->setBlurRadius(6);
    se->setColor(QColor(36, 35, 44));
    
    setGraphicsEffect(se);
    
    CustomMenu::setStyleSheet("");
}

// 最后设置的qss会覆盖默认设定，放心用
void CustomMenu::setStyleSheet(const QString& qss) {
    QMenu::setStyleSheet(menuSheet + menuItemSheet + indicatorSheet + qss);
}

void CustomMenu::setEnableAutoRange(bool value) {
    m_enableAutoRange = value;
}

void CustomMenu::showEvent(QShowEvent*) {
    if (!m_enableAutoRange && parent() == nullptr) {
        return;
    }
    
    auto rect = screen()->availableGeometry();
    
    if (x() + width() > rect.width() + rect.x()) {
        move(rect.x() + rect.width() - width(), y());
    }
    if (y() + height() > rect.height() + rect.y()) {
        move(x(), rect.height() + rect.y() - height());
    }
}
