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

#include "keyframemodellist.hpp"
#include "doc/docundostack.hpp"
#include "core.h"
#include "macros.hpp"
#include "klocalizedstring.h"
#include "keyframemodel.hpp"
#include "assets/model/assetparametermodel.hpp"

#include <QDebug>


KeyframeModelList::KeyframeModelList(std::weak_ptr<AssetParameterModel> model, const QModelIndex &index, std::weak_ptr<DocUndoStack> undo_stack)
    : m_model(model)
    , m_undoStack(undo_stack)
    , m_lock(QReadWriteLock::Recursive)
{
    addParameter(index);
    connect(m_parameters.begin()->second.get(), &KeyframeModel::modelChanged, this, &KeyframeModelList::modelChanged);
}

void KeyframeModelList::addParameter(const QModelIndex &index)
{
    std::shared_ptr<KeyframeModel> parameter (new KeyframeModel(m_model, index, m_undoStack));
    m_parameters.insert({index, std::move(parameter)});
}


bool KeyframeModelList::applyOperation(const std::function<bool(std::shared_ptr<KeyframeModel>, Fun&, Fun&)> &op, const QString &undoString)
{
    QWriteLocker locker(&m_lock);
    Q_ASSERT(m_parameters.size() > 0);
    Fun undo = []() { return true; };
    Fun redo = []() { return true; };

    bool res = true;
    for (const auto& param : m_parameters) {
        res = op(param.second, undo, redo);
        if (!res) {
            bool undone = undo();
            Q_ASSERT(undone);
            return res;
        }
    }
    if (res && !undoString.isEmpty()) {
        PUSH_UNDO(undo, redo, undoString);
    }
    return res;
}

bool KeyframeModelList::addKeyframe(GenTime pos, KeyframeType type)
{
    QWriteLocker locker(&m_lock);
    Q_ASSERT(m_parameters.size() > 0);
    bool update = (m_parameters.begin()->second->hasKeyframe(pos) > 0);
    auto op = [pos, type](std::shared_ptr<KeyframeModel> param, Fun &undo, Fun &redo){
        double value = param->getInterpolatedValue(pos);
        return param->addKeyframe(pos, type, value, undo, redo);
    };
    return applyOperation(op, update ? i18n("Change keyframe type") : i18n("Add keyframe"));
}

bool KeyframeModelList::removeKeyframe(GenTime pos)
{
    QWriteLocker locker(&m_lock);
    Q_ASSERT(m_parameters.size() > 0);
    auto op = [pos](std::shared_ptr<KeyframeModel> param, Fun &undo, Fun &redo){
        return param->removeKeyframe(pos, undo, redo);
    };
    return applyOperation(op, i18n("Delete keyframe"));
}

bool KeyframeModelList::removeAllKeyframes()
{
    QWriteLocker locker(&m_lock);
    Q_ASSERT(m_parameters.size() > 0);
    auto op = [](std::shared_ptr<KeyframeModel> param, Fun &undo, Fun &redo){
        return param->removeAllKeyframes(undo, redo);
    };
    return applyOperation(op, i18n("Delete all keyframes"));
}

bool KeyframeModelList::moveKeyframe(GenTime oldPos, GenTime pos, bool logUndo)
{
    QWriteLocker locker(&m_lock);
    Q_ASSERT(m_parameters.size() > 0);
    auto op = [oldPos, pos](std::shared_ptr<KeyframeModel> param, Fun &undo, Fun &redo){
        return param->moveKeyframe(oldPos, pos, undo, redo);
    };
    return applyOperation(op, logUndo ? i18n("Move keyframe") : QString());
}

bool KeyframeModelList::updateKeyframe(GenTime pos, double value)
{
    QWriteLocker locker(&m_lock);
    Q_ASSERT(m_parameters.size() > 0);
    auto op = [value, pos](std::shared_ptr<KeyframeModel> param, Fun &undo, Fun &redo){
        return param->updateKeyframe(pos, value, undo, redo);
    };
    return applyOperation(op, i18n("Update keyframe"));
}


Keyframe KeyframeModelList::getKeyframe(const GenTime &pos, bool *ok) const
{
    READ_LOCK();
    Q_ASSERT(m_parameters.size() > 0);
    return m_parameters.begin()->second->getKeyframe(pos, ok);
}

Keyframe KeyframeModelList::getNextKeyframe(const GenTime &pos, bool *ok) const
{
    READ_LOCK();
    Q_ASSERT(m_parameters.size() > 0);
    return m_parameters.begin()->second->getNextKeyframe(pos, ok);
}

Keyframe KeyframeModelList::getPrevKeyframe(const GenTime &pos, bool *ok) const
{
    READ_LOCK();
    Q_ASSERT(m_parameters.size() > 0);
    return m_parameters.begin()->second->getPrevKeyframe(pos, ok);
}

Keyframe KeyframeModelList::getClosestKeyframe(const GenTime &pos, bool *ok) const
{
    READ_LOCK();
    Q_ASSERT(m_parameters.size() > 0);
    return m_parameters.begin()->second->getClosestKeyframe(pos, ok);
}


bool KeyframeModelList::hasKeyframe(int frame) const
{
    READ_LOCK();
    Q_ASSERT(m_parameters.size() > 0);
    return m_parameters.begin()->second->hasKeyframe(frame);
}

void KeyframeModelList::refresh()
{
    for (const auto& param : m_parameters) {
        param.second->refresh();
    }
}
