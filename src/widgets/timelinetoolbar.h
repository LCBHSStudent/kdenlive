#ifndef TIMELINETOOLBAR_H
#define TIMELINETOOLBAR_H

#include <QFrame>
#include <QPushButton>

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

class ToolBtnLayoutManager;
class TimelineToolBar: public QFrame {
	Q_OBJECT
public:
    explicit TimelineToolBar(QWidget* parent = nullptr);
	~TimelineToolBar() override = default;
	
protected:
	void resizeEvent(QResizeEvent*) override;
	
private:
	ToolBtnLayoutManager* m_manager = nullptr;
	
};

#endif // TIMELINETOOLBAR_H
