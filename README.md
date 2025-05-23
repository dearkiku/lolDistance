## 使用方法   
**将编译好的dinput8.dll放到game目录之后开始游戏，然后在游戏中快捷键进行操作**   
**Ctrl + Z 放大 | Ctrl + X 缩小 | Ctrl + S 保存 -> 之后每局都会在开始时自动修改视距**   
--
## **特别注意** 
- 如果你使用本dll那么你将不能直接使用鼠标滚轮来调整视距，因为那样会在部分技能 - > 努努 - 'W' | 烬 - 泽拉斯 - 佐伊 - 'R' |  < - 释放或者释放结束的时候自动缩小视距。   
- 若你想使用滚轮调整视野，你只需要在调整视野的时候按住CTRL键 ！   
- 如果你有能力可以hook视距处代码，然后在程序中判断是否更改，但是我没有这方面知识，并且你需要小心被检测到hook造成封号。     

### 说明
- 修改英雄联盟的视野范围！可实现无限视距~~~~~  
- 纯C++代码 完全开源，不必担心有恶意行为！  
- 如果你想使用R3nzskin.dll换肤那么直接放在同一目录即可在游戏开始时加载，不用使用软件注入；  
- 我不是很想使用英文命名，但是中文有时候编译会报错……

## 配置说明   
[Config]  
DistanceValue=3810.00  
#保存的视距数值  
SKINChangeDll=Skin.dll   
#载入的DLL名（同目录）  
OpenConsole=1   
#开启控制台界面 1=开启  
OffDistance=0    
#关闭视距修改 1=关闭  
OffDllLoad=0   
#关闭DLL载入 1=关闭
--
## 更新历史
**2025-03-31更新**
- 1.添加了很多的注释
- 2.规范化输出调试信息
- 3.解决控制台乱码

**2025-03-30更新**
- 1.适配了新版本，也是使用特征码定位，应该能用一段时间……吧
- 2.CTRL + 鼠标滚轮缩放视距的逻辑更符合游戏习惯（向前/上 靠近英雄/缩小视距）

  
**2024-06-10更新的东西 (我加入了更多的注释，然后修改了一些函数名看起来更炫酷！**  
- 1.我加入了CTRL + 滚轮的方式，可以更快的来调节视距，但是你依旧需要使用CTRL + S 来让下一局默认使用该数据。
- 2.现在不会默认开启控制台，除非你修改配置文件加入'OpenConsole=1'这一条内容
- 3.'SKINChangeDll=R3NZ.dll' 因为其它服务器的反作弊系统，R3NZ已经不再更新，但是如果你有其它DLL可以把他的名字放在这里，如果你不需要这个功能那么只要留空就不会继续这个操作，也可以加入'OffDllLoad=0'。
- 4.有些人只想使用DLL加载的功能而不想开启视距，那么需要加入'OffDistance=1'。
- 5.由于我比较喜欢使用卡牌大师，但是很难切牌，所以我打算加入图色功能进行半自动切牌的操作，但是还没完成，所以暂时搁置（因为我写了另外一款软件，懒得集成进来），如果有人需要我可以加入这个功能！  

这是我接触C++的第一个项目，代码并不规范，但是本着能跑就行的编程理念便不再更新(除非不能用)！  
