
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/input.h>


/*检测是否发生屏幕触碰
返回0为点触，1为上滑，2为下滑*/
int touchpanel(int *tx, int *ty)
{
    int fd = open("/dev/input/event0", O_RDWR);  //打开屏幕输入节点文件
    if(fd < 0){
        perror("file open failed.");
        return -1;
    }

    struct input_event buf;
    int ret = 0;
    int lx, ly;

    /*循环检测屏幕是否有被点击*/
    while(1){
        read(fd, &buf, sizeof(buf));

        if(buf.type == EV_ABS && buf.code == ABS_X){
            lx = buf.value * 800 / 1024;
        }
        if(buf.type == EV_ABS && buf.code == ABS_Y){
            ly = buf.value * 480 / 600;
        }

        if(buf.type == EV_KEY && buf.code == BTN_TOUCH)  //当检测到触屏或离屏事件
        {
            /*如果是触屏*/
            if(buf.value == 1){
                printf(">>>>>>>>>>>>>>>>>>>>touch\n");
                *tx = lx;
                *ty = ly;
            }
            /*如果是离屏*/
            if(buf.value == 0){
                printf(">>>>>>>>>>>>>>>>>>>>leave\n");
                break;
            }
        }
    }
    printf("when touch>>> tx = %d, ty = %d\n", *tx, *ty);
    printf("when leave>>> lx = %d, ly = %d\n", lx, ly);
    if(*tx == lx && *ty == ly){
        printf(">>>>>>>>>>  touch   >>>>>>>>>>\n");
        ret = 0;
    }
    if(ly > *ty){
        printf(">>>>>>>>>>slide down>>>>>>>>>>\n");
        ret = 2;
    }
    if(ly < *ty){
        printf(">>>>>>>>>>slide  up >>>>>>>>>>\n");
        ret = 1;
    }
    return ret;
}
