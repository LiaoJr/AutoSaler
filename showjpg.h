
#ifndef _SHOWIMG_H_

#define _SHOWIMG_H_

extern int *lcd;

/*图片信息结构体*/
typedef struct imginfo{
    char imgpath[128];   //图片路径
    unsigned int pos_x;
    unsigned int pos_y;
    unsigned int width;  //图片宽度
    unsigned int height; //图片高度
    unsigned int bits;   //图片颜色位深
    char *imgdata;       //图片数据首地址
}imginfo_t;



/*lcd初始化，得到LCD设备内存映射首地址lcd全局变量*/
int lcd_init();  //初始化成功返回0，失败返回-1




/********************************************************************
* 函数名称: showjpg
* 功能描述: 根据屏幕坐标向lcd映射内存中写入图片数据，以显示jpeg图片
* 函数参数: 
*         @fp(type:char *):图片路径
* 返回结果: (type:void)
* 函数作者: 廖杰文
* 编写日期: 2023-06-01
* 函数版本: V1.0
* 注意事项: 无
* 版权声明: 
* ******************************************************************/
void showjpg(imginfo_t *img);
// void showjpg(char *fp, int pos_x, int pos_y);


/********************************************************************
* 函数名称: read_JPEG_file
* 功能描述: 读取jpeg图片信息和数据
* 函数参数: 
*         @filenamee(type:char *):图片路径
* 返回结果: (type:int)成功返回0，失败返回-1；
* 函数作者: libjpeg官方源码修改
* 编写日期: 2023-06-01
* 函数版本: V1.0
* 注意事项: 无
* 版权声明: 
* ******************************************************************/
int read_JPEG_file(imginfo_t *img);

#endif

