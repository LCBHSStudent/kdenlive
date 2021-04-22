#include "projectsettingswidget.h"

#include <KLocalizedContext>
#include <QQmlEngine>
#include <QQmlContext>

ProjectSettingsWidget::ProjectSettingsWidget(QWidget* parent)
	: QQuickWidget(parent)
{
	setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_AlwaysStackOnTop, true);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setClearColor(Qt::transparent);
	
	setResizeMode(QQuickWidget::SizeRootObjectToView);
    engine()->rootContext()->setContextObject(new KLocalizedContext(this));
	
	setFixedSize(500, 576);
	
	setSource(QUrl("qrc:/qml/ProjectSettings.qml"));
    show();
}
