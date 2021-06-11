#ifndef TIMELINETOOLBAR_H
#define TIMELINETOOLBAR_H

#include <QFrame>
#include <QPushButton>
#include <QLabel>

class TimelineTabs;

class TimelineToolButton: public QPushButton {
    Q_OBJECT
public:
    TimelineToolButton(
		const QString&	iconName,
		const QString&	toolTip,
		QWidget*		parent = nullptr
	);
    static const QString defaultStyleSheet;
};

class TimelineTimecodeLabel: public QLabel {
    Q_OBJECT
public:
    explicit TimelineTimecodeLabel(QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent*) override;
};

/**
 * @brief 时间线工具栏
 * @abstract 放置时间线上方工具按钮或自定义widget 由ToolBtnLayoutManager管理布局
 */
class ToolBtnLayoutManager;
class TimelineToolBar: public QFrame {
	Q_OBJECT
public:
    explicit TimelineToolBar(TimelineTabs* tabs, QWidget* parent = nullptr);
	~TimelineToolBar() override = default;
    
protected:
	void resizeEvent(QResizeEvent*) override;
	
private:
	ToolBtnLayoutManager*   m_manager = nullptr;
	TimelineTabs*           m_timelineTabs = nullptr;
};

#endif // TIMELINETOOLBAR_H
