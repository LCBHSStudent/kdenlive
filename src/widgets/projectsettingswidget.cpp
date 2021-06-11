#include "projectsettingswidget.h"

#include "core.h"
#include "profiles/profileinfo.hpp"
#include "profiles/profilerepository.hpp"
#include "profiles/profilemodel.hpp"
#include "doc/kdenlivedoc.h"
#include "mainwindow.h"
#include "kdenlivesettings.h"
#include "jobs/taskmanager.h"

#include <QAction>
#include <KLocalizedContext>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <config-kdenlive.h>

#include "macros.hpp"

constexpr int __dropshadowMargin = 0;


#ifdef DEBUG_BUILD
    static const char* qmlPath = "file:///" SMARTIP_PWD "/src/widgets/ProjectSettings.qml";
#else
    static const char* qmlPath = "qrc:/qml/ProjectSettings.qml";
#endif

#define GET_AND_SET(__NAME, __TYPE, __DEFAULT) \
    rootObj->setProperty(#__NAME, m_projSettings.value(#__NAME, __DEFAULT).to##__TYPE())
    
ProjectSettingsWidget::ProjectSettingsWidget(QWidget* parent)
	: QQuickWidget(parent)
{
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_AlwaysStackOnTop, true);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setClearColor(Qt::transparent);
	
	setResizeMode(QQuickWidget::SizeRootObjectToView);
    engine()->rootContext()->setContextObject(new KLocalizedContext(this));
	
	setFixedSize(500, 576 + __dropshadowMargin);
	
    connect(this, &QQuickWidget::statusChanged, [this] {
        auto rootObj = rootObject();
        if (rootObj) {
            std::unique_ptr<ProfileModel>& profile = pCore->getCurrentProfile();
            
            rootObj->setProperty("__dropshadowMargin", __dropshadowMargin);
            rootObj->setProperty("profileW", profile->width());
            rootObj->setProperty("profileH", profile->height());
            rootObj->setProperty("profileFps", profile->fps());
            
            GET_AND_SET(subtitleMarginHor, Int, 20);
            GET_AND_SET(subtitleMarginVer, Int, 20);
            GET_AND_SET(actionMarginHor, Int, 10);
            GET_AND_SET(actionMarginVer, Int, 10);
            GET_AND_SET(scanMethod, Int, 0);
            GET_AND_SET(aspectIndex, Int, 6);
            GET_AND_SET(channelIndex, Int, 0);
            GET_AND_SET(audioSampleIndex, Int, 2);
            GET_AND_SET(audioCodecIndex, Int, 0);
            GET_AND_SET(bitRateControlIndex, Int, 0);
            GET_AND_SET(bitRateIndex, Int, 5);
            GET_AND_SET(disableAudio, Bool, false);      
            
            connect(rootObj, SIGNAL(move(QVariant,QVariant)), this, SLOT(move(QVariant,QVariant)));
            connect(rootObj, SIGNAL(cancel()), this, SLOT(close()));
            connect(rootObj, SIGNAL(confirm()), this, SLOT(confirmSettings()));
        } else {
            return;
        }
    });
        
    setSource(QUrl(qmlPath));
    
    auto hotReload = new QAction(this);
    hotReload->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    addAction(hotReload);
    
    connect(hotReload, &QAction::triggered, [this] {
        setSource(QUrl());
        engine()->clearComponentCache();
        setSource(QUrl(qmlPath));
    });
    
    show();
}

#undef GET_AND_SET

void ProjectSettingsWidget::move(QVariant x, QVariant y) {
    auto&& mainGeo = pCore->window()->geometry();
    
    QQuickWidget::move(
        qBound(mainGeo.x(), x.toInt() + pos().x(), mainGeo.x() + mainGeo.width() - width()),
        qBound(mainGeo.y(), y.toInt() + pos().y(), mainGeo.y() + mainGeo.height() - height())
    );
}

#define GET_AND_SET(__NAME, __TYPE) \
    auto __NAME = rootObj->property(#__NAME).to##__TYPE(); \
    m_projSettings.setValue(#__NAME, __NAME) 

void ProjectSettingsWidget::confirmSettings() {
    auto rootObj = rootObject();
    if (rootObj) {
        GET_AND_SET(subtitleMarginHor, Int);
        GET_AND_SET(subtitleMarginVer, Int);
        GET_AND_SET(actionMarginHor, Int);
        GET_AND_SET(actionMarginVer, Int);
        
        auto profileW           = rootObj->property("profileW").toInt();
        auto profileH           = rootObj->property("profileH").toInt();
        auto profileFps         = rootObj->property("profileFps").toReal();
        
        std::unique_ptr<Mlt::Profile> blankProfile(new Mlt::Profile());
        blankProfile->set_frame_rate(profileFps * 10000, 10000);
        blankProfile->set_width(profileW);
        blankProfile->set_height(profileH);
        
        std::unique_ptr<ProfileParam> profile(new ProfileParam(blankProfile.get()));
        profile->m_description = QStringLiteral("%1x%2 %3fps")
                                     .arg(profile->m_width)
                                     .arg(profile->m_height)
                                     .arg(QString::number(double(profile->m_frame_rate_num) / profile->m_frame_rate_den, 'f', 2));
        QString profilePath = ProfileRepository::get()->saveProfile(profile.get());
        // Discard all current jobs
        pCore->taskManager.slotCancelJobs();
        pCore->setCurrentProfile(profilePath);   
        
        pCore->currentDoc()->updateProjectProfile(true);
        emit pCore->currentDoc()->docModified(true);
        
        GET_AND_SET(scanMethod, Int);
        GET_AND_SET(channelIndex, Int);
        GET_AND_SET(aspectIndex, Int);
        GET_AND_SET(audioSampleIndex, Int);
        GET_AND_SET(audioCodecIndex, Int);
        GET_AND_SET(bitRateControlIndex, Int);
        GET_AND_SET(bitRateIndex, Int);
        GET_AND_SET(disableAudio, Bool);        
    }
    
    close();
}

#undef GET_AND_SET
