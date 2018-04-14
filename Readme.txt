version server:   github, oschina

1 项目简介
 C++代码，用于btctrade.com的机器人。

2 代码目录结构
 2.1）所有代码都存在于Codes目录下。
 2.2）Codes/Include 用于存放功能模块需要被其他模块引用的.h文件。
 2.4）Codes/Src 用于存放功能模块.cpp文件。

        
3 实测数据

    
4 备注


5 History
 ver.0.1:  针对btctrade.com的API介绍, 通过Http Get, 和Md5 + Hmac + Sha256签名的功能验证的代码的存档。
 ver.0.3:  为了使用C++11最新特性, 将项目工具从vs2012改为vs2013。
 ver.0.4:  BuyLowSellHighRobot 需求基本稳定.
 ver.0.5:  完善BuyLowSellHighRobot相关的需求。  
           完成了90%的 BuyLowSellHighRobotSm 状态机的RetreatOrder()处理函数, 并通过了单元测试。
 ver.0.7:  redefined the database structure, and passed the unit test.
 ver.0.8:  finished all coding for state machine, and passed the unit test.
 ver.0.9:  测试通过基本的BtcTradePlatform的下订单, 取消订单, 查询订单的功能。 
           此版本的BtcTradePlatform参照了 The.ACE.Programmers.Guide.chm 16.2章节介绍的模式， 但是做了一定的简化，测试发现， ~BtcTradePlatform() 有逻辑上的漏洞,可能导致crash 或内存泄露。 计划在一个版本中对BtcTradePlatform进行修改。
 ver.1.0:  解决了已知的BtcTradePlatform相关的bug。
 ver.1.1:  基本功能可以运行。  实际测试3小时， 可以按照预定的方式下单。  
           此版本有2个遗留问题需要在下一版本中解决： 
             1 有时BtcTradeMarketPoller所在的线程会挂起， 原因未知。
             2 当前版本在多线程中处理订单状态的查询， 这钟处理办法容易出现nonce expire的错误。 下一个版本将采取批量查询多个订单状态的方式。
 ver.1.5:  单robot的情况下, 功能基本稳定, 进入实测阶段.
 ver.1.6:  新增功能: 1 利用垃圾回收机制, 定期检查被遗漏的币(cancel order时,order完成数量和平台不一致造成的), 并在新的买入订单中将被遗漏的币回收.  2 对order通知做排序处理, 如: 低价卖出的单还没有被通知时,收到了高价卖出的单, 则暂时忽略当前order通知.
 ver.1.7:  实测24小时无故障稳定版本.
 ver.1.8:  1, robot 增加了强制价格上限的配置,  并且修改了浮动价格区间的算法. 2, 完成了同时运行2个robot的测试, 并修改了测试中发现的bug.
 ver.2.0:  除了网络不稳定时可能出现重复下单的bug, 其他功能都已经稳定的版本.
 ver.2.1:  修改网络不稳定时可能出现重复下单的bug, 添加了一部分支持okcoin平台的代码.
 ver.2.2:  1, 生成Windows Service版本可执行程序.  2, 修改了一些导致程序crash的bug.

 ver.2.4:  svn version 1153.  通过当前所有的单元测试.
           svn version 1330.  Windows 版本HedgeRobot, 单Account Group正式开始运行的版本。.

 ver.2.6:  单group, HedgeRobot 稳定版本.

6 手工撤单
    凡是手工撤销的买单或卖单, 统一closing time 为 1577808000, 即"2020-01-01 00:00:00".  Cancel Reson为: CanceledByCmd.

7 ....