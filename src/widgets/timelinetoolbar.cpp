#include "timelinetoolbar.h"

#include "mainwindow.h"
#include "core.h"

#include "timeline2/view/timelinetabs.hpp"
#include "macros.hpp"
#include "doc/kdenlivedoc.h"
#include "appconsts.h"
#include "timeline2/view/timelinewidget.h"
#include "timeline2/view/timelinecontroller.h"
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
    QPushButton::disabled {
        color: #000000;
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
                curPos += dockWidth * posInfo.spacingFactor * dockWidth / 1920;
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
    setStyleSheet(QString(R"(
        QLabel {        
            font-size: 10px;
            color: #9F9F9F;
            border-bottom-color: #9F9F9F;
            border-bottom-width: 1px;
            border-bottom-style: %1;
        }
    )").arg(APPCONSTS.foregroundColor.name()));
}

void TimelineTimecodeLabel::paintEvent(QPaintEvent* pe) {
    QLabel::paintEvent(pe);
    QPainter p(this);
    p.setPen(QPen(APPCONSTS.foregroundColor, 1));
    constexpr auto dashCount = 10;
    
    auto dashWidth = width() / dashCount + 1;
    
    for (auto i = 0; i < dashCount; i++) {
        p.drawLine(3 + i * dashWidth, height() - 2, 3 + (i * dashWidth) - dashWidth / 2, height() - 2);
    }
}










// 时间线工具栏构造函数


#define ADD_NEW_BTN(ICON_NAME, TOOL_TIP) \
    info.w = new TimelineToolButton(ICON_NAME, i18n(TOOL_TIP), this); \
    m_manager->addInfo(info);

#define GET_KDE_ACTION(__ENUM__) \
    pCore->window()->actionCollection()->action(KStandardAction::name(KStandardAction::StandardAction::__ENUM__))

#define ACTION_COLL(__name__) \
    pCore->window()->actionCollection()->action(QStringLiteral(__name__))

TimelineToolBar::TimelineToolBar(TimelineTabs* tabs, QWidget* parent)
    : QFrame(parent)
    , m_manager(new ToolBtnLayoutManager(static_cast<QWidget*>(this)))
    , m_timelineTabs(tabs)
{
    setStyleSheet(QString(R"(QFrame { background-color: %1; })").arg(APPCONSTS.darkerSpaceColor.name()));
    
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
    connect(static_cast<TimelineToolButton*>(info.w), &TimelineToolButton::clicked, [] {
        pCore->window()->getMainTimeline()->controller()->gotoPreviousSnap();
    });
    
    info.spacingFactor = 0.003125f;
    ADD_NEW_BTN("prev_frame", "前一帧");
    connect(static_cast<TimelineToolButton*>(info.w), &TimelineToolButton::clicked, this, [controller] {
        controller->setPosition(controller->getPosition() - 1);
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
        controller->setPosition(controller->getPosition() + 1);
    });
    
    ADD_NEW_BTN("next_keypoint", "跳到下一个关键位置");
    connect(static_cast<TimelineToolButton*>(info.w), &TimelineToolButton::clicked, [] {
        pCore->window()->getMainTimeline()->controller()->gotoNextSnap();
    });
    
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
    connect(
        static_cast<QPushButton*>(info.w), &QPushButton::clicked, 
        ACTION_COLL("prerender_timeline_zone"), &QAction::triggered
    );
    
    m_manager->addInfo(info);
    
    info.w = new TimelineTimecodeLabel(this);
    static_cast<TimelineTimecodeLabel*>(info.w)->setText("00:00:00:00 ");
    connect(controller, &MonitorProxy::positionChanged, this, [info, controller] {
        auto label = static_cast<TimelineTimecodeLabel*>(info.w);
        label->setText(controller->timecode());
        label->adjustSize();
    });
    info.w->adjustSize();
    m_manager->addInfo(info);
    
    // 缩窄时间线
    info.spacingFactor = 0.191666666666667f;
    ADD_NEW_BTN("timeline_narrow", "缩窄时间线");
    auto zoomIn = static_cast<TimelineToolButton*>(info.w);
    
    // 设置时间线缩放滑动条
    info.spacingFactor = 0.003125f;
    auto scaleSlider = new QSlider(Qt::Horizontal, this);
    
    scaleSlider->setRange(0, 20);
    scaleSlider->setPageStep(1);
    scaleSlider->setInvertedAppearance(true);
    scaleSlider->setInvertedControls(true);
    
    scaleSlider->setMaximumWidth(150);
    scaleSlider->setMinimumWidth(100);
    scaleSlider->setFixedSize(130, 30);
    
    scaleSlider->setStyleSheet(QString(R"(
    QSlider::groove:horizontal {
        border: 0px;
        height: 5px;
        background-color: #FFFFFF;
        margin: 0px;
    }
    
    QSlider::handle:horizontal {
        background-color: %1;
        border: 0px;
        width: 14px;
        height: 14px;
        margin: 0;
        border-radius: 7px;
    })").arg(APPCONSTS.foregroundColor.name()));
    info.w = scaleSlider;
    m_manager->addInfo(info);
    
    // 拓宽时间线
    ADD_NEW_BTN("timeline_boost", "拓宽时间线");
    auto zoomOut = static_cast<TimelineToolButton*>(info.w);
    
    // 时间线缩放条信号槽
    connect(m_timelineTabs, &TimelineTabs::updateZoom, this, [scaleSlider, zoomIn, zoomOut](int value) {
        KdenliveDoc *project = pCore->currentDoc();
        if (project) {
            project->setZoom(value);
        }
        zoomIn->setEnabled(value <= scaleSlider->maximum());
        zoomOut->setEnabled(value >= scaleSlider->minimum());
        QSignalBlocker blocker(scaleSlider);
        scaleSlider->setValue(value);
    });
    // valueChanged
    connect(scaleSlider, &QSlider::valueChanged, [this, scaleSlider](int value) {
        value = qBound(scaleSlider->minimum(), value, scaleSlider->maximum());        
        emit m_timelineTabs->changeZoom(value, false);
        LOG_DEBUG() << "coco";
    });
    // zoomIn
    connect(zoomIn, &QPushButton::clicked, [this, scaleSlider] {
        int value = scaleSlider->value() + 1;
        value = qBound(scaleSlider->minimum(), value, scaleSlider->maximum());
        emit m_timelineTabs->changeZoom(value, false);
        emit m_timelineTabs->updateZoom(value);
    });
    // zoomOut
    connect(zoomOut, &QPushButton::clicked, [this, scaleSlider] {
        int value = scaleSlider->value() - 1;
        value = qBound(scaleSlider->minimum(), value, scaleSlider->maximum());
        emit m_timelineTabs->changeZoom(value, false);
        emit m_timelineTabs->updateZoom(value);
    });
    
    // 时间轴自适应按钮
    info.spacingFactor = 0.00416666666666667f;
    ADD_NEW_BTN("timeline_fit", "时间轴自适应");
    connect(static_cast<TimelineToolButton*>(info.w), &QPushButton::clicked, [this] {
        emit m_timelineTabs->fitZoom();
    });    
    
    info.spacingFactor = 0.0078125f;
    info.w = new QPushButton(i18n("导出"), this);
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
}

void TimelineToolBar::resizeEvent(QResizeEvent*) {
    m_manager->fitPosition();
}
