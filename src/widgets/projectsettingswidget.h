#ifndef PROJECTSETTINGSWIDGET_H
#define PROJECTSETTINGSWIDGET_H

#include <QQuickWidget>
#include <QSettings>

class ProjectSettingsWidget: public QQuickWidget {
	Q_OBJECT
public:
    explicit ProjectSettingsWidget(QWidget* parent);
	
private:
    QSettings m_projSettings = {};
};

#endif // PROJECTSETTINGSWIDGET_H
