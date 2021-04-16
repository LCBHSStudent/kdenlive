#include "topnavigationbar.h"

#include <QMouseEvent>
#include <QDebug>

#include "macros.hpp"

constexpr auto __menuBarHeight = 42;

TopNavigationBar::TopNavigationBar(QWidget* parent)
    : QLabel(parent)
    , m_currentTab(EDITOR)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(300, __menuBarHeight - 1);
    connect(this, &TopNavigationBar::tabChanged, this, &TopNavigationBar::onTabChanged);
    
    m_editorPix     = new QPixmap(":/classic/components/headerbar_mode_edit.png");
    m_materialDbPix = new QPixmap(":/classic/components/headerbar_mode_medialib.png");
    
    onTabChanged();
    
    parent->installEventFilter(this);
    
}

void TopNavigationBar::mousePressEvent(QMouseEvent* e) {
    if (e->button() != Qt::LeftButton) {
        return;
    }
    auto&& x = e->pos().x();
    
    if (x >= width() / 2) {
        if (m_currentTab == MATERIAL_DATABASE) {
            m_currentTab = EDITOR;
            emit tabChanged(EDITOR);
        }
    } else {
        if (m_currentTab == EDITOR) {
            m_currentTab = MATERIAL_DATABASE;
            emit tabChanged(MATERIAL_DATABASE);
        }
    }
}

bool TopNavigationBar::eventFilter(QObject*, QEvent* e) {
    if (e->type() == QEvent::Resize) {
        move((parentWidget()->width() - width()) / 2, 0);
    }
    return false;
}

void TopNavigationBar::onTabChanged() {
    if (m_currentTab == MATERIAL_DATABASE) {
        setPixmap(*m_materialDbPix);
    }
    else {
        setPixmap(*m_editorPix);
    }
}
