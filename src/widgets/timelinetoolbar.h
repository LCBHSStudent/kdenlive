#ifndef TIMELINETOOLBAR_H
#define TIMELINETOOLBAR_H

#include <QFrame>
#include <QPushButton>
#include <QLabel>

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
