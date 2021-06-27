#ifndef PROJECTMONITORFRAME_H
#define PROJECTMONITORFRAME_H

#include <QFrame>

class Monitor;
class AssetPanel;

class ProjectMonitorFrame: public QFrame {
	Q_OBJECT
public:
    explicit ProjectMonitorFrame(Monitor* monitor, QWidget* parent = nullptr);
	
    void setAssetPanel(AssetPanel* rPanel);
    
protected:
	void resizeEvent(QResizeEvent*) override;
	void paintEvent(QPaintEvent*) override;
	
public slots:
	
private:
	// weak_ptr
	Monitor*
        m_projectMonitor = nullptr;
    AssetPanel*
        s_assetPanel = nullptr;
};

#endif // PROJECTMONITORFRAME_H
