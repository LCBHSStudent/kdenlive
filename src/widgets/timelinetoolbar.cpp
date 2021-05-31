#include "timelinetoolbar.h"

#include "mainwindow.h"
#include "core.h"

#include "macros.hpp"
#include "appconsts.h"

#include <KLocalizedString>

const QString TimelineToolButton::defaultStyleSheet = R"(
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

class ToolBtnLayoutManager {
public:
    ToolBtnLayoutManager(QWidget* containerWidget) {
        m_containerWidget = containerWidget;
    }
    
    // 假定Widget为固定大小
    struct PosInfo {
        bool        isRelative = false;
        int         spacing = 0;
        double      spacingFactor = 1.0f;
        QWidget*    w = nullptr;
    };

    void addInfo(const PosInfo& info) {
        if (info.w) {
            info.w->setParent(m_containerWidget);            
        }
        m_managedWidgetList.append(info);
    }
    
    void fitPosition() {
        if (m_containerWidget == nullptr) {
            return;
        }
        int curPos      = 0;
        int dockWidth   = m_containerWidget->width();
        int dockHeight  = m_containerWidget->height();
        
        for (int i = 0; i < m_managedWidgetList.length(); i++) {
            const PosInfo& posInfo = m_managedWidgetList.at(i);
            if (posInfo.isRelative) {
                curPos += dockWidth * posInfo.spacingFactor;
            } else {
                curPos += posInfo.spacing;                
            }
            
            if (posInfo.w) {
                auto&& size = posInfo.w->size();
                auto y = (dockHeight - size.height()) / 2;
                posInfo.w->move(curPos, y);
                
                curPos += size.width();
            }
        }
    }
    
private:
    QWidget*
        m_containerWidget = nullptr;
    QList<PosInfo> 
        m_managedWidgetList = {};
};

// TIMELINE TOOL BUTTON

TimelineToolButton::TimelineToolButton(
    const QString&  iconName,
    const QString&  toolTip,        
    QWidget*        parent
)    
    : QPushButton(parent)
{
    setToolTip(toolTip);
    setStyleSheet(defaultStyleSheet);
    
    setCheckable(true);
    setFixedSize(30, 30);
    setIconSize(size());
    setIcon(QIcon(":/classic/components/ptcontroller/" + iconName + ".png"));
}

TimelineTimecodeLabel::TimelineTimecodeLabel(QWidget* parent)
    : QLabel(parent) 
{
    setStyleSheet(R"(

    )");
}

void TimelineTimecodeLabel::paintEvent(QPaintEvent*) {
    
}

TimelineToolBar::TimelineToolBar(QWidget* parent)
    : QFrame(parent)
    , m_manager(new ToolBtnLayoutManager(static_cast<QWidget*>(this)))
{
    using PosInfo = ToolBtnLayoutManager::PosInfo;
    
    setFixedHeight(40);
    
    PosInfo info = {
        .isRelative = true,
        .spacingFactor = 0.0125f,
        .w = new TimelineToolButton("undo", i18n("上一步"), this)
    };
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("redo", i18n("下一步"), this);
    info.spacingFactor = 0.00417f;
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("record_audio", i18n("录音"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("audio_to_text", i18n("音频文字互转"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("record_stream", i18n("直播剪辑/录屏"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("keyframes", i18n("关键帧"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("speed_adjust", i18n("变速"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("mark", i18n("标记"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("clip", i18n("裁剪"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("gb", i18n("绿屏抠图"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("snapshot", i18n("快照"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("mask", i18n("蒙版"), this);
    m_manager->addInfo(info);
    
    info.spacingFactor = 0.12f;
    info.w = new TimelineToolButton("prev_keypoint", i18n("跳到上一个关键位置"), this);
    m_manager->addInfo(info);
    
    info.spacingFactor = 0.003125f;
    info.w = new TimelineToolButton("prev_frame", i18n("前一帧"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("play", i18n("播放"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("next_frame", i18n("后一帧"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("next_keypoint", i18n("跳到下一个关键位置"), this);
    m_manager->addInfo(info);
    
    info.spacingFactor = 0.00417f;
    info.w = new TimelineToolButton("volume", i18n("音量"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("fullscreen", i18n("全屏"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("scale_level", i18n("缩放级别"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("safe_margin", i18n("安全边距"), this);
    m_manager->addInfo(info);
    
    info.w = new TimelineToolButton("grid", i18n("网格"), this);
    m_manager->addInfo(info);
    
    info.spacingFactor = 0.0125f;
    info.w = new QPushButton(i18n("渲染"), this);
    info.w->setFixedSize(QSize(59, 25));
    info.w->setStyleSheet(QString(R"(
        QPushButton {
            font-size: 12px;
            border: none;
            border-radius: 16;
            background-color: %1;
            font-size: 12px;
            color: %2;
        }
        QPushButton::hover {
            background-color: %3;
        }
        QPushButton::pressed {
            background-color: %4;
        }
    )").arg(
        APPCONSTS.foregroundColor.name(), 
        APPCONSTS.normalFontColor.name(), 
        APPCONSTS.foregroundColor.lighter(30).name(), 
        APPCONSTS.foregroundColor.darker(30).name())
    );
    
    m_manager->addInfo(info);
    
    info.w = new QLabel(this);
    info.w->setStyleSheet(QString(R"(
        QLabel {        
            font-size: 12px;
            color: #9F9F9F;
            border-bottom-color: #9F9F9F;
            border-bottom-width: 1px;
            border-bottom-style: %1;
        }
    )").arg(APPCONSTS.foregroundColor.name()));
    dynamic_cast<QLabel*>(info.w)->setText("00:00:00:00 ");
    info.w->adjustSize();
    m_manager->addInfo(info);
}

void TimelineToolBar::resizeEvent(QResizeEvent*) {
    m_manager->fitPosition();
}
