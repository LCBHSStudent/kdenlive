#ifndef EFFECTANDTRANSTAB_H
#define EFFECTANDTRANSTAB_H

#include <QFrame>

class QButtonGroup;

class TabsWidget: public QFrame {
    Q_OBJECT
public:
    explicit TabsWidget(QWidget* parent = nullptr);
    
    void addTab(QWidget* tabWidget, const QString& tabTitile);
    void selectTab(int index);
    
protected:
    void resizeEvent(QResizeEvent*) override;
    void showEvent(QShowEvent*) override;
    
    virtual void fix();
    
private:
    int m_currentTabIdx = -1;
    
    QWidget* m_tabContainer = nullptr;
    QButtonGroup* m_tabBtnGroup = nullptr;
    
    QVector<QString> m_tabTitles = {};    
    QVector<QWidget*> m_tabs = {};
};

#endif // EFFECTANDTRANSTAB_H
