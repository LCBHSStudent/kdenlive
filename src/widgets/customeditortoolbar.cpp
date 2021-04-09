#include "customeditortoolbar.h"

#include <QGraphicsDropShadowEffect>

#include <QPushButton>
#include <QLabel>

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
    )";
    
    m_undoViewBtn->setIcon(QIcon(":/classic/controllers/btn_undo_view.png"));
    m_undoViewBtn->setIconSize(m_undoViewBtn->size());
    m_undoViewBtn->setStyleSheet(btnQSS);
    m_messionViewBtn->setIcon(QIcon(":/classic/controllers/btn_mession_view.png"));
    m_messionViewBtn->setIconSize(m_messionViewBtn->size());
    m_messionViewBtn->setStyleSheet(btnQSS);
    
    m_documentName = new QLabel(this);
    m_documentName->setText("文件名称");
    
}

void CustomEditorToolBar::resizeEvent(QResizeEvent*) {
    m_messionViewBtn->move(width() - 43, 6);
    m_undoViewBtn->move(width() - 72, 6);
    m_documentName->move(
        (width() - m_documentName->width()) / 2,
        (height() - m_documentName->height()) / 2
    );
}

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
