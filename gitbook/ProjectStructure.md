# 项目目录结构

```bash
├─cmake
│  └─modules 查找依赖库所用的findPackage cmake文件，勿修改
│
├─data 包含程序所使用的资源文件(图标)，过滤器(filter: mlt_service)的参数配置文件.xml 
│	以及其他的一些配置文件(如KdenliveSettings类的生成文件kdenliveSettings.kcfg)
│		[注——mlt_service中效果的类别主要有：1. effect-效果 2. transition-转场 3. mix-混合 generator-生成器]
│
├─fuzzer 模糊测试器，通过连接到kdenlivelib从而能够使用特定的操作步骤(如mock)模拟项目资源的变化，生成的可执行文件为 fuzz 和 fuzz_reproduce
│      fakeit_standalone.hpp	此独立头文件提供基于c++ template的mock手段，用于生成虚拟对象
│      fuzzing.cpp				|---|
│      fuzzing.hpp				    | 定义和实现了模糊测试内容
│      main_fuzzer.cpp	    依据内容生成测试执行文件fuzz
│      main_reproducer.cpp	依据内容生成测试执行文件fuzz_reproduce
│      
├─gitbook gitbook文档目录
│
├─packaging (打包/安装)(应用程序/依赖)的相关脚本文件
│          
├─plugins 演示了一个通过继承ClipGenerator类并设置参数调用kdenlive renderer，创建mlt-xml文档后生成剪辑素材的软件插件
│
├─renderer 简单封装了mlt-melt的视频渲染器
│ kdenlive_render [-erase] [-kuiserver] [-locale:LOCALE] [in=pos] [out=pos] [render] [profile] [rendermodule] [player] [src] [dest]
│ 	[[arg1] [arg2] ...]
                "  -erase: 渲染结束时删除传入的src文件"
                "  -kuiserver: 使用 KDE 任务追踪器UI组件显示任务状态"
                "  -locale:LOCALE : 设置渲染用的 locale 如： -locale:zh_CN.UTF-8 将使用简体中文 locale (',' 逗号是数值分隔符)"
                "  in=pos: 渲染起始帧号"
                "  out=pos: 渲染终止帧号"
                "  render: melt渲染器可执行程序的位置"
                "  profile: MLT 视频预设格式"
                "  rendermodule: 用于渲染的 MLT consumer，通常为 ffmpeg avformat 编码器"
                "  player: 播放器应用的可执行程序位置, 使用参数 '-' 来禁用渲染完成后自动使用播放器播放"
                "  src: 输入文件 (通常为 MLT XML 或编辑软件项目文件)"
                "  dest: 输出文件"
                "  args: 由空格分割开的 libavformat 参数"
├─src 构建主要target: 编辑器、kdenliveLib所用的源代码文件夹
│          
├─testingArea 连接kdenliveLib，使用项目资源进行测试的程序代码区
│  
├─tests kdenlive原测试样例代码区，包含了测试数据模型、小组件函数(utils)、添加效果(effect)等测试程序源码
│      
└─thumbnailer 依赖于kde框架的生成获取缩略图用的组件，编译生成mltpreview.dll
```



