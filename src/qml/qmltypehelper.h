#ifndef QMLTYPEHELPER_H
#define QMLTYPEHELPER_H

class QmlTypeHelper final {
public:
    QmlTypeHelper()  = delete;
    ~QmlTypeHelper() = delete;
    
    static void registerAllQmlTypes();
};

#endif // QMLTYPEHELPER_H
