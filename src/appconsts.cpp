#include "appconsts.h"

AppConsts::AppConsts(QObject *parent) : QObject(parent) {
    lighterSpaceColor = QColor(62,61,76);
    darkerSpaceColor = QColor(45,44,57);
	normalFontColor = QColor(255,255,255);
	lighterFontColor = QColor(255, 255, 255, 229);
	backgroundColor = QColor(41,40,51);
	foregroundColor = QColor(45,140,240);
    normalButtonColor = QColor(62,61,76);
	controlHighlightColor = QColor(42,40,54);
	controlBorderColor = QColor(94,95,114);
            
    fontFamily = "Microsoft YaHei";
}

const AppConsts& AppConsts::instance() {
    static AppConsts instance(nullptr);
    return instance;
}

void AppConsts::reloadColorScheme() {
    
}
