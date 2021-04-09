#ifndef CUSTOMEDITORTOOLBAR_H
#define CUSTOMEDITORTOOLBAR_H

#include <QWidget>

class QPushButton;
class QLabel;

class CustomEditorToolBar: public QWidget {
	Q_OBJECT
public:
    explicit CustomEditorToolBar(QWidget* parent = nullptr);

protected:
	void resizeEvent(QResizeEvent*) override;
	
public slots:
	void slotOpenUndoView();
	void slotCloseUndoView();
	void slotOpenMessionView();
	void slotCloseMessionView();
	void slotOpenProjMediaset();
	void slotCloseProjMediaset();
	
private:
	QLabel* 
		m_documentName		= nullptr;
	
	QPushButton* 
		m_leadinBtn			= nullptr;
	QPushButton* 
		m_undoViewBtn		= nullptr;
	QPushButton* 
		m_messionViewBtn	= nullptr;
	QPushButton* 
		m_projMediasetBtn	= nullptr;
};

#endif // CUSTOMEDITORTOOLBAR_H
