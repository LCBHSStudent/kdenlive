# 其他常用操作

## 一、环境配置

### 1.1、配置craft

#### 1.1.1、安装python3.x

[下载python3.x版本](https://www.python.org/downloads/)，安装后配置好环境变量，确保`python`命令能够执行

#### 1.1.2、安装powershell 5.0以上版本

[下载powershell 7](https://github.com/PowerShell/PowerShell/releases/tag/v7.1.3)  
运行`PowerShell-7.1.3-win-x64.msi`

* 安装流程安装完毕后默认情况下，包安装位置为 `盘符:ProgramFiles\PowerShell\<version>`
* 如果安装时没勾选添加安装路径添加到`${PATH}`内，则进行添加
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602104832207.png)  
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602104929223.png)
* 使用`管理员权限`运行powershell，键入以下命令以启用脚本运行

> Set-ExecutionPolicy -Scope CurrentUser RemoteSigned

* 运行`iex ((new-object net.webclient).DownloadString('https://raw.githubusercontent.com/KDE/craft/master/setup/install_craft.ps1'))`

#### 1.1.3、配置编译器

* 下载MinGW64 8.1.0版本(一些版本的Qt自带，可以在`Qt安装目录\Tools\mingw810_64\`目录下找到)
* 添加`Qt安装目录\Tools\mingw810_64\bin`至环境变量

#### 1.1.4、使用方才安装的powershell运行craft.ps1 

> 此步骤中的错误大多为网络问题，注意fetch/get failed等词标志此错误
>
> * 选择craft的安装目录  
>   ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602105617891.png)
> * 选择默认编译器为MinGW(输入0)  
>   ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602105952101.png)

* 随喜好选择，如果选择yes则需要额外下载一些组件
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/2021060211002624.png)  
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602110115185.png)
* 等待下载，安装完成，如果由于网络问题导致软件包下载失败，重新运行`craft.ps1`重复安装流程，此前可以将download包剪切到目录外，删除安装目录(如: C:\CraftRoot\) 
* 选择完安装目录后，将download文件夹重新粘贴进安装目录，可以节省下载时间

#### 1.1.5、安装完成

安装完成后，运行Craft安装目录\craft\craftenv.ps1，出现类似以下信息则说明craft配置完成

> Craft               : A:\CraftRoot  
> Version             : master  
> ABI                 : windows-mingw_64-gcc  
> Download directory  : A:\CraftRoot\download

编辑环境变量:

> PATH=$(PATH);C:\CraftRoot\bin;C:\CraftRoot\dev-utils\bin\;C:\CraftRoot\mingw64\bin  

> 编辑变量 CMAKE_PREFIX_PATH, 添加Craft安装目录 (如: CMAKE_PREFIX_PATH:STRING=%{Qt:QT_INSTALL_PREFIX};C:\CraftRoot)
> ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602112749586.png)

### 1.2、编译kdenlive

* 命令行中输入craft kdenlive (`默认构建模式为DebWithRelInfo，如果想以debug模式构建，则输入craft --buildtype Debug kdenlive`)
* 如果出现由于zlib库导致的编译失败，运行`craft zlib`待安装完成后重试

### 1.3、配置SmartIP-Editor项目

#### 1.3.1、获取源代码

* 运行powershell，cd到该目录下: `craft安装目录\build\kde\kdemultimedia\kdenlive\work\`
* git clone https://github.com/LCBHSStudent/kdenlive kde_based_editor

#### 1.3.2、配置QtCreator

* 点击QtCreator菜单栏`工具`->`选项`打开选项窗口
* 选择Kits栏目，打开`Qt Versions`选项卡，点击添加，选择`Craft安装目录\bin\qmake.exe`  
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602111625341.png)
* 打开`编译器`选项卡，点击添加，分别添加一个C/C++编译器如下，注意ABI的选择  
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602111842340.png)  
  路径为`Craft安装目录\mingw64\bin\gcc(g++).exe`
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602111928617.png)  
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/2021060211195686.png)
* 添加新的调试器  
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602112139789.png)
* 添加新的CMake  
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602112227131.png)
* 最后，添加Qt Kit，选择CMake、c\c++编译器、调试器、Qt版本为刚才手动添加的版本  
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602112357294.png)
* 打开项目文件夹根目录下的CMakeLists.txt，选择编译模式为`ReleaseWithDebugInformation`
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602112452199.png)
* 选择build target为`all`和`install`，编译
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602112749586.png)

### 1.4、部署项目

* 编辑python脚本如下

```python
import shutil

shutil.copy(r'.\bin\SmartIP-Editor.exe', r'A:\CraftRoot\bin') # 改为你的craft安装目录
```

* 修改项目运行设置，添加部署方法运行刚刚的python脚本  
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602113336304.png)

* 添加项目运行配置  
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602113440874.png)

* 修改后的编译运行小窗：

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20210602113519633.png)



## 二、添加文件到项目

### 2.1、传统widget类的 `.UI` 文件和 C++源文件`.h .cpp`

​		本项目中使用到的`.ui`设计师文件皆位于`src/ui`文件夹内，添加传统widget类时最好分开进行添加，先添加`.ui`文件，再添加类文件`.cpp .h`

#### 2.1.1 添加UI

​		cmake项目文件中定义了ui的加载位置和使用moc生成xxx_ui.h的函数

```
81 ## UI's
82 file(GLOB kdenlive_UIS "ui/*.ui")
83 ki18n_wrap_ui(kdenlive_UIS ${kdenlive_UIS})
```

现在需要将ui文件添加到`src/ui`文件夹内

![](.\assets\add_ui_1.png)

选择`Qt Designer Form`

![](.\assets\add_ui_2.png)

按照引导后，确定添加

#### 2.1.2 添加 class 文件

​		右键文件夹，选择Add new - c++ class创建`.cpp 和 .h` 文件，添加到项目和添加到版本控制系统都选择none

​		随后，找到添加的文件夹位置上层的第一个`CMakeLists.txt`文件，比如此处向widget文件夹中添加了新的文件，则编辑`widget`文件夹下的`CMakeLists.txt`

![](.\assets\add_code_1.png)

```bash
set(kdenlive_SRCS
  ${kdenlive_SRCS}
  xxx/xxx.cpp
  xxx/xxx.cpp
  ...
  # 这里另起一行，写你新加入的文件相对路径
  PARENT_SCOPE)
  
```

#### 2.1.3、 类代码引用UI

```c++
// 包含加入的ui生成的头文件，格式为ui_文件名.h
#include "ui_xxxxxx.h"
```

随后，采用继承或成员的方式组合到类中即可

### 2.2、注意事项

​		如果遇到编译失败问题，报出元对象相关错误，请选择`构建-重新构建整个项目`选项

## 三、打包项目

### 3.1、简便方法

