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
    void paintEvent(QPaintEvent*) override;
	
signals:
	void tabChanged(int);
	
private slots:
	void onTabChanged();
    
private:
	TabType m_currentTab;
    
    int     m_fMaterialWidth = 0;
    int     m_fEditorWidth = 0;
    int     m_fMaterialHeight = 0;
    int     m_fEditorHeight = 0;
    
    QColor  m_tabColorEditor = {};
    QColor  m_tabColorMaterial = {};
    bool    m_fontBoldEditor = false;
    bool    m_fontBoldMaterial = false;
};

#endif // TOPNAVIGATIONBAR_H
