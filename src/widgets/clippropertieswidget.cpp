#include "clippropertieswidget.h"

#include "bin/bin.h"
#include "core.h"
#include "definitions.h"
#include "mltcontroller/clipcontroller.h"

#include "macros.hpp"

#include <QPainter>
#include <klocalizedstring.h>

constexpr int fixedWidth = 370;
constexpr int binLeftMargin = 20;
constexpr int binTopMargin = 16;

ClipPropertiesWidget::ClipPropertiesWidget(QWidget* parent)
	: FramelessMovableWidget(parent)
{   
    setFixedWidth(fixedWidth);
    resize(fixedWidth, 490);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    
    setStyleSheet(R"(
    QFrame {
        background-color: #3E3D4C;
        border: 0px;
    })");
    setHeaderHeight(40);
    setTitle(i18n("属    性"));
}

void ClipPropertiesWidget::showEvent(QShowEvent*) {
    auto&& binGeo = pCore->bin()->geometry();
    move(binGeo.width() + binGeo.x() + binLeftMargin, binGeo.y() + binTopMargin);
}

constexpr auto lineMargin           = 14;
constexpr auto blockMargin          = 41;
constexpr auto leftWidth            = 97;
constexpr auto rightContentWidth    = 168;
constexpr auto lrSpacing            = 40;

#define CPW_DRAW_LABEL(__TEXT) \
    p.drawText(12, dy, leftWidth, lineHeight, Qt::AlignVCenter | Qt::AlignRight, i18n(__TEXT))
#define CPW_DRAW_CONTENT(__TEXT) \
    p.drawText(12 + leftWidth + lrSpacing, dy, rightContentWidth, lineHeight, Qt::AlignVCenter | Qt::AlignLeft, __TEXT)

void ClipPropertiesWidget::paintEvent(QPaintEvent* e) {
    FramelessMovableWidget::paintEvent(e);
    
    auto w = width();
    auto h = height();
    
    QPainter p(this);
    p.save();
    p.setPen(Qt::transparent);
    p.setBrush(QColor(45, 44, 57));
    p.drawRect(QRect(12, headerHeight(), w - 24, h - headerHeight() - 12));
    p.restore();
    
    QFont YaHei("Microsoft YaHei");
    YaHei.setPixelSize(12);
    QFontMetrics fm(YaHei);
    p.setBrush(Qt::white);
    
    auto dy = 16 + headerHeight();
    auto lineHeight = fm.height();
    
    {
        CPW_DRAW_LABEL("名称");
        CPW_DRAW_CONTENT(fm.elidedText(m_title, Qt::ElideRight, rightContentWidth));
        dy += lineMargin + lineHeight;
    }
    if (m_hasPath) {
        CPW_DRAW_LABEL("素材位置");
        CPW_DRAW_CONTENT(fm.elidedText(m_path, Qt::ElideRight, rightContentWidth));
        dy += lineMargin + lineHeight;
    }
    {
        CPW_DRAW_LABEL("时长");
        CPW_DRAW_CONTENT(m_duration);
    }
    
    if (m_hasVideo) {
        dy += lineHeight +  blockMargin;
        {
            CPW_DRAW_LABEL("视频编码");
            CPW_DRAW_CONTENT(m_vCodec);
            dy += lineMargin + lineHeight;
        }
        {
            CPW_DRAW_LABEL("视频分辨率");
            CPW_DRAW_CONTENT(QString::number(m_resolution.width()) + " X " + QString::number(m_resolution.height()));
            dy += lineMargin + lineHeight;
        }
        {
            CPW_DRAW_LABEL("宽高比");
            CPW_DRAW_CONTENT(QString::number(m_aspectRatio.width()) + " : " + QString::number(m_aspectRatio.height()));
            dy += lineMargin + lineHeight;
        }
        {
            CPW_DRAW_LABEL("视频帧率");
            CPW_DRAW_CONTENT(QString::number(m_fps, 'g', 4) + "fps");
            dy += lineMargin + lineHeight;
        }   
        {
            CPW_DRAW_CONTENT(QString::number(m_vBitRate) + "kbps");
            CPW_DRAW_LABEL("视频码率");
        }
    }
    
    if (m_hasAudio) {
        dy += lineHeight +  blockMargin;
        {
            CPW_DRAW_LABEL("音频编码器");
            CPW_DRAW_CONTENT(m_aCodec);
            dy += lineMargin + lineHeight;
        }
        {
            CPW_DRAW_LABEL("声道");
            CPW_DRAW_CONTENT(QString::number(m_audioChannel));
            dy += lineMargin + lineHeight;
        }
        {
            CPW_DRAW_LABEL("音频采样率");
            CPW_DRAW_CONTENT(QString::number(m_sampleRate) + "Hz");
            dy += lineMargin + lineHeight;
        }
        {
            CPW_DRAW_CONTENT(QString::number(m_aBitRate) + "kbps");
            CPW_DRAW_LABEL("音频码率");
        }
    }
}

#undef CPW_DRAW_LABEL
#undef CPW_DRAW_CONTENT

void ClipPropertiesWidget::slotSetClipController(ClipController* con) {
    m_controller = con;
}

void ClipPropertiesWidget::slotRefreshProperties() {
    // clear prev properties cache
    {
        
    }
    
    if (m_controller == nullptr) {
        return;
    }
    m_properties.reset(new Mlt::Properties(m_controller->properties()));
    m_controller->mirrorOriginalProperties(m_sourceProperties);
    
    m_type      = m_controller->clipType();
    m_hasVideo  = m_controller->hasVideo();
    m_hasAudio  = m_controller->hasAudio();
    m_hasPath   = !m_controller->clipUrl().isEmpty();
    
    m_title = m_controller->clipName();
    m_path = m_controller->clipUrl();
    m_duration = m_controller->getStringDuration();
    
    if (m_type == ClipType::AV ||
        m_type == ClipType::Video ||
        m_type == ClipType::Image
    ) {
        int width  = m_properties->get_int("meta.media.width");
        int height = m_properties->get_int("meta.media.height");
        double sar = m_properties->get_double("meta.media.sample_aspect_num");
        
        if (m_properties->get_double("meta.media.sample_aspect_den") > 0) {
            sar /= m_properties->get_double("meta.media.sample_aspect_den");
        }
    
        if (m_properties->get("force_aspect_ratio")) {
            sar = m_properties->get_double("force_aspect_ratio");
        }
    
        m_aspectRatio.setWidth(width * sar);
        m_aspectRatio.setHeight(height);
    
        if (height > 0) {
            switch (int(sar * width / height * 100)) {
            case 133:
                m_aspectRatio.setWidth(4);
                m_aspectRatio.setHeight(3);
                break;
    
            case 177:
                m_aspectRatio.setWidth(16);
                m_aspectRatio.setHeight(9);
                break;
    
            case 56:
                m_aspectRatio.setWidth(9);
                m_aspectRatio.setHeight(16);
                break;
            default:
                break;
            }
        }  
    }
    
    if (m_hasVideo) {
        m_vCodec = m_controller->codec(false);
        m_resolution = QSize(m_controller->videoCodecProperty("width").toInt(), m_controller->videoCodecProperty("height").toInt());
        int vindex = m_sourceProperties.get_int("video_index");
        QString codecInfo = QString("meta.media.%1.codec.").arg(vindex);
        m_vBitRate = m_sourceProperties.get_int((codecInfo + QStringLiteral("bit_rate")).toUtf8().constData()) / 1000;
    
        m_fps = m_properties->get_double("meta.media.frame_rate_num");
        
        if (m_properties->get_double("meta.media.frame_rate_den") > 0) {
            m_fps /= m_properties->get_double("meta.media.frame_rate_den");
        }
    }
    
    if (m_hasAudio) {
        m_aCodec = m_controller->codec(true);
        
        int aindex = m_sourceProperties.get_int("audio_index");
        QString codecInfo = QString("meta.media.%1.codec.").arg(aindex);
       
        QString property = codecInfo + QStringLiteral("channels");
        m_audioChannel = m_sourceProperties.get_int(property.toUtf8().constData());
        
        property = codecInfo + QStringLiteral("bit_rate");
        m_aBitRate = m_sourceProperties.get_int(property.toUtf8().constData()) / 1000;
    
        property = codecInfo + QStringLiteral("sample_rate");
        m_sampleRate = m_sourceProperties.get_int(property.toUtf8().constData());
    }
}
