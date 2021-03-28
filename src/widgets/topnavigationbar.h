#ifndef TOPNAVIGATIONBAR_H
#define TOPNAVIGATIONBAR_H

#include <QLabel>

class TopNavigationBar: public QLabel {
	Q_OBJECT
public:
    explicit TopNavigationBar(QWidget* parent = nullptr);
	
	enum TabType: int {
		EDITOR = 114514,
		MATERIAL_DATABASE
	}; Q_ENUM(TabType);
	
protected:
	void mousePressEvent(QMouseEvent*) override;
	bool eventFilter(QObject*, QEvent*) override;
	
signals:
	void tabChanged(int);
	
private slots:
	void onTabChanged();
	
private:
	TabType m_currentTab;
	
	QPixmap* m_editorPix = nullptr;
	QPixmap* m_materialDbPix = nullptr;
};

#endif // TOPNAVIGATIONBAR_H
