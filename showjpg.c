

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include "hjpeg/jpeglib.h"
#include "showjpg.h"


extern unsigned char *image_buffer; /* Points to large array of R,G,B-order data */
extern int image_height;      /* Number of rows in image */
extern int image_width;       /* Number of columns in image */


struct my_error_mgr{
	struct jpeg_error_mgr pub; /* "public" fields */

	jmp_buf setjmp_buffer; /* for return to caller */
};


typedef struct my_error_mgr *my_error_ptr;


static void my_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr)cinfo->err;

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message)(cinfo);

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}


/********************************************************************
* 函数名称: read_JPEG_file
* 功能描述: 读取jpeg图片信息和数据，存放在参数img指向的结构体中
* 函数参数:
*         @filenamee(type:char *):图片路径
* 返回结果: (type:int)成功返回0，失败返回-1；
* 函数作者: libjpeg官方源码修改
* 编写日期: 2023-06-01
* 函数版本: V1.0
* 注意事项: 无
* 版权声明:
* ******************************************************************/
int read_JPEG_file(imginfo_t *img)
{
	/*定义解压结构体，此结构体里包含一些解压参数和指向工作空间的指针(这些指针是因JPEG库所需而分配的)*/
	struct jpeg_decompress_struct cinfo;

	/*使用扩展的错误处理结构体
	该结构体声明周期与JPEG参数结构体一样，以防止野指针问题*/
	struct my_error_mgr jerr;

	FILE *infile;  //源文件    /* source file */
	// JSAMPARRAY buffer;  // /* Output row buffer */
	// unsigned char **buffer;

	int row_stride;    /* physical row width in output buffer */

	/*打开要读取的图片文件*/
	if((infile = fopen(img->imgpath, "rb")) == NULL){
		fprintf(stderr, "can't open %s\n", img->imgpath);
		return -1;
	}

	/*第一步：分配和初始化JPEG解压的相关对象(结构体)*/

	/*设置错误信息*/
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	/*如果检测到错误信息*/
	if(setjmp(jerr.setjmp_buffer)){
		jpeg_destroy_decompress(&cinfo);  //释放JPEG解压结构体
		fclose(infile);  //关闭图片文件
		return -1;
	}

	/*初始化解压结构体cinfo*/
	jpeg_create_decompress(&cinfo);

	/*第二步：指定源数据*/

	jpeg_stdio_src(&cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */
	/*第三步：使用jpeg_read_header读取文件参数*/
	(void)jpeg_read_header(&cinfo, TRUE);  //这里可以忽略返回值


	/* Step 4: set parameters for decompression */
	/*第四步：设置解压参数*/
	/*此例程中，我们按找jpeg_read_header()中的默认设置就行*/


	/* Step 5: Start decompressor */
	/*第五步：开始解压*/

	(void)jpeg_start_decompress(&cinfo);

	// printf("file path: %s\n", img->imgpath);
	// printf(">>>>image width: %d\n", cinfo.image_width);
	// printf(">>>>image height: %d\n", cinfo.image_height);
	// printf(">>>>colormap: %d\n\n", cinfo.output_components);

	unsigned int w = cinfo.image_width;
	unsigned int h = cinfo.image_height;
	unsigned int bits = cinfo.output_components;

	JSAMPARRAY buffer; /* Output row buffer */// unsigned char** buffer;

	row_stride = w * bits;  //得到每行的数据点数量

	/*前面定义的char **buffer，用于存取读到的数据，将在后面遍历获取*/
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	char rgb[w*h*bits];

	/*这个循环就是读取解压到的数据*/
	while(cinfo.output_scanline < cinfo.output_height){
		unsigned int curline = cinfo.output_scanline;

		(void)jpeg_read_scanlines(&cinfo, buffer, 1);  //一行一行的读, 读进buffer[height][width]
		memcpy(rgb + w * bits * curline, (*(buffer)), row_stride);  //将解析到的每行数据存到rgb数组中
	}

	char *bgra = (char *)calloc(1, w * h * (bits + 1));
	for(int i = 0;i < w * h;i++){
		bgra[i * (bits+1) + 0] = rgb[bits * i + 2];
		bgra[i * (bits+1) + 1] = rgb[bits * i + 1];
		bgra[i * (bits+1) + 2] = rgb[bits * i + 0];
		bgra[i * (bits+1) + 3] = 0;
	}

	/*第七步：完成解压*/
	(void)jpeg_finish_decompress(&cinfo);

	/*第八步：释放解压结构体cinfo*/
	jpeg_destroy_decompress(&cinfo);

	/*关闭图片文件流*/
	fclose(infile);

	//====================================================
	// int fd = open("/dev/fb0", O_RDWR);
	// if(fd<0){
	//   perror("lcd open failed.");
	//   return -1;
	// }
	// write(fd, argb, w*h*(bits+1));  //图像显示正常，说明argb数据数据正常
	// close(fd);
	//=====================================================

	/*保存图片信息数据*/
	img->width = w;
	img->height = h;
	img->bits = bits;
	img->imgdata = bgra;

	return 0;  //返回读取到的图片数据的首地址(一段堆内存)
}

/*lcd设备初始化， 得到映射内存的首地址lcd全局变量*/
int lcd_init()
{
	/*open LCD device*/
	int fd = open("/dev/fb0", O_RDWR);
	if(fd < 0){
		perror("file open failed.");
		return -1;
	}

	/*memory mapping for LCD*/
	lcd = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(lcd == MAP_FAILED){
		perror("mmap failed.");
		return -1;
	}
	else{
		printf("mmap success.\n");
	}

	/*关闭设备*/
	close(fd);

	return 0;
}


/********************************************************************
* 函数名称: showjpg
* 功能描述: 根据屏幕坐标显示jpeg图片
* 函数参数:
*         @fp(type:char *):图片路径
* 返回结果: (type:void)
* 函数作者: 廖杰文
* 编写日期: 2023-06-01
* 函数版本: V1.0
* 注意事项: 无
* 版权声明:
* ******************************************************************/
// void showjpg(char *fp, int pos_x, int pos_y)
void showjpg(imginfo_t *img)
{
	//imginfo_t img;
	int ret = read_JPEG_file(img);  //读取jpg图片数据并存放在img结构体中
	if(ret != 0){
		perror("read JPEG file failed.");
		return;
	}

	int *ImgData = (int *)(img->imgdata);

	for(int y = 0;y < img->height;y++){
		for(int x = 0;x < img->width;x++){
			if((img->pos_y + y) < 480 && (img->pos_x + x) < 800){  //范围控制，方式内存溢出段错误
				*(lcd + (img->pos_y + y) * 800 + img->pos_x + x) = *(ImgData + y * img->width + x);
			}
		}
	}
	printf("show jpeg %s success.\n", img->imgpath);

	/*图片显示完后释放存放图片数据所存储的堆内存*/
	free(img->imgdata);
}

