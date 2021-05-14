#ifndef CLIPPROPERTIESWIDGET_H
#define CLIPPROPERTIESWIDGET_H

#include "framelessmovablewidget.h"

class ClipController;

#include <memory>
#include <mlt++/MltProperties.h>

class ClipPropertiesWidget: public FramelessMovableWidget {
    Q_OBJECT
public:
    explicit 
		ClipPropertiesWidget(QWidget* parent = nullptr);
    virtual
		~ClipPropertiesWidget() = default;
    
public slots:
    void slotSetClipController(ClipController*);
    void slotRefreshProperties();
        
signals:
    
protected:
	void showEvent(QShowEvent*) override;
	void paintEvent(QPaintEvent*) override;
    
private:
    ClipController* m_controller = nullptr;
    /** @brief: 当前选定的素材属性(非代理) */
    std::shared_ptr<Mlt::Properties> m_properties;
    /** @brief: 当前选定的素材源属性(不可为代理) */
    Mlt::Properties m_sourceProperties;

    int         m_type          = 0;    
    QString     m_vCodec        = {};
    QString     m_aCodec        = {};
    QString     m_title         = {};
    QString     m_path          = {};
    QString     m_duration      = {};
    QSize       m_resolution    = {};
    QSize       m_aspectRatio   = {};
    double      m_fps           = 0.0f;
    int         m_vBitRate      = 0;
    int         m_audioChannel  = 0;
    int         m_sampleRate    = 0;
    int         m_aBitRate      = 0;
    bool        m_hasPath       = false;
    bool        m_hasVideo      = false;
    bool        m_hasAudio      = false;
};

#endif // CLIPPROPERTIESWIDGET_H
