#include "projectsettingswidget.h"

#include "core.h"
#include "profiles/profileinfo.hpp"
#include "profiles/profilemodel.hpp"
#include "mainwindow.h"

#include <QAction>
#include <KLocalizedContext>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>

#include "macros.hpp"

constexpr int __dropshadowMargin = 6;


#ifdef DEBUG_BUILD
    static const char* qmlPath = "file:///A:/CraftRoot/build/kde/kdemultimedia/kdenlive/work/kde_based_editor/src/widgets/ProjectSettings.qml";
#else
    static const char* qmlPath = "qrc:/qml/ProjectSettings.qml";
#endif

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
            rootObj->setProperty("subtitleMarginHor", m_projSettings.value("subtitleMarginHor", 20).toInt());
            rootObj->setProperty("subtitleMarginVer", m_projSettings.value("subtitleMarginVer", 20).toInt());
            rootObj->setProperty("actionMarginHor", m_projSettings.value("actionMarginHor", 10).toInt());
            rootObj->setProperty("actionMarginVer", m_projSettings.value("actionMarginVer", 10).toInt());
            rootObj->setProperty("profileW", profile->width());
            rootObj->setProperty("profileH", profile->height());
            rootObj->setProperty("profileFps", profile->fps());
            
            connect(rootObj, SIGNAL(move(QVariant,QVariant)), this, SLOT(move(QVariant,QVariant)));
            connect(rootObj, SIGNAL(close()), this, SLOT(close()));
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

void ProjectSettingsWidget::move(QVariant x, QVariant y) {
    auto&& mainGeo = pCore->window()->geometry();
    
    QQuickWidget::move(
        qBound(mainGeo.x(), x.toInt(), mainGeo.x() + mainGeo.width()),
        qBound(mainGeo.y(), y.toInt(), mainGeo.y() + mainGeo.height())
    );
}
