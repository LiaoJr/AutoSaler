/****************************************************************************************
*      @文件名称: font.c
*      @实现功能: gec6818开发板字体显示接口定义
* 
*      @文件作者: 廖杰文
*      @创建日期: 2023-06-06
*      @修改版本: V1.0
*      @注意事项: 无
*      @版权声明: 无
* ***************************************************************************************/


#include "font.h"
#include "showjpg.h"

//arm-linux-gcc  test.c   -L./   -lfont   -lm 

/*用于字体显示
参数定义在font_t类型的结构体中
再将其地址传入*/
void showfont(font_t *myfont)
{
	// 打开字体
	font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");

	// 字体大小的设置
	fontSetSize(f, myfont->size);

    unsigned int r = (myfont->bgcolor & 0xff0000) >> 16;
    unsigned int g = (myfont->bgcolor & 0x00ff00) >> 8;
    unsigned int b = (myfont->bgcolor & 0x0000ff);
    // printf("RGB: %x %x %x\n", r,g,b);
	// 创建一个画板（点阵图）
	bitmap *bm = createBitmapWithInit(myfont->bgw,myfont->bgh,4,getColor(0,b,g,r)); //也可使用createBitmapWithInit函数，改变画板颜色
	// bitmap *bm = createBitmap(200, 100, 4); // 创建一块大小为 200*200 的画板

    unsigned int fr = (myfont->fcolor & 0xff0000) >> 16;
    unsigned int fg = (myfont->fcolor & 0x00ff00) >> 8;
    unsigned int fb = (myfont->fcolor & 0x0000ff);
    // printf("fRGB: %x %x %x\n", fr,fg,fb);
	// 将字体写到点阵图上
	fontPrint(f, bm, 0, 0, myfont->content, getColor(0, fb,fg,fr), myfont->linew);

	// 把字体框输出到LCD屏幕上
	show_font_to_lcd(lcd, myfont->pos_x, myfont->pos_y, bm);

	// 关闭字库
	fontUnload(f);
	// 销毁画板
	destroyBitmap(bm);
}
