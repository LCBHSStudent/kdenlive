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
{}

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
    
    clear();
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
}

void AssetController::clearAssetData(int itemId) {
    if (itemId == -1) {
        // closing project, reset panel
        clear();
        return;
    }
    if (!(m_effectsModel || m_transModel || m_mixModel)) {
        return;
    }
    ObjectId id = m_effectsModel->getOwnerId();
    if (id.first == ObjectType::TimelineClip && id.second == itemId) {
        clear();
        return;
    }
    id = m_transModel->getOwnerId();
    if (id.first == ObjectType::TimelineComposition && id.second == itemId) {
        clear();
        return;
    }
    id = m_mixModel->getOwnerId();
    if (id.first == ObjectType::TimelineMix && id.second == itemId) {
        clear();
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
    m_effectsModel.reset();
    m_transModel.reset();
    m_mixModel.reset();
}

bool AssetController::selectSizePositionAdjust() {
    const QString& id = KdenliveSettings::gpu_accel()? "movit.rect": "affine";
    return addEffect(id);
}
