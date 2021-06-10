/***************************************************************************
 *   Copyright (C) 2017 by Nicolas Carion                                  *
 *   This file is part of Kdenlive. See www.kdenlive.org.                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) version 3 or any later version accepted by the       *
 *   membership of KDE e.V. (or its successor approved  by the membership  *
 *   of KDE e.V.), which shall act as a proxy defined in Section 14 of     *
 *   version 3 of the license.                                             *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "assetcontroller.hpp"
#include "core.h"
#include "definitions.h"
#include "macros.hpp"
#include "effects/effectstack/model/effectitemmodel.hpp"
#include "effects/effectstack/model/effectstackmodel.hpp"
#include "effects/effectstack/view/effectstackview.hpp"
#include "kdenlivesettings.h"
#include "assets/model/assetparametermodel.hpp"
#include "transitions/transitionsrepository.hpp"
#include "effects/effectsrepository.hpp"
#include "transitions/view/transitionstackview.hpp"
#include "transitions/view/mixstackview.hpp"

#include "assets/view/assetparameterview.hpp"
#include "monitor/monitor.h"
#include "utils/util.h"

#include <mutex>
#include <KColorScheme>
#include <KColorUtils>
#include <KDualAction>
#include <KSqueezedTextLabel>
#include <KMessageWidget>
#include <QApplication>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QComboBox>
#include <QFontDatabase>
#include <klocalizedstring.h>


std::unique_ptr<AssetController>& AssetController::instance() {
    std::once_flag init;
    std::call_once(init, [] {
        s_instance.reset(new AssetController(nullptr));
    });
    
    return s_instance;
}

AssetController::AssetController(QObject* parent)
    : QObject(parent)
{
    connect(this, &AssetController::saveStack, this, &AssetController::slotSaveStack);
    connect(this, &AssetController::activateEffect, this, [=](int row) {
        m_effectsModel->setActiveEffect(row);
    });
    connect(this, &AssetController::seekToPos, this, [this](int pos) {
        if (!m_effectsModel) return;
        // at this point, the effects returns a pos relative to the clip. We need to convert it to a global time
        int clipIn = pCore->getItemPosition(m_effectsModel->getOwnerId());
        emit seekToPos(pos + clipIn);
    });
    connect(pCore.get(), &Core::updateEffectZone, this, &AssetController::slotUpdateEffectZone);
    connect(this, &AssetController::showEffectZone, pCore.get(), &Core::showEffectZone);
}

void AssetController::selectTransition(
    int                                         tid, 
    const std::shared_ptr<AssetParameterModel>& transitionModel
) {
    Q_UNUSED(tid)
    ObjectId id = transitionModel->getOwnerId();
    if (m_transModel && m_transModel->getOwnerId() == id) {
        // already on this effect stack, do nothing
        return;
    }
    clear();
    m_transModel.reset();
    m_transModel = transitionModel;
}

void AssetController::selectMix(
    int cid, 
    const std::shared_ptr<AssetParameterModel>& mixModel
) {
    if (cid == -1) {
        clear();
        return;
    }
    ObjectId id = {ObjectType::TimelineMix, cid};
    if (m_mixModel && m_mixModel->getOwnerId() == id) {
        return;
    }
    m_mixModel.reset();
    m_mixModel = mixModel;
}

void AssetController::selectEffectStack(
    const QString&                              itemName,
    const std::shared_ptr<EffectStackModel>&    effectsModel,
    QSize                                       frameSize,
    bool                                        showKeyframes
){
    if (effectsModel == nullptr) {
        clear();
        return;        
    }
    ObjectId id = effectsModel->getOwnerId();
    
    QString title;
    bool showSplit = false;
    bool enableKeyframes = false;
    switch (id.first) {
    case ObjectType::TimelineClip:
        title = i18n("%1 effects", itemName);
        showSplit = true;
        enableKeyframes = true;
        break;
    case ObjectType::TimelineComposition:
        title = i18n("%1 parameters", itemName);
        enableKeyframes = true;
        break;
    case ObjectType::TimelineTrack:
        title = i18n("Track %1 effects", itemName);
        // TODO: track keyframes
        // enableKeyframes = true;
        break;
    case ObjectType::BinClip:
        title = i18n("Bin %1 effects", itemName);
        showSplit = true;
        break;
    default:
        title = itemName;
        break;
    }
    if (showSplit) {
        
    }
    if (enableKeyframes) {
        
    }
    
    m_effectsModel.reset();
    m_effectsModel = effectsModel;
    
    connect(this, &AssetController::deleteEffect, m_effectsModel.get(), &EffectStackModel::removeEffect);
    
    connect(this, &AssetController::createGroup, m_effectsModel.get(), &EffectStackModel::slotCreateGroup);
    
    connect(
        m_effectsModel.get(), &EffectStackModel::dataChanged,
        this, &AssetController::slotEffectModelDataUpdated
    );
    
    slotLoadEffectParameters();    
}

void AssetController::slotEffectModelDataUpdated(
    const QModelIndex&  topLeft,
    const QModelIndex&  bottomRight,
    const QVector<int>& 
) {
    if (!topLeft.isValid() || !bottomRight.isValid()) {
        return;
    }
    
    std::shared_ptr<EffectItemModel> effectItem = std::static_pointer_cast<EffectItemModel>(m_effectsModel->getEffectStackRow(topLeft.row()));
    
    QVector<QPair<QString, QVariant>> currentValues = effectItem->getAllParameters();
    QMap<QString, QString> values;
    for (const auto &param : qAsConst(currentValues)) {
        values.insert(param.first, param.second.toString());
    }
    
    emit refreshParams(effectItem->getAssetId(), values);
}

void AssetController::clearAssetData(int itemId) {
    if (itemId == -1) {
        // closing project, reset panel
        clear();
        return;
    }

    if (m_effectsModel) {
        ObjectId id = m_effectsModel->getOwnerId();
        if ((id.first == ObjectType::TimelineClip/* || id.first == ObjectType::BinClip*/) && id.second == itemId) {
            clear();
            return;
        }
    }
    if (m_transModel) {
        ObjectId id = m_transModel->getOwnerId();
        if (id.first == ObjectType::TimelineComposition && id.second == itemId) {
            clear();
            return;
        }
    }
    if (m_mixModel) {
        ObjectId id = m_mixModel->getOwnerId();
        if (id.first == ObjectType::TimelineMix && id.second == itemId) {
            clear();
        }
    }
}

void AssetController::assetControllerWarning(
    const QString service,
    const QString ,
    const QString messgae
) {
    QString finalMessage;
    if (!service.isEmpty() && EffectsRepository::get()->exists(service)) {
        QString effectName = EffectsRepository::get()->getName(service);
        if (!effectName.isEmpty()) {
            finalMessage = QStringLiteral("<b>") + effectName + QStringLiteral("</b><br />");
        }
    }
    finalMessage.append(messgae);
    
    qDebug() << finalMessage;
}

ObjectId AssetController::effectStackOwner() {
    return m_effectsModel? m_effectsModel->getOwnerId(): std::pair<ObjectType, int>(ObjectType::NoItem, -1);
}

bool AssetController::addEffect(const QString& effectId) {
    if (m_effectsModel) {
        return m_effectsModel->appendEffect(effectId, true);
    }
    return false;
}

void AssetController::clear() {
    
}

bool AssetController::selectService(const QString &serviceId) {
    if (!EffectsRepository::get()->exists(serviceId)) {
        return false;
    }
    if (m_effectsModel) {
        auto index = m_effectsModel->filterIndex(serviceId);
        if (index >= 0) {
            m_effectsModel->setActiveEffect(index);
            return true;
        }
    }
    return addEffect(serviceId);
}

QVariant AssetController::getFilterParam(QString key) const {
    if (m_effectsModel) {
        std::shared_ptr<EffectItemModel> effectModel = std::static_pointer_cast<EffectItemModel>(m_effectsModel->getEffectStackRow(m_effectsModel->getActiveEffect())) ;
        effectModel->filter().get(key.toUtf8().constData());
        
    }
    
    return QVariant();
}

void AssetController::setFilterParam(QString key, QVariant value) {
    if (m_effectsModel) {
        std::shared_ptr<EffectItemModel> effectModel = std::static_pointer_cast<EffectItemModel>(m_effectsModel->getEffectStackRow(m_effectsModel->getActiveEffect()));
        if (key == "rect") {
            effectModel->setParameter(key, value.toString());
        }
        else {
            effectModel->setParameter(key, value.toInt());
        }
    }
}

void AssetController::clearAssetParameterModel(bool isTransitionModel) {
    if (isTransitionModel) {
        if (m_transModel) {
            
        }
    } else {
        if (m_mixModel) {
            
        }
    }
}

void AssetController::clearEffectStackModel() {
    if (m_effectsModel) {
        
    }
}

void AssetController::slotLoadEffectParameters() {
    int max = m_effectsModel->rowCount();
    LOG_DEBUG() << "model effects count: " << max;
    
    if (max == 0) {
        // blank stack
        ObjectId item = m_effectsModel->getOwnerId();
        pCore->getMonitor(item.first == ObjectType::BinClip ? Kdenlive::ClipMonitor : Kdenlive::ProjectMonitor)->slotShowEffectScene(MonitorSceneDefault);
        return;
    }
    int active = qBound(0, m_effectsModel->getActiveEffect(), max - 1);
    bool hasLift = false;
    QModelIndex activeIndex;
    
    for (int i = 0; i < max; i++) {
        std::shared_ptr<AbstractEffectItem> item = m_effectsModel->getEffectStackRow(i);
        
        if (item->childCount() > 0) {
            // group, create sub stack
            continue;
        }
        std::shared_ptr<EffectItemModel> effectModel = std::static_pointer_cast<EffectItemModel>(item);
        // We need to rebuild the effect view
        if (effectModel->getAssetId() == QLatin1String("lift_gamma_gain")) {
            hasLift = true;
        }
        
        QModelIndex ix = m_effectsModel->getIndexFromItem(effectModel);
        if (active == i) {
            effectModel->setActive(true);
            activeIndex = ix;
            
            for (auto i = 0; i < effectModel->rowCount(); i++) {
                auto type = effectModel->data(effectModel->index(i), AssetParameterModel::TypeRole).value<ParamType>();
                if (type == ParamType::Geometry || type == ParamType::AnimatedRect) {
                    pCore->getMonitor(m_effectsModel->getOwnerId().first == ObjectType::BinClip ? Kdenlive::ClipMonitor : Kdenlive::ProjectMonitor)->slotShowEffectScene(MonitorSceneGeometry);
                }
            }
        }
        
    }
    if (!hasLift) {
        // 需要重建视图?
    }
    if (activeIndex.isValid()) {
        m_effectsModel->setActiveEffect(activeIndex.row());
    }
    
    qDebug() << "MUTEX UNLOCK!!!!!!!!!!!! loadEffects";
}

void AssetController::slotSaveStack() {
    
}

void AssetController::slotUpdateEffectZone(const QPoint p, bool withUndo) {
    
}

void AssetController::attachEffectItemControllerToQml() {
    
}
