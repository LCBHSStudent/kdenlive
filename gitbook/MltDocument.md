[TOC]

# MLT

​		MLT是为电视广播设计的开源多媒体框架。严格来说，它为使项目包含新的音视频源、 滤镜、场景过渡和播放设备提供了可插拔式的架构。

本框架为使用了MLT的服务或应用程序提供了结构体系与实用功能。

就框架本身而言，它只提供了为管理资源，如内存，属性，动态对象加载和实例化服务的抽象类和实用功能程序。

本文档大致分为三部分。第一部分提供对MLT的基础描述，第二部分展示了它如何被使用，最后一部分则结合了扩展系统的强调提示展示了框架的结构与设计。

## 基本概述
### 基础设计信息
​		MLT使用C语言编写，框架除了C99标准与pthread库外没有别的依赖。它遵循基本的面向对象设计范式，许多设计宽松地基于生产者/消费者设计模式。

框架被设计为不影响色彩空间——然而当前实现的模块非常趋向于8bit YUV422格式，但是理论上，这些模块可以被完全替换掉，一些关于这些术语的粗略解读将贯穿于这篇文档的剩余部分。

### 结构与流程
​		一个MLT ‘网络’ 的总体结构可描述为一个‘生产者’与一个‘消费者’之间的连接:

​																									![在这里插入图片描述](https://img-blog.csdnimg.cn/20200914203615450.png#pic_left)
消费者从生产者处请求MLT帧对象，随后对其进行一些操作，并在完成一帧后将其关闭。

一个常见的对此处使用到的”生产者/消费者“术语的混淆是，消费者也许会’生产‘某些东西。举个例子，libdv消费者生产DV并且libdv生产者似乎会去消耗DV。然而，此处的命名约定仅表示MLT 帧对象的生产者与消费者。

换言之——`一个生产者生产MLT帧对象，并且一个消费者将消耗MLT帧对象`。

一个MLT帧本质上提供一张未压缩的，且与音频样本相关联的图片。

`滤镜(Filter)` 也可以被放置于生产者与消费者之间：

​																							![在这里插入图片描述](https://img-blog.csdnimg.cn/20200914204616211.png#pic_left)
一个'服务'是一组生产者、滤镜、消费者的集合名称。

连接起来的生产者与消费者或服务之间的交流将执行三个阶段：
* 获取帧
* 获取图像
* 获取音频  

MLT采用‘懒加载’——图像与音频不需要从源中解压，直到获取图片与音频数据的函数被调用。

实质上，消费者从其所连接的模块中获取资源——这意味着线程通常属于一个消费者需要实现的领域，并且在消费者类上提供一些基本方法以确保实时吞吐量。

### 使用说明
#### Hello World
在我们进入框架架构细节之前，下面提供了一个有效的使用例。

以下部分简单的提供了一个媒体播放器：
```c
#include <stdio.h>
#include <unistd.h>
#include <framework/mlt.h>

int main( int argc, char *argv[] )
{
    // Initialise the factory
    if ( mlt_factory_init( NULL ) == 0 )
    {
        // Create the default consumer
        mlt_consumer hello = mlt_factory_consumer( NULL, NULL );

        // Create via the default producer
        mlt_producer world = mlt_factory_producer( NULL, argv[ 1 ] );

        // Connect the producer to the consumer
        mlt_consumer_connect( hello, mlt_producer_service( world ) );

        // Start the consumer
        mlt_consumer_start( hello );

        // Wait for the consumer to terminate
        while( !mlt_consumer_is_stopped( hello ) )
            sleep( 1 );

        // Close the consumer
        mlt_consumer_close( hello );

        // Close the producer
        mlt_producer_close( world );

        // Close the factory
        mlt_factory_close( );
    }
    else
    {
        // Report an error during initialisation
        fprintf( stderr, "Unable to locate factory modules\n" );
    }

    // End of program
    return 0;
}
```
这是一个简单的例子——它不提供任何查找功能或运行时配置设定。

任何MLT应用程序的第一步都是工厂的初始化——这保证了环境配置与MLT能够正常运行。下面是对工厂的细节介绍。

如上例的mlt_factory_consumer与mlt_factory_producer调用，所有的服务都通过工厂实例化。对于滤镜和过渡效果也有类似的工厂。在services.txt中包括了标准服务的详细信息。

此处要求的默认值是一个特殊情况——NULL使用请求代表使用默认的生产者与消费者。

默认生产者是“加载器(loader)”。此生产者通过匹配文件名来定位要使用的服务，并且附加‘标准化滤镜’(如缩放、去交错、重采样和字段标准化器) 到加载的内容当中——这些滤镜保证了消费者得到它所寻求的结果。(Frame?)

默认的消费者是“sdl”。加载器与sdl的组合将提供一个媒体播放器。

在这个例子当中，我们连接生产者并随后启动消费者。我们接下来等待直到消费者运行停止 (此例子中指关闭SDL_Window这一动作) 并在应用程序退出前最终关闭消费者、生产者与工厂。

注意，消费者是线程化(异步?)的——在启动消费者之后以及停止或关闭消费者之前，总是需要等待一些种类的事件。

另外也请注意。你可以重载默认值如下：
`MLT_CONSUMER=xml ./hello file.avi`
这将在标准输出上创建一个xml文档
`MLT_CONSUMER=xml MLT_PRODUCER=avformat ./hello file.avi`
这将会直接使用avformat生产者播放视频，因此他将避开标准化方法。
`MLT_CONSUMER=libdv ./hello file.avi > /dev/dv1394`
如果您足够幸运，可以随手将`file.avi`实时转换为DV格式，并将其广播到您的DV设备中。
#### 工厂
正如'Hello World'例子中所展示的那样，工厂可以创建服务对象。

框架本身不提供服务——服务以插件的形式被提供。插件以"模块"的形式组织，并且一个模块可以提供许多不同种类的服务。

一旦工厂被初始化，所有配置好的服务就都可以被使用。

`mlt_factory_prefix()`返回安装各模块的目录路径，这可以被明确表示在`mlt_factory_init`调用其本身种，或它可以通过环境变量`MLT_REPOSITORY`明确表示，亦或者在这两者都缺乏配置的情况下，它将默认返回安装了`prefix/shared/mlt/modules`的路径。

`mlt_environment()`提供与如下表格中所示的`名称=值`集合的只读连接：
| 名称              | 描述             | 值             |
| ----------------- | ---------------- | -------------- |
| MLT_NORMALISATION | 系统的标准化     | PAL或NTSC      |
| MLT_PRODUCER      | 默认生产者       | “loader”或其他 |
| MLT_CONSUMER      | 默认消费者       | "sdl"或其他    |
| MLT_TEST_CARD     | 默认检测卡生产者 | 任何生产者     |
这些值将从同名的环境变量中初始化。

如上方所展示的，一个生产者可以用"默认标准化"生产者来创建，并且他们也可以被使用名字来请求。滤镜和过渡总是被用名字来请求——此处没有它们的'默认'概念。
#### Service的属性
 所有的服务都拥有它们能够用来操纵影响它们行为的属性集合。

为了在服务上设置属性，我们需要检索与他相联系的属性。对生产者来说，这是被调用以下方法完成的：

```c
mlt_properties properties = mlt_producer_properties(producer);
```
所有的服务都有一个相似的关系方法。
一旦完成了数据检索，设置与获取属性就能够直接在这个对象上操作完成，举个例子：
```c
mlt_properties_set(properties, "name", "value");
```
更多关于属性对象的完备的描述可以在下方找到。
#### 播放列表
到目前为止，我们已经展示了一个简单的生产者/消费者配置——下一阶段是要将生产者组织到播放列表当中。

假设我们正在改写"Hello World"样例，并且希望添加一系列文件到播放队列，即：

`hello*.avi`

我们将创建一个新的名为`create_playlist`的函数而不是直接调用`mlt_factory_producer`。此函数负责创建播放列表，创建每一个生产者并将它们添加到播放列表当中。
```c
mlt_producer create_playlist( int argc, char **argv )
{
    // We're creating a playlist here
    mlt_playlist playlist = mlt_playlist_init( );

    // We need the playlist properties to ensure clean up
    mlt_properties properties = mlt_playlist_properties( playlist );

    // Loop through each of the arguments
    int i = 0;
    for ( i = 1; i < argc; i ++ )
    {
        // Create the producer
        mlt_producer producer = mlt_factory_producer( NULL, argv[ i ] );

        // Add it to the playlist
        mlt_playlist_append( playlist, producer );

        // Close the producer (see below)
        mlt_producer_close( producer );
    }

    // Return the playlist as a producer
    return mlt_playlist_producer( playlist );
}
```

注意到我们在添加生产者到播放列表后就关闭了它们，实际上，我们所做的是关闭我们对此生产者的引用——播放列表创建了属于播放列表自身的对生产者的引用并将其插入，并且当播放列表被销毁时，它会关闭自身对生产者的引用。

还要注意，如果添加同一生产者的多个实例到播放列表，它将创建对其的多个引用。

现在我们所要做的是替换主函数中的这一行：
```c
// Create a normalised producer
        mlt_producer world = mlt_factory_prodi
```
为
```c
// Create a normalised producer
		mlt_producer world = create_playlist(argc, argv);
```
然后我们便有方法去播放复数片段。

[*] 此处的引用设计在MLT 0.1.2中有介绍——它100%适用于早期的注册引用与销毁播放列表对象的属性。
#### 滤镜
在生产者和消费者之间插入滤镜只是对其的一种实例化，第一步滤镜连接到生产者，再将滤镜连接到消费者。

举个例子：
```c
// Create a producer from something
mlt_producer producer = mlt_factory_producer( ... );

// Create a consumer from something
mlt_consumer consumer = mlt_factory_consumer( ... );

// Create a greyscale filter
mlt_filter filter = mlt_factory_filter( "greyscale", NULL );

// Connect the filter to the producer
mlt_filter_connect( filter, mlt_producer_service( producer ), 0 );

// Connect the consumer to filter
mlt_consumer_connect( consumer, mlt_filter_service( filter ) );
```
与生产者和消费者一样，滤镜也能被通过修改它的属性对象操作——`mlt_filter_properties`方法能够被调用并且属性可以按要求进行设定。

滤镜连接函数中的附加参数很重要，因为它规定了滤镜运作的'轨迹'。对基础的生产者与播放列表，它们只有一个轨迹(0)，正如您将在下一节看到的，即使有多个轨迹也只有单一的能够产生输出。
#### 为Service附加滤镜
所有的服务都可以拥有附加滤镜。

考虑下面的例子：
```c
// Create a producer
    mlt_producer producer = mlt_factory_producer( NULL, clip );

    // Get the service object of the producer
    mlt_producer service = mlt_producer_service( producer );

    // Create a filter
    mlt_filter filter = mlt_factory_filter( "greyscale" );

    // Create a playlist
    mlt_playlist playlist = mlt_playlist_init( );

    // Attach the filter to the producer
    mlt_service_attach( producer, filter );

    // Construct a playlist with various cuts from the producer
    mlt_playlist_append_io( producer, 0, 99 );
    mlt_playlist_append_io( producer, 450, 499 );
    mlt_playlist_append_io( producer, 200, 399 );

    // We can close the producer and filter now
    mlt_producer_close( producer );
    mlt_filter_close( filter );
```
当播放结束时，灰度缩放滤镜将会对播放列表中来自特点生产者的每一帧执行处理。

此外，每个剪辑都可以拥有它们自身的附加滤镜，这些滤镜将在生产者的滤镜后执行。举个例子：
```c
// Create a new filter
filter = mlt_factory_filter( "invert", NULL );

// Get the second 'clip' in the playlist
producer = mlt_playlist_get_clip( 1 );

// Get the service object of the clip
service = mlt_producer_service( producer );

// Attach the filter
mlt_service_attach( producer, filter );

// Close the filter
mlt_filter_close( filter );
```
甚至播放列表本身也可以附加滤镜
```c
// Create a new filter
filter = mlt_factory_filter( "watermark", "+Hello.txt" );

// Get the service object of the playlist
service = mlt_playlist_service( playlist );

// Attach the filter
mlt_service_attach( service, filter );

// Close the filter
mlt_filter_close( filter );
```
当然，播放列表作为生产者，可以被切分并放置在另一个播放列表上，并且滤镜可以被添加到这些切分或新的播放列表本身上。

附加滤镜的主要优势是它们能够保持连接并且不会遭受插入项目 和计算替换出入点等维护问题的影响——如果您在多轨道领域内大量使用插入分离式的滤镜，这将会成为一个主要问题。
#### 混合剪辑
混合是最简单的介绍播放列表中相邻的两个剪辑间过渡的方式  
考虑存在下面的播放列表：
![在这里插入图片描述](https://img-blog.csdnimg.cn/20201028104013229.png#pic_center)
让我们假设'X'是由50帧长度的黑幕组成  

当播放时，将会获得50帧黑幕，然后开始播放A，紧接着切换到播放B，最终又播放黑幕'X'  

我们那的目的是将播放列表转换为像这样的模式：
![在这里插入图片描述](https://img-blog.csdnimg.cn/20201028104320434.png#pic_center)
当一块剪辑中指向两个剪辑时会展示一个过渡，注意这个播放列表会较上一个短一些，这也是我们所期望的：再两个剪辑中添加一个简单的50帧过渡来代替直接播放50帧黑幕。

这一工作使用`mlt_playlist_mix`方法来完成，因此，假设你有一个像原图中那样的播放列表，在做第一个混合片段时，你可以这样做：
```c
// 创建一个过渡
mlt_transition transition = mlt_factor_transition( "luma", NULL );

// 将第一个和第二个剪辑混合50帧长度
mlt_playlist_mix( playlist, 0, 50, transition );

// 关闭过渡
mlt_transition_close( transition );
```
这将会提供给你第一个过渡（转场）注意这将会在播放列表中创建一个新的剪辑。

作为一般提示，为了获取下一个剪辑索引的这一需求，你需要仿照以下的例子：
```c
// 获得播放列表中的剪辑数量
int i = mlt_playlist_count( );

// 通过逆序迭代它们
while ( i -- )
{
    // 开启转场
    mlt_transition transition = mlt_factor_transition( "luma", NULL );

    // 混合第1，2个剪辑的各50帧
    mlt_playlist_mix( playlist, i, 50, transition );

    // 关闭转场
    mlt_transition_close( transition );
}
```
这是另一种像在新创建的剪辑和当前剪辑间使用`mlt_playlist_join`的方式（你可以通过比较调用mix命令前后播放列表的长度变化来确定新的剪辑是否被创建）

内部的，`mlt_playlist_mix`调用生成了像下方所描述那样的多轨道。如同附加滤镜，混合操作使插入剪辑到列表中变得非常简单。

同样需要注意：混合使用允许一个简单的用户接口 - 不同于强制使用复杂的多轨道对象，你可以在单个轨道上进行许多操作。因此，额外的轨道可以提供给音频混录、复合等，混合或合成等被独立定位的操作而不会影响到其他轨道。

#### 多轨道与过渡
MLT框架处理多轨道时需要受到两个约束：
1. 消费者和生产者需要通过一个框架相互通信
2. 能够序列化和操作一个‘网络’(或者说滤镜图)

我们可以可视化一个多轨道模型如下：
![在这里插入图片描述](https://img-blog.csdnimg.cn/20201028145507576.png#pic_center)
轨道0和轨道1的重合区域可能会又某种转场 - 没有转场时，从b1到a2中的帧将在重叠区域中显示（由高到低图层叠加顺序）

MLT有一个多轨道对象，但它并不是生产者，因为它可以直接连接到使用者，并且消费者会像对待普通生产者一样对待它。在上图的多轨道模型种，除了剪辑之间的过渡外，从轨道1种看不到任何a1-a2相关的东西

消费者从与其连接的生产者处拉出一帧，而多轨道模型种的每个轨道提供一帧，必须在某处确保能够拉出每一条轨道中的帧，并选择正确的方式进行传递。

因此，MLT为多轨道模型提供了封装（称为‘tractor’轨道头、牵引头）牵引头会保证输出正确的帧以及保证所具有的类似生产者行为。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20201028150128461.png#pic_center)
有了牵引头和多轨道模型的连接，现在我们可以将多轨道连接到消费者。

轨道可以是生产者，播放列表，甚至其他的牵引头。

现在我们希望在牵引头和多轨道模型种插入滤镜和转场，我们可以直接使用插入滤镜在二者中间完成这一工作，但是这涉及到左右生产者和消费者的连接和重连，似乎只有我们令这一进程自动化后才显得很恰当。

新的概念‘field’场地诞生了，我们在场地种‘种植’滤镜和转场，牵引头拉动多轨道模型经过场地并生产出一帧。
![在这里插入图片描述](https://img-blog.csdnimg.cn/20201028150900467.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzM1NTg3NDYz,size_16,color_FFFFFF,t_70#pic_center)
这样，我们需要先创建牵引头，然后跟上我们获得的多轨道模型和场地对象。我们能够为他们输入数据并最终将牵引头连接到一个消费者

本质上来说，它在消费者看起来是这样的：
![在这里插入图片描述](https://img-blog.csdnimg.cn/20201028151322153.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzM1NTg3NDYz,size_16,color_FFFFFF,t_70#pic_center)
一个例子将帮助我们如愿以偿地弄清楚它。

让我们假设要提供‘水印’到我们的‘hello world’例子当中，我们已经扩展了例子到能够播放多个剪辑，现在我们将放置一个基于文本的水印，于左上角写着‘Hello World’：
```c
mlt_producer create_tracks( int argc, char **argv )
{
    // 创建牵引头
    mlt_tractor tractor = mlt_tractor_new( );

    // 创建场地对象
    mlt_field field = mlt_tractor_field( tractor );

    // 创建多轨道对象
    mlt_multitrack multitrack = mlt_tractor_multitrack( tractor );

    // 创建一个复合转场
    mlt_transition transition = mlt_factory_transition( "composite", "10%/10%:15%x15%" );

    // 创建轨道0
    mlt_producer track0 = create_playlist( argc, argv );

    // 创建水印轨道1
    mlt_producer track1 = mlt_factory_producer( "loader", "pango" );

    // 获得轨道0的长度
    mlt_position length = mlt_producer_get_playtime( track0 );

    // 设置轨道1的属性
    mlt_properties properties = mlt_producer_properties( track1 );
    mlt_properties_set( properties, "text", "Hello\nWorld" );
    mlt_properties_set_position( properties, "in", 0 );
    mlt_properties_set_position( properties, "out", length - 1 );
    mlt_properties_set_position( properties, "length", length );
    mlt_properties_set_int( properties, "a_track", 0 );
    mlt_properties_set_int( properties, "b_track", 1 );

    // 现在设置转场的属性
    properties = mlt_transition_properties( transition );
    mlt_properties_set_position( properties, "in", 0 );
    mlt_properties_set_position( properties, "out", length - 1 );

    // 添加我们的轨道到多轨道模型种
    mlt_multitrack_connect( multitrack, track0, 0 );
    mlt_multitrack_connect( multitrack, track1, 1 );

    // 种植滤镜到场地
    mlt_field_plant_transition( field, transition, 0, 1 );

    // 关闭引用
    mlt_producer_close( track0 );
    mlt_producer_close( track1 );
    mlt_transition_close( transition );

    // 返回牵引头
    return mlt_tractor_producer( tractor );
}
```
现在我们所要做的就是替换主函数中的这些行
```c
// 创建播放列表
mlt_producer world = create_playlist(argc, argv);
```
用以下内容替换：
```c
// 创建带水印的播放列表
mlt_producer world = create_tracks(argc, argv);
```
顺便一提，在生产者与消费者之间插入更简单的水印滤镜也可以实现相同的目的。
