#include "customeditortoolbar.h"

#include <QGraphicsDropShadowEffect>

#include <QPushButton>
#include <QLabel>

#include "macros.hpp"

CustomEditorToolBar::CustomEditorToolBar(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(40);
    
    // setup right side buttons
    m_messionViewBtn = new QPushButton(this);
    m_undoViewBtn = new QPushButton(this);
    
    m_messionViewBtn->setFixedSize(28, 28);
    m_undoViewBtn->setFixedSize(28, 28);
    
    const char* btnQSS = R"(
        QPushButton {
            background-color: #FF2D2C39;
            border-width: 0px;
            border-color: transparent;
            border-radius: 0px;
        }
        QPushButton::hover {
            background-color: #FF3E3D4C;
        }
        QPushButton::checked {
            background-color: #FF7781F4;
        }
    )";
    
    std::array<QString, 2> iconName = {
        "btn_undo_view", "btn_mession_view"
    };
    int i = 0;
    
    for (auto button: {m_undoViewBtn, m_messionViewBtn}) {
        button->setIcon(QIcon(":/classic/controllers/" + iconName[i] + ".png"));
        button->setIconSize(button->size());
        button->setStyleSheet(btnQSS);
        button->setCheckable(true);
        
        i++;
    }
    
    m_documentStr = new QLabel(this);
    m_documentStr->setStyleSheet(R"(
        QLabel {
            padding-top: 12px;
            padding-bottom: 12px;
            font-size: 12px;
            color: #99FFFFFF;
        }
    )");
}

#define MOVE_DOCSTR_LABEL                                                                           \
    m_documentStr->move(                                                                            \
        (width()  - m_documentStr->width()) / 2,                                                    \
        (height() - m_documentStr->height()) / 2)                                                   \

void CustomEditorToolBar::resizeEvent(QResizeEvent*) {
    m_messionViewBtn->move(width() - 43, 6);
    m_undoViewBtn->move(width() - 82, 6);
    
    MOVE_DOCSTR_LABEL;
}

void CustomEditorToolBar::setDocumentString(const QString &docStr) {
    m_documentStr->setText(docStr);
    m_documentStr->adjustSize();
    
    MOVE_DOCSTR_LABEL;
}

QString CustomEditorToolBar::documentString() const {
    return m_documentStr->text();
}

#undef MOVE_DOCSTR_LABEL

void CustomEditorToolBar::slotOpenUndoView() {
    
}

void CustomEditorToolBar::slotCloseUndoView() {
    
}

void CustomEditorToolBar::slotOpenMessionView() {
    
}

void CustomEditorToolBar::slotCloseMessionView() {
    
}

void CustomEditorToolBar::slotOpenProjMediaset() {
    
}

void CustomEditorToolBar::slotCloseProjMediaset() {
    
}
