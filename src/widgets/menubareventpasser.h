#ifndef MENU_BAR_EVENT_PASSER_H
#define MENU_BAR_EVENT_PASSER_H

#include <QWidget>

class MenuBarEventPasser: public QWidget {
	Q_OBJECT
public:
	explicit MenuBarEventPasser(QWidget* parent);
	~MenuBarEventPasser() override = default;
	
protected:
	bool event(QEvent*) override;
};

#endif
