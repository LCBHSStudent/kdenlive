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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QDBusAbstractAdaptor>
#include <QEvent>
#include <QImage>
#include <QMap>
#include <QShortcut>
#include <QString>
#include <QUndoView>

#include <KActionCategory>
#include <KColorSchemeManager>
#include <KSelectAction>
#include <KXmlGuiWindow>
#include <kautosavefile.h>
#include <utility>
#include <mlt++/Mlt.h>

#include "bin/bin.h"
#include "definitions.h"
#include "gentime.h"
#include "kdenlive_debug.h"
#include "kdenlivecore_export.h"
#include "statusbarmessagelabel.h"
#include "utils/otioconvertions.h"

class AssetPanel;
class AudioGraphSpectrum;
class EffectBasket;
class EffectListWidget;
class TransitionListWidget;
class KIconLoader;
class KdenliveDoc;
class Monitor;
class Render;
class RenderWidget;
class TimelineTabs;
class TimelineWidget;
class TimelineContainer;
class Transition;
class CustomMenu;
class FramelessHelper;
class ClipMonitorFrame;
class ProjectMonitorFrame;
class CustomEditorToolBar;
class KDualAction;
class TabsWidget;

class MltErrorEvent : public QEvent
{
public:
    explicit MltErrorEvent(QString message)
        : QEvent(QEvent::User)
        , m_message(std::move(message))
    {
    }

    QString message() const { return m_message; }

private:
    QString m_message;
};

class /*KDENLIVECORE_EXPORT*/ MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    /** @brief Initialises the main window.
     * @param MltPath (optional) path to MLT environment
     * @param Url (optional) file to open
     * @param clipsToLoad (optional) a comma separated list of clips to import in project
     *
     * If Url is present, it will be opened, otherwise, if openlastproject is
     * set, latest project will be opened. If no file is open after trying this,
     * a default new file will be created. */
    void init(const QString &mltPath);
    ~MainWindow() override;

    /** @brief Cache for luma files thumbnails. */
    static QMap<QString, QImage> m_lumacache;
    static QMap<QString, QStringList> m_lumaFiles;

    /** @brief Adds an action to the action collection and stores the name. */
    void addAction(const QString &name, QAction *action, const QKeySequence &shortcut = QKeySequence(), KActionCategory *category = nullptr);
    /** @brief Adds an action to the action collection and stores the name. */
    QAction *addAction(const QString &name, const QString &text, const QObject *receiver, const char *member, const QIcon &icon = QIcon(),
                       const QKeySequence &shortcut = QKeySequence(), KActionCategory *category = nullptr);

    QUndoGroup *m_commandStack;
    QUndoView *m_undoView;
    /** @brief holds info about whether movit is available on this system */
    bool m_gpuAllowed;
    int m_exitCode{EXIT_SUCCESS};
    QMap<QString, KActionCategory *> kdenliveCategoryMap;
    QList<QAction *> getExtraActions(const QString &name);

    /** @brief Returns a ptr to the main timeline widget of the project */
    TimelineWidget *getMainTimeline() const;

    /** @brief Returns a pointer to the current timeline */
    TimelineWidget *getCurrentTimeline() const;

    /** @brief Returns true if a timeline widget is available */
    bool hasTimeline() const;
    
    /** @brief Returns true if the timeline widget is visible */
    bool timelineVisible() const;
    
    /** @brief Raise (show) the clip or project monitor */
    void raiseMonitor(bool clipMonitor);

    /** @brief Raise (show) the project bin*/
    void raiseBin();

    /** @brief Hide subtitle track */
    void resetSubtitles();
    
    /** @brief Restart the application and delete config files if clean is true */
    void cleanRestart(bool clean);

    /** @brief Show current tool key combination in status bar */
    void showToolMessage();
    /** @brief Show the widget's default key binding message */
    void setWidgetKeyBinding(const QString &text = QString());
    /** @brief Show a key binding in status bar */
    void showKeyBinding(const QString &text = QString());
    /** @brief Override for QWidget::setWindowModified(void) */
    void setWindowModified(bool isModified = true);
    /** @brief get project monitor frame */
    ProjectMonitorFrame* projectMonitorFrame() const { return m_projectMonitorFrame; }    
    /** @brief get clip monitor frame */
    ClipMonitorFrame* clipMonitorFrame() const { return m_clipMonitorFrame; }
    
protected:
    /** @brief Closes the window.
     * @return false if the user presses "Cancel" on a confirmation dialog or
     *     the operation requested (starting waiting jobs or saving file) fails,
     *     true otherwise */
    bool queryClose() override;
    void closeEvent(QCloseEvent *) override;

    /** @brief Reports a message in the status bar when an error occurs. */
    void customEvent(QEvent *e) override;

    /** @brief Stops the active monitor when the window gets hidden. */
    void hideEvent(QHideEvent *e) override;

    /** @brief Saves the file and the window properties when saving the session. */
    void saveProperties(KConfigGroup &config) override;

    void saveNewToolbarConfig() override;
    bool eventFilter(QObject*, QEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private:
    /** @brief Sets up all the actions and attaches them to the collection. */
    void setupActions();
    void setupMenuBar();

    OtioConvertions m_otioConvertions;
    KColorSchemeManager *m_colorschemes;

    TransitionListWidget *m_transitionList2;
    EffectListWidget *m_effectList2;

    Monitor *m_clipMonitor{nullptr};

    Monitor *m_projectMonitor{nullptr};
    
    AssetPanel *m_assetPanel;
    
    ClipMonitorFrame* m_clipMonitorFrame = nullptr;
    ProjectMonitorFrame* m_projectMonitorFrame = nullptr;
    CustomEditorToolBar* m_editorToolBar = nullptr;

    AudioGraphSpectrum *m_audioSpectrum;

    KSelectAction *m_timeFormatButton;
    KSelectAction *m_compositeAction;

    TabsWidget* m_effectTransWidget = nullptr;
    TimelineTabs *m_timelineTabs{nullptr};

    KActionCategory *m_effectActions;
    KActionCategory *m_transitionActions;
    CustomMenu *m_effectsMenu;
    CustomMenu *m_transitionsMenu;
    CustomMenu *m_timelineContextMenu;
    QList<QAction *> m_timelineClipActions;
    KDualAction *m_useTimelineZone;

    /** Action names that can be used in the slotDoAction() slot, with their i18n() names */
    QStringList m_actionNames;

    /** @brief Shortcut to remove the focus from any element.
     *
     * It allows one to get out of e.g. text input fields and to press another
     * shortcut. */
    QShortcut *m_shortcutRemoveFocus;

    RenderWidget *m_renderWidget{nullptr};
    StatusBarMessageLabel *m_messageLabel{nullptr};
    QList<QAction *> m_transitions;
    QAction *m_buttonAudioThumbs;
    QAction *m_buttonVideoThumbs;
    QAction *m_buttonShowMarkers;
    QAction *m_buttonFitZoom;
    QAction *m_buttonTimelineTags;
    QAction *m_normalEditTool;
    QAction *m_overwriteEditTool;
    QAction *m_insertEditTool;
    QAction *m_buttonSelectTool;
    QAction *m_buttonRazorTool;
    QAction *m_buttonSpacerTool;
    QAction *m_buttonSnap;
    QAction *m_saveAction;
    QAction *m_loopZone;
    QAction *m_playZone;
    QAction *m_loopClip;
    QAction *m_proxyClip;
    QAction *m_buttonSubtitleEditTool;
    QString m_theme;
    KIconLoader *m_iconLoader;
    QFrame *m_timelineToolBar;
    TimelineContainer *m_timelineToolBarContainer;
    QLabel *m_trimLabel;
    QActionGroup *m_scaleGroup;
    
    CustomMenu  *m_fileMenu     = nullptr,
                *m_editMenu     = nullptr,
                *m_cutMenu      = nullptr,
                *m_settingMenu  = nullptr,
                *m_helpMenu     = nullptr;
    
    FramelessHelper* m_framelessHelper = nullptr;
    QFrame* m_windCtrlBtnFrame = nullptr;

    /** @brief initialize startup values, return true if first run. */
    bool readOptions();
    void saveOptions();

    void loadGenerators();
    /** @brief Instantiates a "Get Hot New Stuff" dialog.
     * @param configFile configuration file for KNewStuff
     * @return number of installed items */
    int getNewStuff(const QString &configFile = QString());
    QStringList m_pluginFileNames;
    QByteArray m_timelineState;
    void buildDynamicActions();
    void loadClipActions();

    QTime m_timer;
    KXMLGUIClient *m_extraFactory;
    bool m_themeInitialized{false};
    bool m_isDarkTheme{false};
    EffectBasket *m_effectBasket;
    /** @brief Update widget style. */
    void doChangeStyle();
    void updateActionsToolTip();

    QSettings m_mwSettings;
    
public slots:
    void slotReloadEffects(const QStringList &paths);
    Q_SCRIPTABLE void setRenderingProgress(const QString &url, int progress, int frame);
    Q_SCRIPTABLE void setRenderingFinished(const QString &url, int status, const QString &error);
    Q_SCRIPTABLE void addProjectClip(const QString &url, const QString & folder = QStringLiteral("-1"));
    Q_SCRIPTABLE void addTimelineClip(const QString &url);
    Q_SCRIPTABLE void addEffect(const QString &effectId);
    Q_SCRIPTABLE void scriptRender(const QString &url);
    Q_NOREPLY void exitApp();

    void slotSwitchVideoThumbs();
    void slotSwitchAudioThumbs();

    void slotPreferences(int page = -1, int option = -1);
    void connectDocument();
    /** @brief Reload project profile in config dialog if changed. */
    void slotRefreshProfiles();
    void updateDockTitleBars(bool isTopLevel = true);
    /** @brief Add/remove Dock tile bar depending on state (tabbed, floating, ...) */
    void slotUpdateDockLocation(Qt::DockWidgetArea dockLocationArea);
    void configureToolbars() override;
    /** @brief Enable or disable the use of timeline zone for edits. */
    void slotSwitchTimelineZone(bool toggled);
    /** @brief Open the online services search dialog. */
    void slotDownloadResources();
    void slotEditSubtitle(QMap<QString, QString> subProperties = {});
    void slotTranscode(const QStringList &urls = QStringList());
    /** @brief Add subtitle clip to timeline */
    void slotAddSubtitle(const QString &text = QString());
    /** @brief Ensure subtitle track is displayed */
    void showSubtitleTrack();
    /** @brief 设置项目资源库可见性：bool: show/hide */
    void setProjectMediasetVisible(bool visible);

private slots:
    /** @brief Shows the shortcut dialog. */
    void slotEditKeys();
    void loadDockActions();
    /** @brief Reflects setting changes to the GUI. */
    void updateConfiguration();
    void slotConnectMonitors();
    void slotUpdateMousePosition(int pos);
    void slotUpdateProjectDuration(int pos);
    void slotEditProjectSettings();
    void slotSwitchMarkersComments();
    void slotSwitchSnap();
    void slotShowTimelineTags();
    void slotRenderProject();
    void slotStopRenderProject();
    void slotFullScreen();
    /** @brief if modified is true adds "modified" to the caption and enables the save button.
     * (triggered by KdenliveDoc::setModified()) */
    void slotUpdateDocumentState(bool modified);
    
    /** @brief Deletes item in timeline, project tree or effect stack depending on focus. */
    void slotDeleteItem();
    void slotAddClipMarker();
    void slotDeleteClipMarker(bool allowGuideDeletion = false);
    void slotDeleteAllClipMarkers();
    void slotEditClipMarker();

    /** @brief Adds marker or guide at the current position without showing the marker dialog.
     *
     * Adds a marker if clip monitor is active, otherwise a guide.
     * The comment is set to the current position (therefore not dialog).
     * This can be useful to mark something during playback. */
    void slotAddMarkerGuideQuickly();
    void slotCutTimelineClip();
    void slotCutTimelineAllClips();
    void slotInsertClipOverwrite();
    void slotInsertClipInsert();
    void slotExtractZone();
    void slotLiftZone();
    void slotPreviewRender();
    void slotStopPreviewRender();
    void slotDefinePreviewRender();
    void slotRemovePreviewRender();
    void slotClearPreviewRender(bool resetZones = true);
    void slotSelectTimelineClip();
    void slotSelectTimelineTransition();
    void slotDeselectTimelineClip();
    void slotDeselectTimelineTransition();
    void slotSelectAddTimelineClip();
    void slotSelectAddTimelineTransition();
    void slotAddEffect(QAction *result);
    void slotAddTransition(QAction *result);
    void slotAddProjectClip(const QUrl &url, const QString &folderInfo);
    void slotAddTextNote(const QString &text);
    void slotAddProjectClipList(const QList<QUrl> &urls);
    void slotChangeTool(QAction *action);
    void slotChangeEdit(QAction *action);
    void slotSetTool(ProjectTool tool);
    void slotSnapForward();
    void slotSnapRewind();
    void slotGuideForward();
    void slotGuideRewind();
    void slotClipStart();
    void slotClipEnd();
    void slotSelectClipInTimeline();
    void slotClipInTimeline(const QString &clipId, const QList<int> &ids);

    void slotInsertSpace();
    void slotRemoveSpace();
    void slotRemoveAllSpace();
    void slotAddGuide();
    void slotEditGuide();
    void slotLockGuides(bool lock);
    void slotDeleteGuide();
    void slotDeleteAllGuides();
    void slotGuidesUpdated();

    void slotCopy();
    void slotPaste();
    void slotPasteEffects();
    void slotResizeItemStart();
    void slotResizeItemEnd();
    void configureNotifications();
    void slotSeparateAudioChannel();
    /** @brief Normalize audio channels before displaying them */
    void slotNormalizeAudioChannel();
    void slotInsertTrack();
    void slotDeleteTrack();
    /** @brief Show context menu to switch current track target audio stream. */
    void slotSwitchTrackAudioStream();
    void slotShowTrackRec();
    /** @brief Select all clips in active track. */
    void slotSelectTrack();
    /** @brief Select all clips in timeline. */
    void slotSelectAllTracks();
    void slotUnselectAllTracks();
    void slotGetNewKeyboardStuff(QComboBox *schemesList);
    void slotAutoTransition();
    void slotRunWizard();
    void slotGroupClips();
    void slotUnGroupClips();
    void slotEditItemDuration();
    void slotClipInProjectTree();
    // void slotClipToProjectTree();
    void slotSplitAV();
    void slotSwitchClip();
    void slotSetAudioAlignReference();
    void slotAlignAudio();
    void slotUpdateClipType(QAction *action);
    void slotUpdateTimelineView(QAction *action);
    void slotShowTimeline(bool show);
    void slotTranscodeClip();
    /** @brief Archive project: creates a copy of the project file with all clips in a new folder. */
    void slotArchiveProject();
    void slotSetDocumentRenderProfile(const QMap<QString, QString> &props);

    /** @brief Switches between displaying frames or timecode.
     * @param ix 0 = display timecode, 1 = display frames. */
    void slotUpdateTimecodeFormat(int ix);

    /** @brief Removes the focus of anything. */
    void slotRemoveFocus();
    void slotCleanProject();
    void slotShutdown();

    void slotSwitchMonitors();
    void slotSwitchMonitorOverlay(QAction *);
    void slotSwitchDropFrames(bool drop);
    void slotSetMonitorGamma(int gamma);
    void slotCheckRenderStatus();
    void slotInsertZoneToTree();

    /** @brief The monitor informs that it needs (or not) to have frames sent by the renderer. */
    void slotMonitorRequestRenderFrame(bool request);
    /** @brief Update project because the use of proxy clips was enabled / disabled. */
    void slotUpdateProxySettings();
    /** @brief Disable proxies for this project. */
    void slotDisableProxies();

    /** @brief Process keyframe data sent from a clip to effect / transition stack. */
    void slotProcessImportKeyframes(GraphicsRectItem type, const QString &tag, const QString &keyframes);
    /** @brief Move playhead to mouse cursor position if defined key is pressed */
    void slotAlignPlayheadToMousePos();

    void slotThemeChanged(const QString &name);
    /** @brief Close Kdenlive and try to restart it */
    void slotRestart(bool clean = false);
    void triggerKey(QKeyEvent *ev);
    /** @brief Update monitor overlay actions on monitor switch */
    void slotUpdateMonitorOverlays(int id, int code);
    /** @brief Update widget style */
    void slotChangeStyle(QAction *a);
    /** @brief Create temporary top track to preview an effect */
    void createSplitOverlay(std::shared_ptr<Mlt::Filter> filter);
    void removeSplitOverlay();
    /** @brief Create a generator's setup dialog */
    void buildGenerator(QAction *action);
    void slotCheckTabPosition();
    /** @brief Toggle automatic timeline preview on/off */
    void slotToggleAutoPreview(bool enable);
    /** @brief Rebuild/reload timeline toolbar. */
    void rebuildTimlineToolBar();
    /** @brief Open Cached Data management dialog. */
    void slotManageCache();
    void showMenuBar(bool show);
    /** @brief Change forced icon theme setting (asks for app restart). */
    void forceIconSet(bool force);
    /** @brief Toggle current project's compositing mode. */
    void slotUpdateCompositing(QAction *compose);
    /** @brief Update compositing action to display current project setting. */
    void slotUpdateCompositeAction(int mode);
    /** @brief Cycle through the different timeline trim modes. */
    void slotSwitchTrimMode();
    void setTrimMode(const QString &mode);
    /** @brief Set timeline toolbar icon size. */
    void setTimelineToolbarIconSize(QAction *a);
    void slotEditItemSpeed();
    /** @brief Request adjust of timeline track height */
    void resetTimelineTracks();
    /** @brief Set keyboard grabbing on current timeline item */
    void slotGrabItem();
    /** @brief Collapse or expand current item (depending on focused widget: effet, track)*/
    void slotCollapse();
    /** @brief Save currently selected timeline clip as bin subclip*/
    void slotExtractClip();
    /** @brief Save currently selected timeline clip as bin subclip*/
    void slotSaveZoneToBin();
    /** @brief Expand current timeline clip (recover clips and tracks from an MLT playlist) */
    void slotExpandClip();
    /** @brief Focus and activate an audio track from a shortcut sequence */
    void slotActivateAudioTrackSequence();
    /** @brief Focus and activate a video track from a shortcut sequence */
    void slotActivateVideoTrackSequence();
    /** @brief Select target for current track */
    void slotActivateTarget();
    /** @brief Enable/disable subtitle track */
    void slotDisableSubtitle();
    /** @brief Lock / unlock subtitle track */
    void slotLockSubtitle();
    /** @brief Import a subtitle file */
    void slotImportSubtitle();
    /** @brief Export a subtitle file */
    void slotExportSubtitle();
    /** @brief Start a speech recognition on timeline zone */
    void slotSpeechRecognition();
    /** @brief Copy debug information like lib versions, gpu mode state,... to clipboard */
    void slotCopyDebugInfo();

signals:
    Q_SCRIPTABLE void abortRenderJob(const QString &url);
    void configurationChanged();
    void GUISetupDone();
    void setPreviewProgress(int);
    void setRenderProgress(int);
    void displayMessage(const QString &, MessageType, int);
    void displayProgressMessage(const QString &, MessageType, int);
    /** @brief Project profile changed, update render widget accordingly. */
    void updateRenderWidgetProfile();
    /** @brief Clear asset view if itemId is displayed. */
    void clearAssetPanel(int itemId = -1);
    void assetPanelWarning(const QString service, const QString id, const QString message);
    void adjustAssetPanelRange(int itemId, int in, int out);
    /** @brief Enable or disable the undo stack. For example undo/redo should not be enabled when dragging a clip in timeline or we risk corruption. */
    void enableUndo(bool enable);
    bool focusTimeline(bool focus, bool highlight);
    void updateProjectPath(const QString &path);
};

#endif
