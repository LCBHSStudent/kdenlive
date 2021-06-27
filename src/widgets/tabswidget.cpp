#include "tabswidget.h"
#include "appconsts.h"

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QPainter>
#include <QDebug>

constexpr auto __contentMargins = 14;
constexpr auto __tabBarHeight = 32;

TabsWidget::TabsWidget(QWidget* parent)
    : QFrame(parent)
    , m_tabContainer(new QWidget(this))
    , m_tabBtnGroup(new QButtonGroup(this))
{
    setStyleSheet(QString(R"(
    TabsWidget {
        background-color: %1;
    })").arg(APPCONSTS.lighterSpaceColor.name()));
    
    auto tabLayout = new QHBoxLayout(m_tabContainer);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    tabLayout->setSpacing(0);
    
    m_tabBtnGroup->setExclusive(true);
    
    connect(m_tabBtnGroup, &QButtonGroup::idClicked, [this](int idx) {
        this->selectTab(idx);
    });
    
    m_tabContainer->setLayout(tabLayout);
    m_tabContainer->setFixedHeight(__tabBarHeight);
}


void TabsWidget::addTab(QWidget* tabWidget, const QString& tabTitile) {
    m_tabTitles.append(tabTitile);
    m_tabs.append(tabWidget);
    
    tabWidget->setParent(this);
    tabWidget->resize(width() - __contentMargins * 2, height() - __tabBarHeight - __contentMargins * 2);
    tabWidget->move(__contentMargins, __tabBarHeight + __contentMargins);
    tabWidget->hide();
    
    auto tabButton = new QPushButton(tabTitile, m_tabContainer);
    tabButton->setFixedHeight(__tabBarHeight);
    tabButton->setStyleSheet(QString(R"(
    QPushButton {
        font-size: 14px;
        font-family: "%1";
        background-color: %2;
        border-width: 0px;
        border-color: transparent;
        border-radius: 0px;
    }

    QPushButton::pressed {
        background-color: %3;
    }

    QPushButton::checked {
        background-color: %4;
    }
)").arg(APPCONSTS.fontFamily).arg(APPCONSTS.backgroundColor.name()).arg(APPCONSTS.darkerSpaceColor.lighter(110).name()).arg(APPCONSTS.darkerSpaceColor.name()));
    
    tabButton->setCheckable(true);
    
    m_tabBtnGroup->addButton(tabButton, m_tabBtnGroup->buttons().size());
    
    m_tabContainer->layout()->addWidget(tabButton);
}

void TabsWidget::selectTab(int index) {
    m_currentTabIdx = index;
    
    auto targetBtn = m_tabBtnGroup->buttons().at(index);
    if (!targetBtn->isChecked()) {
        targetBtn->setChecked(true);
    }
    
    fix();
}

void TabsWidget::resizeEvent(QResizeEvent *) {
    fix();
    
    m_tabs.at(m_currentTabIdx)->resize(width() - __contentMargins * 2, height() - __tabBarHeight - __contentMargins * 2);
    m_tabs.at(m_currentTabIdx)->move(__contentMargins, __tabBarHeight + __contentMargins);
    m_tabContainer->resize(width() - __contentMargins * 2, __tabBarHeight);
    m_tabContainer->move(__contentMargins, __contentMargins);   
}

void TabsWidget::fix() {    
    m_currentTabIdx = qBound(0, m_currentTabIdx, m_tabs.count() - 1);
    for (int i = 0; i < m_tabs.count(); i++) {
        if (m_currentTabIdx == i) {
            m_tabBtnGroup->buttons().at(i)->setChecked(true);
            m_tabs.at(i)->show();
        } else {
            m_tabs.at(i)->hide();
        }
    }
    
    update();
}
