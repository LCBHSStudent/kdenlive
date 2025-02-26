/***************************************************************************
 *   Copyright (C) 2007 by Jean-Baptiste Mardelle (jb@kdenlive.org)        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#include "mainwindow.h"
#include "assetcontroller.hpp"
#include "assets/assetpanel.hpp"
#include "bin/clipcreator.hpp"
#include "bin/generators/generators.h"
#include "bin/model/subtitlemodel.hpp"
#include "bin/projectclip.h"
#include "bin/projectfolder.h"
#include "bin/projectitemmodel.h"
#include "core.h"
#include "dialogs/clipcreationdialog.h"
#include "dialogs/kdenlivesettingsdialog.h"
#include "dialogs/renderwidget.h"
#include "dialogs/wizard.h"
#include "dialogs/subtitleedit.h"
#include "doc/docundostack.hpp"
#include "doc/kdenlivedoc.h"
#include "dockareaorientationmanager.h"
#include "effects/effectlist/view/effectlistwidget.hpp"
#include "effectslist/effectbasket.h"
#include "hidetitlebars.h"
#include "jobs/scenesplittask.h"
#include "jobs/transcodetask.h"
#include "jobs/stabilizetask.h"
#include "jobs/speedtask.h"
#include "jobs/audiolevelstask.h"
#include "kdenlivesettings.h"
#include "layoutmanagement.h"
#include "library/librarywidget.h"
#include "audiomixer/mixermanager.hpp"
#include "mainwindowadaptor.h"
#include "mltconnection.h"
#include "mltcontroller/clipcontroller.h"
#include "monitor/monitor.h"
#include "monitor/monitormanager.h"
#include "monitor/scopes/audiographspectrum.h"
#include "onlineresources/resourcewidget.hpp"
#include "profiles/profilemodel.hpp"
#include "project/cliptranscode.h"
#include "project/dialogs/archivewidget.h"
#include "project/dialogs/projectsettings.h"
#include "project/projectcommands.h"
#include "project/projectmanager.h"
#include "scopes/scopemanager.h"
#include "timeline2/view/timelinecontroller.h"
#include "timeline2/view/timelinetabs.hpp"
#include "timeline2/view/timelinewidget.h"
#include "titler/titlewidget.h"
#include "transitions/transitionlist/view/transitionlistwidget.hpp"
#include "transitions/transitionsrepository.hpp"
//#include "utils/resourcewidget_old.h" //TODO
#include "utils/thememanager.h"
#include "utils/otioconvertions.h"
#include "utils/framelesshelper/FramelessHelper.h"
#include "utils/thumbnailcache.hpp"
#include "lib/localeHandling.h"
#include "profiles/profilerepository.hpp"
#include "widgets/progressbutton.h"
#include "widgets/custommenu.h"
#include "widgets/customtooltip.h"
#include "widgets/topnavigationbar.h"
#include "widgets/customeditortoolbar.h"
#include "widgets/projectmonitorframe.h"
#include "widgets/timelinetoolbar.h"
#include "widgets/projectsettingswidget.h"
#include "widgets/menubareventpasser.h"
#include "widgets/clipmonitorframe.h"
#include "widgets/tabswidget.h"
#include <config-kdenlive.h>
#include "dialogs/textbasededit.h"
#include "project/dialogs/temporarydata.h"

#include <framework/mlt_version.h>

#ifdef USE_JOGSHUTTLE
#include "jogshuttle/jogmanager.h"
#endif

#include <KAboutData>
#include <KCoreAddons>
#include <KActionCategory>
#include <KActionCollection>
#include <KActionMenu>
#include <KColorScheme>
#include <KColorSchemeManager>
#include <KConfigDialog>
#include <KDualAction>
#include <KEditToolBar>
#include <KIconTheme>
#include <KMessageBox>
#include <KNotifyConfigWidget>
#include <KRecentDirs>
#include <KShortcutsDialog>
#include <KStandardAction>
#include <KToolBar>
#include <KXMLGUIFactory>
#include <klocalizedstring.h>
#include <kns3/downloaddialog.h>
#include <kns3/knewstuffaction.h>
#include <ktogglefullscreenaction.h>
#include <kwidgetsaddons_version.h>
#include <KRun>

#include "kdenlive_debug.h"
#include <QAction>
#include <QFileDialog>
#include <QInternal>
#include <QMenuBar>
#include <QStatusBar>
#include <QStyleFactory>
#include <QUndoGroup>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QScreen>
#include <QStandardPaths>
#include <QVBoxLayout>

#include "macros.hpp"

constexpr auto __menuBarHeight = 42;
constexpr auto __windowCtrlBtnWidth = 54;
constexpr auto __ltLabelWidth = 118;
constexpr auto __menuTabWidth = 48;

static const char version[] = SMARTIP_EDITOR_VERSION;
namespace Mlt {
class Producer;
}

QMap<QString, QImage> MainWindow::m_lumacache;
QMap<QString, QStringList> MainWindow::m_lumaFiles;

/*static bool sortByNames(const QPair<QString, QAction *> &a, const QPair<QString, QAction*> &b)
{
    return a.first < b.first;
}*/

// determine the default KDE style as defined BY THE USER
// (as opposed to whatever style KDE considers default)
static QString defaultStyle(const char *fallback = nullptr)
{
    KSharedConfigPtr kdeGlobals = KSharedConfig::openConfig(QStringLiteral("kdeglobals"), KConfig::NoGlobals);
    KConfigGroup cg(kdeGlobals, "KDE");
    qDebug() << cg.readEntry("widgetStyle", fallback);
    
    
    return cg.readEntry("widgetStyle", fallback);
}

static bool eventDebugCallback(void** data) {
    QEvent *event = reinterpret_cast<QEvent *>(data[1]);

    if (event->type() == QEvent::KeyPress ||
        event->type() == QEvent::KeyRelease
    ) {
        QObject *receiver = reinterpret_cast<QObject *>(data[0]);
        qDebug() << event << "->" << receiver;

    } else if (
//        event->type() == QEvent::Drop ||
//        event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::ContextMenu
    ) {
        QObject *receiver = reinterpret_cast<QObject *>(data[0]);
        qDebug() << event << "->" << receiver;
    }

    return false;
}

// posChange(-8, -31): 119 217 / 127 248 / 135 279 / 143 310

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
{
    hide();
}

#ifndef DEBUG_BUILD
class MyProxyStyle: public QProxyStyle {
public:
    int pixelMetric(
        QStyle::PixelMetric metric,
        const QStyleOption* option = nullptr,
        const QWidget*      widget = nullptr
    ) const override {
        if (metric == QStyle::PixelMetric::PM_ButtonShiftHorizontal | 
            metric == QStyle::PixelMetric::PM_ButtonShiftVertical
        ) {
            return 0;
        } else {
            return QProxyStyle::pixelMetric(metric, option, widget);
        }
    }
    
};

#endif

void MainWindow::init(const QString &mltPath) {
    setMinimumWidth(1024);
    qApp->setStyleSheet(qApp->styleSheet() + R"( * { font-family: "Microsoft YaHei"; } )");

#ifndef DEBUG_BUILD
    qApp->setStyle(new MyProxyStyle);
#endif

    QString desktopStyle = QApplication::style()->objectName();
    // Load themes
    auto themeManager = new ThemeManager(actionCollection());
    actionCollection()->addAction(QStringLiteral("themes_menu"), themeManager);
    connect(themeManager, &ThemeManager::themeChanged, this, &MainWindow::slotThemeChanged);

    CustomToolTip::installToolTip(this);
    
    if (!KdenliveSettings::widgetstyle().isEmpty() && QString::compare(desktopStyle, KdenliveSettings::widgetstyle(), Qt::CaseInsensitive) != 0) {
        // User wants a custom widget style, init
        doChangeStyle();
    }

    if (!qEnvironmentVariableIsEmpty("EVENT_DEBUG")) {
        QInternal::registerCallback(QInternal::EventNotifyCallback, eventDebugCallback);        
    }
    
    // Widget themes for non KDE users
    KActionMenu *stylesAction = new KActionMenu(i18n("Style"), this);
    stylesAction->setDefaultWidget(new CustomMenu(this));
    
    auto *stylesGroup = new QActionGroup(stylesAction);

    // GTK theme does not work well with Kdenlive, and does not support color theming, so avoid it
    QStringList availableStyles = QStyleFactory::keys();
    if (KdenliveSettings::widgetstyle().isEmpty()) {
        // First run
        QStringList incompatibleStyles = {QStringLiteral("GTK+"), QStringLiteral("windowsvista"), QStringLiteral("Windows")};

        if (incompatibleStyles.contains(desktopStyle, Qt::CaseInsensitive)) {
            if (availableStyles.contains(QStringLiteral("breeze"), Qt::CaseInsensitive)) {
                // Auto switch to Breeze theme
                KdenliveSettings::setWidgetstyle(QStringLiteral("Breeze"));
		QApplication::setStyle(QStyleFactory::create(QStringLiteral("Breeze")));
            } else if (availableStyles.contains(QStringLiteral("fusion"), Qt::CaseInsensitive)) {
                KdenliveSettings::setWidgetstyle(QStringLiteral("Fusion"));
		QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
            }
        } else {
            KdenliveSettings::setWidgetstyle(QStringLiteral("Default"));
        }
    }
    

    // Add default style action
    QAction *defaultStyle = new QAction(i18n("Default"), stylesGroup);
    defaultStyle->setData(QStringLiteral("Default"));
    defaultStyle->setCheckable(true);
    stylesAction->addAction(defaultStyle);
    if (KdenliveSettings::widgetstyle() == QLatin1String("Default") || KdenliveSettings::widgetstyle().isEmpty()) {
        defaultStyle->setChecked(true);
    }

    for (const QString &style : qAsConst(availableStyles)) {
        auto *a = new QAction(style, stylesGroup);
        a->setCheckable(true);
        a->setData(style);
        if (KdenliveSettings::widgetstyle() == style) {
            a->setChecked(true);
        }
        stylesAction->addAction(a);
    }
    connect(stylesGroup, &QActionGroup::triggered, this, &MainWindow::slotChangeStyle);
    // QIcon::setThemeSearchPaths(QStringList() <<QStringLiteral(":/icons/"));

    new RenderingAdaptor(this);
    QString defaultProfile = KdenliveSettings::default_profile();
    
    // Initialise MLT connection
    MltConnection::construct(mltPath);
    pCore->setCurrentProfile(defaultProfile.isEmpty() ? ProjectManager::getDefaultProjectFormat() : defaultProfile);
    m_commandStack = new QUndoGroup();

    // If using a custom profile, make sure the file exists or fallback to default
    QString currentProfilePath = pCore->getCurrentProfile()->path();
    if (currentProfilePath.startsWith(QLatin1Char('/')) && !QFile::exists(currentProfilePath)) {
        KMessageBox::sorry(this, i18n("Cannot find your default profile, switching to ATSC 1080p 25"));
        pCore->setCurrentProfile(QStringLiteral("atsc_1080p_25"));
        KdenliveSettings::setDefault_profile(QStringLiteral("atsc_1080p_25"));
    }
    m_gpuAllowed = EffectsRepository::get()->hasInternalEffect(QStringLiteral("glsl.manager"));

    m_shortcutRemoveFocus = new QShortcut(QKeySequence(QStringLiteral("Esc")), this);
    connect(m_shortcutRemoveFocus, &QShortcut::activated, this, &MainWindow::slotRemoveFocus);

    /// Add Widgets
    setDockOptions(dockOptions() | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
    setDockOptions(dockOptions() | QMainWindow::GroupedDragging);
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::TabPosition(KdenliveSettings::tabposition()));
    
    m_timelineToolBarContainer = new TimelineContainer(this);
    
    
    
    auto *ctnLay = new QVBoxLayout;
    ctnLay->setSpacing(0);
    ctnLay->setContentsMargins(0, 0, 0, 0);
    m_timelineToolBarContainer->setLayout(ctnLay);
    
    QSplitter* splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    ctnLay->addWidget(splitter);
    splitter->setStyleSheet(R"(QSplitter::handle { background-color: #292833; })");
    splitter->setHandleWidth(1);
    
    // setup centralWidget toolbar
    m_editorToolBar = new CustomEditorToolBar(m_timelineToolBarContainer);
    {
        connect(
            this, &MainWindow::windowTitleChanged,
        [this] {
            QString&& title = this->windowTitle();
            auto index = title.lastIndexOf('[');
            title = title.mid(0, index);
            
            this->m_editorToolBar->setDocumentString(title);
        });
    }
    
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup mainConfig(config, QStringLiteral("MainWindow"));

    // 这里包括了assetPanel的初始化
    setupActions();
    auto *layoutManager = new LayoutManagement(this);

    m_clipMonitor = new Monitor(Kdenlive::ClipMonitor, pCore->monitorManager(), this);
    pCore->bin()->setMonitor(m_clipMonitor);
    connect(m_clipMonitor, &Monitor::addMarker, this, &MainWindow::slotAddMarkerGuideQuickly);
    connect(m_clipMonitor, &Monitor::deleteMarker, this, &MainWindow::slotDeleteClipMarker);
    connect(m_clipMonitor, &Monitor::seekToPreviousSnap, this, &MainWindow::slotSnapRewind);
    connect(m_clipMonitor, &Monitor::seekToNextSnap, this, &MainWindow::slotSnapForward);

    connect(pCore->bin(), &Bin::findInTimeline, this, &MainWindow::slotClipInTimeline, Qt::DirectConnection);
    connect(pCore->bin(), &Bin::setupTargets, this, [&] (bool hasVideo, QMap <int, QString> audioStreams) {
            getCurrentTimeline()->controller()->setTargetTracks(hasVideo, audioStreams);
        }
    );

    connect(m_clipMonitor, &Monitor::passKeyPress, this, &MainWindow::triggerKey);
    m_clipMonitorFrame = new ClipMonitorFrame(m_clipMonitor, this);    
    
    m_projectMonitor = new Monitor(Kdenlive::ProjectMonitor, pCore->monitorManager(), this);
    connect(m_projectMonitor, &Monitor::passKeyPress, this, &MainWindow::triggerKey);
    connect(m_projectMonitor, &Monitor::addMarker, this, &MainWindow::slotAddMarkerGuideQuickly);
    connect(m_projectMonitor, &Monitor::deleteMarker, this, &MainWindow::slotDeleteGuide);
    connect(m_projectMonitor, &Monitor::seekToPreviousSnap, this, &MainWindow::slotSnapRewind);
    connect(m_projectMonitor, &Monitor::seekToNextSnap, this, &MainWindow::slotSnapForward);
    connect(m_loopClip, &QAction::triggered, this, [&]() {
        QPoint inOut = getMainTimeline()->controller()->selectionInOut();
        m_projectMonitor->slotLoopClip(inOut);
    });
    m_projectMonitorFrame = new ProjectMonitorFrame(m_projectMonitor, this);
    m_projectMonitorFrame->setAssetPanel(m_assetPanel);
    
    // TODO deprecated, replace with Bin methods if necessary
    /*connect(m_projectList, SIGNAL(loadingIsOver()), this, SLOT(slotElapsedTime()));
    connect(m_projectList, SIGNAL(updateRenderStatus()), this, SLOT(slotCheckRenderStatus()));
    connect(m_projectList, SIGNAL(updateProfile(QString)), this, SLOT(slotUpdateProjectProfile(QString)));
    connect(m_projectList, SIGNAL(refreshClip(QString,bool)), pCore->monitorManager(), SLOT(slotRefreshCurrentMonitor(QString)));
    connect(m_clipMonitor, SIGNAL(zoneUpdated(QPoint)), m_projectList, SLOT(slotUpdateClipCut(QPoint)));*/ 
    
    auto __toolPlusFrame = new QWidget(this);
    auto toolPlusFrameLayout = new QVBoxLayout(__toolPlusFrame);
    toolPlusFrameLayout->setContentsMargins(0, 0, 0, 0);
    toolPlusFrameLayout->setSpacing(0);
    toolPlusFrameLayout->addWidget(m_editorToolBar);
    toolPlusFrameLayout->addWidget(m_projectMonitorFrame);
    
    __toolPlusFrame->setLayout(toolPlusFrameLayout);
    
    splitter->addWidget(__toolPlusFrame);
    

    pCore->monitorManager()->initMonitors(m_clipMonitor, m_projectMonitor);
    connect(m_clipMonitor, &Monitor::addMasterEffect, pCore->bin(), &Bin::slotAddEffect);

    m_timelineTabs = new TimelineTabs(this);
    
    auto tabsPlusToolbar = new QWidget(this);
    auto __plusLayout = new QVBoxLayout(tabsPlusToolbar);
    __plusLayout->setContentsMargins(0, 0, 0, 0);
    __plusLayout->setSpacing(0);
    
    
    m_timelineToolBar = new TimelineToolBar(m_timelineTabs, this);
    
    __plusLayout->addWidget(m_timelineToolBar);
    __plusLayout->addWidget(m_timelineTabs);
    
    tabsPlusToolbar->setLayout(__plusLayout);
    
    splitter->addWidget(tabsPlusToolbar);
    setCentralWidget(m_timelineToolBarContainer);
    
    // 禁用缩放窗口时的时间线自动缩放
    splitter->setStretchFactor(1, 0);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    m_timelineTabs->resize(width(), m_mwSettings.value("timelineHeight").toInt());
    
    // Screen grab widget
    QWidget *grabWidget = new QWidget(this);
    auto *grabLayout = new QVBoxLayout;
    grabWidget->setLayout(grabLayout);
    auto *recToolbar = new QToolBar(grabWidget);
    grabLayout->addWidget(recToolbar);
    grabLayout->addStretch(10);
    // Check number of monitors for FFmpeg screen capture
    int screens = QApplication::screens().count();
    if (screens > 1) {
        auto *screenCombo = new QComboBox(recToolbar);
        for (int ix = 0; ix < screens; ix++) {
            screenCombo->addItem(i18n("Monitor %1", ix));
        }
        connect(screenCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), m_clipMonitor, &Monitor::slotSetScreen);
        recToolbar->addWidget(screenCombo);
        // Update screen grab monitor choice in case we changed from fullscreen
        screenCombo->setEnabled(KdenliveSettings::grab_capture_type() == 0);
    }
    QAction *recAction = m_clipMonitor->recAction();
    addAction(QStringLiteral("screengrab_record"), recAction);
    recToolbar->addAction(recAction);
    QAction *recConfig = new QAction(QIcon::fromTheme(QStringLiteral("configure")), i18n("Configure Recording"), this);
    recToolbar->addAction(recConfig);
    connect(recConfig, &QAction::triggered, [&]() {
        emit pCore->showConfigDialog(4, 0);
    });
    
    // Audio spectrum scope
    m_audioSpectrum = new AudioGraphSpectrum(pCore->monitorManager());
    
    // Project bin
    // pCore->bin();

    // Media browser widget
//    QDockWidget* clipDockWidget = addDock(i18n("Media Browser"), QStringLiteral("bin_clip"), pCore->bin()->getWidget());
//    pCore->bin()->dockWidgetInit(clipDockWidget);
    
    auto projectBin = pCore->bin();
    
    projectBin->move(20, 87);
    projectBin->setParent(this);
    projectBin->setMinimumSize(314, 433);
    // projectBin->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    
    // Online resources widget
    auto *onlineResources = new ResourceWidget(this);
    onlineResources->hide();
    connect(onlineResources, &ResourceWidget::previewClip, [&](const QString &path, const QString &title) {
    qDebug()<<"MLT VER: "<<LIBMLT_VERSION_INT<<"; QTVER: "<<QT_VERSION_CHECK(6,26,0);
#if LIBMLT_VERSION_INT == QT_VERSION_CHECK(6,26,0)
        new KRun(QUrl(path), this);
#else
        m_clipMonitor->slotPreviewResource(path, title);
#endif
    });

    connect(onlineResources, &ResourceWidget::addClip, this, &MainWindow::slotAddProjectClip);
    connect(onlineResources, &ResourceWidget::addLicenseInfo, this, &MainWindow::slotAddTextNote);

//    m_effectStackDock = addDock(i18n("Effect/Composition Stack"), QStringLiteral("effect_stack"), m_assetPanel);
    
    connect(m_assetPanel, &AssetPanel::doSplitEffect, m_projectMonitor, &Monitor::slotSwitchCompare);
    connect(m_assetPanel, &AssetPanel::doSplitBinEffect, m_clipMonitor, &Monitor::slotSwitchCompare);
    connect(m_assetPanel, &AssetPanel::switchCurrentComposition, this, [&](int cid, const QString &compositionId) {
        getMainTimeline()->controller()->getModel()->switchComposition(cid, compositionId);
    });

    connect(m_timelineTabs, &TimelineTabs::showMixModel, m_assetPanel, &AssetPanel::showMix);
    connect(m_timelineTabs, &TimelineTabs::showTransitionModel, m_assetPanel, &AssetPanel::showTransition);
    connect(m_timelineTabs, &TimelineTabs::showTransitionModel, this, [&] () {
        m_assetPanel->raise();
    });
    connect(m_timelineTabs, &TimelineTabs::showItemEffectStack, m_assetPanel, &AssetPanel::showEffectStack);
    connect(m_timelineTabs, &TimelineTabs::showItemEffectStack, this, [&] () {
        m_assetPanel->raise();
    });
    
    
    connect(m_timelineTabs, &TimelineTabs::showSubtitle, this, [&] (int id) {
        pCore->subtitleWidget()->setActiveSubtitle(id);
    });

    connect(pCore->bin(), &Bin::requestShowEffectStack, m_assetPanel, &AssetPanel::showEffectStack);
    connect(pCore->bin(), &Bin::requestShowEffectStack, [&] () {
        // Don't raise effect stack on clip bin in case it is docked with bin or clip monitor
        // m_effectStackDock->raise();
    });
    connect(this, &MainWindow::clearAssetPanel, m_assetPanel, &AssetPanel::clearAssetPanel, Qt::DirectConnection);
    connect(this, &MainWindow::assetPanelWarning, m_assetPanel, &AssetPanel::assetPanelWarning);
    connect(m_assetPanel, &AssetPanel::seekToPos, this, [this](int pos) {
        ObjectId oId = m_assetPanel->effectStackOwner();
        switch (oId.first) {
        case ObjectType::TimelineTrack:
        case ObjectType::TimelineClip:
        case ObjectType::TimelineComposition:
        case ObjectType::Master:
        case ObjectType::TimelineMix:
            m_projectMonitor->requestSeek(pos);
            break;
        case ObjectType::BinClip:
            m_clipMonitor->requestSeek(pos);
            break;
        default:
            qDebug() << "ERROR unhandled object type";
            break;
        }
    });   
   

    // 设置左侧合成-效果工具栏
    {
        m_effectList2 = new EffectListWidget(this);
        connect(m_effectList2, &EffectListWidget::activateAsset, pCore->projectManager(), &ProjectManager::activateAsset);
        connect(m_assetPanel, &AssetPanel::reloadEffect, m_effectList2, &EffectListWidget::reloadCustomEffect);
    
        m_transitionList2 = new TransitionListWidget(this);
        m_effectTransWidget = new TabsWidget(this);
        m_effectTransWidget->resize(400, 500);
        
        m_effectTransWidget->addTab(m_transitionList2, i18n("合成"));
        m_effectTransWidget->addTab(m_effectList2, i18n("效果"));
    }
    
    
    m_undoView = new QUndoView(this);
    m_undoView->setEmptyLabel(i18n("新建/打开"));
    m_undoView->setGroup(m_commandStack);

    // Color and icon theme stuff
    connect(m_commandStack, &QUndoGroup::cleanChanged, m_saveAction, &QAction::setDisabled);
    addAction(QStringLiteral("styles_menu"), stylesAction);

    QAction *iconAction = new QAction(i18n("Force Breeze Icon Theme"), this);
    iconAction->setCheckable(true);
    iconAction->setChecked(KdenliveSettings::force_breeze());
    addAction(QStringLiteral("force_icon_theme"), iconAction);
    connect(iconAction, &QAction::triggered, this, &MainWindow::forceIconSet);

    // Audio Mixer
    // pCore->mixer()
    QAction *showMixer = new QAction(QIcon::fromTheme(QStringLiteral("view-media-equalizer")), i18n("Audio Mixer"), this);
    showMixer->setCheckable(true);
    addAction(QStringLiteral("audiomixer_button"), showMixer);
    // 连接Mixer    
//    connect(m_mixerDock, &QDockWidget::visibilityChanged, this, [&, showMixer](bool visible) {
//        pCore->mixer()->connectMixer(visible);
//        showMixer->setChecked(visible);
//    });
    connect(showMixer, &QAction::triggered, this, [&]() {
        
    });

    bool firstRun = readOptions();

    // Build effects menu
    m_effectsMenu = new CustomMenu(i18n("Add Effect"), this);
    m_effectActions = new KActionCategory(i18n("Effects"), actionCollection());
    m_effectList2->reloadEffectMenu(m_effectsMenu, m_effectActions);

    m_transitionsMenu = new CustomMenu(i18n("Add Transition"), this);
    m_transitionActions = new KActionCategory(i18n("Transitions"), actionCollection());

    auto *scmanager = new ScopeManager(this);

    auto *titleBars = new HideTitleBars(this);
    connect(layoutManager, &LayoutManagement::updateTitleBars, titleBars, &HideTitleBars::updateTitleBars);
    new DockAreaOrientationManager(this);
    m_extraFactory = new KXMLGUIClient(this);
    buildDynamicActions();

    // Create Effect Basket (dropdown list of favorites)
    m_effectBasket = new EffectBasket(this);
    connect(m_effectBasket, &EffectBasket::activateAsset, pCore->projectManager(), &ProjectManager::activateAsset);
    connect(m_effectList2, &EffectListWidget::reloadFavorites, m_effectBasket, &EffectBasket::slotReloadBasket);
    auto *widgetlist = new QWidgetAction(this);
    widgetlist->setDefaultWidget(m_effectBasket);
    // widgetlist->setText(i18n("Favorite Effects"));
    widgetlist->setToolTip(i18n("Favorite Effects"));
    widgetlist->setIcon(QIcon::fromTheme(QStringLiteral("favorite")));
    auto *menu = new CustomMenu(this);
    menu->addAction(widgetlist);

    auto *basketButton = new QToolButton(this);
    basketButton->setMenu(menu);
    basketButton->setToolButtonStyle(toolBar()->toolButtonStyle());
    basketButton->setDefaultAction(widgetlist);
    basketButton->setPopupMode(QToolButton::InstantPopup);
    // basketButton->setText(i18n("Favorite Effects"));
    basketButton->setToolTip(i18n("Favorite Effects"));
    basketButton->setIcon(QIcon::fromTheme(QStringLiteral("favorite")));

    auto *toolButtonAction = new QWidgetAction(this);
    toolButtonAction->setText(i18n("Favorite Effects"));
    toolButtonAction->setIcon(QIcon::fromTheme(QStringLiteral("favorite")));
    toolButtonAction->setDefaultWidget(basketButton);
    addAction(QStringLiteral("favorite_effects"), toolButtonAction);
    connect(toolButtonAction, &QAction::triggered, basketButton, &QToolButton::showMenu);
    connect(m_effectBasket, &EffectBasket::activateAsset, menu, &CustomMenu::close);

    // Render button
    ProgressButton *timelineRender = new ProgressButton(i18n("Render"), 100, this);
    auto *tlrMenu = new CustomMenu(this);
    timelineRender->setMenu(tlrMenu);
    connect(this, &MainWindow::setRenderProgress, timelineRender, &ProgressButton::setProgress);
    auto *renderButtonAction = new QWidgetAction(this);
    renderButtonAction->setText(i18n("Render Button"));
    renderButtonAction->setIcon(QIcon::fromTheme(QStringLiteral("media-record")));
    renderButtonAction->setDefaultWidget(timelineRender);
    addAction(QStringLiteral("project_render_button"), renderButtonAction);

    // Timeline preview button
    ProgressButton *timelinePreview = new ProgressButton(i18n("Rendering preview"), 1000, this);
    auto *tlMenu = new CustomMenu(this);
    timelinePreview->setMenu(tlMenu);
    connect(this, &MainWindow::setPreviewProgress, timelinePreview, &ProgressButton::setProgress);
    auto *previewButtonAction = new QWidgetAction(this);
    previewButtonAction->setText(i18n("Timeline Preview"));
    previewButtonAction->setIcon(QIcon::fromTheme(QStringLiteral("preview-render-on")));
    previewButtonAction->setDefaultWidget(timelinePreview);
    addAction(QStringLiteral("timeline_preview_button"), previewButtonAction);

    setupGUI(KXmlGuiWindow::ToolBar | KXmlGuiWindow::StatusBar | KXmlGuiWindow::Create);
    // after-jobs
    {
        setStandardToolBarMenuEnabled(false);
        foreach (auto innerTB, toolBars()) {
            innerTB->hide();
            innerTB->deleteLater();
        }
    }
    
    
    LocaleHandling::resetLocale();
    if (firstRun) {
        if (QScreen *current = QApplication::primaryScreen()) {
            int screenHeight = current->availableSize().height();
            if (screenHeight < 1000) {
                resize(current->availableSize());
            } else if (screenHeight < 2000) {
                resize(current->availableSize() / 1.2);
            } else {
                resize(current->availableSize() / 1.6);
            }
        }
    }

    m_timelineToolBar->setProperty("otherToolbar", true);
    timelinePreview->setToolButtonStyle(Qt::ToolButtonFollowStyle);

    timelineRender->setToolButtonStyle(toolBar()->toolButtonStyle());
    /*ScriptingPart* sp = new ScriptingPart(this, QStringList());
    guiFactory()->addClient(sp);*/

    loadGenerators();
    loadDockActions();
    loadClipActions();

    // Timeline clip menu
    auto *timelineClipMenu = new CustomMenu(this);
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("edit_copy")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("paste_effects")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("delete_effects")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("group_clip")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("ungroup_clip")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("edit_item_duration")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("clip_split")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("clip_switch")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("delete_timeline_clip")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("extract_clip")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("save_to_bin")));

    CustomMenu *markerMenu = static_cast<CustomMenu *>(factory()->container(QStringLiteral("marker_menu"), this));
    timelineClipMenu->addMenu(markerMenu);

    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("set_audio_align_ref")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("align_audio")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("edit_item_speed")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("clip_in_project_tree")));
    timelineClipMenu->addAction(actionCollection()->action(QStringLiteral("cut_timeline_clip")));

    // Timeline composition menu
    auto *compositionMenu = new CustomMenu(this);
    compositionMenu->addAction(actionCollection()->action(QStringLiteral("edit_item_duration")));
    compositionMenu->addAction(actionCollection()->action(QStringLiteral("edit_copy")));
    compositionMenu->addAction(actionCollection()->action(QStringLiteral("delete_timeline_clip")));

    // Timeline main menu
    auto *timelineMenu = new CustomMenu(this);
    timelineMenu->addAction(actionCollection()->action(QStringLiteral("edit_paste")));
    timelineMenu->addAction(actionCollection()->action(QStringLiteral("insert_space")));
    timelineMenu->addAction(actionCollection()->action(QStringLiteral("delete_space")));
    timelineMenu->addAction(actionCollection()->action(QStringLiteral("delete_space_all_tracks")));
    timelineMenu->addAction(actionCollection()->action(QStringLiteral("add_guide")));
    timelineMenu->addAction(actionCollection()->action(QStringLiteral("edit_guide")));
    CustomMenu *guideMenu = new CustomMenu(i18n("Go to Guide..."), this);
    timelineMenu->addMenu(guideMenu);

    // Timeline ruler menu
    auto *timelineRulerMenu = new CustomMenu(this);
    timelineRulerMenu->addAction(actionCollection()->action(QStringLiteral("add_guide")));
    timelineRulerMenu->addAction(actionCollection()->action(QStringLiteral("edit_guide")));
    timelineRulerMenu->addAction(actionCollection()->action(QStringLiteral("lock_guides")));
    timelineRulerMenu->addMenu(guideMenu);
    timelineRulerMenu->addAction(actionCollection()->action(QStringLiteral("add_project_note")));
    timelineRulerMenu->addAction(actionCollection()->action(QStringLiteral("add_subtitle")));

    //Timeline subtitle menu
    auto *timelineSubtitleMenu = new CustomMenu(this);
    timelineSubtitleMenu->addAction(actionCollection()->action(QStringLiteral("edit_copy")));
    timelineSubtitleMenu->addAction(actionCollection()->action(QStringLiteral("delete_subtitle_clip")));

    // Timeline headers menu
    auto *timelineHeadersMenu = new CustomMenu(this);
    timelineHeadersMenu->addAction(actionCollection()->action(QStringLiteral("insert_track")));
    timelineHeadersMenu->addAction(actionCollection()->action(QStringLiteral("delete_track")));
    timelineHeadersMenu->addAction(actionCollection()->action(QStringLiteral("show_track_record")));

    QAction *separate_channels = new QAction(QIcon(), i18n("Separate Channels"), this);
    separate_channels->setCheckable(true);
    separate_channels->setChecked(KdenliveSettings::displayallchannels());
    separate_channels->setData("separate_channels");
    connect(separate_channels, &QAction::triggered, this, &MainWindow::slotSeparateAudioChannel);
    timelineHeadersMenu->addAction(separate_channels);
    
    QAction *normalize_channels = new QAction(QIcon(), i18n("Normalize Audio Thumbnails"), this);
    normalize_channels->setCheckable(true);
    normalize_channels->setChecked(KdenliveSettings::normalizechannels());
    normalize_channels->setData("normalize_channels");
    connect(normalize_channels, &QAction::triggered, this, &MainWindow::slotNormalizeAudioChannel);
    timelineHeadersMenu->addAction(normalize_channels);

    CustomMenu *thumbsMenu = new CustomMenu(i18n("Thumbnails"), this);
    auto *thumbGroup = new QActionGroup(this);
    QAction *inFrame = new QAction(i18n("In Frame"), thumbGroup);
    inFrame->setData(QStringLiteral("2"));
    inFrame->setCheckable(true);
    thumbsMenu->addAction(inFrame);
    QAction *inOutFrame = new QAction(i18n("In/Out Frames"), thumbGroup);
    inOutFrame->setData(QStringLiteral("0"));
    inOutFrame->setCheckable(true);
    thumbsMenu->addAction(inOutFrame);
    QAction *allFrame = new QAction(i18n("All Frames"), thumbGroup);
    allFrame->setData(QStringLiteral("1"));
    allFrame->setCheckable(true);
    thumbsMenu->addAction(allFrame);
    QAction *noFrame = new QAction(i18n("No Thumbnails"), thumbGroup);
    noFrame->setData(QStringLiteral("3"));
    noFrame->setCheckable(true);
    thumbsMenu->addAction(noFrame);

    CustomMenu *openGLMenu = static_cast<CustomMenu *>(factory()->container(QStringLiteral("qt_opengl"), this));
#if defined(Q_OS_WIN)
    connect(openGLMenu, &CustomMenu::triggered, [&](QAction *ac) {
        KdenliveSettings::setOpengl_backend(ac->data().toInt());
        if (KMessageBox::questionYesNo(this, i18n("SmartIP-Editor 需要重启来应用设置更改, 确定要继续吗?")) != KMessageBox::Yes) {
            return;
        }
        slotRestart(false);
    });
#else
    if (openGLMenu) {
        openGLMenu->menuAction()->setVisible(false);;
    }
#endif
    // Connect monitor overlay info menu.
    CustomMenu *monitorOverlay = static_cast<CustomMenu *>(factory()->container(QStringLiteral("monitor_config_overlay"), this));
    connect(monitorOverlay, &CustomMenu::triggered, this, &MainWindow::slotSwitchMonitorOverlay);

    m_projectMonitor->setupMenu(static_cast<CustomMenu *>(factory()->container(QStringLiteral("monitor_go"), this)), monitorOverlay, m_playZone, m_loopZone, nullptr,
                                m_loopClip);
    m_clipMonitor->setupMenu(static_cast<CustomMenu *>(factory()->container(QStringLiteral("monitor_go"), this)), monitorOverlay, m_playZone, m_loopZone,
                             static_cast<CustomMenu *>(factory()->container(QStringLiteral("marker_menu"), this)));

    CustomMenu *clipInTimeline = static_cast<CustomMenu *>(factory()->container(QStringLiteral("clip_in_timeline"), this));
    clipInTimeline->setIcon(QIcon::fromTheme(QStringLiteral("go-jump")));
    pCore->bin()->setupGeneratorMenu();

    connect(pCore->monitorManager(), &MonitorManager::updateOverlayInfos, this, &MainWindow::slotUpdateMonitorOverlays);

    // Setup and fill effects and transitions menus.
    CustomMenu *m = static_cast<CustomMenu *>(factory()->container(QStringLiteral("video_effects_menu"), this));
    connect(m, &CustomMenu::triggered, this, &MainWindow::slotAddEffect);
    connect(m_effectsMenu, &CustomMenu::triggered, this, &MainWindow::slotAddEffect);
    connect(m_transitionsMenu, &CustomMenu::triggered, this, &MainWindow::slotAddTransition);

    m_timelineContextMenu = new CustomMenu(this);

    m_timelineContextMenu->addAction(actionCollection()->action(QStringLiteral("insert_space")));
    m_timelineContextMenu->addAction(actionCollection()->action(QStringLiteral("delete_space")));
    m_timelineContextMenu->addAction(actionCollection()->action(QStringLiteral("delete_space_all_tracks")));
    m_timelineContextMenu->addAction(actionCollection()->action(KStandardAction::name(KStandardAction::Paste)));

    // CustomMenu *markersMenu = static_cast<CustomMenu *>(factory()->container(QStringLiteral("marker_menu"), this));

    /*m_timelineClipActions->addMenu(markersMenu);
    m_timelineClipActions->addSeparator();
    m_timelineClipActions->addMenu(m_transitionsMenu);
    m_timelineClipActions->addMenu(m_effectsMenu);*/

    slotConnectMonitors();

    // TODO: let user select timeline toolbar toolbutton style
    // connect(toolBar(), &QToolBar::iconSizeChanged, m_timelineToolBar, &QToolBar::setToolButtonStyle);
    m_timelineToolBar->setContextMenuPolicy(Qt::CustomContextMenu);

    QAction *prevRender = actionCollection()->action(QStringLiteral("prerender_timeline_zone"));
    QAction *stopPrevRender = actionCollection()->action(QStringLiteral("stop_prerender_timeline"));
    tlMenu->addAction(stopPrevRender);
    tlMenu->addAction(actionCollection()->action(QStringLiteral("set_render_timeline_zone")));
    tlMenu->addAction(actionCollection()->action(QStringLiteral("unset_render_timeline_zone")));
    tlMenu->addAction(actionCollection()->action(QStringLiteral("clear_render_timeline_zone")));

    // Automatic timeline preview action
    QAction *autoRender = new QAction(QIcon::fromTheme(QStringLiteral("view-refresh")), i18n("Automatic Preview"), this);
    autoRender->setCheckable(true);
    autoRender->setChecked(KdenliveSettings::autopreview());
    connect(autoRender, &QAction::triggered, this, &MainWindow::slotToggleAutoPreview);
    tlMenu->addAction(autoRender);
    tlMenu->addSeparator();
    tlMenu->addAction(actionCollection()->action(QStringLiteral("disable_preview")));
    tlMenu->addAction(actionCollection()->action(QStringLiteral("manage_cache")));
    timelinePreview->defineDefaultAction(prevRender, stopPrevRender);
    timelinePreview->setAutoRaise(true);

    QAction *showRender = actionCollection()->action(QStringLiteral("project_render"));
    tlrMenu->addAction(showRender);
    tlrMenu->addAction(actionCollection()->action(QStringLiteral("stop_project_render")));
    timelineRender->defineDefaultAction(showRender, showRender);
    timelineRender->setAutoRaise(true);

    // Populate encoding profiles
    KConfig conf(QStringLiteral("encodingprofiles.rc"), KConfig::CascadeConfig, QStandardPaths::AppDataLocation);
    /*KConfig conf(QStringLiteral("encodingprofiles.rc"), KConfig::CascadeConfig, QStandardPaths::AppDataLocation);
    if (KdenliveSettings::proxyparams().isEmpty() || KdenliveSettings::proxyextension().isEmpty()) {
        KConfigGroup group(&conf, "proxy");
        QMap<QString, QString> values = group.entryMap();
        QMapIterator<QString, QString> i(values);
        if (i.hasNext()) {
            i.next();
            QString proxystring = i.value();
            KdenliveSettings::setProxyparams(proxystring.section(QLatin1Char(';'), 0, 0));
            KdenliveSettings::setProxyextension(proxystring.section(QLatin1Char(';'), 1, 1));
        }
    }*/
    if (KdenliveSettings::v4l_parameters().isEmpty() || KdenliveSettings::v4l_extension().isEmpty()) {
        KConfigGroup group(&conf, "video4linux");
        QMap<QString, QString> values = group.entryMap();
        QMapIterator<QString, QString> i(values);
        if (i.hasNext()) {
            i.next();
            QString v4lstring = i.value();
            KdenliveSettings::setV4l_parameters(v4lstring.section(QLatin1Char(';'), 0, 0));
            KdenliveSettings::setV4l_extension(v4lstring.section(QLatin1Char(';'), 1, 1));
        }
    }
    if (KdenliveSettings::grab_parameters().isEmpty() || KdenliveSettings::grab_extension().isEmpty()) {
        KConfigGroup group(&conf, "screengrab");
        QMap<QString, QString> values = group.entryMap();
        QMapIterator<QString, QString> i(values);
        if (i.hasNext()) {
            i.next();
            QString grabstring = i.value();
            KdenliveSettings::setGrab_parameters(grabstring.section(QLatin1Char(';'), 0, 0));
            KdenliveSettings::setGrab_extension(grabstring.section(QLatin1Char(';'), 1, 1));
        }
    }
    if (KdenliveSettings::decklink_parameters().isEmpty() || KdenliveSettings::decklink_extension().isEmpty()) {
        KConfigGroup group(&conf, "decklink");
        QMap<QString, QString> values = group.entryMap();
        QMapIterator<QString, QString> i(values);
        if (i.hasNext()) {
            i.next();
            QString decklinkstring = i.value();
            KdenliveSettings::setDecklink_parameters(decklinkstring.section(QLatin1Char(';'), 0, 0));
            KdenliveSettings::setDecklink_extension(decklinkstring.section(QLatin1Char(';'), 1, 1));
        }
    }
    if (!QDir(KdenliveSettings::currenttmpfolder()).isReadable())
        KdenliveSettings::setCurrenttmpfolder(QStandardPaths::writableLocation(QStandardPaths::TempLocation));

    updateActionsToolTip();
    if (firstRun) {
        // Load editing layout
        layoutManager->loadLayout(QStringLiteral("kdenlive_editing"), true);
    }
    QTimer::singleShot(0, this, &MainWindow::GUISetupDone);

#ifdef USE_JOGSHUTTLE
    new JogManager(this);
#endif
    getMainTimeline()->setTimelineMenu(timelineClipMenu, compositionMenu, timelineMenu, guideMenu, timelineRulerMenu, actionCollection()->action(QStringLiteral("edit_guide")), timelineHeadersMenu, thumbsMenu , timelineSubtitleMenu);
    scmanager->slotCheckActiveScopes();
    connect(qApp, &QGuiApplication::applicationStateChanged, [&](Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive) {
            getMainTimeline()->regainFocus();
        }
    });
    // m_messageLabel->setMessage(QStringLiteral("This is a beta version. Always backup your data"), MltError);
    
    setWindowFlags(
        Qt::FramelessWindowHint |
        Qt::WindowCloseButtonHint |
        Qt::WindowMinimizeButtonHint |
        Qt::WindowMaximizeButtonHint |
        Qt::WindowFullscreenButtonHint |
        Qt::WindowTitleHint
    );
    
    
    
    // 设置无边框窗口辅助类
    m_framelessHelper = new FramelessHelper(this);
    m_framelessHelper->setDraggableMargins(2, 2, 2, 1);
    m_framelessHelper->setMaximizedMargins(0, 0, 0, 0);
    m_framelessHelper->setTitleBarHeight(42);
    
    // 设置菜单栏
    setupMenuBar();
    
    // 添加事件暴露组件
    m_framelessHelper->addExcludeItem(new TopNavigationBar(menuBar()));
    m_framelessHelper->addExcludeItem(m_clipMonitorFrame);
    m_framelessHelper->addExcludeItem(m_clipMonitorFrame->closeBtn());    
    
    // 设置顶端导航栏
    auto passer = new MenuBarEventPasser(menuBar());
    passer->setFixedSize(48 * menuBar()->actions().count(), menuBar()->height());
    passer->move(__ltLabelWidth, 0);
    m_framelessHelper->addExcludeItem(passer);
    
    // 处理由于删去dockWidget导致的widget残留显示

    grabWidget->hide();
    
    m_effectList2->hide();
    m_audioSpectrum->hide();    
    m_transitionList2->hide();
    
    pCore->mixer()->hide();
    pCore->library()->hide();
    pCore->subtitleWidget()->hide();
    pCore->textEditWidget()->hide();
    
    show();
    // 由于隐藏标题栏导致的窗口移动
    QTimer::singleShot(1, this, [this] {
        auto isMax = m_mwSettings.value("isMax", false).toBool();
        if (isMax) {
            m_framelessHelper->triggerMaximizeButtonAction();
        } else {
            auto&& scrRect = screen()->geometry();
            
            auto&& rect = m_mwSettings.value(
                "geo",
                QRect((scrRect.width() - 1280) / 2, (scrRect.height() - 720) / 2, 1280, 720)
            ).toRect();
            
            if (rect.x() < -rect.width() * 0.8 || rect.x() > screen()->geometry().width() * 0.9) {
                rect.setX(0);
            }
            if (rect.y() < -rect.height() * 0.8 || rect.y() > screen()->geometry().height() * 0.9) {
                rect.setX(0);
            }
            
            this->setGeometry(rect);
        }
    });
    
    projectBin->raise();
    projectBin->hide();
    
    m_effectTransWidget->selectTab(0);
}

void MainWindow::slotThemeChanged(const QString &name)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(name);
    QPalette plt = KColorScheme::createApplicationPalette(config);
    // qApp->setPalette(plt);
    // Required for qml palette change
    QGuiApplication::setPalette(plt);

    QColor background = plt.window().color();
    bool useDarkIcons = background.value() < 100;

    if (m_effectList2) {
        // Trigger a repaint to have icons adapted
        m_effectList2->reset();
    }
    if (m_transitionList2) {
        // Trigger a repaint to have icons adapted
        m_transitionList2->reset();
    }
    if (m_clipMonitor) {
        m_clipMonitor->setPalette(plt);
    }
    if (m_projectMonitor) {
        m_projectMonitor->setPalette(plt);
    }
    if (m_timelineTabs) {
        m_timelineTabs->setPalette(plt);
        getMainTimeline()->controller()->resetView();
    }
    if (m_audioSpectrum) {
        m_audioSpectrum->refreshPixmap();
    }

    KSharedConfigPtr kconfig = KSharedConfig::openConfig();
    KConfigGroup initialGroup(kconfig, "version");
    if (initialGroup.exists() && KdenliveSettings::force_breeze() && useDarkIcons != KdenliveSettings::use_dark_breeze()) {
        // We need to reload icon theme
        QIcon::setThemeName(useDarkIcons ? QStringLiteral("breeze-dark") : QStringLiteral("breeze"));
        KdenliveSettings::setUse_dark_breeze(useDarkIcons);
    }
}

void MainWindow::updateActionsToolTip()
{
    // Add shortcut to action tooltips
    QList<KActionCollection *> collections = KActionCollection::allCollections();
    for (int i = 0; i < collections.count(); ++i) {
        KActionCollection *coll = collections.at(i);
        for (QAction *tempAction : coll->actions()) {
            if (tempAction == m_timeFormatButton) {
                continue;
            }
            // find the shortcut pattern and delete (note the preceding space in the RegEx)
            QString toolTip = KLocalizedString::removeAcceleratorMarker(tempAction->toolTip());
            QString strippedTooltip = toolTip.remove(QRegExp(QStringLiteral("\\s\\(.*\\)")));
            QKeySequence shortCut = tempAction->shortcut();
            if (shortCut == QKeySequence()) {
                tempAction->setToolTip(strippedTooltip);
            } else {
                tempAction->setToolTip(QString("%1 (%2)").arg(strippedTooltip).arg(shortCut.toString()));
            }
        }
    }
}

MainWindow::~MainWindow()
{
    if (KdenliveSettings::clearRecentExit()) {
        auto recentAction = pCore->projectManager()->recentFilesAction();
        recentAction->clear();
        emit pCore->projectManager()->sigSaveRecentFiles();
    }
    if (m_framelessHelper->isMaximized()) {
        m_mwSettings.setValue("isMax", true);
    } else {
        m_mwSettings.setValue("isMax", false);
        auto&& geo = geometry();
        m_mwSettings.setValue("geo", geo);
    }
    m_mwSettings.setValue("timelineHeight", m_timelineTabs->height());

    pCore->prepareShutdown();
    delete m_timelineTabs;
    delete m_audioSpectrum;
    if (m_projectMonitor) {
        m_projectMonitor->stop();
    }
    if (m_clipMonitor) {
        m_clipMonitor->stop();
    }
    ClipController::mediaUnavailable.reset();
    delete m_projectMonitor;
    delete m_clipMonitor;
    delete m_shortcutRemoveFocus;
    delete m_effectList2;
    delete m_transitionList2;
    qDeleteAll(m_transitions);
    // Mlt::Factory::close();
}

// virtual
bool MainWindow::queryClose()
{
    if (m_renderWidget) {
        int waitingJobs = m_renderWidget->waitingJobsCount();
        if (waitingJobs > 0) {
            switch (
                KMessageBox::warningYesNoCancel(this,
                                                i18np("You have 1 rendering job waiting in the queue.\nWhat do you want to do with this job?",
                                                      "You have %1 rendering jobs waiting in the queue.\nWhat do you want to do with these jobs?", waitingJobs),
                                                QString(), KGuiItem(i18n("Start them now")), KGuiItem(i18n("Delete them")))) {
            case KMessageBox::Yes:
                // create script with waiting jobs and start it
                if (!m_renderWidget->startWaitingRenderJobs()) {
                    return false;
                }
                break;
            case KMessageBox::No:
                // Don't do anything, jobs will be deleted
                break;
            default:
                return false;
            }
        }
    }
    saveOptions();

    // WARNING: According to KMainWindow::queryClose documentation we are not supposed to close the document here?
    return pCore->projectManager()->closeCurrentDocument(true, true);
}

void MainWindow::loadGenerators()
{
    CustomMenu *addMenu = static_cast<CustomMenu *>(factory()->container(QStringLiteral("generators"), this));
    Generators::getGenerators(KdenliveSettings::producerslist(), addMenu);
    connect(addMenu, &CustomMenu::triggered, this, &MainWindow::buildGenerator);
}

void MainWindow::buildGenerator(QAction *action)
{
    Generators gen(action->data().toString(), this);
    if (gen.exec() == QDialog::Accepted) {
        pCore->bin()->slotAddClipToProject(gen.getSavedClip());
    }
}

void MainWindow::saveProperties(KConfigGroup &config)
{
    // save properties here
    KXmlGuiWindow::saveProperties(config);
    // TODO: fix session management
    if (qApp->isSavingSession() && pCore->projectManager()) {
        if (pCore->currentDoc() && !pCore->currentDoc()->url().isEmpty()) {
            config.writeEntry("kdenlive_lastUrl", pCore->currentDoc()->url().toLocalFile());
        }
    }
}

void MainWindow::saveNewToolbarConfig()
{
    KXmlGuiWindow::saveNewToolbarConfig();
    // TODO for some reason all dynamically inserted actions are removed by the save toolbar
    // So we currently re-add them manually....
    loadDockActions();
    loadClipActions();
    pCore->bin()->rebuildMenu();
    CustomMenu *monitorOverlay = static_cast<CustomMenu *>(factory()->container(QStringLiteral("monitor_config_overlay"), this));
    if (monitorOverlay) {
        m_projectMonitor->setupMenu(static_cast<CustomMenu *>(factory()->container(QStringLiteral("monitor_go"), this)), monitorOverlay, m_playZone, m_loopZone,
                                    nullptr, m_loopClip);
        m_clipMonitor->setupMenu(static_cast<CustomMenu *>(factory()->container(QStringLiteral("monitor_go"), this)), monitorOverlay, m_playZone, m_loopZone,
                                 static_cast<CustomMenu *>(factory()->container(QStringLiteral("marker_menu"), this)));
    }
}

void MainWindow::slotReloadEffects(const QStringList &paths)
{
    for (const QString &p : paths) {
        EffectsRepository::get()->reloadCustom(p);
    }
    m_effectList2->reloadEffectMenu(m_effectsMenu, m_effectActions);
}

void MainWindow::configureNotifications()
{
    KNotifyConfigWidget::configure(this);
}

void MainWindow::slotFullScreen()
{
    KToggleFullScreenAction::setFullScreen(this, actionCollection()->action(QStringLiteral("fullscreen"))->isChecked());
}

void MainWindow::slotConnectMonitors()
{
    // connect(m_projectList, SIGNAL(deleteProjectClips(QStringList,QMap<QString,QString>)), this,
    // SLOT(slotDeleteProjectClips(QStringList,QMap<QString,QString>)));
    connect(m_clipMonitor, &Monitor::refreshClipThumbnail, pCore->bin(), &Bin::slotRefreshClipThumbnail);
    connect(m_projectMonitor, &Monitor::requestFrameForAnalysis, this, &MainWindow::slotMonitorRequestRenderFrame);
    connect(m_projectMonitor, &Monitor::createSplitOverlay, this, &MainWindow::createSplitOverlay, Qt::DirectConnection);
    connect(m_projectMonitor, &Monitor::removeSplitOverlay, this, &MainWindow::removeSplitOverlay, Qt::DirectConnection);
}

void MainWindow::createSplitOverlay(std::shared_ptr<Mlt::Filter> filter)
{
    if (m_assetPanel->effectStackOwner().first == ObjectType::TimelineClip) {
        getMainTimeline()->controller()->createSplitOverlay(m_assetPanel->effectStackOwner().second, filter);
        m_projectMonitor->activateSplit();
    } else {
        pCore->displayMessage(i18n("Select a clip to compare effect"), ErrorMessage);
    }
}

void MainWindow::removeSplitOverlay()
{
    getMainTimeline()->controller()->removeSplitOverlay();
}

void MainWindow::addAction(const QString &name, QAction *action, const QKeySequence &shortcut, KActionCategory *category)
{
    m_actionNames.append(name);
    if (category) {
        category->addAction(name, action);
    } else {
        actionCollection()->addAction(name, action);
    }
    actionCollection()->setDefaultShortcut(action, shortcut);
}

QAction *MainWindow::addAction(const QString &name, const QString &text, const QObject *receiver, const char *member, const QIcon &icon,
                               const QKeySequence &shortcut, KActionCategory *category)
{
    auto *action = new QAction(text, this);
    if (!icon.isNull()) {
        action->setIcon(icon);
    }
    addAction(name, action, shortcut, category);
    connect(action, SIGNAL(triggered(bool)), receiver, member);

    return action;
}

void MainWindow::setupActions()
{
    // create edit mode buttons
    m_normalEditTool = new QAction(QIcon::fromTheme(QStringLiteral("kdenlive-normal-edit")), i18n("Normal mode"), this);
    m_normalEditTool->setCheckable(true);
    m_normalEditTool->setChecked(true);

    m_overwriteEditTool = new QAction(QIcon::fromTheme(QStringLiteral("kdenlive-overwrite-edit")), i18n("Overwrite mode"), this);
    m_overwriteEditTool->setCheckable(true);
    m_overwriteEditTool->setChecked(false);

    m_insertEditTool = new QAction(QIcon::fromTheme(QStringLiteral("kdenlive-insert-edit")), i18n("Insert mode"), this);
    m_insertEditTool->setCheckable(true);
    m_insertEditTool->setChecked(false);

    KSelectAction *sceneMode = new KSelectAction(i18n("Timeline Edit Mode"), this);
    sceneMode->addAction(m_normalEditTool);
    sceneMode->addAction(m_overwriteEditTool);
    sceneMode->addAction(m_insertEditTool);
    sceneMode->setCurrentItem(0);
    connect(sceneMode, static_cast<void (KSelectAction::*)(QAction *)>(&KSelectAction::triggered), this, &MainWindow::slotChangeEdit);
    addAction(QStringLiteral("timeline_mode"), sceneMode);
    actionCollection()->setShortcutsConfigurable(sceneMode, false);

    m_useTimelineZone = new KDualAction(i18n("Do not Use Timeline Zone for Insert"), i18n("Use Timeline Zone for Insert"), this);
    m_useTimelineZone->setActiveIcon(QIcon::fromTheme(QStringLiteral("timeline-use-zone-on")));
    m_useTimelineZone->setInactiveIcon(QIcon::fromTheme(QStringLiteral("timeline-use-zone-off")));
    m_useTimelineZone->setAutoToggle(true);
    connect(m_useTimelineZone, &KDualAction::activeChangedByUser, this, &MainWindow::slotSwitchTimelineZone);
    addAction(QStringLiteral("use_timeline_zone_in_edit"), m_useTimelineZone);

    m_compositeAction = new KSelectAction(QIcon::fromTheme(QStringLiteral("composite-track-off")), i18n("Track compositing"), this);
    m_compositeAction->setToolTip(i18n("Track compositing"));
    QAction *noComposite = new QAction(QIcon::fromTheme(QStringLiteral("composite-track-off")), i18n("None"), this);
    noComposite->setCheckable(true);
    noComposite->setData(0);
    m_compositeAction->addAction(noComposite);
    QString compose = TransitionsRepository::get()->getCompositingTransition();
    if (compose == QStringLiteral("movit.overlay")) {
        // Movit, do not show "preview" option since movit is faster
        QAction *hqComposite = new QAction(QIcon::fromTheme(QStringLiteral("composite-track-on")), i18n("High Quality"), this);
        hqComposite->setCheckable(true);
        hqComposite->setData(2);
        m_compositeAction->addAction(hqComposite);
        m_compositeAction->setCurrentAction(hqComposite);
    } else {
        QAction *previewComposite = new QAction(QIcon::fromTheme(QStringLiteral("composite-track-preview")), i18n("Preview"), this);
        previewComposite->setCheckable(true);
        previewComposite->setData(1);
        m_compositeAction->addAction(previewComposite);
        if (compose != QStringLiteral("composite")) {
            QAction *hqComposite = new QAction(QIcon::fromTheme(QStringLiteral("composite-track-on")), i18n("High Quality"), this);
            hqComposite->setData(2);
            hqComposite->setCheckable(true);
            m_compositeAction->addAction(hqComposite);
            m_compositeAction->setCurrentAction(hqComposite);
        } else {
            m_compositeAction->setCurrentAction(previewComposite);
        }
    }
    connect(m_compositeAction, static_cast<void (KSelectAction::*)(QAction *)>(&KSelectAction::triggered), this, &MainWindow::slotUpdateCompositing);
    addAction(QStringLiteral("timeline_compositing"), m_compositeAction);
    actionCollection()->setShortcutsConfigurable(m_compositeAction, false);

    QAction *splitView = new QAction(QIcon::fromTheme(QStringLiteral("view-split-top-bottom")), i18n("Split Audio Tracks"), this);
    addAction(QStringLiteral("timeline_view_split"), splitView);
    splitView->setData(QVariant::fromValue(1));
    splitView->setCheckable(true);
    splitView->setChecked(KdenliveSettings::audiotracksbelow() == 1);

    QAction *splitView2 = new QAction(QIcon::fromTheme(QStringLiteral("view-split-top-bottom")), i18n("Split Audio Tracks (reverse)"), this);
    addAction(QStringLiteral("timeline_view_split_reverse"), splitView2);
    splitView2->setData(QVariant::fromValue(2));
    splitView2->setCheckable(true);
    splitView2->setChecked(KdenliveSettings::audiotracksbelow() == 2);

    QAction *mixedView = new QAction(QIcon::fromTheme(QStringLiteral("document-new")), i18n("Mixed Audio tracks"), this);
    addAction(QStringLiteral("timeline_mixed_view"), mixedView);
    mixedView->setData(QVariant::fromValue(0));
    mixedView->setCheckable(true);
    mixedView->setChecked(KdenliveSettings::audiotracksbelow() == 0);

    auto *clipTypeGroup = new QActionGroup(this);
    clipTypeGroup->addAction(mixedView);
    clipTypeGroup->addAction(splitView);
    clipTypeGroup->addAction(splitView2);
    connect(clipTypeGroup, &QActionGroup::triggered, this, &MainWindow::slotUpdateTimelineView);

    auto tlsettings = new CustomMenu(this);
    tlsettings->setIcon(QIcon::fromTheme(QStringLiteral("configure")));
    tlsettings->addAction(m_compositeAction);
    tlsettings->addAction(mixedView);
    tlsettings->addAction(splitView);
    tlsettings->addAction(splitView2);

    auto *timelineSett = new QToolButton(this);
    timelineSett->setPopupMode(QToolButton::InstantPopup);
    timelineSett->setMenu(tlsettings);
    timelineSett->setIcon(QIcon::fromTheme(QStringLiteral("configure")));
    auto *tlButtonAction = new QWidgetAction(this);
    tlButtonAction->setDefaultWidget(timelineSett);
    tlButtonAction->setText(i18n("Track menu"));
    addAction(QStringLiteral("timeline_settings"), tlButtonAction);

    m_timeFormatButton = new KSelectAction(QStringLiteral("00:00:00:00 / 00:00:00:00"), this);
    m_timeFormatButton->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    m_timeFormatButton->addAction(i18n("hh:mm:ss:ff"));
    m_timeFormatButton->addAction(i18n("Frames"));
    if (KdenliveSettings::frametimecode()) {
        m_timeFormatButton->setCurrentItem(1);
    } else {
        m_timeFormatButton->setCurrentItem(0);
    }
#if KWIDGETSADDONS_VERSION < QT_VERSION_CHECK(5,78,0)
    connect(m_timeFormatButton, static_cast<void (KSelectAction::*)(int)>(&KSelectAction::triggered), this, &MainWindow::slotUpdateTimecodeFormat);
#else
    connect(m_timeFormatButton, &KSelectAction::indexTriggered, this, &MainWindow::slotUpdateTimecodeFormat);
#endif
    m_timeFormatButton->setToolBarMode(KSelectAction::MenuMode);
    m_timeFormatButton->setToolButtonPopupMode(QToolButton::InstantPopup);
    addAction(QStringLiteral("timeline_timecode"), m_timeFormatButton);
    actionCollection()->setShortcutsConfigurable(m_timeFormatButton, false);

    m_buttonSubtitleEditTool = new QAction(QIcon::fromTheme(QStringLiteral("add-subtitle")), i18n("Edit Subtitle tool"), this);
    m_buttonSubtitleEditTool->setCheckable(true);
    m_buttonSubtitleEditTool->setChecked(false);
    addAction(QStringLiteral("subtitle_tool"), m_buttonSubtitleEditTool);
    connect(m_buttonSubtitleEditTool, &QAction::triggered, [this]() {
        slotEditSubtitle();
    });

    // create tools buttons
    m_buttonSelectTool = new QAction(QIcon::fromTheme(QStringLiteral("cursor-arrow")), i18n("Selection tool"), this);
    // toolbar->addAction(m_buttonSelectTool);
    m_buttonSelectTool->setCheckable(true);
    m_buttonSelectTool->setChecked(true);

    m_buttonRazorTool = new QAction(QIcon::fromTheme(QStringLiteral("edit-cut")), i18n("Razor tool"), this);
    // toolbar->addAction(m_buttonRazorTool);
    m_buttonRazorTool->setCheckable(true);
    m_buttonRazorTool->setChecked(false);

    m_buttonSpacerTool = new QAction(QIcon::fromTheme(QStringLiteral("distribute-horizontal-x")), i18n("Spacer tool"), this);
    // toolbar->addAction(m_buttonSpacerTool);
    m_buttonSpacerTool->setCheckable(true);
    m_buttonSpacerTool->setChecked(false);
    auto *toolGroup = new QActionGroup(this);
    toolGroup->addAction(m_buttonSelectTool);
    toolGroup->addAction(m_buttonRazorTool);
    toolGroup->addAction(m_buttonSpacerTool);
    toolGroup->setExclusive(true);
    
    QAction *collapseItem = new QAction(QIcon::fromTheme(QStringLiteral("collapse-all")), i18n("Collapse/Expand Item"), this);
    addAction(QStringLiteral("collapse_expand"), collapseItem, Qt::Key_Less);
    connect(collapseItem, &QAction::triggered, this, &MainWindow::slotCollapse);
    
    QAction *sameTrack = new QAction(QIcon::fromTheme(QStringLiteral("composite-track-preview")), i18n("Mix Clips"), this);
    addAction(QStringLiteral("mix_clip"), sameTrack, Qt::Key_U);
    connect(sameTrack, &QAction::triggered, [this]() {
       getCurrentTimeline()->controller()->mixClip(); 
    });
    
    // toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    /*QWidget * actionWidget;
    int max = toolbar->iconSizeDefault() + 2;
    actionWidget = toolbar->widgetForAction(m_normalEditTool);
    actionWidget->setMaximumWidth(max);
    actionWidget->setMaximumHeight(max - 4);

    actionWidget = toolbar->widgetForAction(m_insertEditTool);
    actionWidget->setMaximumWidth(max);
    actionWidget->setMaximumHeight(max - 4);

    actionWidget = toolbar->widgetForAction(m_overwriteEditTool);
    actionWidget->setMaximumWidth(max);
    actionWidget->setMaximumHeight(max - 4);

    actionWidget = toolbar->widgetForAction(m_buttonSelectTool);
    actionWidget->setMaximumWidth(max);
    actionWidget->setMaximumHeight(max - 4);

    actionWidget = toolbar->widgetForAction(m_buttonRazorTool);
    actionWidget->setMaximumWidth(max);
    actionWidget->setMaximumHeight(max - 4);

    actionWidget = toolbar->widgetForAction(m_buttonSpacerTool);
    actionWidget->setMaximumWidth(max);
    actionWidget->setMaximumHeight(max - 4);*/

    connect(toolGroup, &QActionGroup::triggered, this, &MainWindow::slotChangeTool);

    m_buttonVideoThumbs = new QAction(QIcon::fromTheme(QStringLiteral("kdenlive-show-videothumb")), i18n("Show video thumbnails"), this);

    m_buttonVideoThumbs->setCheckable(true);
    m_buttonVideoThumbs->setChecked(KdenliveSettings::videothumbnails());
    connect(m_buttonVideoThumbs, &QAction::triggered, this, &MainWindow::slotSwitchVideoThumbs);

    m_buttonAudioThumbs = new QAction(QIcon::fromTheme(QStringLiteral("kdenlive-show-audiothumb")), i18n("Show audio thumbnails"), this);

    m_buttonAudioThumbs->setCheckable(true);
    m_buttonAudioThumbs->setChecked(KdenliveSettings::audiothumbnails());
    connect(m_buttonAudioThumbs, &QAction::triggered, this, &MainWindow::slotSwitchAudioThumbs);

    m_buttonShowMarkers = new QAction(QIcon::fromTheme(QStringLiteral("kdenlive-show-markers")), i18n("Show markers comments"), this);

    m_buttonShowMarkers->setCheckable(true);
    m_buttonShowMarkers->setChecked(KdenliveSettings::showmarkers());
    connect(m_buttonShowMarkers, &QAction::triggered, this, &MainWindow::slotSwitchMarkersComments);

    m_buttonSnap = new QAction(QIcon::fromTheme(QStringLiteral("kdenlive-snap")), i18n("Snap"), this);

    m_buttonSnap->setCheckable(true);
    m_buttonSnap->setChecked(KdenliveSettings::snaptopoints());
    connect(m_buttonSnap, &QAction::triggered, this, &MainWindow::slotSwitchSnap);

    m_buttonTimelineTags = new QAction(QIcon::fromTheme(QStringLiteral("tag")), i18n("Show color tags in timeline"), this);

    m_buttonTimelineTags->setCheckable(true);
    m_buttonTimelineTags->setChecked(KdenliveSettings::tagsintimeline());
    connect(m_buttonTimelineTags, &QAction::triggered, this, &MainWindow::slotShowTimelineTags);

    m_buttonFitZoom = new QAction(QIcon::fromTheme(QStringLiteral("zoom-fit-best")), i18n("Fit zoom to project"), this);

    m_buttonFitZoom->setCheckable(false);

    // timeline tool bar
    KToolBar *toolbar = new KToolBar(QStringLiteral("statusToolBar"), this, Qt::BottomToolBarArea);
    toolbar->setMovable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    if (KdenliveSettings::gpu_accel()) {
        QLabel *warnLabel = new QLabel(i18n("Experimental GPU processing enabled - not for production"), this);
        warnLabel->setFont(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont));
        warnLabel->setAlignment(Qt::AlignHCenter);
        warnLabel->setStyleSheet(QStringLiteral("QLabel { background-color :red; color:black;padding-left:2px;padding-right:2px}"));
        toolbar->addWidget(warnLabel);
    }

    m_trimLabel = new QLabel(QString(), this);
    m_trimLabel->setFont(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont));
    m_trimLabel->setAlignment(Qt::AlignHCenter);
    //m_trimLabel->setStyleSheet(QStringLiteral("QLabel { background-color :red; }"));


    toolbar->addWidget(m_trimLabel);
    toolbar->addAction(m_buttonTimelineTags);
    toolbar->addAction(m_buttonVideoThumbs);
    toolbar->addAction(m_buttonAudioThumbs);
    toolbar->addAction(m_buttonShowMarkers);
    toolbar->addAction(m_buttonSnap);
    toolbar->addSeparator();
    toolbar->addAction(m_buttonFitZoom);

    int small = style()->pixelMetric(QStyle::PM_SmallIconSize);
    statusBar()->setMaximumHeight(2 * small);
    m_messageLabel = new StatusBarMessageLabel(this);
    m_messageLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    connect(this, &MainWindow::displayMessage, m_messageLabel, &StatusBarMessageLabel::setMessage);
    connect(this, &MainWindow::displayProgressMessage, m_messageLabel, &StatusBarMessageLabel::setProgressMessage);
    statusBar()->addWidget(m_messageLabel, 10);
    statusBar()->addPermanentWidget(toolbar);
    toolbar->setIconSize(QSize(small, small));
    toolbar->layout()->setContentsMargins(0, 0, 0, 0);
    statusBar()->setContentsMargins(0, 0, 0, 0);
    
    addAction(QStringLiteral("normal_mode"), m_normalEditTool);
    addAction(QStringLiteral("overwrite_mode"), m_overwriteEditTool);
    addAction(QStringLiteral("insert_mode"), m_insertEditTool);
    addAction(QStringLiteral("select_tool"), m_buttonSelectTool, Qt::Key_S);
    addAction(QStringLiteral("razor_tool"), m_buttonRazorTool, Qt::Key_X);
    addAction(QStringLiteral("spacer_tool"), m_buttonSpacerTool, Qt::Key_M);

    addAction(QStringLiteral("automatic_transition"), m_buttonTimelineTags);
    addAction(QStringLiteral("show_video_thumbs"), m_buttonVideoThumbs);
    addAction(QStringLiteral("show_audio_thumbs"), m_buttonAudioThumbs);
    addAction(QStringLiteral("show_markers"), m_buttonShowMarkers);
    addAction(QStringLiteral("snap"), m_buttonSnap);
    addAction(QStringLiteral("zoom_fit"), m_buttonFitZoom);

#if defined(Q_OS_WIN)
    int glBackend = KdenliveSettings::opengl_backend();
    QAction *openGLAuto = new QAction(i18n("自动"), this);
    openGLAuto->setData(0);
    openGLAuto->setCheckable(true);
    openGLAuto->setChecked(glBackend == 0);

    QAction *openGLDesktop = new QAction(i18n("OpenGL"), this);
    openGLDesktop->setData(Qt::AA_UseDesktopOpenGL);
    openGLDesktop->setCheckable(true);
    openGLDesktop->setChecked(glBackend == Qt::AA_UseDesktopOpenGL);

    QAction *openGLES = new QAction(i18n("DirectX (ANGLE)"), this);
    openGLES->setData(Qt::AA_UseOpenGLES);
    openGLES->setCheckable(true);
    openGLES->setChecked(glBackend == Qt::AA_UseOpenGLES);

    QAction *openGLSoftware = new QAction(i18n("Software OpenGL"), this);
    openGLSoftware->setData(Qt::AA_UseSoftwareOpenGL);
    openGLSoftware->setCheckable(true);
    openGLSoftware->setChecked(glBackend == Qt::AA_UseSoftwareOpenGL);
    addAction(QStringLiteral("opengl_auto"), openGLAuto);
    addAction(QStringLiteral("opengl_desktop"), openGLDesktop);
    addAction(QStringLiteral("opengl_es"), openGLES);
    addAction(QStringLiteral("opengl_software"), openGLSoftware);
#endif

    addAction(QStringLiteral("run_wizard"), i18n("Run Config Wizard"), this, SLOT(slotRunWizard()), QIcon::fromTheme(QStringLiteral("tools-wizard")));
    addAction(QStringLiteral("project_settings"), i18n("Project Settings"), this, SLOT(slotEditProjectSettings()),
              QIcon::fromTheme(QStringLiteral("configure")));

    addAction(QStringLiteral("project_render"), i18n("Render"), this, SLOT(slotRenderProject()), QIcon::fromTheme(QStringLiteral("media-record")),
              Qt::CTRL + Qt::Key_Return);

    addAction(QStringLiteral("stop_project_render"), i18n("Stop Render"), this, SLOT(slotStopRenderProject()),
              QIcon::fromTheme(QStringLiteral("media-record")));

    addAction(QStringLiteral("project_clean"), i18n("Clean Project"), this, SLOT(slotCleanProject()), QIcon::fromTheme(QStringLiteral("edit-clear")));

    QAction *resetAction = new QAction(QIcon::fromTheme(QStringLiteral("view-refresh")), i18n("Reset configuration"), this);
    addAction(QStringLiteral("reset_config"), resetAction);
    connect(resetAction, &QAction::triggered, this, [&]() {
        slotRestart(true);
    });

    addAction("project_adjust_profile", i18n("Adjust Profile to Current Clip"), pCore->bin(), SLOT(adjustProjectProfileToItem()));

    m_playZone = addAction(QStringLiteral("monitor_play_zone"), i18n("Play Zone"), pCore->monitorManager(), SLOT(slotPlayZone()),
                           QIcon::fromTheme(QStringLiteral("media-playback-start")), Qt::CTRL + Qt::Key_Space);
    m_loopZone = addAction(QStringLiteral("monitor_loop_zone"), i18n("Loop Zone"), pCore->monitorManager(), SLOT(slotLoopZone()),
                           QIcon::fromTheme(QStringLiteral("media-playback-start")), Qt::CTRL + Qt::SHIFT + Qt::Key_Space);
    m_loopClip = new QAction(QIcon::fromTheme(QStringLiteral("media-playback-start")), i18n("Loop selected clip"), this);
    addAction(QStringLiteral("monitor_loop_clip"), m_loopClip);
    m_loopClip->setEnabled(false);

    addAction(QStringLiteral("transcode_clip"), i18n("Transcode Clips"), this, SLOT(slotTranscodeClip()), QIcon::fromTheme(QStringLiteral("edit-copy")));
    QAction *exportAction = new QAction(QIcon::fromTheme(QStringLiteral("document-export")), i18n("OpenTimelineIO E&xport"), this);
    connect(exportAction, &QAction::triggered, &m_otioConvertions, &OtioConvertions::slotExportProject);
    addAction(QStringLiteral("export_project"), exportAction);
    QAction *importAction = new QAction(QIcon::fromTheme(QStringLiteral("document-import")), i18n("OpenTimelineIO &Import"), this);
    connect(importAction, &QAction::triggered, &m_otioConvertions, &OtioConvertions::slotImportProject);
    addAction(QStringLiteral("import_project"), importAction);

    addAction(QStringLiteral("archive_project"), i18n("Archive Project"), this, SLOT(slotArchiveProject()),
              QIcon::fromTheme(QStringLiteral("document-save-all")));
    addAction(QStringLiteral("switch_monitor"), i18n("Switch monitor"), this, SLOT(slotSwitchMonitors()), QIcon(), Qt::Key_T);
    addAction(QStringLiteral("expand_timeline_clip"), i18n("Expand Clip"), this, SLOT(slotExpandClip()),
              QIcon::fromTheme(QStringLiteral("document-open")));

    QAction *overlayInfo = new QAction(QIcon::fromTheme(QStringLiteral("help-hint")), i18n("Monitor Info Overlay"), this);
    addAction(QStringLiteral("monitor_overlay"), overlayInfo);
    overlayInfo->setCheckable(true);
    overlayInfo->setData(0x01);

    QAction *overlayTCInfo = new QAction(QIcon::fromTheme(QStringLiteral("help-hint")), i18n("Monitor Overlay Timecode"), this);
    addAction(QStringLiteral("monitor_overlay_tc"), overlayTCInfo);
    overlayTCInfo->setCheckable(true);
    overlayTCInfo->setData(0x02);

    QAction *overlayFpsInfo = new QAction(QIcon::fromTheme(QStringLiteral("help-hint")), i18n("Monitor Overlay Playback Fps"), this);
    addAction(QStringLiteral("monitor_overlay_fps"), overlayFpsInfo);
    overlayFpsInfo->setCheckable(true);
    overlayFpsInfo->setData(0x20);

    QAction *overlayMarkerInfo = new QAction(QIcon::fromTheme(QStringLiteral("help-hint")), i18n("Monitor Overlay Markers"), this);
    addAction(QStringLiteral("monitor_overlay_markers"), overlayMarkerInfo);
    overlayMarkerInfo->setCheckable(true);
    overlayMarkerInfo->setData(0x04);

    QAction *overlayAudioInfo = new QAction(QIcon::fromTheme(QStringLiteral("help-hint")), i18n("Monitor Overlay Audio Waveform"), this);
    addAction(QStringLiteral("monitor_overlay_audiothumb"), overlayAudioInfo);
    overlayAudioInfo->setCheckable(true);
    overlayAudioInfo->setData(0x10);

    connect(overlayInfo, &QAction::toggled, this, [&, overlayTCInfo, overlayFpsInfo, overlayMarkerInfo, overlayAudioInfo](bool toggled) {
        overlayTCInfo->setEnabled(toggled);
        overlayFpsInfo->setEnabled(toggled);
        overlayMarkerInfo->setEnabled(toggled);
        overlayAudioInfo->setEnabled(toggled);
    });

#if LIBMLT_VERSION_INT >= QT_VERSION_CHECK(6,20,0)
    // Monitor resolution scaling
    KActionCategory *resolutionActionCategory = new KActionCategory(i18n("Preview Resolution"), actionCollection());
    m_scaleGroup = new QActionGroup(this);
    m_scaleGroup->setExclusive(true);
    m_scaleGroup->setEnabled(!KdenliveSettings::external_display());
    QAction *scale_no = new QAction(i18n("Full Resolution (1:1)"), m_scaleGroup);
    addAction(QStringLiteral("scale_no_preview"), scale_no, QKeySequence(Qt::Key_F6), resolutionActionCategory);
    scale_no->setCheckable(true);
    scale_no->setData(1);
    QAction *scale_2 = new QAction(i18n("720p"), m_scaleGroup);
    addAction(QStringLiteral("scale_2_preview"), scale_2, QKeySequence(Qt::Key_F7), resolutionActionCategory);
    scale_2->setCheckable(true);
    scale_2->setData(2);
    QAction *scale_4 = new QAction(i18n("540p"), m_scaleGroup);
    addAction(QStringLiteral("scale_4_preview"), scale_4, QKeySequence(Qt::Key_F8), resolutionActionCategory);
    scale_4->setCheckable(true);
    scale_4->setData(4);
    QAction *scale_8 = new QAction(i18n("360p"), m_scaleGroup);
    addAction(QStringLiteral("scale_8_preview"), scale_8, QKeySequence(Qt::Key_F9), resolutionActionCategory);
    scale_8->setCheckable(true);
    scale_8->setData(8);
    QAction *scale_16 = new QAction(i18n("270p"), m_scaleGroup);
    addAction(QStringLiteral("scale_16_preview"), scale_16, QKeySequence(Qt::Key_F10), resolutionActionCategory);
    scale_16->setCheckable(true);
    scale_16->setData(16);
    connect(pCore->monitorManager(), &MonitorManager::scalingChanged, this, [scale_2, scale_4, scale_8, scale_16, scale_no]() {
        switch (KdenliveSettings::previewScaling()) {
            case 2:
                scale_2->setChecked(true);
                break;
            case 4:
                scale_4->setChecked(true);
                break;
            case 8:
                scale_8->setChecked(true);
                break;
            case 16:
                scale_16->setChecked(true);
                break;
            default:
                scale_no->setChecked(true);
                break;
        }
    });
    emit pCore->monitorManager()->scalingChanged();
    connect(m_scaleGroup, &QActionGroup::triggered, this, [] (QAction *ac) {
        int scaling = ac->data().toInt();
        KdenliveSettings::setPreviewScaling(scaling);
        // Clear timeline selection so that any qml monitor scene is reset
        emit pCore->monitorManager()->updatePreviewScaling();
    });
#endif

    QAction *dropFrames = new QAction(QIcon(), i18n("Real Time (drop frames)"), this);
    dropFrames->setCheckable(true);
    dropFrames->setChecked(KdenliveSettings::monitor_dropframes());
    addAction(QStringLiteral("mlt_realtime"), dropFrames);
    connect(dropFrames, &QAction::toggled, this, &MainWindow::slotSwitchDropFrames);

    KSelectAction *monitorGamma = new KSelectAction(i18n("Monitor Gamma"), this);
    monitorGamma->addAction(i18n("sRGB (computer)"));
    monitorGamma->addAction(i18n("Rec. 709 (TV)"));
    addAction(QStringLiteral("mlt_gamma"), monitorGamma);
    monitorGamma->setCurrentItem(KdenliveSettings::monitor_gamma());
#if KWIDGETSADDONS_VERSION < QT_VERSION_CHECK(5,78,0)
    connect(monitorGamma, static_cast<void (KSelectAction::*)(int)>(&KSelectAction::triggered), this, &MainWindow::slotSetMonitorGamma);
#else
    connect(monitorGamma, &KSelectAction::indexTriggered, this, &MainWindow::slotSetMonitorGamma);
#endif
    actionCollection()->setShortcutsConfigurable(monitorGamma, false);

    addAction(QStringLiteral("switch_trim"), i18n("Trim Mode"), this, SLOT(slotSwitchTrimMode()), QIcon::fromTheme(QStringLiteral("cursor-arrow")));
    // disable shortcut until fully working, Qt::CTRL + Qt::Key_T);

    addAction(QStringLiteral("insert_project_tree"), i18n("Insert Zone in Project Bin"), this, SLOT(slotInsertZoneToTree()),
              QIcon::fromTheme(QStringLiteral("kdenlive-add-clip")), Qt::CTRL + Qt::Key_I);

    addAction(QStringLiteral("monitor_seek_snap_backward"), i18n("Go to Previous Snap Point"), this, SLOT(slotSnapRewind()),
              QIcon::fromTheme(QStringLiteral("media-seek-backward")), Qt::ALT + Qt::Key_Left);
    addAction(QStringLiteral("monitor_seek_guide_backward"), i18n("Go to Previous Guide"), this, SLOT(slotGuideRewind()),
              QIcon::fromTheme(QStringLiteral("media-seek-backward")), Qt::CTRL + Qt::Key_Left);
    addAction(QStringLiteral("seek_clip_start"), i18n("Go to Clip Start"), this, SLOT(slotClipStart()), QIcon::fromTheme(QStringLiteral("media-seek-backward")),
              Qt::Key_Home);
    addAction(QStringLiteral("seek_clip_end"), i18n("Go to Clip End"), this, SLOT(slotClipEnd()), QIcon::fromTheme(QStringLiteral("media-seek-forward")),
              Qt::Key_End);
    addAction(QStringLiteral("monitor_seek_snap_forward"), i18n("Go to Next Snap Point"), this, SLOT(slotSnapForward()),
              QIcon::fromTheme(QStringLiteral("media-seek-forward")), Qt::ALT + Qt::Key_Right);
    addAction(QStringLiteral("monitor_seek_guide_forward"), i18n("Go to Next Guide"), this, SLOT(slotGuideForward()),
              QIcon::fromTheme(QStringLiteral("media-seek-forward")), Qt::CTRL + Qt::Key_Right);
    addAction(QStringLiteral("align_playhead"), i18n("Align Playhead to Mouse Position"), this, SLOT(slotAlignPlayheadToMousePos()), QIcon(), Qt::Key_P);

    addAction(QStringLiteral("grab_item"), i18n("Grab Current Item"), this, SLOT(slotGrabItem()), QIcon::fromTheme(QStringLiteral("transform-move")),
              Qt::SHIFT + Qt::Key_G);

    QAction *stickTransition = new QAction(i18n("Automatic Transition"), this);
    stickTransition->setData(QStringLiteral("auto"));
    stickTransition->setCheckable(true);
    stickTransition->setEnabled(false);
    addAction(QStringLiteral("auto_transition"), stickTransition);
    connect(stickTransition, &QAction::triggered, this, &MainWindow::slotAutoTransition);

    addAction(QStringLiteral("overwrite_to_in_point"), i18n("Overwrite Clip Zone in Timeline"), this, SLOT(slotInsertClipOverwrite()),
              QIcon::fromTheme(QStringLiteral("timeline-overwrite")), Qt::Key_B);
    addAction(QStringLiteral("insert_to_in_point"), i18n("插入"), this, SLOT(slotInsertClipInsert()),
              QIcon());
    addAction(QStringLiteral("remove_extract"), i18n("Extract Timeline Zone"), this, SLOT(slotExtractZone()),
              QIcon::fromTheme(QStringLiteral("timeline-extract")), Qt::SHIFT + Qt::Key_X);
    addAction(QStringLiteral("remove_lift"), i18n("Lift Timeline Zone"), this, SLOT(slotLiftZone()), QIcon::fromTheme(QStringLiteral("timeline-lift")),
              Qt::Key_Z);
    addAction(QStringLiteral("set_render_timeline_zone"), i18n("Add Preview Zone"), this, SLOT(slotDefinePreviewRender()),
              QIcon::fromTheme(QStringLiteral("preview-add-zone")));
    addAction(QStringLiteral("unset_render_timeline_zone"), i18n("Remove Preview Zone"), this, SLOT(slotRemovePreviewRender()),
              QIcon::fromTheme(QStringLiteral("preview-remove-zone")));
    addAction(QStringLiteral("clear_render_timeline_zone"), i18n("Remove All Preview Zones"), this, SLOT(slotClearPreviewRender()),
              QIcon::fromTheme(QStringLiteral("preview-remove-all")));
    addAction(QStringLiteral("prerender_timeline_zone"), i18n("Start Preview Render"), this, SLOT(slotPreviewRender()),
              QIcon::fromTheme(QStringLiteral("preview-render-on")), QKeySequence(Qt::SHIFT + Qt::Key_Return));
    addAction(QStringLiteral("stop_prerender_timeline"), i18n("Stop Preview Render"), this, SLOT(slotStopPreviewRender()),
              QIcon::fromTheme(QStringLiteral("preview-render-off")));

    addAction(QStringLiteral("select_timeline_clip"), i18n("Select Clip"), this, SLOT(slotSelectTimelineClip()),
              QIcon::fromTheme(QStringLiteral("edit-select")), Qt::Key_Plus);
    addAction(QStringLiteral("deselect_timeline_clip"), i18n("Deselect Clip"), this, SLOT(slotDeselectTimelineClip()),
              QIcon::fromTheme(QStringLiteral("edit-select")), Qt::Key_Minus);
    addAction(QStringLiteral("select_add_timeline_clip"), i18n("Add Clip To Selection"), this, SLOT(slotSelectAddTimelineClip()),
              QIcon::fromTheme(QStringLiteral("edit-select")), Qt::ALT + Qt::Key_Plus);
    addAction(QStringLiteral("select_timeline_transition"), i18n("Select Transition"), this, SLOT(slotSelectTimelineTransition()),
              QIcon::fromTheme(QStringLiteral("edit-select")), Qt::SHIFT + Qt::Key_Plus);
    addAction(QStringLiteral("deselect_timeline_transition"), i18n("Deselect Transition"), this, SLOT(slotDeselectTimelineTransition()),
              QIcon::fromTheme(QStringLiteral("edit-select")), Qt::SHIFT + Qt::Key_Minus);
    addAction(QStringLiteral("select_add_timeline_transition"), i18n("Add Transition To Selection"), this, SLOT(slotSelectAddTimelineTransition()),
              QIcon::fromTheme(QStringLiteral("edit-select")), Qt::ALT + Qt::SHIFT + Qt::Key_Plus);

    addAction(QStringLiteral("delete_all_clip_markers"), i18n("Delete All Markers"), this, SLOT(slotDeleteAllClipMarkers()),
              QIcon::fromTheme(QStringLiteral("edit-delete")));
    addAction(QStringLiteral("add_marker_guide_quickly"), i18n("Add Marker/Guide quickly"), this, SLOT(slotAddMarkerGuideQuickly()),
              QIcon::fromTheme(QStringLiteral("bookmark-new")), Qt::Key_Asterisk);

    // Clip actions. We set some category info on the action data to enable/disable it contextually in timelinecontroller
    KActionCategory *clipActionCategory = new KActionCategory(i18n("Current Selection"), actionCollection());

    QAction *addMarker = addAction(QStringLiteral("add_clip_marker"), i18n("Add Marker"), this, SLOT(slotAddClipMarker()), QIcon::fromTheme(QStringLiteral("bookmark-new")), QKeySequence(), clipActionCategory);
    addMarker->setData('P');

    QAction *delMarker = addAction(QStringLiteral("delete_clip_marker"), i18n("Delete Marker"), this, SLOT(slotDeleteClipMarker()), QIcon::fromTheme(QStringLiteral("edit-delete")), QKeySequence(), clipActionCategory);
    delMarker->setData('P');

    QAction *editClipMarker = addAction(QStringLiteral("edit_clip_marker"), i18n("Edit Marker"), this, SLOT(slotEditClipMarker()),
                                        QIcon::fromTheme(QStringLiteral("document-properties")), QKeySequence(), clipActionCategory);
    editClipMarker->setObjectName(QStringLiteral("edit_marker"));
    editClipMarker->setData('P');

    QAction *splitAudio = addAction(QStringLiteral("clip_split"), i18n("Restore Audio"), this, SLOT(slotSplitAV()),
                                    QIcon::fromTheme(QStringLiteral("document-new")), QKeySequence(), clipActionCategory);
    // "S" will be handled specifically to change the action name depending on current selection
    splitAudio->setData('S');
    splitAudio->setEnabled(false);

    QAction *extractClip = addAction(QStringLiteral("extract_clip"), i18n("Extract Clip"), this, SLOT(slotExtractClip()), QIcon::fromTheme(QStringLiteral("timeline-extract")), QKeySequence(), clipActionCategory);
    extractClip->setData('C');
    extractClip->setEnabled(false);

    QAction *extractToBin = addAction(QStringLiteral("save_to_bin"), i18n("Save Timeline Zone to Bin"), this, SLOT(slotSaveZoneToBin()), QIcon(), QKeySequence(), clipActionCategory);
    extractToBin->setData('C');
    extractToBin->setEnabled(false);

    QAction *switchEnable = addAction(QStringLiteral("clip_switch"), i18n("Disable Clip"), this, SLOT(slotSwitchClip()),
                                    QIcon(), QKeySequence(), clipActionCategory);
    // "W" will be handled specifically to change the action name depending on current selection
    switchEnable->setData('W');
    switchEnable->setEnabled(false);

    QAction *setAudioAlignReference = addAction(QStringLiteral("set_audio_align_ref"), i18n("Set Audio Reference"), this, SLOT(slotSetAudioAlignReference()),
                                                QIcon(), QKeySequence(), clipActionCategory);
    // "A" as data means this action should only be available for clips with audio
    setAudioAlignReference->setData('A');
    setAudioAlignReference->setEnabled(false);

    QAction *alignAudio =
        addAction(QStringLiteral("align_audio"), i18n("Align Audio to Reference"), this, SLOT(slotAlignAudio()), QIcon(), QKeySequence(), clipActionCategory);
    // "A" as data means this action should only be available for clips with audio
    //alignAudio->setData('A');
    alignAudio->setEnabled(false);

    QAction *act = addAction(QStringLiteral("edit_item_duration"), i18n("Edit Duration"), this, SLOT(slotEditItemDuration()),
                             QIcon::fromTheme(QStringLiteral("measure")), QKeySequence(), clipActionCategory);
    act->setEnabled(false);

    act = addAction(QStringLiteral("edit_item_speed"), i18n("Change Speed"), this, SLOT(slotEditItemSpeed()),
                             QIcon::fromTheme(QStringLiteral("speedometer")), QKeySequence(), clipActionCategory);
    act->setEnabled(false);

    act = addAction(QStringLiteral("clip_in_project_tree"), i18n("Clip in Project Bin"), this, SLOT(slotClipInProjectTree()),
                    QIcon::fromTheme(QStringLiteral("find-location")), QKeySequence(), clipActionCategory);
    act->setEnabled(false);
    // "C" as data means this action should only be available for clips - not for compositions
    act->setData('C');

    act = addAction(QStringLiteral("cut_timeline_clip"), i18n("Cut Clip"), this, SLOT(slotCutTimelineClip()), QIcon::fromTheme(QStringLiteral("edit-cut")),
                    Qt::SHIFT + Qt::Key_R);

    act = addAction(QStringLiteral("cut_timeline_all_clips"), i18n("Cut All Clips"), this, SLOT(slotCutTimelineAllClips()), QIcon::fromTheme(QStringLiteral("edit-cut")),
                    Qt::CTRL + Qt::SHIFT + Qt::Key_R);

    act = addAction(QStringLiteral("delete_timeline_clip"), i18n("Delete Selected Item"), this, SLOT(slotDeleteItem()),
                    QIcon::fromTheme(QStringLiteral("edit-delete")), Qt::Key_Delete);

    QAction *resizeStart = new QAction(QIcon(), i18n("Resize Item Start"), this);
    addAction(QStringLiteral("resize_timeline_clip_start"), resizeStart, QKeySequence(Qt::Key_ParenLeft));
    connect(resizeStart, &QAction::triggered, this, &MainWindow::slotResizeItemStart);

    QAction *resizeEnd = new QAction(QIcon(), i18n("Resize Item End"), this);
    addAction(QStringLiteral("resize_timeline_clip_end"), resizeEnd, QKeySequence(Qt::Key_ParenRight));
    connect(resizeEnd, &QAction::triggered, this, &MainWindow::slotResizeItemEnd);

    QAction *pasteEffects = addAction(QStringLiteral("paste_effects"), i18n("Paste Effects"), this, SLOT(slotPasteEffects()),
                                      QIcon::fromTheme(QStringLiteral("edit-paste")), QKeySequence(), clipActionCategory);
    pasteEffects->setEnabled(false);
    // "C" as data means this action should only be available for clips - not for compositions
    pasteEffects->setData('C');
    
    QAction *delEffects = new QAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Delete Effects"), this);
    addAction(QStringLiteral("delete_effects"), delEffects, QKeySequence(), clipActionCategory);
    delEffects->setEnabled(false);
    // "C" as data means this action should only be available for clips - not for compositions
    delEffects->setData('C');
    connect(delEffects, &QAction::triggered, [this]() {
        getMainTimeline()->controller()->deleteEffects();
    });
    

    QAction *groupClip = addAction(QStringLiteral("group_clip"), i18n("Group Clips"), this, SLOT(slotGroupClips()),
                                   QIcon::fromTheme(QStringLiteral("object-group")), Qt::CTRL + Qt::Key_G, clipActionCategory);
    // "G" as data means this action should only be available for multiple items selection
    groupClip->setData('G');
    groupClip->setEnabled(false);

    QAction *ungroupClip = addAction(QStringLiteral("ungroup_clip"), i18n("Ungroup Clips"), this, SLOT(slotUnGroupClips()),
                                     QIcon::fromTheme(QStringLiteral("object-ungroup")), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_G), clipActionCategory);
    // "U" as data means this action should only be available if selection is a group
    ungroupClip->setData('U');
    ungroupClip->setEnabled(false);

    act = clipActionCategory->addAction(KStandardAction::Copy, this, SLOT(slotCopy()));
    act->setEnabled(false);

    KStandardAction::paste(this, SLOT(slotPaste()), actionCollection());

    // Keyframe actions
    m_assetPanel = new AssetPanel(this);
    KActionCategory *kfActions = new KActionCategory(i18n("Effect Keyframes"), actionCollection());
    addAction(QStringLiteral("keyframe_add"), i18n("Add/Remove Keyframe"), m_assetPanel, SLOT(slotAddRemoveKeyframe()),
                                     QIcon::fromTheme(QStringLiteral("keyframe-add")), QKeySequence(), kfActions);
    addAction(QStringLiteral("keyframe_next"), i18n("Go to next keyframe"), m_assetPanel, SLOT(slotNextKeyframe()),
                                     QIcon::fromTheme(QStringLiteral("keyframe-next")), QKeySequence(), kfActions);
    addAction(QStringLiteral("keyframe_previous"), i18n("Go to previous keyframe"), m_assetPanel, SLOT(slotPreviousKeyframe()),
                                     QIcon::fromTheme(QStringLiteral("keyframe-previous")), QKeySequence(), kfActions);

    /*act = KStandardAction::copy(this, SLOT(slotCopy()), actionCollection());
    clipActionCategory->addAction(KStandardAction::name(KStandardAction::Copy), act);
    act->setEnabled(false);
    act = KStandardAction::paste(this, SLOT(slotPaste()), actionCollection());
    clipActionCategory->addAction(KStandardAction::name(KStandardAction::Paste), act);
    act->setEnabled(false);*/

    kdenliveCategoryMap.insert(QStringLiteral("timelineselection"), clipActionCategory);

    addAction(QStringLiteral("insert_space"), i18n("Insert Space"), this, SLOT(slotInsertSpace()));
    addAction(QStringLiteral("delete_space"), i18n("Remove Space"), this, SLOT(slotRemoveSpace()));
    addAction(QStringLiteral("delete_space_all_tracks"), i18n("Remove Space In All Tracks"), this, SLOT(slotRemoveAllSpace()));

    KActionCategory *timelineActions = new KActionCategory(i18n("Tracks"), actionCollection());
    QAction *insertTrack = new QAction(QIcon(), i18nc("@action", "Insert Track"), this);
    connect(insertTrack, &QAction::triggered, this, &MainWindow::slotInsertTrack);
    timelineActions->addAction(QStringLiteral("insert_track"), insertTrack);

    QAction *masterEffectStack = new QAction(QIcon::fromTheme(QStringLiteral("kdenlive-composite")), i18n("Master effects"), this);
    connect(masterEffectStack, &QAction::triggered, this, [&]() {
        pCore->monitorManager()->activateMonitor(Kdenlive::ProjectMonitor);
        getCurrentTimeline()->controller()->showMasterEffects();
    });
    timelineActions->addAction(QStringLiteral("master_effects"), masterEffectStack);

    QAction *switchTrackTarget = new QAction(QIcon(), i18n("Switch Track Target Audio Stream"), this);
    connect(switchTrackTarget, &QAction::triggered, this, &MainWindow::slotSwitchTrackAudioStream);
    timelineActions->addAction(QStringLiteral("switch_target_stream"), switchTrackTarget);
    actionCollection()->setDefaultShortcut(switchTrackTarget, Qt::Key_Apostrophe);

    QAction *deleteTrack = new QAction(QIcon(), i18n("Delete Track"), this);
    connect(deleteTrack, &QAction::triggered, this, &MainWindow::slotDeleteTrack);
    timelineActions->addAction(QStringLiteral("delete_track"), deleteTrack);
    deleteTrack->setData("delete_track");

    QAction *showAudio = new QAction(QIcon(), i18n("Show Record Controls"), this);
    connect(showAudio, &QAction::triggered, this, &MainWindow::slotShowTrackRec);
    timelineActions->addAction(QStringLiteral("show_track_record"), showAudio);
    showAudio->setCheckable(true);
    showAudio->setData("show_track_record");

    QAction *selectTrack = new QAction(QIcon(), i18n("Select All in Current Track"), this);
    connect(selectTrack, &QAction::triggered, this, &MainWindow::slotSelectTrack);
    timelineActions->addAction(QStringLiteral("select_track"), selectTrack);

    QAction *selectAll = KStandardAction::selectAll(this, SLOT(slotSelectAllTracks()), this);
    selectAll->setIcon(QIcon::fromTheme(QStringLiteral("kdenlive-select-all")));
    selectAll->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    timelineActions->addAction(QStringLiteral("select_all_tracks"), selectAll);

    QAction *unselectAll = KStandardAction::deselect(this, SLOT(slotUnselectAllTracks()), this);
    unselectAll->setIcon(QIcon::fromTheme(QStringLiteral("kdenlive-unselect-all")));
    unselectAll->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    timelineActions->addAction(QStringLiteral("unselect_all_tracks"), unselectAll);

    kdenliveCategoryMap.insert(QStringLiteral("timeline"), timelineActions);

    // Cached data management
    addAction(QStringLiteral("manage_cache"), i18n("Manage Cached Data"), this, SLOT(slotManageCache()),
              QIcon::fromTheme(QStringLiteral("network-server-database")));

    QAction *disablePreview = new QAction(i18n("Disable Timeline Preview"), this);
    disablePreview->setCheckable(true);
    addAction(QStringLiteral("disable_preview"), disablePreview);

    addAction(QStringLiteral("add_guide"), i18n("Add/Remove Guide"), this, SLOT(slotAddGuide()), QIcon::fromTheme(QStringLiteral("list-add")), Qt::Key_G);
    addAction(QStringLiteral("delete_guide"), i18n("Delete Guide"), this, SLOT(slotDeleteGuide()), QIcon::fromTheme(QStringLiteral("edit-delete")));
    addAction(QStringLiteral("edit_guide"), i18n("Edit Guide"), this, SLOT(slotEditGuide()), QIcon::fromTheme(QStringLiteral("document-properties")));

    QAction *lockGuides = addAction(QStringLiteral("lock_guides"), i18n("Guides Locked"), this, SLOT(slotLockGuides(bool)), QIcon::fromTheme(QStringLiteral("kdenlive-lock")));
    lockGuides->setCheckable(true);
    lockGuides->setChecked(KdenliveSettings::lockedGuides());

    addAction(QStringLiteral("delete_all_guides"), i18n("Delete All Guides"), this, SLOT(slotDeleteAllGuides()),
              QIcon::fromTheme(QStringLiteral("edit-delete")));
    addAction(QStringLiteral("add_subtitle"), i18n("Add Subtitle"), this, SLOT(slotAddSubtitle()), QIcon::fromTheme(QStringLiteral("list-add")), Qt::SHIFT +Qt::Key_S);
    addAction(QStringLiteral("disable_subtitle"), i18n("Disable Subtitle"), this, SLOT(slotDisableSubtitle()), QIcon::fromTheme(QStringLiteral("view-hidden")));
    addAction(QStringLiteral("lock_subtitle"), i18n("Lock Subtitle"), this, SLOT(slotLockSubtitle()), QIcon::fromTheme(QStringLiteral("kdenlive-lock")));

    addAction(QStringLiteral("import_subtitle"), i18n("Import Subtitle File"), this, SLOT(slotImportSubtitle()), QIcon::fromTheme(QStringLiteral("document-import")));
    addAction(QStringLiteral("export_subtitle"), i18n("Export Subtitle File"), this, SLOT(slotExportSubtitle()), QIcon::fromTheme(QStringLiteral("document-export")));
    addAction(QStringLiteral("delete_subtitle_clip"), i18n("Delete Subtitle"), this, SLOT(slotDeleteItem()), QIcon::fromTheme(QStringLiteral("edit-delete")));
    addAction(QStringLiteral("audio_recognition"), i18n("Speech Recognition"), this, SLOT(slotSpeechRecognition()), QIcon::fromTheme(QStringLiteral("autocorrection")));

    m_saveAction = KStandardAction::save(pCore->projectManager(), SLOT(saveFile()), actionCollection());
    m_saveAction->setIcon(QIcon::fromTheme(QStringLiteral("document-save")));

    QAction *sentToLibrary = addAction(QStringLiteral("send_library"), i18n("Add Timeline Selection to Library"), pCore->library(), SLOT(slotAddToLibrary()),
                                       QIcon::fromTheme(QStringLiteral("bookmark-new")));
    sentToLibrary->setEnabled(false);

    pCore->library()->setupActions(QList<QAction *>() << sentToLibrary);

    KStandardAction::showMenubar(this, SLOT(showMenuBar(bool)), actionCollection());

    act = KStandardAction::quit(this, SLOT(close()), actionCollection());
    // act->setIcon(QIcon::fromTheme(QStringLiteral("application-exit")));

    KStandardAction::keyBindings(this, SLOT(slotEditKeys()), actionCollection());
    KStandardAction::preferences(this, SLOT(slotPreferences()), actionCollection());
    KStandardAction::configureNotifications(this, SLOT(configureNotifications()), actionCollection());
    KStandardAction::fullScreen(this, SLOT(slotFullScreen()), this, actionCollection());

    QAction *undo = KStandardAction::undo(m_commandStack, SLOT(undo()), actionCollection());
    undo->setEnabled(false);
    connect(m_commandStack, &QUndoGroup::canUndoChanged, undo, &QAction::setEnabled);
    connect(this, &MainWindow::enableUndo, this, [this, undo] (bool enable) {
        if (enable) {
            enable = m_commandStack->activeStack()->canUndo();
        }
        undo->setEnabled(enable);
    });

    QAction *redo = KStandardAction::redo(m_commandStack, SLOT(redo()), actionCollection());
    redo->setEnabled(false);
    connect(m_commandStack, &QUndoGroup::canRedoChanged, redo, &QAction::setEnabled);
    connect(this, &MainWindow::enableUndo, this, [this, redo] (bool enable) {
        if (enable) {
            enable = m_commandStack->activeStack()->canRedo();
        }
        redo->setEnabled(enable);
    });

    addAction(QStringLiteral("copy_debuginfo"), i18n("Copy Debug Information"), this, SLOT(slotCopyDebugInfo()),
              QIcon::fromTheme(QStringLiteral("edit-copy")));

    QAction *disableEffects = addAction(QStringLiteral("disable_timeline_effects"), i18n("Disable Timeline Effects"), pCore->projectManager(),
                                        SLOT(slotDisableTimelineEffects(bool)), QIcon::fromTheme(QStringLiteral("favorite")));
    disableEffects->setData("disable_timeline_effects");
    disableEffects->setCheckable(true);
    disableEffects->setChecked(false);

    addAction(QStringLiteral("switch_track_lock"), i18n("Toggle Track Lock"), pCore->projectManager(), SLOT(slotSwitchTrackLock()), QIcon(),
              Qt::SHIFT + Qt::Key_L);
    addAction(QStringLiteral("switch_all_track_lock"), i18n("Toggle All Track Lock"), pCore->projectManager(), SLOT(slotSwitchAllTrackLock()), QIcon(),
              Qt::CTRL + Qt::SHIFT + Qt::Key_L);
    addAction(QStringLiteral("switch_track_target"), i18n("Toggle Track Target"), pCore->projectManager(), SLOT(slotSwitchTrackTarget()), QIcon(),
              Qt::SHIFT + Qt::Key_T);
    addAction(QStringLiteral("switch_active_target"), i18n("Toggle Track Active"), pCore->projectManager(), SLOT(slotSwitchTrackActive()), QIcon(),
              Qt::Key_A);
    addAction(QStringLiteral("switch_all_targets"), i18n("Toggle All Tracks Active"), pCore->projectManager(), SLOT(slotSwitchAllTrackActive()), QIcon(),
              Qt::SHIFT + Qt::Key_A);
    addAction(QStringLiteral("activate_all_targets"), i18n("Switch All Tracks Active"), pCore->projectManager(), SLOT(slotMakeAllTrackActive()), QIcon(),
              Qt::SHIFT + Qt::ALT + Qt::Key_A);
    addAction(QStringLiteral("restore_all_sources"), i18n("Restore Current Clip Target Tracks"), pCore->projectManager(), SLOT(slotRestoreTargetTracks()));
    addAction(QStringLiteral("add_project_note"), i18n("Add Project Note"), pCore->projectManager(), SLOT(slotAddProjectNote()),
              QIcon::fromTheme(QStringLiteral("bookmark-new")));
    
    
    // Build activate track shortcut sequences
    QList <int> keysequence{Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9};
    for (int i = 1; i < 10; i++) {
        QAction *ac = new QAction(QIcon(), i18n("Select Audio Track %1", i), this);
        ac->setData(i - 1);
        connect(ac, &QAction::triggered, this, &MainWindow::slotActivateAudioTrackSequence);
        addAction(QString("activate_audio_%1").arg(i), ac, QKeySequence(int(Qt::ALT) + keysequence[i-1]), timelineActions);
        QAction *ac2 = new QAction(QIcon(), i18n("Select Video Track %1", i), this);
        ac2->setData(i - 1);
        connect(ac2, &QAction::triggered, this, &MainWindow::slotActivateVideoTrackSequence);
        addAction(QString("activate_video_%1").arg(i), ac2, QKeySequence(keysequence[i-1]), timelineActions);
        QAction *ac3 = new QAction(QIcon(), i18n("Select Target %1", i), this);
        ac3->setData(i - 1);
        connect(ac3, &QAction::triggered, this, &MainWindow::slotActivateTarget);
        addAction(QString("activate_target_%1").arg(i), ac3, QKeySequence(Qt::CTRL, keysequence[i-1]), timelineActions);
    }

    pCore->bin()->setupMenu();

    // Setup effects and transitions actions.
    KActionCategory *transitionActions = new KActionCategory(i18n("Transitions"), actionCollection());
    // m_transitions = new QAction*[transitions.count()];
    auto allTransitions = TransitionsRepository::get()->getNames();
    for (const auto &transition : qAsConst(allTransitions)) {
        auto *transAction = new QAction(transition.first, this);
        transAction->setData(transition.second);
        transAction->setIconVisibleInMenu(false);
        transitionActions->addAction("transition_" + transition.second, transAction);
    }

    // monitor actions
    addAction(QStringLiteral("extract_frame"), i18n("Extract frame..."), pCore->monitorManager(), SLOT(slotExtractCurrentFrame()),
              QIcon::fromTheme(QStringLiteral("insert-image")));

    addAction(QStringLiteral("extract_frame_to_project"), i18n("Extract frame to project..."), pCore->monitorManager(),
              SLOT(slotExtractCurrentFrameToProject()), QIcon::fromTheme(QStringLiteral("insert-image")));
}

void MainWindow::saveOptions()
{
    KdenliveSettings::self()->save();
}

bool MainWindow::readOptions()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    pCore->projectManager()->recentFilesAction()->loadEntries(KConfigGroup(config, "Recent Files"));

    if (KdenliveSettings::defaultprojectfolder().isEmpty()) {
        QDir dir(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
        dir.mkpath(QStringLiteral("."));
        KdenliveSettings::setDefaultprojectfolder(dir.absolutePath());
    }
    QFont ft = QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont);
    // Default unit for timeline.qml objects size
    int baseUnit = qMax(28, int(QFontInfo(ft).pixelSize() * 1.8 + 0.5));
    if (KdenliveSettings::trackheight() == 0) {
        int trackHeight = qMax(50, int(2.2 * baseUnit + 6));
        KdenliveSettings::setTrackheight(trackHeight);
    }
    bool firstRun = false;
    KConfigGroup initialGroup(config, "version");
    if (!initialGroup.exists() || KdenliveSettings::sdlAudioBackend().isEmpty()) {
        // First run, check if user is on a KDE Desktop
        firstRun = true;
        //Define default video location for first run
        KRecentDirs::add(QStringLiteral(":KdenliveClipFolder"), QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));

        // this is our first run, show Wizard
        QPointer<Wizard> w = new Wizard(true, false);
        if (w->exec() == QDialog::Accepted && w->isOk()) {
            w->adjustSettings();
            delete w;
        } else {
            delete w;
            ::exit(1);
        }
    } else if (!KdenliveSettings::ffmpegpath().isEmpty() && !QFile::exists(KdenliveSettings::ffmpegpath())) {
        // Invalid entry for FFmpeg, check system
        QPointer<Wizard> w = new Wizard(true, config->name().contains(QLatin1String("appimage")));
        if (w->exec() == QDialog::Accepted && w->isOk()) {
            w->adjustSettings();
        }
        delete w;
    }
    initialGroup.writeEntry("version", version);
    return firstRun;
}

void MainWindow::slotRunWizard()
{
    QPointer<Wizard> w = new Wizard(false, false, this);
    if (w->exec() == QDialog::Accepted && w->isOk()) {
        w->adjustSettings();
    }
    delete w;
}

void MainWindow::slotRefreshProfiles()
{
    KdenliveSettingsDialog *d = static_cast<KdenliveSettingsDialog *>(KConfigDialog::exists(QStringLiteral("settings")));
    if (d) {
        d->checkProfile();
    }
}

void MainWindow::slotEditProjectSettings()
{
    KdenliveDoc *project = pCore->currentDoc();
    QPair <int, int> p = getMainTimeline()->getTracksCount();
    int channels = qMin(project->getDocumentProperty(QStringLiteral("audioChannels"), QStringLiteral("2")).toInt(), 2);
    ProjectSettings *w = new ProjectSettings(project, project->metadata(), getMainTimeline()->controller()->extractCompositionLumas(), p.first, p.second, channels,
                                             project->projectTempFolder(), true, !project->isModified(), this);
    connect(w, &ProjectSettings::disableProxies, this, &MainWindow::slotDisableProxies);
    // connect(w, SIGNAL(disablePreview()), pCore->projectManager()->currentTimeline(), SLOT(invalidateRange()));
    connect(w, &ProjectSettings::refreshProfiles, this, &MainWindow::slotRefreshProfiles);

    if (w->exec() == QDialog::Accepted) {
        QString profile = w->selectedProfile();
        // project->setProjectFolder(w->selectedFolder());
        bool modified = false;
        if (m_renderWidget) {
            m_renderWidget->updateDocumentPath();
        }
        if (KdenliveSettings::videothumbnails() != w->enableVideoThumbs()) {
            slotSwitchVideoThumbs();
        }
        if (KdenliveSettings::audiothumbnails() != w->enableAudioThumbs()) {
            slotSwitchAudioThumbs();
        }
        if (project->getDocumentProperty(QStringLiteral("previewparameters")) != w->proxyParams() ||
            project->getDocumentProperty(QStringLiteral("previewextension")) != w->proxyExtension()) {
            modified = true;
            project->setDocumentProperty(QStringLiteral("previewparameters"), w->previewParams());
            project->setDocumentProperty(QStringLiteral("previewextension"), w->previewExtension());
            slotClearPreviewRender(false);
        }
        if (project->getDocumentProperty(QStringLiteral("proxyparams")) != w->proxyParams() ||
            project->getDocumentProperty(QStringLiteral("proxyextension")) != w->proxyExtension()) {
            modified = true;
            project->setDocumentProperty(QStringLiteral("proxyparams"), w->proxyParams());
            project->setDocumentProperty(QStringLiteral("proxyextension"), w->proxyExtension());
            if (pCore->projectItemModel()->clipsCount() > 0 &&
                KMessageBox::questionYesNo(this, i18n("You have changed the proxy parameters. Do you want to recreate all proxy clips for this project?")) ==
                    KMessageBox::Yes) {
                pCore->bin()->rebuildProxies();
            }
        }

        if (project->getDocumentProperty(QStringLiteral("externalproxyparams")) != w->externalProxyParams()) {
            modified = true;
            project->setDocumentProperty(QStringLiteral("externalproxyparams"), w->externalProxyParams());
            if (pCore->projectItemModel()->clipsCount() > 0 &&
                KMessageBox::questionYesNo(this, i18n("You have changed the proxy parameters. Do you want to recreate all proxy clips for this project?")) ==
                    KMessageBox::Yes) {
                pCore->bin()->rebuildProxies();
            }
        }

        if (project->getDocumentProperty(QStringLiteral("generateproxy")) != QString::number(int(w->generateProxy()))) {
            modified = true;
            project->setDocumentProperty(QStringLiteral("generateproxy"), QString::number(int(w->generateProxy())));
        }
        if (project->getDocumentProperty(QStringLiteral("proxyminsize")) != QString::number(w->proxyMinSize())) {
            modified = true;
            project->setDocumentProperty(QStringLiteral("proxyminsize"), QString::number(w->proxyMinSize()));
        }
        if (project->getDocumentProperty(QStringLiteral("generateimageproxy")) != QString::number(int(w->generateImageProxy()))) {
            modified = true;
            project->setDocumentProperty(QStringLiteral("generateimageproxy"), QString::number(int(w->generateImageProxy())));
        }
        if (project->getDocumentProperty(QStringLiteral("proxyimageminsize")) != QString::number(w->proxyImageMinSize())) {
            modified = true;
            project->setDocumentProperty(QStringLiteral("proxyimageminsize"), QString::number(w->proxyImageMinSize()));
        }
        if (project->getDocumentProperty(QStringLiteral("proxyimagesize")) != QString::number(w->proxyImageSize())) {
            modified = true;
            project->setDocumentProperty(QStringLiteral("proxyimagesize"), QString::number(w->proxyImageSize()));
        }
        if (project->getDocumentProperty(QStringLiteral("proxyresize")) != QString::number(w->proxyResize())) {
            modified = true;
            project->setDocumentProperty(QStringLiteral("proxyresize"), QString::number(w->proxyResize()));
        }
        if (QString::number(int(w->useProxy())) != project->getDocumentProperty(QStringLiteral("enableproxy"))) {
            project->setDocumentProperty(QStringLiteral("enableproxy"), QString::number(int(w->useProxy())));
            modified = true;
            slotUpdateProxySettings();
        }
        if (QString::number(int(w->useExternalProxy())) != project->getDocumentProperty(QStringLiteral("enableexternalproxy"))) {
            project->setDocumentProperty(QStringLiteral("enableexternalproxy"), QString::number(int(w->useExternalProxy())));
            modified = true;
        }
        if (w->metadata() != project->metadata()) {
            project->setMetadata(w->metadata());
        }
        QString newProjectFolder = w->storageFolder();
        if (newProjectFolder.isEmpty()) {
            newProjectFolder = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        }
        if (newProjectFolder != project->projectTempFolder()) {
            KMessageBox::ButtonCode answer;
            // Project folder changed:
            if (project->isModified()) {
                answer = KMessageBox::warningContinueCancel(this, i18n("The current project has not been saved. This will first save the project, then move "
                                                                       "all temporary files from <b>%1</b> to <b>%2</b>, and the project file will be reloaded",
                                                                       project->projectTempFolder(), newProjectFolder));
                if (answer == KMessageBox::Continue) {
                    pCore->projectManager()->saveFile();
                }
            } else {
                answer = KMessageBox::warningContinueCancel(
                    this, i18n("This will move all temporary files from <b>%1</b> to <b>%2</b>, the project file will then be reloaded",
                               project->projectTempFolder(), newProjectFolder));
            }
            if (answer == KMessageBox::Continue) {
                // Proceed with move
                QString documentId = QDir::cleanPath(project->getDocumentProperty(QStringLiteral("documentid")));
                bool ok;
                documentId.toLongLong(&ok, 10);
                if (!ok || documentId.isEmpty()) {
                    KMessageBox::sorry(this, i18n("Cannot perform operation, invalid document id: %1", documentId));
                } else {
                    QDir newDir(newProjectFolder);
                    QDir oldDir(project->projectTempFolder());
                    if (newDir.exists(documentId)) {
                        KMessageBox::sorry(this, i18n("Cannot perform operation, target directory already exists: %1", newDir.absoluteFilePath(documentId)));
                    } else {
                        // Proceed with the move
                        pCore->projectManager()->moveProjectData(oldDir.absoluteFilePath(documentId), newDir.absolutePath());
                    }
                }
            }
        }
        if (pCore->getCurrentProfile()->path() != profile || project->profileChanged(profile)) {
            if (!qFuzzyCompare(pCore->getCurrentProfile()->fps() - ProfileRepository::get()->getProfile(profile)->fps(), 0.)) {
                // Fps was changed, we save the project to an xml file with updated profile and reload project
                // Check if blank project
                if (project->url().fileName().isEmpty() && !project->isModified()) {
                    // Trying to switch project profile from an empty project
                    pCore->setCurrentProfile(profile);
                    pCore->projectManager()->newFile(profile, false);
                    return;
                }
                pCore->projectManager()->saveWithUpdatedProfile(profile);
            } else {
                bool darChanged = !qFuzzyCompare(pCore->getCurrentProfile()->dar(), ProfileRepository::get()->getProfile(profile)->dar());
                pCore->setCurrentProfile(profile);
                pCore->projectManager()->slotResetProfiles(darChanged);
                slotUpdateDocumentState(true);
            }
        } else if (modified) {
            project->setModified();
        }
    }
    delete w;
}

void MainWindow::slotDisableProxies()
{
    pCore->currentDoc()->setDocumentProperty(QStringLiteral("enableproxy"), QString::number(false));
    pCore->currentDoc()->setModified();
    slotUpdateProxySettings();
}

void MainWindow::slotStopRenderProject()
{
    if (m_renderWidget) {
        m_renderWidget->slotAbortCurrentJob();
    }
}

void MainWindow::slotRenderProject()
{
    KdenliveDoc *project = pCore->currentDoc();

    if ( !m_renderWidget && project ) {
        m_renderWidget = new RenderWidget(project->useProxy(), this);
        connect(m_renderWidget, &RenderWidget::shutdown, this, &MainWindow::slotShutdown);
        connect(m_renderWidget, &RenderWidget::selectedRenderProfile, this, &MainWindow::slotSetDocumentRenderProfile);
        connect(m_renderWidget, &RenderWidget::abortProcess, this, &MainWindow::abortRenderJob);
        connect(this, &MainWindow::updateRenderWidgetProfile, m_renderWidget, &RenderWidget::adjustViewToProfile);
        connect(this, &MainWindow::updateProjectPath, m_renderWidget, &RenderWidget::resetRenderPath);
        m_renderWidget->setGuides(project->getGuideModel());
        m_renderWidget->updateDocumentPath();
        m_renderWidget->setRenderProfile(project->getRenderProperties());
    }
    if ( m_renderWidget && m_compositeAction && m_compositeAction->currentAction() ) {
        m_renderWidget->errorMessage(RenderWidget::CompositeError, m_compositeAction->currentAction()->data().toInt() == 1
                                                                        ? i18n("Rendering using low quality track compositing")
                                                                        : QString());
    }

    slotCheckRenderStatus();
    if ( m_renderWidget ) {
        m_renderWidget->showNormal();
    }

    // What are the following lines supposed to do?
    // m_renderWidget->enableAudio(false);
    // m_renderWidget->export_audio;
}

void MainWindow::slotCheckRenderStatus()
{
    // Make sure there are no missing clips
    // TODO
    /*if (m_renderWidget)
        m_renderWidget->missingClips(pCore->bin()->hasMissingClips());*/
}

void MainWindow::setRenderingProgress(const QString &url, int progress, int frame)
{
    emit setRenderProgress(progress);
    if (m_renderWidget) {
        m_renderWidget->setRenderJob(url, progress, frame);
    }
}

void MainWindow::setRenderingFinished(const QString &url, int status, const QString &error)
{
    emit setRenderProgress(100);
    if (m_renderWidget) {
        m_renderWidget->setRenderStatus(url, status, error);
    }
}

void MainWindow::addProjectClip(const QString &url, const QString &folder)
{
    if (pCore->currentDoc()) {
        QStringList ids = pCore->projectItemModel()->getClipByUrl(QFileInfo(url));
        if (!ids.isEmpty()) {
            // Clip is already in project bin, abort
            return;
        }
        ClipCreator::createClipFromFile(url, folder, pCore->projectItemModel());
    }
}

void MainWindow::addTimelineClip(const QString &url)
{
    if (pCore->currentDoc()) {
        QStringList ids = pCore->projectItemModel()->getClipByUrl(QFileInfo(url));
        if (!ids.isEmpty()) {
            pCore->selectBinClip(ids.constFirst());
            slotInsertClipInsert();
        }
    }
}

void MainWindow::scriptRender(const QString &url)
{
    slotRenderProject();
    m_renderWidget->slotPrepareExport(true, url);
}

void MainWindow::exitApp()
{
    QApplication::exit(0);
}

void MainWindow::slotCleanProject()
{
    if (KMessageBox::warningContinueCancel(this, i18n("This will remove all unused clips from your project."), i18n("Clean up project")) ==
        KMessageBox::Cancel) {
        return;
    }
    pCore->bin()->cleanupUnused();
}

void MainWindow::slotUpdateMousePosition(int pos)
{
    if (pCore->currentDoc()) {
        switch (m_timeFormatButton->currentItem()) {
        case 0:
            m_timeFormatButton->setText(pCore->currentDoc()->timecode().getTimecodeFromFrames(pos) + QStringLiteral(" / ") +
                                        pCore->currentDoc()->timecode().getTimecodeFromFrames(getMainTimeline()->controller()->duration() - 1));
            break;
        default:
            m_timeFormatButton->setText(
                QStringLiteral("%1 / %2").arg(pos, 6, 10, QLatin1Char('0')).arg(getMainTimeline()->controller()->duration() - 1, 6, 10, QLatin1Char('0')));
        }
    }
}

void MainWindow::slotUpdateProjectDuration(int pos)
{
    Q_UNUSED(pos)
    if (pCore->currentDoc()) {
        slotUpdateMousePosition(getMainTimeline()->controller()->getMousePos());
    }
}

void MainWindow::slotUpdateDocumentState(bool modified)
{
    setWindowTitle(pCore->currentDoc()->description());
    setWindowModified(modified);
    m_saveAction->setEnabled(modified);
}

void MainWindow::connectDocument()
{
    KdenliveDoc *project = pCore->currentDoc();
    connect(project, &KdenliveDoc::startAutoSave, pCore->projectManager(), &ProjectManager::slotStartAutoSave);
    connect(project, &KdenliveDoc::reloadEffects, this, &MainWindow::slotReloadEffects);
    KdenliveSettings::setProject_fps(pCore->getCurrentFps());
    m_projectMonitor->slotLoadClipZone(project->zone());
    m_clipMonitor->updateDocumentUuid();
    connect(m_projectMonitor, &Monitor::multitrackView, getMainTimeline()->controller(), &TimelineController::slotMultitrackView, Qt::UniqueConnection);
    connect(m_projectMonitor, &Monitor::activateTrack, getMainTimeline()->controller(), &TimelineController::activateTrackAndSelect, Qt::UniqueConnection);
    connect(getMainTimeline()->controller(), &TimelineController::timelineClipSelected, this, [&] (bool selected) {
        m_loopClip->setEnabled(selected);
        emit pCore->library()->enableAddSelection(selected);
    });
    connect(pCore->library(), &LibraryWidget::saveTimelineSelection, getMainTimeline()->controller(), &TimelineController::saveTimelineSelection,
            Qt::UniqueConnection);
    connect(pCore->monitorManager(), &MonitorManager::frameDisplayed, [&](const SharedFrame &frame) {
        emit pCore->mixer()->updateLevels(frame.get_position());
        //QMetaObject::invokeMethod(this, "setAudioValues", Qt::QueuedConnection, Q_ARG(const QVector<int> &, levels));
    });
    connect(pCore->mixer(), &MixerManager::purgeCache, m_projectMonitor, &Monitor::purgeCache);

    // TODO REFAC: reconnect to new timeline
    /*
    Timeline *trackView = pCore->projectManager()->currentTimeline();
    connect(trackView, &Timeline::configTrack, this, &MainWindow::slotConfigTrack);
    connect(trackView, &Timeline::updateTracksInfo, this, &MainWindow::slotUpdateTrackInfo);
    connect(trackView, &Timeline::mousePosition, this, &MainWindow::slotUpdateMousePosition);
    connect(pCore->producerQueue(), &ProducerQueue::infoProcessingFinished, trackView->projectView(), &CustomTrackView::slotInfoProcessingFinished,
    Qt::DirectConnection);

    connect(trackView->projectView(), &CustomTrackView::importKeyframes, this, &MainWindow::slotProcessImportKeyframes);
    connect(trackView->projectView(), &CustomTrackView::updateTrimMode, this, &MainWindow::setTrimMode);
    connect(m_projectMonitor, SIGNAL(renderPosition(int)), trackView, SLOT(moveCursorPos(int)));
    connect(m_projectMonitor, SIGNAL(zoneUpdated(QPoint)), trackView, SLOT(slotSetZone(QPoint)));

    connect(trackView->projectView(), &CustomTrackView::guidesUpdated, this, &MainWindow::slotGuidesUpdated);
    connect(trackView->projectView(), &CustomTrackView::loadMonitorScene, m_projectMonitor, &Monitor::slotShowEffectScene);
    connect(trackView->projectView(), &CustomTrackView::setQmlProperty, m_projectMonitor, &Monitor::setQmlProperty);
    connect(m_projectMonitor, SIGNAL(acceptRipple(bool)), trackView->projectView(), SLOT(slotAcceptRipple(bool)));
    connect(m_projectMonitor, SIGNAL(switchTrimMode(int)), trackView->projectView(), SLOT(switchTrimMode(int)));
    connect(project, &KdenliveDoc::saveTimelinePreview, trackView, &Timeline::slotSaveTimelinePreview);

    connect(trackView, SIGNAL(showTrackEffects(int, TrackInfo)), this, SLOT(slotTrackSelected(int, TrackInfo)));

    connect(trackView->projectView(), &CustomTrackView::clipItemSelected, this, &MainWindow::slotTimelineClipSelected, Qt::DirectConnection);
    connect(trackView->projectView(), &CustomTrackView::setActiveKeyframe, m_effectStack, &EffectStackView2::setActiveKeyframe);
    connect(trackView->projectView(), SIGNAL(transitionItemSelected(Transition *, int, QPoint, bool)), m_effectStack, SLOT(slotTransitionItemSelected(Transition
    *, int, QPoint, bool)), Qt::DirectConnection);

    connect(trackView->projectView(), SIGNAL(transitionItemSelected(Transition *, int, QPoint, bool)), this, SLOT(slotActivateTransitionView(Transition *)));

    connect(trackView->projectView(), &CustomTrackView::zoomIn, this, &MainWindow::slotZoomIn);
    connect(trackView->projectView(), &CustomTrackView::zoomOut, this, &MainWindow::slotZoomOut);
    connect(trackView, SIGNAL(setZoom(int)), this, SLOT(slotSetZoom(int)));

    connect(trackView, SIGNAL(displayMessage(QString, MessageType)), m_messageLabel, SLOT(setMessage(QString, MessageType)));
    connect(trackView->projectView(), SIGNAL(displayMessage(QString, MessageType)), m_messageLabel, SLOT(setMessage(QString, MessageType)));
    connect(pCore->bin(), &Bin::clipNameChanged, trackView->projectView(), &CustomTrackView::clipNameChanged);

    connect(trackView->projectView(), SIGNAL(showClipFrame(QString, int)), pCore->bin(), SLOT(selectClipById(QString, int)));
    connect(trackView->projectView(), SIGNAL(playMonitor()), m_projectMonitor, SLOT(slotPlay()));
    connect(trackView->projectView(), &CustomTrackView::pauseMonitor, m_projectMonitor, &Monitor::pause, Qt::DirectConnection);

    connect(m_projectMonitor, &Monitor::addEffect, trackView->projectView(), &CustomTrackView::slotAddEffectToCurrentItem);

    connect(trackView->projectView(), SIGNAL(transitionItemSelected(Transition *, int, QPoint, bool)), m_projectMonitor, SLOT(slotSetSelectedClip(Transition
    *)));

    connect(pCore->bin(), SIGNAL(gotFilterJobResults(QString, int, int, stringMap, stringMap)), trackView->projectView(), SLOT(slotGotFilterJobResults(QString,
    int, int, stringMap, stringMap)));

    //TODO
    //connect(m_projectList, SIGNAL(addMarkers(QString,QList<CommentedTime>)), trackView->projectView(), SLOT(slotAddClipMarker(QString,QList<CommentedTime>)));

    // Effect stack signals
    connect(m_effectStack, &EffectStackView2::updateEffect, trackView->projectView(), &CustomTrackView::slotUpdateClipEffect);
    connect(m_effectStack, &EffectStackView2::updateClipRegion, trackView->projectView(), &CustomTrackView::slotUpdateClipRegion);
    connect(m_effectStack, SIGNAL(removeEffect(ClipItem *, int, QDomElement)), trackView->projectView(), SLOT(slotDeleteEffect(ClipItem *, int, QDomElement)));
    connect(m_effectStack, SIGNAL(removeEffectGroup(ClipItem *, int, QDomDocument)), trackView->projectView(), SLOT(slotDeleteEffectGroup(ClipItem *, int,
    QDomDocument)));

    connect(m_effectStack, SIGNAL(addEffect(ClipItem *, QDomElement, int)), trackView->projectView(), SLOT(slotAddEffect(ClipItem *, QDomElement, int)));
    connect(m_effectStack, SIGNAL(changeEffectState(ClipItem *, int, QList<int>, bool)), trackView->projectView(), SLOT(slotChangeEffectState(ClipItem *, int,
    QList<int>, bool)));
    connect(m_effectStack, SIGNAL(changeEffectPosition(ClipItem *, int, QList<int>, int)), trackView->projectView(), SLOT(slotChangeEffectPosition(ClipItem *,
    int, QList<int>, int)));

    connect(m_effectStack, &EffectStackView2::refreshEffectStack, trackView->projectView(), &CustomTrackView::slotRefreshEffects);
    connect(m_effectStack, &EffectStackView2::seekTimeline, trackView->projectView(), &CustomTrackView::seekCursorPos);
    connect(m_effectStack, SIGNAL(importClipKeyframes(GraphicsRectItem, ItemInfo, QDomElement, QMap<QString, QString>)), trackView->projectView(),
    SLOT(slotImportClipKeyframes(GraphicsRectItem, ItemInfo, QDomElement, QMap<QString, QString>)));

    // Transition config signals
    connect(m_effectStack->transitionConfig(), SIGNAL(transitionUpdated(Transition *, QDomElement)), trackView->projectView(),
    SLOT(slotTransitionUpdated(Transition *, QDomElement)));
    connect(m_effectStack->transitionConfig(), &TransitionSettings::seekTimeline, trackView->projectView(), &CustomTrackView::seekCursorPos);

    connect(trackView->projectView(), SIGNAL(activateDocumentMonitor()), m_projectMonitor, SLOT(slotActivateMonitor()), Qt::DirectConnection);
    connect(project, &KdenliveDoc::updateFps, this,
            [this](double changed) {
        if (changed == 0.0) {
            slotUpdateProfile(false);
        } else {
            slotUpdateProfile(true);
        }
    }, Qt::DirectConnection);
    connect(trackView, &Timeline::zoneMoved, this, &MainWindow::slotZoneMoved);
    trackView->projectView()->setContextMenu(m_timelineContextMenu, m_timelineClipActions, m_timelineContextTransitionMenu, m_clipTypeGroup,
    static_cast<CustomMenu *>(factory()->container(QStringLiteral("marker_menu"), this)));
    */
    
    emit m_timelineTabs->changeZoom(project->zoom().x(), false);
    emit m_timelineTabs->updateZoom(project->zoom().x());
    
    getMainTimeline()->controller()->clipActions = kdenliveCategoryMap.value(QStringLiteral("timelineselection"))->actions();
    connect(m_projectMonitor, SIGNAL(zoneUpdated(QPoint)), project, SLOT(setModified()));
    connect(m_clipMonitor, SIGNAL(zoneUpdated(QPoint)), project, SLOT(setModified()));
    connect(project, &KdenliveDoc::docModified, this, &MainWindow::slotUpdateDocumentState);

    if (m_renderWidget) {
        slotCheckRenderStatus();
        m_renderWidget->setGuides(pCore->currentDoc()->getGuideModel());
        m_renderWidget->updateDocumentPath();
        m_renderWidget->setRenderProfile(project->getRenderProperties());
    }
    m_commandStack->setActiveStack(project->commandStack().get());
    setWindowTitle(project->description());
    setWindowModified(project->isModified());
    m_saveAction->setEnabled(project->isModified());
    m_normalEditTool->setChecked(true);
    connect(m_projectMonitor, &Monitor::durationChanged, this, &MainWindow::slotUpdateProjectDuration);
    connect(m_effectList2, &EffectListWidget::reloadFavorites, getMainTimeline(), &TimelineWidget::updateEffectFavorites);
    connect(m_transitionList2, &TransitionListWidget::reloadFavorites, getMainTimeline(), &TimelineWidget::updateTransitionFavorites);
    connect(pCore->bin(), &Bin::processDragEnd, getMainTimeline(), &TimelineWidget::endDrag);
    
    // Load master effect zones
    getMainTimeline()->controller()->updateMasterZones(getMainTimeline()->controller()->getModel()->getMasterEffectZones());

    // TODO REFAC: fix
    // trackView->updateProfile(1.0);
    // Init document zone
    // m_projectMonitor->slotZoneMoved(trackView->inPoint(), trackView->outPoint());
    // Update the mouse position display so it will display in DF/NDF format by default based on the project setting.
    // slotUpdateMousePosition(0);

    // Update guides info in render widget
    // slotGuidesUpdated();

    // set tool to select tool
    setTrimMode(QString());

    m_buttonSelectTool->setChecked(true);
    getMainTimeline()->focusTimeline();
}

void MainWindow::slotGuidesUpdated()
{
    if (m_renderWidget) {
        m_renderWidget->setGuides(pCore->currentDoc()->getGuideModel());
    }
}

void MainWindow::slotEditKeys()
{
    KShortcutsDialog dialog(KShortcutsEditor::AllActions, KShortcutsEditor::LetterShortcutsAllowed, this);
    // Find the combobox inside KShortcutsDialog for choosing keyboard scheme
    QComboBox *schemesList = nullptr;
    foreach (QLabel *label, dialog.findChildren<QLabel *>()) {
        if (label->text() == i18n("Current scheme:")) {
            schemesList = qobject_cast<QComboBox *>(label->buddy());
            break;
        }
    }
    // If scheme choosing combobox was found, find the "More Actions" button in the same
    // dialog that provides a dropdown menu with additional actions, and add
    // "Download New Keyboard Schemes..." button into that menu
    if (schemesList) {
        foreach (QPushButton *button, dialog.findChildren<QPushButton *>()) {
            if (button->text() == i18n("More Actions")) {
                auto moreActionsMenu = button->menu();
                moreActionsMenu->addAction(i18n("Download New Keyboard Schemes..."), this, [this, schemesList] { slotGetNewKeyboardStuff(schemesList); });
                break;
            }
        }
    } else {
        qWarning() << "Could not get list of schemes. Downloading new schemes is not available.";
    }
    dialog.addCollection(actionCollection(), i18nc("general keyboard shortcuts", "General"));
    dialog.configure();
    updateActionsToolTip();
}

void MainWindow::slotPreferences(int page, int option)
{
    /*
     * An instance of your dialog could be already created and could be
     * cached, in which case you want to display the cached dialog
     * instead of creating another one
     */
    if (KConfigDialog::showDialog(QStringLiteral("settings"))) {
        KdenliveSettingsDialog *d = static_cast<KdenliveSettingsDialog *>(KConfigDialog::exists(QStringLiteral("settings")));
        if (page != -1) {
            d->showPage(page, option);
        }
        return;
    }

    // KConfigDialog didn't find an instance of this dialog, so lets
    // create it :

    // Get the mappable actions in localized form
    QMap<QString, QString> actions;
    KActionCollection *collection = actionCollection();
    QRegExp ampEx("&{1,1}");
    for (const QString &action_name : qAsConst(m_actionNames)) {
        QString action_text = collection->action(action_name)->text();
        action_text.remove(ampEx);
        actions[action_text] = action_name;
    }

    auto *dialog = new KdenliveSettingsDialog(actions, m_gpuAllowed, this);
    connect(dialog, &KConfigDialog::settingsChanged, this, &MainWindow::updateConfiguration);
    connect(dialog, &KConfigDialog::settingsChanged, this, &MainWindow::configurationChanged);
    connect(dialog, &KdenliveSettingsDialog::doResetConsumer, this, [this] (bool fullReset) {
        m_scaleGroup->setEnabled(!KdenliveSettings::external_display());
        pCore->projectManager()->slotResetConsumers(fullReset);
    });
    connect(dialog, &KdenliveSettingsDialog::checkTabPosition, this, &MainWindow::slotCheckTabPosition);
    connect(dialog, &KdenliveSettingsDialog::restartKdenlive, this, &MainWindow::slotRestart);
    connect(dialog, &KdenliveSettingsDialog::updateLibraryFolder, pCore.get(), &Core::updateLibraryPath);
    connect(dialog, &KdenliveSettingsDialog::audioThumbFormatChanged, m_timelineTabs, &TimelineTabs::audioThumbFormatChanged);
    connect(dialog, &KdenliveSettingsDialog::resetView, this, &MainWindow::resetTimelineTracks);
    connect(dialog, &KdenliveSettingsDialog::updateMonitorBg, [&]() {
        pCore->monitorManager()->updateBgColor();
    });

    dialog->show();
    if (page != -1) {
        dialog->showPage(page, option);
    }
}

void MainWindow::slotCheckTabPosition()
{
    int pos = tabPosition(Qt::LeftDockWidgetArea);
    if (KdenliveSettings::tabposition() != pos) {
        setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::TabPosition(KdenliveSettings::tabposition()));
    }
}

void MainWindow::slotRestart(bool clean)
{
    if (clean) {
        if (KMessageBox::questionYesNo(this, i18n("This will delete Kdenlive's configuration file and restart the application. Do you want to proceed?")) != KMessageBox::Yes) {
            return;
        }
    }
    cleanRestart(clean);
}

void MainWindow::cleanRestart(bool clean)
{
    m_exitCode = clean ? EXIT_CLEAN_RESTART : EXIT_RESTART;
    QApplication::closeAllWindows();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    KXmlGuiWindow::closeEvent(event);
    if (event->isAccepted()) {
        QApplication::exit(m_exitCode);
        return;
    }
}

void MainWindow::updateConfiguration()
{
    // TODO: we should apply settings to all projects, not only the current one
    m_buttonAudioThumbs->setChecked(KdenliveSettings::audiothumbnails());
    m_buttonVideoThumbs->setChecked(KdenliveSettings::videothumbnails());
    m_buttonShowMarkers->setChecked(KdenliveSettings::showmarkers());

    // Update list of transcoding profiles
    buildDynamicActions();
    loadClipActions();
}

void MainWindow::slotSwitchVideoThumbs()
{
    KdenliveSettings::setVideothumbnails(!KdenliveSettings::videothumbnails());
    emit m_timelineTabs->showThumbnailsChanged();
    m_buttonVideoThumbs->setChecked(KdenliveSettings::videothumbnails());
}

void MainWindow::slotSwitchAudioThumbs()
{
    KdenliveSettings::setAudiothumbnails(!KdenliveSettings::audiothumbnails());
    pCore->bin()->checkAudioThumbs();
    emit m_timelineTabs->showAudioThumbnailsChanged();
    m_buttonAudioThumbs->setChecked(KdenliveSettings::audiothumbnails());
}

void MainWindow::slotSwitchMarkersComments()
{
    KdenliveSettings::setShowmarkers(!KdenliveSettings::showmarkers());
    emit getMainTimeline()->controller()->showMarkersChanged();
    m_buttonShowMarkers->setChecked(KdenliveSettings::showmarkers());
}

void MainWindow::slotSwitchSnap()
{
    KdenliveSettings::setSnaptopoints(!KdenliveSettings::snaptopoints());
    m_buttonSnap->setChecked(KdenliveSettings::snaptopoints());
    emit getMainTimeline()->controller()->snapChanged();
}

void MainWindow::slotShowTimelineTags()
{
    KdenliveSettings::setTagsintimeline(!KdenliveSettings::tagsintimeline());
    m_buttonTimelineTags->setChecked(KdenliveSettings::tagsintimeline());
    // Reset view to update timeline colors
    getMainTimeline()->controller()->getModel()->_resetView();
}

void MainWindow::slotDeleteItem()
{
    if (QApplication::focusWidget() != nullptr && pCore->bin()->isAncestorOf(QApplication::focusWidget())) {
        pCore->bin()->slotDeleteClip();
    } if (QApplication::focusWidget() != nullptr && pCore->textEditWidget()->isAncestorOf(QApplication::focusWidget())) {
        qDebug()<<"===============\nDELETE TEXT BASED ITEM";
        pCore->textEditWidget()->deleteItem();
    } else {
        QWidget *widget = QApplication::focusWidget();
        while ((widget != nullptr) && widget != this) {
//            效果栈Dock
//            if (widget == m_effectStackDock) {
//                m_assetPanel->deleteCurrentEffect();
//                return;
//            }
            if (widget == pCore->bin()->clipPropertiesDock()) {
                emit pCore->bin()->deleteMarkers();
                return;
            }
            widget = widget->parentWidget();
        }

        // effect stack has no focus
        getMainTimeline()->controller()->deleteSelectedClips();
    }
}

void MainWindow::slotAddClipMarker()
{
    std::shared_ptr<ProjectClip> clip(nullptr);
    GenTime pos;
    if (m_projectMonitor->isActive()) {
        getMainTimeline()->controller()->addMarker();
        return;
    } else {
        clip = m_clipMonitor->currentController();
        pos = GenTime(m_clipMonitor->position(), pCore->getCurrentFps());
    }
    if (!clip) {
        m_messageLabel->setMessage(i18n("Cannot find clip to add marker"), ErrorMessage);
        return;
    }
    clip->getMarkerModel()->editMarkerGui(pos, this, true, clip.get());
}

void MainWindow::slotDeleteClipMarker(bool allowGuideDeletion)
{
    std::shared_ptr<ProjectClip> clip(nullptr);
    GenTime pos;
    if (m_projectMonitor->isActive()) {
        getMainTimeline()->controller()->deleteMarker();
        return;
    } else {
        clip = m_clipMonitor->currentController();
        pos = GenTime(m_clipMonitor->position(), pCore->getCurrentFps());
    }
    if (!clip) {
        m_messageLabel->setMessage(i18n("Cannot find clip to remove marker"), ErrorMessage);
        return;
    }

    bool markerFound = false;
    CommentedTime marker = clip->getMarkerModel()->getMarker(pos, &markerFound);
    if (!markerFound) {
        if (allowGuideDeletion && m_projectMonitor->isActive()) {
            slotDeleteGuide();
        } else {
            m_messageLabel->setMessage(i18n("No marker found at cursor time"), ErrorMessage);
        }
        return;
    }
    clip->getMarkerModel()->removeMarker(pos);
}

void MainWindow::slotDeleteAllClipMarkers()
{
    std::shared_ptr<ProjectClip> clip(nullptr);
    if (m_projectMonitor->isActive()) {
        getMainTimeline()->controller()->deleteAllMarkers();
        return;
    } else {
        clip = m_clipMonitor->currentController();
    }
    if (!clip) {
        m_messageLabel->setMessage(i18n("Cannot find clip to remove marker"), ErrorMessage);
        return;
    }
    bool ok = clip->getMarkerModel()->removeAllMarkers();
    if (!ok) {
        m_messageLabel->setMessage(i18n("An error occurred while deleting markers"), ErrorMessage);
        return;
    }
}

void MainWindow::slotEditClipMarker()
{
    std::shared_ptr<ProjectClip> clip(nullptr);
    GenTime pos;
    if (m_projectMonitor->isActive()) {
        getMainTimeline()->controller()->editMarker();
        return;
    } else {
        clip = m_clipMonitor->currentController();
        pos = GenTime(m_clipMonitor->position(), pCore->getCurrentFps());
    }
    if (!clip) {
        m_messageLabel->setMessage(i18n("Cannot find clip to edit marker"), ErrorMessage);
        return;
    }

    bool markerFound = false;
    CommentedTime oldMarker = clip->getMarkerModel()->getMarker(pos, &markerFound);
    if (!markerFound) {
        m_messageLabel->setMessage(i18n("No marker found at cursor time"), ErrorMessage);
        return;
    }

    clip->getMarkerModel()->editMarkerGui(pos, this, false, clip.get());
}

void MainWindow::slotAddMarkerGuideQuickly()
{
    if (!getMainTimeline() || !pCore->currentDoc()) {
        return;
    }

    if (m_clipMonitor->isActive()) {
        pCore->bin()->addClipMarker(m_clipMonitor->activeClipId(), {m_clipMonitor->position()});
    } else {
        int selectedClip = getMainTimeline()->controller()->getMainSelectedItem();
        if (selectedClip == -1) {
            // Add timeline guide
            getMainTimeline()->controller()->switchGuide();
        } else {
            // Add marker to main clip
            getMainTimeline()->controller()->addQuickMarker(selectedClip);
        }
    }
}

void MainWindow::slotAddGuide()
{
    getMainTimeline()->controller()->switchGuide();
}

void MainWindow::slotInsertSpace()
{
    getMainTimeline()->controller()->insertSpace();
}

void MainWindow::slotRemoveSpace()
{
    getMainTimeline()->controller()->removeSpace(-1, -1, false);
}

void MainWindow::slotRemoveAllSpace()
{
    getMainTimeline()->controller()->removeSpace(-1, -1, true);
}

void MainWindow::slotSeparateAudioChannel()
{
    KdenliveSettings::setDisplayallchannels(!KdenliveSettings::displayallchannels());
    emit getCurrentTimeline()->controller()->audioThumbFormatChanged();
    if (m_clipMonitor) {
        m_clipMonitor->refreshAudioThumbs();
    }
}

void MainWindow::slotNormalizeAudioChannel()
{
    KdenliveSettings::setNormalizechannels(!KdenliveSettings::normalizechannels());
    emit getCurrentTimeline()->controller()->audioThumbNormalizeChanged();
    if (m_clipMonitor) {
        m_clipMonitor->normalizeAudioThumbs();
    }
}

void MainWindow::slotInsertTrack()
{
    pCore->monitorManager()->activateMonitor(Kdenlive::ProjectMonitor);
    getCurrentTimeline()->controller()->addTrack(-1);
}

void MainWindow::slotDeleteTrack()
{
    pCore->monitorManager()->activateMonitor(Kdenlive::ProjectMonitor);
   getCurrentTimeline()->controller()->deleteMultipleTracks(-1);
}

void MainWindow::slotSwitchTrackAudioStream()
{
    getCurrentTimeline()->showTargetMenu();
}

void MainWindow::slotShowTrackRec()
{
    getCurrentTimeline()->controller()->switchTrackRecord();
}

void MainWindow::slotSelectTrack()
{
    getCurrentTimeline()->controller()->selectCurrentTrack();
}

void MainWindow::slotSelectAllTracks()
{
    if (QApplication::focusWidget() != nullptr) {
        if (QApplication::focusWidget()->parentWidget() != nullptr && QApplication::focusWidget()->parentWidget() == pCore->bin()) {
            pCore->bin()->selectAll();
            return;
        } 
        if (QApplication::focusWidget()->objectName() == QLatin1String("markers_list")) {
            emit pCore->bin()->selectMarkers();
            return;
        }
    }
    getCurrentTimeline()->controller()->selectAll();
}

void MainWindow::slotUnselectAllTracks()
{
    getCurrentTimeline()->model()->requestClearSelection();
}

void MainWindow::slotEditGuide()
{
    getCurrentTimeline()->controller()->editGuide();
}

void MainWindow::slotLockGuides(bool lock)
{
    KdenliveSettings::setLockedGuides(lock);
    getCurrentTimeline()->controller()->guidesLockedChanged();
}

void MainWindow::slotDeleteGuide()
{
    getCurrentTimeline()->controller()->switchGuide(-1, true);
}

void MainWindow::slotDeleteAllGuides()
{
    pCore->currentDoc()->getGuideModel()->removeAllMarkers();
}

void MainWindow::slotCutTimelineClip()
{
    getMainTimeline()->controller()->cutClipUnderCursor();
}

void MainWindow::slotCutTimelineAllClips()
{
    getMainTimeline()->controller()->cutAllClipsUnderCursor();
}

void MainWindow::slotInsertClipOverwrite()
{
    const QString &binId = m_clipMonitor->activeClipId();
    if (binId.isEmpty()) {
        // No clip in monitor
        return;
    }
    getMainTimeline()->controller()->insertZone(binId, m_clipMonitor->getZoneInfo(), true);
}

void MainWindow::slotInsertClipInsert()
{
    const QString &binId = m_clipMonitor->activeClipId();
    if (binId.isEmpty()) {
        // No clip in monitor
        pCore->displayMessage(i18n("No clip selected in project bin"), ErrorMessage);
        return;
    }
    getMainTimeline()->controller()->insertZone(binId, m_clipMonitor->getZoneInfo(), false);
}

void MainWindow::slotExtractZone()
{
    getMainTimeline()->controller()->extractZone(m_clipMonitor->getZoneInfo());
}

void MainWindow::slotExtractClip()
{
    getMainTimeline()->controller()->extract();
}

void MainWindow::slotSaveZoneToBin()
{
    getMainTimeline()->controller()->saveZone();
}

void MainWindow::slotLiftZone()
{
    getMainTimeline()->controller()->extractZone(m_clipMonitor->getZoneInfo(), true);
}

void MainWindow::slotPreviewRender()
{
    if (pCore->currentDoc()) {
        getCurrentTimeline()->controller()->startPreviewRender();
    }
}

void MainWindow::slotStopPreviewRender()
{
    if (pCore->currentDoc()) {
        getCurrentTimeline()->controller()->stopPreviewRender();
    }
}

void MainWindow::slotDefinePreviewRender()
{
    if (pCore->currentDoc()) {
        getCurrentTimeline()->controller()->addPreviewRange(true);
    }
}

void MainWindow::slotRemovePreviewRender()
{
    if (pCore->currentDoc()) {
        getCurrentTimeline()->controller()->addPreviewRange(false);
    }
}

void MainWindow::slotClearPreviewRender(bool resetZones)
{
    if (pCore->currentDoc()) {
        getCurrentTimeline()->controller()->clearPreviewRange(resetZones);
    }
}

void MainWindow::slotSelectTimelineClip()
{
    getCurrentTimeline()->controller()->selectCurrentItem(ObjectType::TimelineClip, true);
}

void MainWindow::slotSelectTimelineTransition()
{
    getCurrentTimeline()->controller()->selectCurrentItem(ObjectType::TimelineComposition, true);
}

void MainWindow::slotDeselectTimelineClip()
{
    getCurrentTimeline()->controller()->selectCurrentItem(ObjectType::TimelineClip, false);
}

void MainWindow::slotDeselectTimelineTransition()
{
    getCurrentTimeline()->controller()->selectCurrentItem(ObjectType::TimelineComposition, false);
}

void MainWindow::slotSelectAddTimelineClip()
{
    getCurrentTimeline()->controller()->selectCurrentItem(ObjectType::TimelineClip, true, true);
}

void MainWindow::slotSelectAddTimelineTransition()
{
    getCurrentTimeline()->controller()->selectCurrentItem(ObjectType::TimelineComposition, true, true);
}

void MainWindow::slotGroupClips()
{
    getCurrentTimeline()->controller()->groupSelection();
}

void MainWindow::slotUnGroupClips()
{
    getCurrentTimeline()->controller()->unGroupSelection();
}

void MainWindow::slotEditItemDuration()
{
    getCurrentTimeline()->controller()->editItemDuration();
}

void MainWindow::slotAddProjectClip(const QUrl &url, const QString &folderInfo)
{
    pCore->bin()->droppedUrls(QList<QUrl>() << url, folderInfo);
}

void MainWindow::slotAddTextNote(const QString &text)
{
   pCore->projectManager()->slotAddTextNote(text);
}

void MainWindow::slotAddProjectClipList(const QList<QUrl> &urls)
{
    pCore->bin()->droppedUrls(urls);
}

void MainWindow::slotAddTransition(QAction *result)
{
    if (!result) {
        return;
    }
    // TODO refac
    /*
    QStringList info = result->data().toStringList();
    if (info.isEmpty() || info.count() < 2) {
        return;
    }
    QDomElement transition = transitions.getEffectByTag(info.at(0), info.at(1));
    if (pCore->projectManager()->currentTimeline() && !transition.isNull()) {
        pCore->projectManager()->currentTimeline()->projectView()->slotAddTransitionToSelectedClips(transition.cloneNode().toElement());
    }
    */
}

void MainWindow::slotAddEffect(QAction *result) {
    qDebug() << "// EFFECTS MENU TRIGGERED: " << result->data().toString();
    if (!result) {
        return;
    }
    QString effectId = result->data().toString();
    addEffect(effectId);
}

void MainWindow::addEffect(const QString &effectId) {
    if (m_assetPanel->effectStackOwner().first == ObjectType::TimelineClip) {
        // Add effect to the current timeline selection
        QVariantMap effectData;
        effectData.insert(QStringLiteral("kdenlive/effect"), effectId);
        pCore->window()->getMainTimeline()->controller()->addAsset(effectData);
    } else if (m_assetPanel->effectStackOwner().first == ObjectType::TimelineTrack || m_assetPanel->effectStackOwner().first == ObjectType::BinClip || m_assetPanel->effectStackOwner().first == ObjectType::Master) {
        if (!m_assetPanel->addEffect(effectId)) {
            pCore->displayMessage(i18n("Cannot add effect to clip"), ErrorMessage);
        }
    } else {
        pCore->displayMessage(i18n("Select an item to add effect"), ErrorMessage);
    }
}

void MainWindow::customEvent(QEvent *e)
{
    if (e->type() == QEvent::User) {
        m_messageLabel->setMessage(static_cast<MltErrorEvent *>(e)->message(), MltError);
    }
}

void MainWindow::slotSnapRewind()
{
    if (m_projectMonitor->isActive()) {
        getMainTimeline()->controller()->gotoPreviousSnap();
    } else {
        m_clipMonitor->slotSeekToPreviousSnap();
    }
}

void MainWindow::slotSnapForward()
{
    if (m_projectMonitor->isActive()) {
        getMainTimeline()->controller()->gotoNextSnap();
    } else {
        m_clipMonitor->slotSeekToNextSnap();
    }
}

void MainWindow::slotGuideRewind()
{
    if (m_projectMonitor->isActive()) {
        getMainTimeline()->controller()->gotoPreviousGuide();
    } else {
        m_clipMonitor->slotSeekToPreviousSnap();
    }
}

void MainWindow::slotGuideForward()
{
    if (m_projectMonitor->isActive()) {
        getMainTimeline()->controller()->gotoNextGuide();
    } else {
        m_clipMonitor->slotSeekToNextSnap();
    }
}

void MainWindow::slotClipStart()
{
    if (m_projectMonitor->isActive()) {
        getMainTimeline()->controller()->seekCurrentClip(false);
    } else {
        m_clipMonitor->slotStart();
    }
}

void MainWindow::slotClipEnd()
{
    if (m_projectMonitor->isActive()) {
        getMainTimeline()->controller()->seekCurrentClip(true);
    } else {
        m_clipMonitor->slotEnd();
    }
}

void MainWindow::slotChangeTool(QAction *action)
{
    if (action == m_buttonSelectTool) {
        slotSetTool(SelectTool);
    } else if (action == m_buttonRazorTool) {
        slotSetTool(RazorTool);
    } else if (action == m_buttonSpacerTool) {
        slotSetTool(SpacerTool);
    }
}

/** @footnote
 *  作用：更改时间线编辑模式
 *  m_trimLabel: statusBar中最左侧的显示当前编辑状态的label
 */
void MainWindow::slotChangeEdit(QAction *action)
{
    TimelineMode::EditMode mode = TimelineMode::NormalEdit;
    if (action == m_overwriteEditTool) {
        mode = TimelineMode::OverwriteEdit;
        m_trimLabel->setText(i18n("Overwrite"));
        m_trimLabel->setStyleSheet(QStringLiteral("QLabel { padding-left: 2; padding-right: 2; background-color :darkGreen; }"));
    } else if (action == m_insertEditTool) {
        mode = TimelineMode::InsertEdit;
        m_trimLabel->setText(i18n("Insert"));
        m_trimLabel->setStyleSheet(QStringLiteral("QLabel { padding-left: 2; padding-right: 2; background-color :red; }"));
    } else {
        m_trimLabel->setText(QString());
        m_trimLabel->setStyleSheet(QString());
    }
    getMainTimeline()->controller()->getModel()->setEditMode(mode);
    if (mode == TimelineMode::InsertEdit) {
        // Disable spacer tool in insert mode
        if (m_buttonSpacerTool->isChecked()) {
            m_buttonSelectTool->setChecked(true);
            slotSetTool(SelectTool);
        }
        m_buttonSpacerTool->setEnabled(false);
    } else {
        m_buttonSpacerTool->setEnabled(true);
    }
}

void MainWindow::slotSetTool(ProjectTool tool)
{
    if (pCore->currentDoc()) {
        showToolMessage();
        getMainTimeline()->setTool(tool);
    }
}

void MainWindow::showToolMessage()
{
    QString message;
    if (m_buttonSelectTool->isChecked()) {
        message = i18n("<b>Shift drag</b> for rubber-band selection, <b>Shift click</b> for multiple selection, <b>Ctrl drag</b> to pan");
    } else if (m_buttonRazorTool->isChecked()) {
        message = i18n("<b>Shift</b> to preview cut frame");
    } else if (m_buttonSpacerTool->isChecked()) {
        message = i18n("<b>Ctrl</b> to apply on current track only, <b>Shift</b> to also move guides. You can combine both modifiers.");
    }
    m_messageLabel->setKeyMap(message);
}

void MainWindow::setWidgetKeyBinding(const QString &mess)
{
    m_messageLabel->setKeyMap(mess);
}

void MainWindow::showKeyBinding(const QString &text)
{
    m_messageLabel->setTmpKeyMap(text);
}

void MainWindow::slotCopy()
{
    getMainTimeline()->controller()->copyItem();
}

void MainWindow::slotPaste()
{
    getMainTimeline()->controller()->pasteItem();
}

void MainWindow::slotPasteEffects()
{
    getMainTimeline()->controller()->pasteEffects();
}

void MainWindow::slotClipInTimeline(const QString &clipId, const QList<int> &ids)
{
    Q_UNUSED(clipId)
    CustomMenu *inTimelineMenu = static_cast<CustomMenu *>(factory()->container(QStringLiteral("clip_in_timeline"), this));
    QList<QAction *> actionList;
    for (int i = 0; i < ids.count(); ++i) {
        QString track = getMainTimeline()->controller()->getTrackNameFromIndex(pCore->getItemTrack(ObjectId(ObjectType::TimelineClip, ids.at(i))));
        QString start = pCore->currentDoc()->timecode().getTimecodeFromFrames(pCore->getItemPosition(ObjectId(ObjectType::TimelineClip, ids.at(i))));
        int j = 0;
        QAction *a = new QAction(track + QStringLiteral(": ") + start, inTimelineMenu);
        a->setData(ids.at(i));
        connect(a, &QAction::triggered, this, &MainWindow::slotSelectClipInTimeline);
        while (j < actionList.count()) {
            if (actionList.at(j)->text() > a->text()) {
                break;
            }
            j++;
        }
        actionList.insert(j, a);
    }
    QList<QAction *> list = inTimelineMenu->actions();
    unplugActionList(QStringLiteral("timeline_occurences"));
    qDeleteAll(list);
    plugActionList(QStringLiteral("timeline_occurences"), actionList);

    if (actionList.isEmpty()) {
        inTimelineMenu->setEnabled(false);
    } else {
        inTimelineMenu->setEnabled(true);
    }
}

void MainWindow::raiseBin()
{
    // 设置binDock的层级和可见性
}

void MainWindow::slotClipInProjectTree()
{
    QList<int> ids = getMainTimeline()->controller()->selection();
    if (!ids.isEmpty()) {
        raiseBin();
        ObjectId id(ObjectType::TimelineClip, ids.constFirst());
        int start = pCore->getItemIn(id);
        int duration = pCore->getItemDuration(id);
        int pos = m_projectMonitor->position();
        int itemPos = pCore->getItemPosition(id);
        bool containsPos = (pos >= itemPos && pos < itemPos + duration);
        double speed = pCore->getClipSpeed(id.second);
        if (containsPos) {
            pos -= itemPos - start;
        }
        if (!qFuzzyCompare(speed, 1.)) {
            if (speed > 0.) {
                // clip has a speed effect, adjust zone
                start = qRound(start * speed);
                duration = qRound(duration * speed);
                if (containsPos) {
                    pos = qRound(pos * speed);
                }
            } else if (speed < 0.) {
                int max = getMainTimeline()->controller()->clipMaxDuration(id.second);
                if (max > 0) {
                    int invertedPos = itemPos + duration - m_projectMonitor->position();
                    start = qRound((max - (start + duration)) * -speed);
                    duration = qRound(duration * -speed);
                    if (containsPos) {
                        pos = start + qRound(invertedPos * -speed);
                    }
                }
            }
        }
        QPoint zone(start, start + duration);
        if (!containsPos) {
            pos = start;
        }
        pCore->selectBinClip(getMainTimeline()->controller()->getClipBinId(ids.constFirst()), true, pos, zone);
    }
}

void MainWindow::slotSelectClipInTimeline()
{
    pCore->monitorManager()->activateMonitor(Kdenlive::ProjectMonitor);
    auto *action = qobject_cast<QAction *>(sender());
    int clipId = action->data().toInt();
    getMainTimeline()->controller()->focusItem(clipId);
}

/** Gets called when the window gets hidden */
void MainWindow::hideEvent(QHideEvent * /*event*/)
{
    if (isMinimized() && pCore->monitorManager()) {
        pCore->monitorManager()->pauseActiveMonitor();
    }
}

/*void MainWindow::slotSaveZone(Render *render, const QPoint &zone, DocClipBase *baseClip, QUrl path)
{
    QPointer<QDialog> dialog = new QDialog(this);
    dialog->setWindowTitle("Save clip zone");
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    dialog->setLayout(mainLayout);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    dialog->connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    dialog->connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    QLabel *label1 = new QLabel(i18n("Save clip zone as:"), this);
    if (path.isEmpty()) {
        QString tmppath = pCore->currentDoc()->projectFolder().path() + QDir::separator();
        if (baseClip == nullptr) {
            tmppath.append("untitled.mlt");
        } else {
            tmppath.append((baseClip->name().isEmpty() ? baseClip->fileURL().fileName() : baseClip->name()) + '-' + QString::number(zone.x()).rightJustified(4,
'0') + QStringLiteral(".mlt"));
        }
        path = QUrl(tmppath);
    }
    KUrlRequester *url = new KUrlRequester(path, this);
    url->setFilter("video/mlt-playlist");
    QLabel *label2 = new QLabel(i18n("Description:"), this);
    QLineEdit *edit = new QLineEdit(this);
    mainLayout->addWidget(label1);
    mainLayout->addWidget(url);
    mainLayout->addWidget(label2);
    mainLayout->addWidget(edit);
    mainLayout->addWidget(buttonBox);

    if (dialog->exec() == QDialog::Accepted) {
        if (QFile::exists(url->url().path())) {
            if (KMessageBox::questionYesNo(this, i18n("File %1 already exists.\nDo you want to overwrite it?", url->url().path())) == KMessageBox::No) {
                slotSaveZone(render, zone, baseClip, url->url());
                delete dialog;
                return;
            }
        }
        if (baseClip && !baseClip->fileURL().isEmpty()) {
            // create zone from clip url, so that we don't have problems with proxy clips
            QProcess p;
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.remove("MLT_PROFILE");
            p.setProcessEnvironment(env);
            p.start(KdenliveSettings::rendererpath(), QStringList() << baseClip->fileURL().toLocalFile() << "in=" + QString::number(zone.x()) << "out=" +
QString::number(zone.y()) << "-consumer" << "xml:" + url->url().path());
            if (!p.waitForStarted(3000)) {
                KMessageBox::sorry(this, i18n("Cannot start MLT's renderer:\n%1", KdenliveSettings::rendererpath()));
            }
            else if (!p.waitForFinished(5000)) {
                KMessageBox::sorry(this, i18n("Timeout while creating xml output"));
            }
        }
        else render->saveZone(url->url(), edit->text(), zone);
    }
    delete dialog;
}*/

void MainWindow::slotResizeItemStart()
{
    getMainTimeline()->controller()->setInPoint();
}

void MainWindow::slotResizeItemEnd()
{
    getMainTimeline()->controller()->setOutPoint();
}

int MainWindow::getNewStuff(const QString &configFile)
{
    KNS3::Entry::List entries;
    QPointer<KNS3::DownloadDialog> dialog = new KNS3::DownloadDialog(configFile);
    if (dialog->exec() != 0) {
        entries = dialog->changedEntries();
    }
    for (const KNS3::Entry &entry : qAsConst(entries)) {
        if (entry.status() == KNS3::Entry::Installed) {
            qCDebug(KDENLIVE_LOG) << "// Installed files: " << entry.installedFiles();
        }
    }
    delete dialog;
    return entries.size();
}

void MainWindow::slotGetNewKeyboardStuff(QComboBox *schemesList)
{
    if (getNewStuff(QStringLiteral(":data/kdenlive_keyboardschemes.knsrc")) > 0) {
        // Refresh keyboard schemes list (schemes list creation code copied from KShortcutSchemesEditor)
        QStringList schemes;
        schemes << QStringLiteral("Default");
        // List files in the shortcuts subdir, each one is a scheme. See KShortcutSchemesHelper::{shortcutSchemeFileName,exportActionCollection}
        const QStringList shortcutsDirs = QStandardPaths::locateAll(
            QStandardPaths::GenericDataLocation, QCoreApplication::applicationName() + QStringLiteral("/shortcuts"), QStandardPaths::LocateDirectory);
        qCDebug(KDENLIVE_LOG) << "shortcut scheme dirs:" << shortcutsDirs;
        Q_FOREACH (const QString &dir, shortcutsDirs) {
            Q_FOREACH (const QString &file, QDir(dir).entryList(QDir::Files | QDir::NoDotAndDotDot)) {
                qCDebug(KDENLIVE_LOG) << "shortcut scheme file:" << file;
                schemes << file;
            }
        }
        schemesList->clear();
        schemesList->addItems(schemes);
    }
}

void MainWindow::slotAutoTransition()
{
    // TODO refac
    /*
    if (pCore->projectManager()->currentTimeline()) {
        pCore->projectManager()->currentTimeline()->projectView()->autoTransition();
    }
    */
}

void MainWindow::slotSplitAV()
{
    getMainTimeline()->controller()->splitAV();
}

void MainWindow::slotSwitchClip()
{
    getMainTimeline()->controller()->switchEnableState();
}

void MainWindow::slotSetAudioAlignReference()
{
    getMainTimeline()->controller()->setAudioRef();
}

void MainWindow::slotAlignAudio()
{
    getMainTimeline()->controller()->alignAudio();
}

void MainWindow::slotUpdateClipType(QAction *action)
{
    Q_UNUSED(action)
    // TODO refac
    /*
    if (pCore->projectManager()->currentTimeline()) {
        PlaylistState::ClipState state = (PlaylistState::ClipState)action->data().toInt();
        pCore->projectManager()->currentTimeline()->projectView()->setClipType(state);
    }
    */
}

void MainWindow::slotUpdateTimelineView(QAction *action)
{
    int viewMode = action->data().toInt();
    KdenliveSettings::setAudiotracksbelow(viewMode);
    getMainTimeline()->controller()->getModel()->_resetView();
}

void MainWindow::slotShowTimeline(bool show)
{
    if (!show) {
        m_timelineState = saveState();
        centralWidget()->setHidden(true);
    } else {
        centralWidget()->setHidden(false);
        restoreState(m_timelineState);
    }
}

void MainWindow::loadClipActions()
{
    unplugActionList(QStringLiteral("add_effect"));
    plugActionList(QStringLiteral("add_effect"), m_effectsMenu->actions());

    QList<QAction *> clipJobActions = getExtraActions(QStringLiteral("clipjobs"));
    unplugActionList(QStringLiteral("clip_jobs"));
    plugActionList(QStringLiteral("clip_jobs"), clipJobActions);

    QList<QAction *> atcActions = getExtraActions(QStringLiteral("audiotranscoderslist"));
    unplugActionList(QStringLiteral("audio_transcoders_list"));
    plugActionList(QStringLiteral("audio_transcoders_list"), atcActions);

    QList<QAction *> tcActions = getExtraActions(QStringLiteral("transcoderslist"));
    unplugActionList(QStringLiteral("transcoders_list"));
    plugActionList(QStringLiteral("transcoders_list"), tcActions);
}

void MainWindow::loadDockActions()
{
    QList<QAction *> list = kdenliveCategoryMap.value(QStringLiteral("interface"))->actions();
    // Sort actions
    QMap<QString, QAction *> sorted;
    QStringList sortedList;
    for (QAction *a : qAsConst(list)) {
        sorted.insert(a->text(), a);
        sortedList << a->text();
    }
    QList<QAction *> orderedList;
    sortedList.sort(Qt::CaseInsensitive);
    for (const QString &text : qAsConst(sortedList)) {
        orderedList << sorted.value(text);
    }
    unplugActionList(QStringLiteral("dock_actions"));
    plugActionList(QStringLiteral("dock_actions"), orderedList);
}

void MainWindow::buildDynamicActions()
{
    KActionCategory *ts = nullptr;
    if (kdenliveCategoryMap.contains(QStringLiteral("clipjobs"))) {
        ts = kdenliveCategoryMap.take(QStringLiteral("clipjobs"));
        delete ts;
    }
    ts = new KActionCategory(i18n("Clip Jobs"), m_extraFactory->actionCollection());

    Mlt::Profile profile;
    std::unique_ptr<Mlt::Filter> filter = std::make_unique<Mlt::Filter>(profile, "vidstab");
    if ((filter != nullptr) && filter->is_valid()) {
        QAction *action = new QAction(i18n("Stabilize"), m_extraFactory->actionCollection());
        ts->addAction(action->text(), action);
        connect(action, &QAction::triggered, [this]() {
            StabilizeTask::start(this);
        });
    }
    filter = std::make_unique<Mlt::Filter>(profile, "motion_est");
    if (filter) {
        if (filter->is_valid()) {
            QAction *action = new QAction(i18n("Automatic scene split"), m_extraFactory->actionCollection());
            ts->addAction(action->text(), action);
            connect(action, &QAction::triggered, [&]() {
                SceneSplitTask::start(this);
            });
        }
    }
    if (true /* TODO: check if timewarp producer is available */) {
        QAction *action = new QAction(i18n("Duplicate clip with speed change"), m_extraFactory->actionCollection());
        ts->addAction(action->text(), action);
        connect(action, &QAction::triggered,
                [&]() { SpeedTask::start(this); });
    }

    // TODO refac reimplement analyseclipjob
    /*
    QAction *action = new QAction(i18n("Analyse keyframes"), m_extraFactory->actionCollection());
    QStringList stabJob(QString::number((int)AbstractClipJob::ANALYSECLIPJOB));
    action->setData(stabJob);
    ts->addAction(action->text(), action);
    connect(action, &QAction::triggered, pCore->bin(), &Bin::slotStartClipJob);
    */
    kdenliveCategoryMap.insert(QStringLiteral("clipjobs"), ts);

    if (kdenliveCategoryMap.contains(QStringLiteral("transcoderslist"))) {
        ts = kdenliveCategoryMap.take(QStringLiteral("transcoderslist"));
        delete ts;
    }
    if (kdenliveCategoryMap.contains(QStringLiteral("audiotranscoderslist"))) {
        ts = kdenliveCategoryMap.take(QStringLiteral("audiotranscoderslist"));
        delete ts;
    }
    // transcoders
    ts = new KActionCategory(i18n("Transcoders"), m_extraFactory->actionCollection());
    KActionCategory *ats = new KActionCategory(i18n("Extract Audio"), m_extraFactory->actionCollection());
    KSharedConfigPtr config =
        KSharedConfig::openConfig(QStringLiteral("kdenlivetranscodingrc"), KConfig::CascadeConfig, QStandardPaths::AppDataLocation);
    KConfigGroup transConfig(config, "Transcoding");
    // read the entries
    QMap<QString, QString> profiles = transConfig.entryMap();
    QMapIterator<QString, QString> i(profiles);
    while (i.hasNext()) {
        i.next();
        QStringList transList;
        transList << i.value().split(QLatin1Char(';'));
        auto *a = new QAction(i.key(), m_extraFactory->actionCollection());
        a->setData(transList);
        if (transList.count() > 1) {
            a->setToolTip(transList.at(1));
        }
        connect(a, &QAction::triggered, [&, a]() {
            QStringList transcodeData = a->data().toStringList();
            std::vector<QString> ids = pCore->bin()->selectedClipsIds(true);
            for (QString id : ids) {
                std::shared_ptr<ProjectClip> clip = pCore->projectItemModel()->getClipByBinID(id);
                TranscodeTask::start({ObjectType::BinClip,id.toInt()}, transcodeData.first(), -1, -1, false, clip.get());
            }
        });
        if (transList.count() > 2 && transList.at(2) == QLatin1String("audio")) {
            // This is an audio transcoding action
            ats->addAction(i.key(), a);
        } else {
            ts->addAction(i.key(), a);
        }
    }
    kdenliveCategoryMap.insert(QStringLiteral("transcoderslist"), ts);
    kdenliveCategoryMap.insert(QStringLiteral("audiotranscoderslist"), ats);

    // Populate View menu with show / hide actions for dock widgets
    KActionCategory *guiActions = nullptr;
    if (kdenliveCategoryMap.contains(QStringLiteral("interface"))) {
        guiActions = kdenliveCategoryMap.take(QStringLiteral("interface"));
        delete guiActions;
    }
    guiActions = new KActionCategory(i18n("Interface"), actionCollection());
    QAction *showTimeline = new QAction(i18n("Timeline"), this);
    showTimeline->setCheckable(true);
    showTimeline->setChecked(true);
    connect(showTimeline, &QAction::triggered, this, &MainWindow::slotShowTimeline);
    guiActions->addAction(showTimeline->text(), showTimeline);
    actionCollection()->addAction(showTimeline->text(), showTimeline);

    QList<QDockWidget *> docks = findChildren<QDockWidget *>();
    for (auto dock : qAsConst(docks)) {
        QAction *dockInformations = dock->toggleViewAction();
        if (!dockInformations) {
            continue;
        }
        dockInformations->setChecked(!dock->isHidden());
        guiActions->addAction(dockInformations->text(), dockInformations);
    }
    kdenliveCategoryMap.insert(QStringLiteral("interface"), guiActions);
}

QList<QAction *> MainWindow::getExtraActions(const QString &name)
{
    if (!kdenliveCategoryMap.contains(name)) {
        return QList<QAction *>();
    }
    return kdenliveCategoryMap.value(name)->actions();
}

void MainWindow::slotTranscode(const QStringList &urls)
{
    Q_ASSERT(!urls.isEmpty());
    QString params;
    QString desc;
    ClipTranscode *d = new ClipTranscode(urls, params, QStringList(), desc, pCore->bin()->getCurrentFolder());
    connect(d, &ClipTranscode::addClip, this, &MainWindow::slotAddProjectClip);
    d->show();
}

void MainWindow::slotTranscodeClip()
{
    const QString dialogFilter = ClipCreationDialog::getExtensionsFilter(QStringList() << i18n("All Files") + QStringLiteral(" (*)"));
    QString clipFolder = KRecentDirs::dir(QStringLiteral(":KdenliveClipFolder"));
    QStringList urls = QFileDialog::getOpenFileNames(this, i18n("Files to transcode"), clipFolder, dialogFilter);
    if (urls.isEmpty()) {
        return;
    }
    slotTranscode(urls);
}

void MainWindow::slotSetDocumentRenderProfile(const QMap<QString, QString> &props)
{
    KdenliveDoc *project = pCore->currentDoc();
    bool modified = false;
    QMapIterator<QString, QString> i(props);
    while (i.hasNext()) {
        i.next();
        if (project->getDocumentProperty(i.key()) == i.value()) {
            continue;
        }
        project->setDocumentProperty(i.key(), i.value());
        modified = true;
    }
    if (modified) {
        project->setModified();
    }
}

void MainWindow::slotUpdateTimecodeFormat(int ix)
{
    KdenliveSettings::setFrametimecode(ix == 1);
    m_clipMonitor->updateTimecodeFormat();
    m_projectMonitor->updateTimecodeFormat();
    // TODO refac: reimplement ?
    // m_effectStack->transitionConfig()->updateTimecodeFormat();
    // m_effectStack->updateTimecodeFormat();
    pCore->bin()->updateTimecodeFormat();
    emit getMainTimeline()->controller()->frameFormatChanged();
    m_timeFormatButton->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
}

void MainWindow::slotRemoveFocus()
{
    getMainTimeline()->setFocus();
}

void MainWindow::slotShutdown()
{
    pCore->currentDoc()->setModified(false);
    // Call shutdown
    QDBusConnectionInterface *interface = QDBusConnection::sessionBus().interface();
    if ((interface != nullptr) && interface->isServiceRegistered(QStringLiteral("org.kde.ksmserver"))) {
        QDBusInterface smserver(QStringLiteral("org.kde.ksmserver"), QStringLiteral("/KSMServer"), QStringLiteral("org.kde.KSMServerInterface"));
        smserver.call(QStringLiteral("logout"), 1, 2, 2);
    } else if ((interface != nullptr) && interface->isServiceRegistered(QStringLiteral("org.gnome.SessionManager"))) {
        QDBusInterface smserver(QStringLiteral("org.gnome.SessionManager"), QStringLiteral("/org/gnome/SessionManager"),
                                QStringLiteral("org.gnome.SessionManager"));
        smserver.call(QStringLiteral("Shutdown"));
    }
}

void MainWindow::slotSwitchMonitors()
{
    pCore->monitorManager()->slotSwitchMonitors(!m_clipMonitor->isActive());
    if (m_projectMonitor->isActive()) {
        getMainTimeline()->setFocus();
    } else {
        pCore->bin()->focusBinView();
    }
}

void MainWindow::slotSwitchMonitorOverlay(QAction *action)
{
    if (pCore->monitorManager()->isActive(Kdenlive::ClipMonitor)) {
        m_clipMonitor->switchMonitorInfo(action->data().toInt());
    } else {
        m_projectMonitor->switchMonitorInfo(action->data().toInt());
    }
}

void MainWindow::slotSwitchDropFrames(bool drop)
{
    KdenliveSettings::setMonitor_dropframes(drop);
    m_clipMonitor->restart();
    m_projectMonitor->restart();
}

void MainWindow::slotSetMonitorGamma(int gamma)
{
    KdenliveSettings::setMonitor_gamma(gamma);
    m_clipMonitor->restart();
    m_projectMonitor->restart();
}

void MainWindow::slotInsertZoneToTree()
{
    if (!m_clipMonitor->isActive() || m_clipMonitor->currentController() == nullptr) {
        return;
    }
    QPoint info = m_clipMonitor->getZoneInfo();
    QString id;
    pCore->projectItemModel()->requestAddBinSubClip(id, info.x(), info.y(), {}, m_clipMonitor->activeClipId());
}

void MainWindow::slotMonitorRequestRenderFrame(bool request)
{
    if (request) {
        m_projectMonitor->sendFrameForAnalysis(true);
        return;
    }
//    for (int i = 0; i < m_gfxScopesList.count(); ++i) {
//        if (m_gfxScopesList.at(i)->isVisible() && tabifiedDockWidgets(m_gfxScopesList.at(i)).isEmpty() &&
//            static_cast<AbstractGfxScopeWidget *>(m_gfxScopesList.at(i)->widget())->autoRefreshEnabled()) {
//            request = true;
//            break;
//        }
//    }

#ifdef DEBUG_MAINW
    qCDebug(KDENLIVE_LOG) << "Any scope accepting new frames? " << request;
#endif
    if (!request) {
        m_projectMonitor->sendFrameForAnalysis(false);
    }
}

void MainWindow::slotUpdateProxySettings()
{
    KdenliveDoc *project = pCore->currentDoc();
    if (m_renderWidget) {
        m_renderWidget->updateProxyConfig(project->useProxy());
    }
    pCore->bin()->refreshProxySettings();
}

void MainWindow::slotArchiveProject()
{
    KdenliveDoc *doc = pCore->currentDoc();
    pCore->projectManager()->prepareSave();
    QString sceneData = pCore->projectManager()->projectSceneList(doc->url().adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash).toLocalFile());
    if (sceneData.isEmpty()) {
        KMessageBox::error(this, i18n("此项目文件尚无法被打包."));
        return;
    }
    QPointer<ArchiveWidget> d(new ArchiveWidget(doc->url().fileName(), sceneData, getMainTimeline()->controller()->extractCompositionLumas(), getMainTimeline()->controller()->extractExternalEffectFiles(), this));
    if (d->exec() != 0) {
        m_messageLabel->setMessage(i18n("打包项目文件"), OperationCompletedMessage);
    }
}

void MainWindow::slotDownloadResources()
{
    QString currentFolder;
    if (pCore->currentDoc()) {
        currentFolder = pCore->currentDoc()->projectDataFolder();
    } else {
        currentFolder = KdenliveSettings::defaultprojectfolder();
    }
    /*  下载资源后展示在线资源dock
        m_onlineResourcesDock->show();
        m_onlineResourcesDock->raise();
     */
}

void MainWindow::slotProcessImportKeyframes(GraphicsRectItem type, const QString &tag, const QString &keyframes)
{
    Q_UNUSED(keyframes)
    Q_UNUSED(tag)
    if (type == AVWidget) {
        // This data should be sent to the effect stack
        // TODO REFAC reimplement
        // m_effectStack->setKeyframes(tag, data);
    } else if (type == TransitionWidget) {
        // This data should be sent to the transition stack
        // TODO REFAC reimplement
        // m_effectStack->transitionConfig()->setKeyframes(tag, data);
    } else {
        // Error
    }
}

void MainWindow::slotAlignPlayheadToMousePos()
{
    pCore->monitorManager()->activateMonitor(Kdenlive::ProjectMonitor);
    getMainTimeline()->controller()->seekToMouse();
}

void MainWindow::triggerKey(QKeyEvent *ev)
{
    // Hack: The QQuickWindow that displays fullscreen monitor does not integrate with QActions.
    // So on keypress events we parse keys and check for shortcuts in all existing actions
    QKeySequence seq;
    // Remove the Num modifier or some shortcuts like "*" will not work
    if (ev->modifiers() != Qt::KeypadModifier) {
        seq = QKeySequence(ev->key() + static_cast<int>(ev->modifiers()));
    } else {
        seq = QKeySequence(ev->key());
    }
    QList<KActionCollection *> collections = KActionCollection::allCollections();
    for (int i = 0; i < collections.count(); ++i) {
        KActionCollection *coll = collections.at(i);
        for (QAction *tempAction : coll->actions()) {
            if (tempAction->shortcuts().contains(seq)) {
                // Trigger action
                tempAction->trigger();
                ev->accept();
                return;
            }
        }
    }
}

void MainWindow::slotUpdateDockLocation(Qt::DockWidgetArea dockLocationArea)
{
    if (dockLocationArea == Qt::NoDockWidgetArea) {
        updateDockTitleBars(false);
    } else {
        updateDockTitleBars(true);
    }
}

void MainWindow::slotUpdateMonitorOverlays(int id, int code)
{
    CustomMenu *monitorOverlay = static_cast<CustomMenu *>(factory()->container(QStringLiteral("monitor_config_overlay"), this));
    if (!monitorOverlay) {
        return;
    }
    QList<QAction *> actions = monitorOverlay->actions();
    for (QAction *ac : qAsConst(actions)) {
        int mid = ac->data().toInt();
        if (mid == 0x010) {
            ac->setVisible(id == Kdenlive::ClipMonitor);
        }
        ac->setChecked(code & mid);
    }
}

void MainWindow::slotChangeStyle(QAction *a)
{
    QString style = a->data().toString();
    KdenliveSettings::setWidgetstyle(style);
    doChangeStyle();
    // Monitor refresh is necessary
    raiseMonitor(pCore->monitorManager()->isActive(Kdenlive::ClipMonitor));
}

void MainWindow::raiseMonitor(bool clipMonitor)
{
//    if (clipMonitor) {
//        m_clipMonitorDock->show();
//        m_clipMonitorDock->raise();
//    } else {
//        m_projectMonitorDock->show();
//        m_projectMonitorDock->raise();
//    }
}

void MainWindow::doChangeStyle()
{
    QString newStyle = KdenliveSettings::widgetstyle();
    if (newStyle.isEmpty() || newStyle == QStringLiteral("Default")) {
        newStyle = defaultStyle("Classic");
    }
    QApplication::setStyle(QStyleFactory::create(newStyle));
}

void MainWindow::updateDockTitleBars(bool isTopLevel)
{
    QList<QTabBar *> tabbars = findChildren<QTabBar *>();
    for (QTabBar *tab : qAsConst(tabbars)) {
        tab->setAcceptDrops(true);
        tab->setChangeCurrentOnDrag(true);
    }
    if (!KdenliveSettings::showtitlebars() && !isTopLevel) {
        return;
    }
    QList<QDockWidget *> docks = findChildren<QDockWidget *>();
    //qDebug()<<"=== FOUND DOCKS: "<<docks.count();
    for (QDockWidget *dock : qAsConst(docks)) {
        QWidget *bar = dock->titleBarWidget();
        if (dock->isFloating()) {
            if (bar) {
                dock->setTitleBarWidget(nullptr);
                delete bar;
            }
            continue;
        }
        QList<QDockWidget *> docked = tabifiedDockWidgets(dock);
        if (docked.isEmpty()) {
            if (bar) {
                dock->setTitleBarWidget(nullptr);
                delete bar;
            }
            continue;
        }
        bool hasVisibleDockSibling = false;
        for (QDockWidget *sub : qAsConst(docked)) {
            if (sub->toggleViewAction()->isChecked() && !sub->isTopLevel()) {
                // we have another docked widget, so tabs are visible and can be used instead of title bars
                hasVisibleDockSibling = true;
                break;
            }
        }
        if (!hasVisibleDockSibling) {
            if (bar) {
                dock->setTitleBarWidget(nullptr);
                delete bar;
            }
            continue;
        }
        if (!bar) {
            dock->setTitleBarWidget(new QWidget);
        }
    }
}

void MainWindow::slotToggleAutoPreview(bool enable)
{
    KdenliveSettings::setAutopreview(enable);
    if (enable && getMainTimeline()) {
        getMainTimeline()->controller()->startPreviewRender();
    }
}

void MainWindow::configureToolbars()
{
    // Since our timeline toolbar is a non-standard toolbar (as it is docked in a custom widget, not
    // in a QToolBarDockArea, we have to hack KXmlGuiWindow to avoid a crash when saving toolbar config.
    // This is why we hijack the configureToolbars() and temporarily move the toolbar to a standard location
    auto *ctnLay = static_cast<QVBoxLayout *>(m_timelineToolBarContainer->layout());
    auto *toolBarEditor = new KEditToolBar(guiFactory(), this);
    toolBarEditor->setAttribute(Qt::WA_DeleteOnClose);
    connect(toolBarEditor, SIGNAL(newToolBarConfig()), SLOT(saveNewToolbarConfig()));
    connect(toolBarEditor, &QDialog::finished, this, &MainWindow::rebuildTimlineToolBar);
    toolBarEditor->show();
}

void MainWindow::rebuildTimlineToolBar() {
}

void MainWindow::setTimelineToolbarIconSize(QAction *a) {
    if (!a) {
        return;
    }
}

void MainWindow::slotManageCache()
{
    QPointer<TemporaryData> d(new TemporaryData(pCore->currentDoc(), false, this));
    connect(d, &TemporaryData::disableProxies, this, &MainWindow::slotDisableProxies);
    d->exec();
}

void MainWindow::slotUpdateCompositing(QAction *compose)
{
    int mode = compose->data().toInt();
    getMainTimeline()->controller()->switchCompositing(mode);
    if (m_renderWidget) {
        m_renderWidget->errorMessage(RenderWidget::CompositeError, mode == 1 ? i18n("Rendering using low quality track compositing") : QString());
    }
    pCore->currentDoc()->setModified();
}

void MainWindow::slotUpdateCompositeAction(int mode)
{
    QList<QAction *> actions = m_compositeAction->actions();
    for (int i = 0; i < actions.count(); i++) {
        if (actions.at(i)->data().toInt() == mode) {
            m_compositeAction->setCurrentAction(actions.at(i));
            break;
        }
    }
    if (m_renderWidget) {
        m_renderWidget->errorMessage(RenderWidget::CompositeError, mode == 1 ? i18n("Rendering using low quality track compositing") : QString());
    }
}

void MainWindow::showMenuBar(bool show)
{
    if (!show) {
        KMessageBox::information(this, i18n("This will hide the menu bar completely. You can show it again by typing Ctrl+M."), i18n("Hide menu bar"),
                                 QStringLiteral("show-menubar-warning"));
    }
    menuBar()->setVisible(show);
}

void MainWindow::forceIconSet(bool force)
{
    KdenliveSettings::setForce_breeze(force);
    if (force) {
        // Check current color theme
        QColor background = qApp->palette().window().color();
        bool useDarkIcons = background.value() < 100;
        KdenliveSettings::setUse_dark_breeze(useDarkIcons);
    }
    if (KMessageBox::warningContinueCancel(this, i18n("Kdenlive needs to be restarted to apply the icon theme change. Restart now?")) ==
        KMessageBox::Continue) {
        slotRestart();
    }
}

void MainWindow::slotSwitchTrimMode()
{
    // TODO refac
    /*
    if (pCore->projectManager()->currentTimeline()) {
        pCore->projectManager()->currentTimeline()->projectView()->switchTrimMode();
    }
    */
}

void MainWindow::setTrimMode(const QString &mode){
    Q_UNUSED(mode)
    // TODO refac
    /*
    if (pCore->projectManager()->currentTimeline()) {
        m_trimLabel->setText(mode);
        m_trimLabel->setVisible(!mode.isEmpty());
    }
    */
}

TimelineWidget *MainWindow::getMainTimeline() const
{
    return m_timelineTabs->getMainTimeline();
}

TimelineWidget *MainWindow::getCurrentTimeline() const
{
    return m_timelineTabs->getCurrentTimeline();
}

bool MainWindow::hasTimeline() const
{
    return m_timelineTabs != nullptr;
}

void MainWindow::resetTimelineTracks()
{
    TimelineWidget *current = getCurrentTimeline();
    if (current) {
        current->controller()->resetTrackHeight();
    }
}

void MainWindow::slotEditItemSpeed()
{
    TimelineWidget *current = getCurrentTimeline();
    if (current) {
        current->controller()->changeItemSpeed(-1, -1);
    }
}

void MainWindow::slotSwitchTimelineZone(bool active)
{
    pCore->currentDoc()->setDocumentProperty(QStringLiteral("enableTimelineZone"), active ? QStringLiteral("1") : QStringLiteral("0"));
    emit getCurrentTimeline()->controller()->useRulerChanged();
    QSignalBlocker blocker(m_useTimelineZone);
    m_useTimelineZone->setActive(active);
}

void MainWindow::slotGrabItem()
{
    getCurrentTimeline()->controller()->grabCurrent();
}

void MainWindow::slotCollapse()
{
    if ((QApplication::focusWidget() != nullptr) && (QApplication::focusWidget()->parentWidget() != nullptr) &&
        QApplication::focusWidget()->parentWidget() == pCore->bin()) {
        // Bin expand/collapse?

    } else {
        QWidget *widget = QApplication::focusWidget();
        while ((widget != nullptr) && widget != this) {
//            if (widget == m_effectStackDock) {
//                m_assetPanel->collapseCurrentEffect();
//                return;
//            }
            widget = widget->parentWidget();
        }

        // Collapse / expand track
        getMainTimeline()->controller()->collapseActiveTrack();
    }
}


void MainWindow::slotExpandClip()
{
    getCurrentTimeline()->controller()->expandActiveClip();
}

bool MainWindow::timelineVisible() const
{
    return !centralWidget()->isHidden();
}

void MainWindow::slotActivateAudioTrackSequence()
{
    auto *action = qobject_cast<QAction *>(sender());
    const QList<int> trackIds = getMainTimeline()->controller()->getModel()->getTracksIds(true);
    int trackPos = qBound(0, action->data().toInt(), trackIds.count() - 1);
    int tid = trackIds.at(trackPos);
    getCurrentTimeline()->controller()->setActiveTrack(tid);
}

void MainWindow::slotActivateVideoTrackSequence()
{
    auto *action = qobject_cast<QAction *>(sender());
    const QList<int> trackIds = getMainTimeline()->controller()->getModel()->getTracksIds(false);
    int trackPos = qBound(0, action->data().toInt(), trackIds.count() - 1);
    int tid = trackIds.at(trackIds.count() - 1 - trackPos);
    getCurrentTimeline()->controller()->setActiveTrack(tid);
}

void MainWindow::slotActivateTarget()
{
    auto *action = qobject_cast<QAction *>(sender());
    if (action) {
        int ix = action->data().toInt();
        getCurrentTimeline()->controller()->assignCurrentTarget(ix);
    }
}

void MainWindow::resetSubtitles()
{
    // Hide subtitle track
    m_buttonSubtitleEditTool->setChecked(false);
    KdenliveSettings::setShowSubtitles(false);
    pCore->subtitleWidget()->setModel(nullptr);
    if (pCore->currentDoc()) {
        const QString workPath = pCore->currentDoc()->subTitlePath(false);
        QFile workFile(workPath);
        if (workFile.exists()) {
            workFile.remove();
        }
    }
}

void MainWindow::slotEditSubtitle(QMap<QString, QString> subProperties)
{
    std::shared_ptr<SubtitleModel> subtitleModel = pCore->getSubtitleModel();
    if (subtitleModel == nullptr) {
        // Starting a new subtitle for this project
        subtitleModel.reset(new SubtitleModel(getMainTimeline()->controller()->tractor(), getMainTimeline()->controller()->getModel(), this));
        getMainTimeline()->controller()->getModel()->setSubModel(subtitleModel);
        pCore->currentDoc()->initializeSubtitles(subtitleModel);
        pCore->subtitleWidget()->setModel(subtitleModel);
        const QString subPath = pCore->currentDoc()->subTitlePath(true);
        const QString workPath = pCore->currentDoc()->subTitlePath(false);
        QFile subFile(subPath);
        if (subFile.exists()) {
            subFile.copy(workPath);
            subtitleModel->parseSubtitle(workPath);
        }
        if (!subProperties.isEmpty()) {
            subtitleModel->loadProperties(subProperties);
            // Load the disabled / locked state of the subtitle
            getMainTimeline()->controller()->subtitlesLockedChanged();
            getMainTimeline()->controller()->subtitlesDisabledChanged();
        }
        KdenliveSettings::setShowSubtitles(true);
        m_buttonSubtitleEditTool->setChecked(true);
        getMainTimeline()->connectSubtitleModel(true);
    } else {
        KdenliveSettings::setShowSubtitles(m_buttonSubtitleEditTool->isChecked());
        getMainTimeline()->connectSubtitleModel(false);
    }
}

void MainWindow::slotAddSubtitle(const QString &text)
{
    showSubtitleTrack();
    getCurrentTimeline()->controller()->addSubtitle(-1, text);
}

void MainWindow::slotDisableSubtitle()
{
    getCurrentTimeline()->controller()->switchSubtitleDisable();
}

void MainWindow::slotLockSubtitle()
{
    getCurrentTimeline()->controller()->switchSubtitleLock();
}

void MainWindow::showSubtitleTrack()
{
    if (pCore->getSubtitleModel() == nullptr || !KdenliveSettings::showSubtitles()) {
        m_buttonSubtitleEditTool->setChecked(true);
        slotEditSubtitle();
    }
}

void MainWindow::slotImportSubtitle()
{
    showSubtitleTrack();
    getCurrentTimeline()->controller()->importSubtitle();
}

void MainWindow::slotExportSubtitle()
{
    if (pCore->getSubtitleModel() == nullptr) {
        pCore->displayMessage(i18n("No subtitles in current project"), ErrorMessage);
        return;
    }
    getCurrentTimeline()->controller()->exportSubtitle();
}

void MainWindow::slotSpeechRecognition()
{
    if (pCore->getSubtitleModel() == nullptr) {
        slotEditSubtitle();
    }
    getCurrentTimeline()->controller()->subtitleSpeechRecognition();
}

#define GET_KDE_ACTION(__ENUM__) \
    actionCollection()->action(KStandardAction::name(KStandardAction::StandardAction::__ENUM__))
#define ACTION_COLL(__name__) \
    actionCollection()->action(QStringLiteral(__name__))

void MainWindow::setupMenuBar() {
    menuBar()->deleteLater();
    auto menuBar = new QMenuBar(this);
    
    std::array<QString, 3> ctrlBtnIcons = {
        "minimum", "maximum", "close"
    };
    
    // create window controller buttons
    {   
        m_windCtrlBtnFrame  = new QFrame(this);
        m_windCtrlBtnFrame->setWindowFlag(Qt::WindowStaysOnTopHint);
        m_windCtrlBtnFrame->setFixedSize(__windowCtrlBtnWidth * 3, __menuBarHeight - 1);
        auto frameLayout    = new QHBoxLayout(m_windCtrlBtnFrame);
        frameLayout->setContentsMargins(0, 0, 0, 0);
        frameLayout->setSpacing(0);
        
        using BtnType = QPushButton;
        std::array<BtnType*, 3> ctrlBtnGroup = {};
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
        
        for (decltype(ctrlBtnGroup.size()) i = 0; i < ctrlBtnGroup.size(); i++) {
            auto btn = ctrlBtnGroup.at(i);
//            btn = new std::remove_pointer<decltype(btn)>::type(m_windCtrlBtnFrame);
            btn = new BtnType(m_windCtrlBtnFrame);
            btn->setFixedSize(__windowCtrlBtnWidth, __menuBarHeight - 1);
            btn->setIcon(QIcon(":/classic/controllers/window_"+ ctrlBtnIcons[i] +".png"));
            btn->setStyleSheet(btnQSS);
            if (i == ctrlBtnGroup.size() - 1) {
                btn->setStyleSheet(R"(
                    QPushButton {
                        background-color: #FF2D2C39;
                        border-width: 0px;
                        border-color: transparent;
                        border-radius: 0px;
                    }
                    QPushButton::hover {
                        background-color: #FF0000;
                    })"
                );
            }
            
            frameLayout->addWidget(btn, Qt::AlignLeft | Qt::AlignTop);
            
            if (i == 0) {
                connect(btn, &BtnType::clicked, m_framelessHelper, &FramelessHelper::triggerMinimizeButtonAction);
            } else if (i == 1) {
                connect(btn, &BtnType::clicked, m_framelessHelper, &FramelessHelper::triggerMaximizeButtonAction);
            } else if (i == 2) {
                connect(btn, &BtnType::clicked, m_framelessHelper, &FramelessHelper::triggerCloseButtonAction);
            }
            m_framelessHelper->addExcludeItem(btn);
        }
        m_framelessHelper->addExcludeItem(m_windCtrlBtnFrame);
        
        m_windCtrlBtnFrame->setLayout(frameLayout);
    }
    
    menuBar->setFixedHeight(__menuBarHeight);
    menuBar->setStyleSheet(QString(R"(
        QMenuBar {
            padding-left: %1px;
            padding-right: 0px;
            padding-top: 0px;
            padding-bottom: 0px;
            background-color: #2D2C39;
        }
        
        QMenuBar::item {
            padding-left: 12px;
            padding-right: 12px;
            padding-top: 13px;
            padding-bottom: 12px;

            font-size: 12px;
            font-family: "Microsoft YaHei";
        }

        QMenuBar::item:selected {
            background: #FF3E3D4C;
        }
    )").arg(__ltLabelWidth)); // .arg(__menuBarHeight));
    setMenuBar(menuBar);
    
    // create left top icon-text label
    {
        auto iconLabel = new QLabel(menuBar);
        iconLabel->setPixmap(QPixmap(":/classic/smartip_icon_14.png"));
        
        auto textLabel = new QLabel(menuBar);
        textLabel->setText(i18n("SmartIP"));
        textLabel->setStyleSheet(R"(QLabel { color: #E6FFFFFF; font-size: 14px; font-family: "Microsoft YaHei"; font-weight: 400; })");
        
        iconLabel->move(24, 15);
        textLabel->move(44, 12);
    }
    
    
    // create custom menus
    m_fileMenu      = new CustomMenu(i18n("文件"), this);
    m_editMenu      = new CustomMenu(i18n("编辑"), this);
    m_cutMenu       = new CustomMenu(i18n("剪辑"), this);
    m_settingMenu   = new CustomMenu(i18n("设置"), this);
    m_helpMenu      = new CustomMenu(i18n("帮助"), this);
    
    menuBar->addMenu(m_fileMenu);
    menuBar->addMenu(m_editMenu);
    menuBar->addMenu(m_cutMenu);
    menuBar->addMenu(m_settingMenu);
    menuBar->addMenu(m_helpMenu);
    
    auto timelineActColl = kdenliveCategoryMap["timeline"]->collection();
    
    // add contents
    // 文件菜单
    {
        auto createNew = GET_KDE_ACTION(New);
        createNew->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
        createNew->setText(i18n("新建"));
        createNew->setIcon(QIcon());
        m_fileMenu->addAction(createNew);
        
        auto openProject = GET_KDE_ACTION(Open);
        openProject->setText(i18n("打开项目"));
        openProject->setIcon(QIcon());
        m_fileMenu->addAction(openProject);
        openProject->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_O));
        
        auto openRecent = GET_KDE_ACTION(OpenRecent);
        auto openRecent_subMenu = openRecent->menu();
        openRecent_subMenu->setStyleSheet(CustomMenu::menuSheet + CustomMenu::menuItemSheet);
        MOVE_MENU_ABOUT2SHOW(openRecent_subMenu, __customMenuLeftMargin, 0);
        
        openRecent->setIcon(QIcon());
        openRecent->setText(i18n("打开最近项目"));
        m_fileMenu->addAction(openRecent);

        auto leadIn = new CustomMenu(m_fileMenu);
        leadIn->setTitle("导入");
        auto inFromPlayList     = new QAction(tr("从播放列表导入"), leadIn);
        auto inFromMylib        = new QAction(tr("从我的素材库导入"), leadIn);
        auto addLocalFile       = ACTION_COLL("add_clip");
        
        addLocalFile->setText(i18n("从本地导入"));
        addLocalFile->setIcon(QIcon());
        
        leadIn->addAction(inFromPlayList);
        leadIn->addAction(inFromMylib);
        leadIn->addAction(addLocalFile);

        MOVE_MENU_ABOUT2SHOW(leadIn, __customMenuLeftMargin, 0);
        m_fileMenu->addMenu(leadIn);
        m_fileMenu->addSeparator();

        auto save = GET_KDE_ACTION(Save);
        save->setText(i18n("保存"));
        save->setIcon(QIcon());
        save->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
        m_fileMenu->addAction(save);

        auto saveAs = GET_KDE_ACTION(SaveAs);
        saveAs->setText(i18n("另存为"));
        saveAs->setIcon(QIcon());
        saveAs->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
        
        m_fileMenu->addAction(saveAs);
        m_fileMenu->addSeparator();

        auto projectRender = ACTION_COLL("project_render");
        projectRender->setText(i18n("导出"));
        projectRender->setIcon(QIcon());
        projectRender->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
        m_fileMenu->addAction(projectRender);
            
        auto packProjectFiles = ACTION_COLL("archive_project");
        packProjectFiles->setShortcut(QKeySequence(Qt::Key_M));
        packProjectFiles->setText(i18n("打包项目文件        "));
        packProjectFiles->setIcon(QIcon());
        m_fileMenu->addAction(packProjectFiles);

        m_fileMenu->addSeparator();
        
        auto _exit = new QAction(tr("退出"), m_fileMenu);
        _exit->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Q));
        m_fileMenu->addAction(_exit);
        connect(_exit,SIGNAL(triggered()),this,SLOT(close()));
    
        inFromPlayList->setEnabled(false);
        inFromMylib->setEnabled(false);
    }
    
    // 编辑菜单
    {
        auto undo = GET_KDE_ACTION(Undo);
        undo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
        undo->setIcon(QIcon());
        undo->setText(i18n("撤销"));
        m_editMenu->addAction(undo);
        
        auto redo = GET_KDE_ACTION(Redo);
        redo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));
        redo->setIcon(QIcon());
        redo->setText(i18n("重做"));        
        m_editMenu->addAction(redo);
        m_editMenu->addSeparator();
        
        auto shear = new QAction(tr("剪切"), m_editMenu);
        shear->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
        connect(shear, &QAction::triggered, [this] {
            slotCopy();
            slotDeleteItem();
        });
        m_editMenu->addAction(shear);
        
        auto copy = GET_KDE_ACTION(Copy);
        copy->setIcon(QIcon());
        copy->setText(i18n("复制"));
        connect(copy, &QAction::changed, [copy, shear] {
            shear->setEnabled(copy->isEnabled());
        });
        shear->setEnabled(copy->isEnabled());
        copy->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
        m_editMenu->addAction(copy);
        
        // 这里的复制调整效果和复制其实是一个东西
        auto copyEffect = new QAction(tr("复制调整效果"), m_editMenu);
        connect(copy, &QAction::changed, [copy, copyEffect] {
            copyEffect->setEnabled(copy->isEnabled());
        });
        copyEffect->setEnabled(copy->isEnabled());
        connect(copyEffect, &QAction::triggered, copy, &QAction::triggered);
        copyEffect->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_C));
        m_editMenu->addAction(copyEffect);

        auto paste = GET_KDE_ACTION(Paste);
        paste->setText(i18n("粘贴"));
        paste->setIcon(QIcon());
        m_editMenu->addAction(paste);
        paste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
        
        // TODO: 
        auto pasteInsert = new QAction(tr("粘贴插入"), m_editMenu);
        m_editMenu->addAction(pasteInsert);
        pasteInsert->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_V));

        auto pastEffects = ACTION_COLL("paste_effects");
        pastEffects->setIcon(QIcon());
        pastEffects->setText(i18n("粘贴调整效果"));
        pastEffects->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_V));        
        m_editMenu->addAction(pastEffects);
        

//        auto pasteEffectAndAdjustion = new QAction(tr("粘贴效果和调整"), m_editMenu);
//        m_editMenu->addAction(pasteEffectAndAdjustion);
        
        auto _delete = ACTION_COLL("delete_timeline_clip");
        _delete->setText(i18n("删除"));
        _delete->setIcon(QIcon());
        m_editMenu->addAction(_delete);
        _delete->setShortcut(Qt::Key_Delete);

        auto deleteAndClearSpace = new QAction(tr("删除并消除间隙"), m_editMenu);
        connect(_delete, &QAction::changed, [deleteAndClearSpace, _delete] {
            deleteAndClearSpace->setEnabled(_delete->isEnabled());
        });
        deleteAndClearSpace->setEnabled(_delete->isEnabled());
        connect(deleteAndClearSpace, &QAction::triggered, [this] {
            getMainTimeline()->controller()->getModel()->
                setEditMode(TimelineMode::InsertEdit);
            slotDeleteItem();
            getMainTimeline()->controller()->getModel()->
                setEditMode(TimelineMode::NormalEdit);
        });
        deleteAndClearSpace->setShortcut(Qt::Key_Backspace);
        m_editMenu->addAction(deleteAndClearSpace);
        
        auto deleteEffect = ACTION_COLL("delete_effects");
        deleteEffect->setText(i18n("删除调整效果"));
        deleteEffect->setIcon(QIcon());
        m_editMenu->addAction(deleteEffect);
        m_editMenu->addSeparator();
        
        auto selectAll = timelineActColl->action(QStringLiteral("select_all_tracks"));
        selectAll->setText(i18n("全选"));
        selectAll->setIcon(QIcon());
        m_editMenu->addAction(selectAll);
        
        auto unselectAll = timelineActColl->action(QStringLiteral("unselect_all_tracks"));
        unselectAll->setIcon(QIcon());
        unselectAll->setText(i18n("取消全选"));
        unselectAll->setShortcut(Qt::CTRL + Qt::Key_D);
        m_editMenu->addAction(unselectAll);
    }
    
    // 剪辑菜单
    {   
        auto insert = ACTION_COLL("insert_to_in_point");
        m_cutMenu->addAction(insert);
        
        auto replaceClip = ACTION_COLL("replace_clip");
        replaceClip->setText(i18n("替换"));
        replaceClip->setIcon(QIcon());
        m_cutMenu->addAction(replaceClip);
        m_cutMenu->addSeparator();
        
        auto groupClip = timelineActColl->action(QStringLiteral("group_clip"));
        groupClip->setText(i18n("编组"));
        groupClip->setIcon(QIcon());
        groupClip->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
        m_cutMenu->addAction(groupClip);
        
        auto ungroupClip = timelineActColl->action(QStringLiteral("ungroup_clip"));
        ungroupClip->setText(i18n("取消编组"));
        ungroupClip->setIcon(QIcon());
        ungroupClip->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_G));
        m_cutMenu->addAction(ungroupClip);
        
        m_cutMenu->addSeparator();
        
        auto addGuide = ACTION_COLL("add_guide");
        addGuide->setText(i18n("添加标记"));
        addGuide->setIcon(QIcon());
        m_cutMenu->addAction(addGuide);
        
        auto jmp2nextGuide = ACTION_COLL("monitor_seek_guide_forward");
        jmp2nextGuide->setIcon(QIcon());
        jmp2nextGuide->setText(i18n("转到下一标记"));
        m_cutMenu->addAction(jmp2nextGuide);

        auto jmp2prevGuide = ACTION_COLL("monitor_seek_guide_backward");
        jmp2prevGuide->setIcon(QIcon());
        jmp2prevGuide->setText(i18n("转到上一标记"));
        m_cutMenu->addAction(jmp2prevGuide);
        
        auto deleteCurGuide = ACTION_COLL("delete_guide");
        deleteCurGuide->setText(i18n("清除所选标记"));
        deleteCurGuide->setIcon(QIcon());
        m_cutMenu->addAction(deleteCurGuide);
        
        auto deleteAllGuides = ACTION_COLL("delete_all_guides");
        deleteAllGuides->setText(i18n("清除全部标记"));
        deleteAllGuides->setIcon(QIcon());
        m_cutMenu->addAction(deleteAllGuides);
        m_cutMenu->addSeparator();

        auto enableTimelineSnap = ACTION_COLL("snap");
        enableTimelineSnap->setText(i18n("启用时间线吸附"));
        enableTimelineSnap->setIcon(QIcon());
        m_cutMenu->addAction(enableTimelineSnap);
    }
    
    // 设置菜单
    {
        auto projectHeader = new QAction(tr("项目"), m_settingMenu);
        QFont settingLabelFont;
        settingLabelFont.setFamily("Microsoft Yahei");
        settingLabelFont.setPixelSize(14);
        projectHeader->setFont(settingLabelFont);
        projectHeader->setDisabled(true);
        
        m_settingMenu->addAction(projectHeader);
        
        auto projectSetting = new QAction(tr("项目设置"), m_settingMenu);
        projectSetting->setShortcut(Qt::Key_P);
        connect(projectSetting, &QAction::triggered, [] {
            auto mainWindow = pCore->window();
            auto psw = new ProjectSettingsWidget(mainWindow);
            psw->raise();
            psw->QQuickWidget::move((mainWindow->width()  - psw->width()) / 2 + mainWindow->x(), 165);
        });
        
        m_settingMenu->addAction(projectSetting);
        m_settingMenu->addSeparator();

        auto player=new QAction(tr("播放器"), m_settingMenu);
        m_settingMenu->addAction(player);
        player->setFont(settingLabelFont);
        player->setDisabled(true);

        auto checkBoxes         = new QActionGroup(m_settingMenu);
        auto scrubAudio         = new QAction(tr("快速拖动时预览音频"), m_settingMenu);
        auto realTimeProcess    = ACTION_COLL("mlt_realtime");
        auto progressive        = new QAction(tr("逐行"), m_settingMenu);
        scrubAudio->setCheckable(true);
        scrubAudio->setChecked(KdenliveSettings::scrubAudio());
        realTimeProcess->setText(i18n("实时处理（丢帧）"));
        progressive->setCheckable(true);
        progressive->setChecked(KdenliveSettings::progressive());
        
        connect(scrubAudio, &QAction::toggled, [scrubAudio] {
            KdenliveSettings::setScrubAudio(scrubAudio->isChecked());
        });
        
        connect(progressive, &QAction::toggled, [progressive] {
            std::unique_ptr<ProfileModel>& profile = pCore->getCurrentProfile();
            profile->profile().set_progressive(progressive->isChecked());
            
            pCore->profileChanged();
        });
        emit progressive->toggled(progressive->isChecked());
        
        m_settingMenu->addAction(scrubAudio);
        m_settingMenu->addAction(realTimeProcess);
        m_settingMenu->addAction(progressive);

        checkBoxes->addAction(scrubAudio);
        checkBoxes->addAction(realTimeProcess);
        checkBoxes->addAction(progressive);
        
        auto previewScaling = new CustomMenu(i18n("预览缩放"));
        ACTION_COLL("scale_no_preview")->setText(i18n("无"));
        
        previewScaling->addActions(m_scaleGroup->actions());

        const QString dotSheet = "QMenu::indicator { padding-left:14px; padding-right:10px; width: 6px; height:6px;} QMenu::indicator:unchecked { image: none } QMenu::indicator:checked { image:url(:/classic/controllers/indicator-dot-white.png); }";
        previewScaling->setStyleSheet(dotSheet + R"(
            QMenu::item {
                padding-top: 7px;
                padding-left: 0px;
                padding-right: 29px;
                padding-bottom: 7px;
            })"
        ); // previewScalItemSheet
        MOVE_MENU_ABOUT2SHOW(previewScaling, __customMenuLeftMargin, 0);

        auto proxy = new CustomMenu(i18n("代理"));
        MOVE_MENU_ABOUT2SHOW(proxy, __customMenuLeftMargin, 0);
        
        auto useProxy = ACTION_COLL("proxy_clip");
        auto useHardwareEncoder = new QAction(i18n("使用硬件编码器"), proxy);
        useHardwareEncoder->setCheckable(true);
        auto proxyActionGroup=new QActionGroup(proxy);
        proxyActionGroup->addAction(useProxy);
        proxyActionGroup->addAction(useHardwareEncoder);
        proxyActionGroup->setExclusive(false);
        
        auto storage = new CustomMenu(this);
        MOVE_MENU_ABOUT2SHOW(storage, __customMenuLeftMargin, 0);
        
        storage->setTitle(tr("Storage"));//QMenu(tr("Storage"),proxy);
        auto _setting=new QAction(tr("设定..."),storage);
        auto _display=new QAction(tr("显示..."),storage);
        auto useProjFolder=new QAction("Use Project Folder", storage);
        useProjFolder->setCheckable(true);
        auto storageActionGroup=new QActionGroup(storage);
        storageActionGroup->addAction(useProjFolder);
        storageActionGroup->setExclusive(false);
        storage->addAction(_setting);
        storage->addAction(_display);
        storage->addAction(useProjFolder);

        auto configHardwareEncoder=new QAction(tr("配置硬件编码器..."),proxy);
        proxy->addAction(useProxy);
        proxy->addMenu(storage);
        proxy->addAction(useHardwareEncoder);
        proxy->addAction(configHardwareEncoder);
        useProxy->setShortcut(Qt::Key_F4);

        m_settingMenu->addMenu(previewScaling);
        m_settingMenu->addMenu(proxy);
        
        auto interlace = ACTION_COLL("mlt_interlace");
        interlace->setText(i18n("反交错"));
        auto interlaceMenu = interlace->menu();
        MOVE_MENU_ABOUT2SHOW(interlaceMenu, __customMenuLeftMargin, 0);
        interlaceMenu->setStyleSheet(CustomMenu::menuSheet + CustomMenu::menuItemSheet + dotSheet + R"(
            QMenu::item {
                padding-top: 7px;
                padding-left: 0px;
                padding-right: 29px;
                padding-bottom: 7px;
            })"
        );
        std::array<QString, 4> interlaceActText = {
            i18n("仅用单场（快速）"),
            i18n("线性混合（快速）"),
            i18n("YADIF - 时间（质量好）"),
            i18n("YADIF - 时间与空间（质量最佳）")
        };
        
        int iterator = 0;
        foreach(auto action, interlaceMenu->actions()) {
            action->setText(interlaceActText[iterator]);
            iterator++;
        }
        
        auto interpolation = ACTION_COLL("mlt_interpolation");
        interpolation->setText(i18n("插值"));
        auto interpolationMenu = interpolation->menu();
        MOVE_MENU_ABOUT2SHOW(interpolationMenu, __customMenuLeftMargin, 0);
        interpolationMenu->setStyleSheet(CustomMenu::menuSheet + CustomMenu::menuItemSheet + dotSheet + R"(
            QMenu::item {
                padding-top: 7px;
                padding-left: 0px;
                padding-right: 29px;
                padding-bottom: 7px;
            })"
        );
        std::array<QString, 4> interpolationActText = {
            i18n("最近像素（快速）"),
            i18n("双线性（良好）"),
            i18n("双立方（更佳）"),
            i18n("Hyper/Lanczos（最佳）")
        };
        iterator = 0;
        foreach(auto action, interpolationMenu->actions()) {
            action->setText(interpolationActText[iterator]);
            iterator++;
        }
        
        m_settingMenu->addAction(interlace);
        m_settingMenu->addAction(interpolation);
        
        auto sync = new QAction("Synchronization...",m_settingMenu);
        m_settingMenu->addAction(sync);
        m_settingMenu->addSeparator();

        auto userInterface=new QAction(tr("用户界面"),m_settingMenu);
        m_settingMenu->addAction(userInterface);
        userInterface->setFont(settingLabelFont);
        userInterface->setDisabled(true);
        auto lang = new QMenu("语言",m_settingMenu);
        auto theme = new CustomMenu;
        theme->setTitle(tr("主题"));//QMenu("主题",m_settingMenu);

        lang->setEnabled(false);
        theme->setEnabled(false);
        
        auto _system=new QAction(tr("系统"),theme);
        auto fusionDark=new QAction("Fusion Dark",theme);
        auto fusionLight=new QAction("Fusion Light",theme);
        _system->setCheckable(true);
        _system->setChecked(true);
        fusionDark->setCheckable(true);
        fusionLight->setCheckable(true);
        auto themeActionGroup=new QActionGroup(theme);
        themeActionGroup->addAction(_system);
        themeActionGroup->addAction(fusionDark);
        themeActionGroup->addAction(fusionLight);
        themeActionGroup->setExclusive(true);
        theme->addAction(_system);
        theme->addAction(fusionDark);
        theme->addAction(fusionLight);
        theme->setStyleSheet(dotSheet+ R"(
            QMenu::item {
                padding-top: 8px;
                padding-left: 0px;
                padding-right: 25px;
                padding-bottom: 8px;
        })");

        auto displayMethod = new CustomMenu(i18n("显示方式"));
        displayMethod->setStyleSheet(dotSheet + R"(
            QMenu::item {
                padding-top: 7px;
                padding-left: 0px;
                padding-right: 29px;
                padding-bottom: 7px;
            })"
        );
        MOVE_MENU_ABOUT2SHOW(displayMethod, __customMenuLeftMargin, 0);
#if !defined(Q_OS_MAC)
        
#if defined(Q_OS_WIN)
        displayMethod->addAction(ACTION_COLL("opengl_auto"));
        displayMethod->addAction(ACTION_COLL("opengl_desktop"));
        displayMethod->addAction(ACTION_COLL("opengl_es"));
        displayMethod->addAction(ACTION_COLL("opengl_software"));
#endif
    
// DO NOTHING IN MACOS
#endif
        m_settingMenu->addMenu(lang);
        m_settingMenu->addMenu(theme);
        m_settingMenu->addMenu(displayMethod);
        m_settingMenu->addSeparator();
        
        auto programData = new QAction(i18n("程序数据"), m_settingMenu);
        programData->setFont(settingLabelFont);
        programData->setDisabled(true);
        m_settingMenu->addAction(programData);
        
        auto programDataDir = new CustomMenu(i18n("程序数据目录"));
        programDataDir->setTitle(i18n("程序数据目录"));
        auto __setting = new QAction(i18n("设定..."), programDataDir);
        auto __display = new QAction(i18n("显示..."), programDataDir);
        programDataDir->addAction(__setting);
        programDataDir->addAction(__display);
        MOVE_MENU_ABOUT2SHOW(programDataDir, __customMenuLeftMargin, 0);
        m_settingMenu->addMenu(programDataDir);
        
        auto clearThumbnailCache = new QAction(i18n("清除缩略图缓存"));
        connect(clearThumbnailCache, &QAction::triggered, this, [] {
            ThumbnailCache::get()->clearCache();
        }, Qt::QueuedConnection);
        m_settingMenu->addAction(clearThumbnailCache);

        auto clearHistoryWhileExiting = new QAction(tr("退出时清除最近打开历史"), m_settingMenu);
        clearHistoryWhileExiting->setCheckable(true);
        clearHistoryWhileExiting->setChecked(KdenliveSettings::clearRecentExit());
        connect(clearHistoryWhileExiting, &QAction::toggled, [clearHistoryWhileExiting] {
            KdenliveSettings::setClearRecentExit(clearHistoryWhileExiting->isChecked());
        });
        
        m_settingMenu->addAction(clearHistoryWhileExiting);
        
        checkBoxes->addAction(clearHistoryWhileExiting);
        checkBoxes->setExclusive(false);

    }
    
    // 帮助菜单
    {
        auto softwareUpdate= new QAction(tr("软件升级"), m_helpMenu);
        m_helpMenu->addAction(softwareUpdate);
        auto snoobGuide= new QAction(tr("新手教程"),m_helpMenu);
        m_helpMenu->addAction(snoobGuide);
        auto userBBS= new QAction(tr("用户论坛"),m_helpMenu);
        m_helpMenu->addAction(userBBS);
        auto feedBack = new QAction(tr("反馈"),m_helpMenu);
        m_helpMenu->addAction(feedBack);
        m_helpMenu->addSeparator();
        auto about = new QAction(tr("关于"), m_helpMenu);
        m_helpMenu->addAction(about);
    }
    
}

bool MainWindow::eventFilter(QObject* tgt, QEvent* e) {
//    static bool leftPressed = false;
//    static QPoint pressedPos = {};
    
//    switch(e->type()) {
//    case QEvent::MouseMove: {
//        if (tgt == menuBar() && leftPressed) {
//            auto me = static_cast<QMouseEvent*>(e);
//            if (m_framelessHelper->isMaximized()) {
//                m_framelessHelper->triggerMaximizeButtonAction();
//                pressedPos = me->pos();
//            } else {
//                move(pos() + me->pos() - pressedPos);
//            }
            
//            return true;
//        }
//    } return false;
        
//    case QEvent::MouseButtonDblClick: {
//        m_framelessHelper->triggerMaximizeButtonAction();
//    }; return true;
        
//    case QEvent::MouseButtonPress: {
//        if (tgt == menuBar()) {
//            auto me = static_cast<QMouseEvent*>(e);
//            if (me->pos().x() > __ltLabelWidth + __menuTabWidth * 5 || me->pos().x() <= __ltLabelWidth) {
//                leftPressed = true;
//                pressedPos = me->pos();
//            }
//        }
//    }; return false;
        
//    case QEvent::MouseButtonRelease: {
//        if (tgt == menuBar()) {
//            if (leftPressed) {
//                if (y() < 0) {
//                    m_framelessHelper->triggerMaximizeButtonAction();
//                }
//                leftPressed = false;
//            }
//        }
//    }; return false;
        
//    default:
//        return false;
//    }
    return QWidget::eventFilter(tgt, e);
}

void MainWindow::resizeEvent(QResizeEvent*) {
    m_windCtrlBtnFrame->move(width() - m_windCtrlBtnFrame->width(), 0);
    pCore->bin()->setMaximumSize(width() * 0.4, height() * 0.6);
    
    if (!m_effectTransWidget->isHidden()) {
        m_effectTransWidget->move(20, 87);
    }
}

void MainWindow::setWindowModified(bool isModified) {
    QWidget::setWindowModified(isModified);
    
    auto title = "项目 " + windowTitle();
    auto index = title.lastIndexOf(".sip");
    if (index == -1) {
        index = title.lastIndexOf('[');
        title = title.mid(3, index - 3);
    } else {
        title = title.mid(0, index);        
    }
    
    if (isModified) {
        m_editorToolBar->setDocumentString(title + " - " + i18n("已修改"));        
    } else {
        m_editorToolBar->setDocumentString(title);        
    }
}

void MainWindow::setProjectMediasetVisible(bool visible) {
    if (visible) {
        pCore->bin()->show();
    } else {
        pCore->bin()->hide();
    }
}

void MainWindow::slotCopyDebugInfo() {
    QString debuginfo = QStringLiteral("Kdenlive: %1\n").arg(KAboutData::applicationData().version());
    debuginfo.append(QStringLiteral("MLT: %1\n").arg(mlt_version_get_string()));
    debuginfo.append(QStringLiteral("Qt: %1 (built against %2 %3)\n").arg(QString::fromLocal8Bit(qVersion())).arg(QT_VERSION_STR).arg(QSysInfo::buildAbi()));
    debuginfo.append(QStringLiteral("Frameworks: %2\n").arg(KCoreAddons::versionString()));
    debuginfo.append(QStringLiteral("System: %1\n").arg(QSysInfo::prettyProductName()));
    debuginfo.append(QStringLiteral("Kernel: %1 %2\n").arg(QSysInfo::kernelType()).arg(QSysInfo::kernelVersion()));
    debuginfo.append(QStringLiteral("CPU: %1\n").arg(QSysInfo::currentCpuArchitecture()));
    debuginfo.append(QStringLiteral("Windowing System: %1\n").arg(QGuiApplication::platformName()));
    debuginfo.append(QStringLiteral("Movit (GPU): %1\n").arg(KdenliveSettings::gpu_accel() ? QStringLiteral("enabled") : QStringLiteral("disabled")));
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(debuginfo);
}

#ifdef DEBUG_MAINW
#undef DEBUG_MAINW
#endif
