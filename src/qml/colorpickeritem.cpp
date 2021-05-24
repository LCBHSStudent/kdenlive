/*
 * Copyright (c) 2014-2019 Meltytech, LLC
 * Inspiration: KDENLIVE colorpickerwidget.cpp by Till Theato (root@ttill.de)
 * Inspiration: QColorDialog.cpp
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "colorpickeritem.h"

#include "mainwindow.h"
#include "widgets/colorpicker.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QImage>
#include <QScreen>
#include <QTimer>

ColorPickerItem::ColorPickerItem(QObject* parent)
    : QObject(parent)
{
    connect(this, SIGNAL(pickColor()), this, SLOT(startPick()));
    
}

void ColorPickerItem::startPick() {
    auto picker = new ColorPicker(nullptr);
    
    connect(
        picker, SIGNAL(colorPicked(const QColor&)),
        this, SIGNAL(colorPicked(const QColor &))
    );
    connect(picker, SIGNAL(cancelled()), this, SIGNAL(cancelled()));
}
