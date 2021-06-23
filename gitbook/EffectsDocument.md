# 常用效果文档

本文档为项目中所使用的剪辑素材编辑效果以及其对应的`Mlt::Filter`相关参数说明

* `[]` 表示参数可选
* 参考文档[MLT Documentation]([MLT - Documentation (mltframework.org)](https://www.mltframework.org/docs/))

> 注意：形如producer.xxx transition.xxx的属性对应键名称并不是"xxx"，而是"producer.xxx transition.xxx"

## 一、位置与尺寸

### 1.1、affine (cpu)

| 键名                                 | 类型               | 特殊属性   | 备注                                                         |
| ------------------------------------ | ------------------ | ---------- | ------------------------------------------------------------ |
| background                           | string             |            | 通过创建Mlt::Producer来充当背景图片，一般以颜色为主(`colour:0xrrggbbaa` |
| producer.*                           | Mlt::Properties    |            | 对应`Mlt::Producer`的属性键值对                              |
| use_normalised                       | bool               |            | 当此属性值为`true`时，Mlt::Consumer在请求帧对象时会使用项目预设分辨率(从`Mlt::Profile`)而不是素材自身的分辨率 |
| transition.*                         | Mlt::Properties    |            | 作为Filter的效果affine和作为转场Transition的效果具有相同的属性，同样，这也是使用此效果需要设置的主要属性 |
| **转场(transition.*)<br />属性部分** |                    |            |                                                              |
| **keyed**                            | bool               |            | 标注是否启用mlt::animation，默认为`false`                    |
| rect                                 | rect(格式化string) | 支持关键帧 | 指定了一个矩形的位置和大小，格式为"X/Y:WxH[:不透明度]"<br />其中`X Y W H 不透明度`为主要变参，可以使用确切的数值或百分比，各参数是否使用百分比表示需要保持一致<br />一般使用百分比，例：`0%/0%:100%x100%:100%` |
| distort                              | bool               |            | 判断是否无视视频素材的宽高比来填充显示整个区域               |
| **rotate族**                         |                    |            | 如果keyed设置为false，则表示将旋转值值应用到每一帧           |
| fix_rotate_x                         | float              | 支持关键帧 | 绕坐标系X轴旋转的角度                                        |
| fix_rotate_y                         | float              | 支持关键帧 | 绕坐标系Y轴旋转的角度                                        |
| fix_rotate_z                         | float              | 支持关键帧 | 绕坐标系Z轴旋转的角度                                        |
| **shear族**<br />(平行四边形化)      |                    |            |                                                              |
| fix_shear_x                          | float              | 支持关键帧 | X方向的形变数值 <br />default:0                              |
| fix_shear_y                          | float              | 支持关键帧 | Y方向的形变数值 <br />default:0                              |
| fix_shear_z                          | float              | 支持关键帧 | Z方向的形变数值 <br />default:0                              |
|                                      |                    |            |                                                              |
| halign                               | string             |            | 设定水平对齐的方式，合法取值如下<br />* left<br />* center<br />* right |
| valign                               | string             |            | 设定垂直对齐的方式，合法取值如下<br />* left<br />* center<br />* right |

### 1.2、movit.rect (gpu)



## 二、绿屏抠图

### 1.1、frei0r.select0r (cpu)

> 色彩空间: `Red-Green-Blue=RGB 红色-绿色-蓝色` `Hue-Chroma-Intensity=HCI 色相-饱和度-明度`

| 键名 | 标题                | 类型                       | 特殊属性                           | 备注                                                         |
| ---- | ------------------- | -------------------------- | ---------------------------------- | ------------------------------------------------------------ |
| 0    | 色键颜色            | color <br />(格式化string) |                                    | 选定为色键目标的颜色值，格式为`#RRGGBB`<br />默认值: #00cc00 |
| 1    | 透明区域反转        | bool                       |                                    | 设置是否反转Filter处理后的透明度结果值                       |
| 2    | 红色差值/色相差值   | double                     | 范围[0~1]                          | 参数差值分量0                                                |
| 3    | 绿色差值/饱和度差值 | double                     | 范围[0~1]                          | 参数差值分量1                                                |
| 4    | 蓝色差值/明度差值   | double                     | 范围[0~1]                          | 参数差值分量2                                                |
| 5    | 梯度                | double                     | 范围[0~1]                          | 参数变化的梯度大小                                           |
| 6    | 色彩空间            | double                     | 仅使用0、1                         | 确定使用哪种色彩空间<br />0.0：RGB <br />1.0：HCI            |
| 7    | 形状                | double                     | value ∈ [0.0, 0.5, 1.0]            | 0.0：矩形<br />0.5：椭圆形<br />1.0：菱形                    |
| 8    | 边缘类型            | double                     | value ∈ [0.0, 0.35, 0.6, 0.7, 0.9] | 0.0：硬边<br />0.35：厚边<br />0.6：正常<br />0.7：薄边<br />0.8：梯度 |
| 9    | 操作类型            | double                     | value ∈ [0.0, 0.3, 0.5, 0.7, 1.0]  | 0.0：覆盖<br />0.3：最大值<br />0.5：最小值<br />0.7：相加<br />1.0：相减 |



