#include "topnavigationbar.h"

#include <QMouseEvent>
#include <QDebug>
#include <QPainter>

#include <mutex>

#include <klocalizedstring.h>
#include "macros.hpp"

constexpr auto __menuBarHeight = 42;

TopNavigationBar::TopNavigationBar(QWidget* parent)
    : QLabel(parent)
    , m_currentTab(EDITOR)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(300, __menuBarHeight - 1);
    
    parent->installEventFilter(this);
    
    connect(this, &TopNavigationBar::tabChanged, this, &TopNavigationBar::onTabChanged);
    
    onTabChanged();
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

void TopNavigationBar::paintEvent(QPaintEvent*) {
    QPainter p(this);
    
    p.setPen(Qt::transparent);
    p.setRenderHint(QPainter::RenderHint::Antialiasing);
    
    auto widthDiv2  = width() / 2;
    auto _height    = height();
    
    p.setBrush(m_tabColorMaterial);
    p.drawRect(0, 0, widthDiv2, _height);
    p.setBrush(m_tabColorEditor);
    p.drawRect(widthDiv2, 0, widthDiv2, _height);
    
    static auto font = QFont("Microsoft Yahei");
    font.setPixelSize(16);
    
    font.setBold(m_fontBoldMaterial);
    p.setPen(m_fontBoldMaterial? Qt::white: QColor(255, 255, 255, 153));
    p.setFont(font);
    p.drawText(
        (widthDiv2 - m_fMaterialWidth) / 2,
        (_height - 22) / 2,
        m_fMaterialWidth,
        22,
        0, i18n("素材库")
    );
    
    font.setBold(m_fontBoldEditor);
    p.setPen(m_fontBoldEditor? Qt::white: QColor(255, 255, 255, 153));
    p.setFont(font);
    p.drawText(
        widthDiv2 + (widthDiv2 - m_fEditorWidth) / 2,
        (_height - 22) / 2,
        m_fEditorWidth,
        22,
        0, i18n("剪 辑")
    );
    
}

void TopNavigationBar::onTabChanged() {
    QFont font("Microsoft Yahei");
    font.setPixelSize(16);
    
    QFontMetrics fm(font);
    
    font.setBold(true);
    QFontMetrics fmb(font);
    
    if (m_currentTab == MATERIAL_DATABASE) {
        m_tabColorEditor    = QColor(62,61,76);
        m_tabColorMaterial  = QColor(45,140,240);
        
        m_fontBoldEditor    = false;
        m_fontBoldMaterial  = true;
        
        m_fEditorWidth      = fm.horizontalAdvance(i18n("剪 辑"));
        m_fMaterialWidth    = fmb.horizontalAdvance(i18n("素材库"));
        m_fEditorHeight     = fm.height();
        m_fMaterialHeight   = fmb.height();
    }
    else {
        m_tabColorEditor    = QColor(45,140,240);
        m_tabColorMaterial  = QColor(62,61,76);
        
        m_fontBoldEditor    = true;
        m_fontBoldMaterial  = false;
        
        m_fEditorWidth      = fmb.horizontalAdvance(i18n("剪 辑"));
        m_fMaterialWidth    = fm.horizontalAdvance(i18n("素材库"));
        m_fEditorHeight     = fmb.height();
        m_fMaterialHeight   = fm.height();
    }
    update();
}
