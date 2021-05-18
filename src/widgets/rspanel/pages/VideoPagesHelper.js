function selectSizePosition() {
    if (attFilterModel.isProducerSelected) {
        var index = attFilterModel.sizePositionIndex()
        var isNew = false
        if (index < 0) {
            isNew = true
            attFilterModel.add(metadataModel.specifiedFiler("size_position"))

            index = attFilterModel.sizePositionIndex()
        }
        filterCon.setCurrentFilter(index, isNew, Constants.SizePosition)
    }
}

function updateSizePositionData() {
    filter.blockSignals = true
    filter.set(middleValue, Qt.rect(0, 0, profile.width, profile.height))
    filter.set(startValue, Qt.rect(0, 0, profile.width, profile.height))
    filter.set(endValue, Qt.rect(0, 0, profile.width, profile.height))

    // 创建默认rectangle
    if (producer.displayAspectRatio > profile.aspectRatio) {
        defaultRect.width = profile.width
        defaultRect.height = defaultRect.width / producer.displayAspectRatio
    } else {
        defaultRect.height = profile.height
        defaultRect.width = defaultRect.height * producer.displayAspectRatio
    }
    defaultRect.x = Math.round((profile.width - defaultRect.width) / 2)
    defaultRect.y = Math.round((profile.height - defaultRect.height) / 2)

    if (filter.isNew) {
        console.debug("attached new size_position filter")
        if (backgroundProperty)
            filter.set(backgroundProperty, 'color:#00000000')
        filter.set(rectProperty, defaultRect)
    } else {
        console.debug("load former filter arguments")
        if (legacyRectProperty) {
            var old = filter.get(legacyRectProperty)
            if (old && old.length > 0) {
                filter.resetProperty(legacyRectProperty)
                filter.set(rectProperty, old)
            }
        }
        filter.set(middleValue, filter.getRect(rectProperty, filter.animateIn + 1))
        if (filter.animateIn > 0)
            filter.set(startValue, filter.getRect(rectProperty, 0))
        if (filter.animateOut > 0)
            filter.set(endValue, filter.getRect(rectProperty, filter.duration - 1))
        filter.set(rotationMiddleValue, filter.getDouble(rotationProperty, filter.animateIn + 1))
        if (filter.animateIn > 0)
            filter.set(rotationStartValue, filter.getDouble(rotationProperty, 0))
        if (filter.animateOut > 0)
            filter.set(rotationEndValue, filter.getRect(rotationProperty, filter.duration - 1))
    }

    filter.blockSignals = false

    updateAspectRatio()
    // 更新控件值
    setSizePositionControls()
    setKeyframedSizePositionControls()
}

function setSizePositionControls() {
    // // 变形
    // if (filter.get(distortProperty) === '1')
    //     distortRadioButton.checked = true
    // // 否则应用填充模式
    // else if (filter.get(fillProperty) === '1')
    //     fillRadioButton.checked = true
    // else
    //     fitRadioButton.checked = true
    
    // // 旋转中心对齐模式
    // var align = filter.get(halignProperty)
    // if (align === 'left')
    //     leftRadioButton.checked = true
    // else if (align === 'center' || align === 'middle')
    //     centerRadioButton.checked = true
    // else if (align === 'right')
    //     rightRadioButton.checked = true
    // align = filter.get(valignProperty)
    // if (align === 'top')
    //     topRadioButton.checked = true
    // else if (align === 'center' || align === 'middle')
    //     middleRadioButton.checked = true
    // else if (align === 'bottom')
    //     bottomRadioButton.checked = true

    // // 获取填充背景色
    // if (backgroundProperty) {
    //     var s = filter.get(backgroundProperty)
    //     if (s.substring(0, 6) === 'color:')
    //         bgColor.value = s.substring(6)
    //     else if  (s.substring(0, 7) === 'colour:')
    //         bgColor.value = s.substring(7)
    // }
}

function setKeyframedSizePositionControls() {
    var enabled = isSimpleKeyframesActive()
    var position = getPosition()
    var newValue = filter.getRect(rectProperty, position)
    if (filterRect !== newValue) {
        updateAspectRatio()
        if (isFillMode()) {
            // enforce the aspect ratio
            if (producer.displayAspectRatio > profile.aspectRatio) {
                newValue.height = newValue.width / producer.displayAspectRatio
            } else {
                newValue.width = newValue.height * producer.displayAspectRatio
            }
        }
        filterRect = newValue
        xSpin.value = filterRect.x.toFixed()
        ySpin.value = filterRect.y.toFixed()
        wSpin.value = filterRect.width.toFixed()
        hSpin.value = filterRect.height.toFixed()
        blockUpdate = true
        scaleSlider.update()
    }
    positionKeyframesButton.checked = filter.keyframeCount(rectProperty) > 0 && filter.animateIn <= 0 && filter.animateOut <= 0
    if (rotationProperty) {
        blockUpdate = true
        rotationSpin.value = filter.getDouble(rotationProperty, position)
        rotationSpin.enabled = enabled
        rotationKeyframesButton.checked = filter.keyframeCount(rotationProperty) > 0 && filter.animateIn <= 0 && filter.animateOut <= 0
    }
    blockUpdate = false
    xSpin.enabled = enabled
    ySpin.enabled = enabled
    wSpin.enabled = enabled
    hSpin.enabled = enabled
}

function isFitMode() {
    return filter.get(fillProperty) === '0' && filter.get(distortProperty) === '0'
}

function isFillMode() {
    return filter.get(fillProperty) === '1' && filter.get(distortProperty) !== '1'
}

function getPosition() {
    return Math.max(producer.position - (filter.in - producer.in), 0)
}

function setFilter(position) {
    if (position !== null) {
        filter.blockSignals = true
        if (position <= 0 && filter.animateIn > 0)
            filter.set(startValue, filterRect)
        else if (position >= filter.duration - 1 && filter.animateOut > 0)
            filter.set(endValue, filterRect)
        else
            filter.set(middleValue, filterRect)
        filter.blockSignals = false
    }

    if (filter.animateIn > 0 || filter.animateOut > 0) {
        filter.resetProperty(rectProperty)
        positionKeyframesButton.checked = false
        if (filter.animateIn > 0) {
            filter.set(rectProperty, filter.getRect(startValue), 1.0, 0)
            filter.set(rectProperty, filter.getRect(middleValue), 1.0, filter.animateIn - 1)
        }
        if (filter.animateOut > 0) {
            filter.set(rectProperty, filter.getRect(middleValue), 1.0, filter.duration - filter.animateOut)
            filter.set(rectProperty, filter.getRect(endValue), 1.0, filter.duration - 1)
        }
    } else if (!positionKeyframesButton.checked) {
        filter.resetProperty(rectProperty)
        filter.set(rectProperty, filter.getRect(middleValue))
    } else if (position !== null) {
        filter.set(rectProperty, filterRect, 1.0, position)
    }
}

function updateRotation(position) {
    if (blockUpdate) return
    if (position !== null) {
        filter.blockSignals = true
        if (position <= 0 && filter.animateIn > 0) {
            filter.set(rotationStartValue, rotationSpin.value)
        } else if (position >= filter.duration - 1 && filter.animateOut > 0) {
            filter.set(rotationEndValue, rotationSpin.value)
        } else {
            filter.set(rotationMiddleValue, rotationSpin.value)
        }
        filter.blockSignals = false
    }

    if (filter.animateIn > 0 || filter.animateOut > 0) {
        filter.resetProperty(rotationProperty)
        rotationKeyframesButton.checked = false
        if (filter.animateIn > 0) {
            filter.set(rotationProperty, filter.getDouble(rotationStartValue), 0)
            filter.set(rotationProperty, filter.getDouble(rotationMiddleValue), filter.animateIn - 1)
        }
        if (filter.animateOut > 0) {
            filter.set(rotationProperty, filter.getDouble(rotationMiddleValue), filter.duration - filter.animateOut)
            filter.set(rotationProperty, filter.getDouble(rotationEndValue), filter.duration - 1)
        }
    } else if (!rotationKeyframesButton.checked) {
        filter.resetProperty(rotationProperty)
        filter.set(rotationProperty, filter.getDouble(rotationMiddleValue))
    } else if (position !== null) {
        filter.set(rotationProperty, rotationSpin.value, position)
    }
}

function isSimpleKeyframesActive() {
    var position = getPosition()
    return  position <= 0
        || (position >= (filter.animateIn - 1) &&
            position <= (filter.duration - filter.animateOut))
        ||  position >= (filter.duration - 1)
}

function scaleByWidth(value) {
    var centerX = filterRect.x + filterRect.width / 2
    var rightX = filterRect.x + filterRect.width
    filterRect.width = value
    
    // 水平适合
    if (true) {
    //if (centerRadioButton.checked) {
        filterRect.x = xSpin.value = centerX - filterRect.width / 2
    } else if (rightRadioButton.checked) {
        filterRect.x = xSpin.value = rightX - filterRect.width
    }
    var middleY = filterRect.y + filterRect.height / 2
    var bottomY = filterRect.y + filterRect.height
    filterRect.height = hSpin.value = value / Math.max(aspectRatio, 1.0e-6)
    
    // 垂直适合
    if (true) {
    // if (middleRadioButton.checked) {
        filterRect.y = ySpin.value = middleY - filterRect.height / 2
    }
    else if (bottomRadioButton.checked) {
        filterRect.y = ySpin.value = bottomY - filterRect.height
    }
    scaleSlider.update()
    setFilter(getPosition())
}

function scaleByHeight(value) {
    var middleY = filterRect.y + filterRect.height / 2
    var bottomY = filterRect.y + filterRect.height
    filterRect.height = value

    // 垂直适合 
    if (true) {
    // if (middleRadioButton.checked) {
        filterRect.y = ySpin.value = middleY - filterRect.height / 2
    } else if (bottomRadioButton.checked) {
        filterRect.y = ySpin.value = bottomY - filterRect.height
    }
    var centerX = filterRect.x + filterRect.width / 2
    var rightX = filterRect.x + filterRect.width
    filterRect.width = wSpin.value = value * aspectRatio

    // 水平适合
    if (true) {
    // if (centerRadioButton.checked) {
        filterRect.x = xSpin.value = centerX - filterRect.width / 2
    }
    else if (rightRadioButton.checked) {
        filterRect.x = xSpin.value = rightX - filterRect.width
    }
    scaleSlider.update()
    setFilter(getPosition())
}

function updateAspectRatio() {
    if (filter.get(fillProperty) === '1' && filter.get(distortProperty) === '0') {
        aspectRatio = producer.displayAspectRatio
    } else {
        aspectRatio = filterRect.width / Math.max(filterRect.height, 1)
    }
}

function toggleKeyframes(isEnabled, parameter, value) {
    if (isEnabled) {
        blockUpdate = true
        if (filter.animateIn > 0 || filter.animateOut > 0) {
            // Reset all of the simple keyframes.
            resetSimpleKeyframes()
            filter.animateIn = 0
            blockUpdate = false
            filter.animateOut = 0
        } else {
            filter.clearSimpleAnimation(parameter)
            blockUpdate = false
        }
        // Set this keyframe value.
        filter.set(parameter, value, getPosition())
    } else {
        // Remove keyframes and set the parameter.
        filter.resetProperty(parameter)
        filter.set(parameter, value)
    }
}
