
**运行于32位arm平台**


1. 利用链表加载文本中存储的商品信息， 图片的位置，价格，数量。 
2. 把链表中货物信息，利用bmp,jpeg图片显示技术，显示到LCD 屏幕  
3. 用户点击屏幕中的商品，会加入购物车   
4. 购物车功能，在购物车中可以看见用户选择的商品信息。   
5. 点击购物车的结账功能，对应的商品数量会减少，当商品数量为 0 . 则显示已售完。  

功能：并不限制也可以自己设计功能逻辑，尽量符合日常使用的习惯即可！ 

**项目提交时间：下周三 ！**   

设计目标  
实现一个饮料自动售货机系统的交互界面。在此交互界面中，用户可以选择自己想要的商品以及数量，
通过购物车查看购买商品信息，并进行支付结算或取消支付操作。实现了日常生活中自动售货机的基本
功能。 

**功能描述**     
1. 系统启动后，进入主菜单界面。此界面将展示在商品信息文档中读取的商品，可通过翻页查看所有商品信息
用户可以在此界面点击对应的商品位置将想要购买的商品添加到购物车：  
2. 点击购物车图标进入购物车界面，此界面可通过翻页查看购物车信息，点击返回按钮可以返回主界面，点击
结算按钮进入支付结算界面；  
3. 在支付结算界面，用户可以通过翻页查看订单信息(包括商品类别、数量、价格),点击确认支付按钮确认支付，
点击取消支付按钮可以取消并清空购物车，点击返回按钮可以返回购物车界面。  
4. 在主菜单界面点击管理员模式，输入密码正确后进入管理员模式，可以对商品数量进行增补，并更新商品文档信息。


**设计方案**    
1. 使用两个链表来分别存储主菜单展示的商品信息和购物车界面所展示的购物车信息  
2. 使用jpeg库显示商品图片，使用开发板提供的字体显示库来显示界面的相关信息
3. 使用开发板输入事件中的触屏事件来响应用户的触屏操作。通过判断触屏后所获取的坐标值
或滑动屏幕对应的返回值做出不同响应，如购物车商品数量增减，不同界面的跳转等。


**实现过程**    
1. 确认系统框架和设计方案  
2. 编写各个功能模块的代码并测试各个模块的函数接口是否能正常实现响应的功能，具体包括：  
    - 链表相关操作模块，包括链表的增删改查、链表信息写入文档和从文件读取信息加载到链表；  
    - jpeg图片显示接口。使用官方提供的jpeg库，修改官方提供的例程实现对jpeg图片进行解码
      并在LCD屏幕上显示出来，将其封装成接口；  
    - 根据开发板字体显示的例程，封装函数接口，调用后在LCD屏幕上显示文字
    - 编写主界面、购物车界面、结算支付界面的商品图片和信息显示函数
3. 编写售货机系统运行函数，实现正常交互逻辑。每个界面的交互逻辑具体如下：
    - 在循环体中显示界面图片和信息后等待判断用户触屏事件
    - 当判断到发生触屏事件后根据其返回情况判断做出不同响应
4. 在第三步中不断调试实现三个主要界面的功能，最后封装整理代码。


**心得体会**  
这是我第一次使用C语言进行应用层的项目开发。在项目实现过程中，我遇到了许多问题。比如在项目刚开始，
在测试jpeg图片显示接口时遇到了段错误，最后解决这个问题花了大半个晚自习时间。通过这次吸取教训，就是
遇到断错误可以通过在代码里加打印快速定位出错位置，然后对应解决。还有就是如何在每个界面组织代码逻辑
让交互界面趋于合理，还有如何编写和封装函数来降低模块之间的耦合性。总的来说，此次项目让我对链表操作更加熟悉，
组织代码和解决错误的能力得到提高.





