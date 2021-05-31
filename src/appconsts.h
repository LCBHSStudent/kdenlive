#ifndef APPCONSTS_H
#define APPCONSTS_H

#include <QObject>
#include <QColor>

#define APP_PTY(__TYPE, __NAME) \
	Q_PROPERTY(__TYPE __NAME MEMBER __NAME NOTIFY __NAME##Changed)

#define APPUI_PTY(__TYPE, __NAME) \
	Q_PROPERTY(__TYPE __NAME MEMBER __NAME NOTIFY paletteChanged) \
	__TYPE __NAME;

class AppConsts: public QObject {
    Q_OBJECT
public:
	APP_PTY(QString, fontFamily)
	APPUI_PTY(QColor, lighterSpaceColor)
	APPUI_PTY(QColor, darkerSpaceColor)
	APPUI_PTY(QColor, normalFontColor)
	APPUI_PTY(QColor, lighterFontColor)
	APPUI_PTY(QColor, backgroundColor)
	APPUI_PTY(QColor, foregroundColor)
	APPUI_PTY(QColor, controlHighlightColor)
	APPUI_PTY(QColor, controlBorderColor)
	APPUI_PTY(QColor, normalButtonColor)
	
	QString fontFamily;
    
public:
	static const AppConsts& instance();
	
public slots:
	void reloadColorScheme();
	
signals:
    void paletteChanged();
    void fontFamilyChanged();

protected:
	
	
	
private:
	explicit AppConsts(QObject *parent = nullptr);
	virtual ~AppConsts() = default;
	
};

#define APPCONSTS AppConsts::instance()

#endif // APPCONSTS_H
