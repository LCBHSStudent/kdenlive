#include "timelinetoolbar.h"

#include "mainwindow.h"
#include "core.h"

#include "macros.hpp"
#include "appconsts.h"
#include "monitor/monitor.h"
#include "monitor/monitorproxy.h"

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
        bool        checkable = false;
        bool        isRelative = false;
        int         spacing = 0;
        double      spacingFactor = 1.0f;
        QWidget*    w = nullptr;
    };

    void addInfo(const PosInfo& info) {
        if (info.w) {
            auto btn = dynamic_cast<QPushButton*>(info.w);
            if (btn != nullptr) {
                btn->setCheckable(info.checkable);
            }
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

#define ADD_NEW_BTN(ICON_NAME, TOOL_TIP) \
    info.w = new TimelineToolButton(ICON_NAME, i18n(TOOL_TIP), this); \
    m_manager->addInfo(info);

#define GET_KDE_ACTION(__ENUM__) \
    pCore->window()->actionCollection()->action(KStandardAction::name(KStandardAction::StandardAction::__ENUM__))

#define ACTION_COLL(__name__) \
    pCore->window()->actionCollection()->action(QStringLiteral(__name__))

TimelineToolBar::TimelineToolBar(QWidget* parent)
    : QFrame(parent)
    , m_manager(new ToolBtnLayoutManager(static_cast<QWidget*>(this)))
{
    using PosInfo = ToolBtnLayoutManager::PosInfo;
    
    auto controller = pCore->getMonitor(Kdenlive::ProjectMonitor)->getControllerProxy();
    
    setFixedHeight(40);
    
    PosInfo info = {
        .isRelative = true,
        .spacingFactor = 0.0125f,
        .w = new TimelineToolButton("undo", i18n("上一步"), this)
    };
    m_manager->addInfo(info);
    auto undo = GET_KDE_ACTION(Undo);
    connect(undo, &QAction::changed, this, [info, undo] {
        static_cast<TimelineToolButton*>(info.w)->setEnabled(undo->isEnabled());
    });
    connect(static_cast<TimelineToolButton*>(info.w), &TimelineToolButton::clicked, undo, &QAction::triggered);    
    
    info.spacingFactor = 0.00417f;
    ADD_NEW_BTN("redo", "下一步");
    auto redo = GET_KDE_ACTION(Redo);
    connect(redo, &QAction::changed, this, [info, redo] {
        static_cast<TimelineToolButton*>(info.w)->setEnabled(redo->isEnabled());
    });
    connect(static_cast<TimelineToolButton*>(info.w), &TimelineToolButton::clicked, redo, &QAction::triggered);    
    
    ADD_NEW_BTN("record_audio", "录音");
    
    ADD_NEW_BTN("audio_to_text", "音频文字互转");
    
    ADD_NEW_BTN("record_stream", "直播剪辑/录屏");
    
    ADD_NEW_BTN("keyframes", "关键帧");

    ADD_NEW_BTN("speed_adjust", "变速");
    
    auto addGuide = ACTION_COLL("add_guide");
    ADD_NEW_BTN("mark", "标记");
    connect(static_cast<TimelineToolButton*>(info.w), &TimelineToolButton::clicked, addGuide, &QAction::triggered);    
    
    ADD_NEW_BTN("clip", "裁剪");
    
    ADD_NEW_BTN("gb", "绿屏抠图");
    
    ADD_NEW_BTN("snapshot", "快照");
    
    ADD_NEW_BTN("mask", "蒙版");
    
    info.spacingFactor = 0.12f;
    ADD_NEW_BTN("prev_keypoint", "跳到上一个关键位置");
    connect(static_cast<TimelineToolButton*>(info.w), &TimelineToolButton::clicked, controller, &MonitorProxy::seekPreviousKeyframe);
    
    info.spacingFactor = 0.003125f;
    ADD_NEW_BTN("prev_frame", "前一帧");
    connect(static_cast<TimelineToolButton*>(info.w), &TimelineToolButton::clicked, this, [controller] {
        emit controller->requestSeek(controller->getPosition() - 1);
    });
    
    ADD_NEW_BTN("play", "播放");
    connect(static_cast<TimelineToolButton*>(info.w), &TimelineToolButton::clicked, this, [controller] {
        controller->setPlaying(!controller->playing());
    });   
    connect(controller, &MonitorProxy::playingChanged, this, [info, controller] () {
        static QIcon playIcon(":/classic/components/ptcontroller/play.png");
        static QIcon pauseIcon(":/classic/components/ptcontroller/pause.png");
        auto btn = static_cast<TimelineToolButton*>(info.w);
        
        if (controller->playing()) {
            btn->setIcon(pauseIcon);
        } else {
            btn->setIcon(playIcon);
        }
    });
    
    
    ADD_NEW_BTN("next_frame", "后一帧");
    connect(static_cast<TimelineToolButton*>(info.w), &TimelineToolButton::clicked, this, [controller] {
        emit controller->requestSeek(controller->getPosition() + 1);
    });
    
    ADD_NEW_BTN("next_keypoint", "跳到下一个关键位置");
    connect(static_cast<TimelineToolButton*>(info.w), &TimelineToolButton::clicked, controller, &MonitorProxy::seekNextKeyframe);
    
    info.spacingFactor = 0.00417f;
    ADD_NEW_BTN("volume", "音量");
    
    ADD_NEW_BTN("fullscreen", "全屏");
    connect(static_cast<TimelineToolButton*>(info.w), &TimelineToolButton::clicked, pCore->getMonitor(Kdenlive::ProjectMonitor), &Monitor::slotSwitchFullScreen);    
    
    
    ADD_NEW_BTN("scale_level", "缩放级别");
    
    ADD_NEW_BTN("safe_margin", "安全边距");
    
    ADD_NEW_BTN("grid", "网格");
    
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
