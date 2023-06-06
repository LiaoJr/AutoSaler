#include "font.h"
#include "showjpg.h"

//arm-linux-gcc  test.c   -L./   -lfont   -lm 

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
