#ifndef CUSTOMEDITORTOOLBAR_H
#define CUSTOMEDITORTOOLBAR_H

#include <QWidget>

class QPushButton;
class QLabel;

class CustomEditorToolBar: public QWidget {
	Q_OBJECT
public:
    explicit CustomEditorToolBar(QWidget* parent = nullptr);

    void 
        setDocumentString(const QString& docStr);
    QString 
        documentString() const;
    
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
		m_documentStr		= nullptr;
	
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
