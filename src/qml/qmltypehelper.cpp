#include "qmltypehelper.h"

#include "qmlcursorarea.h"
#include "qmlhovertip.h"
#include "qmlrichtext.h"
#include "colorwheelitem.h"
#include "colorpickeritem.h"

#include <QtQml>

void QmlTypeHelper::registerAllQmlTypes() {
    qmlRegisterType<QmlCursorArea>("DFW.Components", 1, 0, "CursorArea");
    qmlRegisterType<QmlHoverTip>("DFW.Components", 1, 0, "HoverTipXX");
    qmlRegisterType<QmlRichText>("DFW.Components", 1, 0, "RichText");
    qmlRegisterType<ColorPickerItem>("DFW.Components", 1, 0, "ColorPickerItem");
    qmlRegisterType<ColorWheelItem>("DFW.Components", 1, 0, "ColorWheelItem");
}
