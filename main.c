
/*
未打包库前编译命令
arm-linux-gcc main.c list.c showjpg.c touch.c font.c saler.c -L./lib -ljpeg -lfont -lm -o main
*/

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdbool.h>

#include "list.h"
#include "showjpg.h"
#include "touch.h"
#include "font.h"
#include "saler.h"




int *lcd = NULL;  //全局变量，lcd设备内存映射首地址
unsigned int *origincount;
int mpage = 1;


void RunSaler()
{
    /*从商品信息文档中读取现有商品信息，并将*/
    char fp[256] = "product.txt";
    man_t *pManager = ProductRead(fp);

    /*加载完商品后记下每种商品的初始数量*/
    origincount = (unsigned int *)calloc(pManager->num, sizeof(unsigned int));
    node_t *nodecount = pManager->head;
    int n = 0;
    while(nodecount != NULL){
        *(origincount + n) = nodecount->pd.count;
        // printf("origincount[n] = %u\n", *(origincount+n));
        n++;
        nodecount = nodecount->next;
    }

    showmenu(pManager, mpage);  //显示主界面

    /*创建一个购物车链表*/
    cman_t *pCart = CListCreat();

    int tx,ty;
    int ReTouch;
    while(1)
    {
        ListPrint(pManager);
        CListPrint(pCart);
        // int mpage = 1;
        ReTouch = touchpanel(&tx, &ty);  //等待触屏事件，检测到才往下响应

        //检测到触屏事件
        if(ReTouch == 0){
            node_t *NodeTouch = pManager->head;
            for(int i=0;i<(mpage-1)*6;i++){
                NodeTouch = NodeTouch->next;
            }
            //----->这时得到该页的起始结点
            int judgetimes = 0;
            while(NodeTouch != NULL){
                unsigned int ID, pos_x, pos_y, w, h, price;
                ID = NodeTouch->pd.ID;
                price = NodeTouch->pd.price;
                pos_x = NodeTouch->pd.pos_x;
                pos_y = NodeTouch->pd.pos_y;
                w = NodeTouch->pd.w;
                h = NodeTouch->pd.h;
                if(tx>pos_x && tx<(pos_x+w) && ty>pos_y && ty<(pos_y+h)){
                    printf(">>>>>>>>>>点到商品: %s\n", NodeTouch->pd.name);
                    break;
                }
                NodeTouch = NodeTouch->next;
                judgetimes++;
                if(judgetimes == 6){
                    NodeTouch = NULL;
                    break;
                }
            }

            //如果没点到商品位置，则判定是否点到了购物车按钮
            if(NULL == NodeTouch){  
                /*如果点击到了购物车按钮*/
                if(CHECKTOUCHPOS(tx, ty, 670, 400, 80, 80)){
                    printf(">>>>>>>>>>>>>>>>>点到购物车\n");
                    showcart(pCart, pManager);  //进入购物车界面
                    continue;
                }
                else if(CHECKTOUCHPOS(tx, ty, 600, 0, 200, 40)){
                    printf("进入管理员模式\n");
                    font_t fontmgr = {"尚未开发!", 100, 200, 150, 0xffffff, 400, 100, 0xae86fd, 500};
                    showfont(&fontmgr);
                    sleep(1);
                    showmenu(pManager, mpage);
                    // showmanager(pManager);
                    continue;
                }
                else{
                    printf(">>>>>>>>>>>>>>>>无效点击\n");
                    continue;
                }
            }

            //----->这里得到了点击到的商品结点NodeTouch
            if(NodeTouch->pd.count == 0){
                font_t ff = {"该商品已售罄", 60, NodeTouch->pd.pos_x, NodeTouch->pd.pos_y, 0xffffff, 200, 130, 0xff0000, 180};
                showfont(&ff);
                continue;
            }

            /*如果点到了商品位置，则将该商品添加到购物车*/
            cnode_t *NodeBuy;
            NodeBuy = CListFind(pCart, NodeTouch->pd.ID);
            if(NULL == NodeBuy){  //如果在购物车链表中没有找到
                cdata_t cdatatmp;
                cdatatmp.ID = NodeTouch->pd.ID;
                strcpy(cdatatmp.name, NodeTouch->pd.name);
                cdatatmp.price = NodeTouch->pd.price;
                cdatatmp.count = 1;  //在购物车链表新添加结点时数量必定先设为1
                CListTailInsert(pCart, &cdatatmp);
            }
            else{
                NodeBuy->cd.count++;  //购物车链表中该商品数量加1
            }
            NodeTouch->pd.count--;  //点击到的商品数量减1

            /*展示商品添加购物车成功页面*/
            NodeBuy = pCart->head;
            while(NodeBuy != NULL){
                if(NodeBuy->cd.ID == NodeTouch->pd.ID){
                    break;
                }
                NodeBuy = NodeBuy->next;
            }
            showaddpd(NodeTouch->pd.name, NodeBuy->cd.count, pManager);
            /*======================*/
        }

        /*如果是在主菜单上滑*/
        if(ReTouch == 1){
            if(mpage*6<pManager->num){
                mpage++;
            }
            else{
                /*提示用户已经在最后一页啦*/
                font_t fontbot = {"已经在最后一页啦!", 80, 100, 0, 0xffffff, 600, 80, 0x998800, 600};
                showfont(&fontbot);
            }
            showmenu(pManager, mpage);
        }

        /*如果是在主菜单下滑*/
        if(ReTouch == 2){
            if(mpage>1){
                mpage--;
            }
            else{
                /*提示用户已经在第一页啦*/
                font_t fonttop = {"已经在第一页啦!", 80, 100, 0, 0xffffff, 600, 80, 0x998800, 600};
                showfont(&fonttop);
            }

            showmenu(pManager, mpage);
        }
    }
}



int main(int argc, char *argv[])
{
    /*LCD初始化*/
    int iret = lcd_init();
    if(iret != 0){
        perror("LCD init failed.");
        return -1;
    }


/*
                    __   _,--="=--,_   __
                    /  \."    .-.    "./  \
                    /  ,/  _   : :   _  \/` \
                    \  `| /o\  :_:  /o\ |\__/
                    `-'| :="~` _ `~"=: |
                        \`     (_)     `/ jgs
                .-"-.   \      |      /   .-"-.
------------------{     }--|  /,.-'-.,\  |--{     }---------------------
                 (_)_)_)  \_/`~-===-~`\_/  (_(_(_)                     
*/                                                                                                 
    RunSaler();  //启动卡布达超级形态------> :)       !~


    /*释放LCD映射的内存*/
    int munmret = munmap(lcd, 800 * 480 * 4);
    if(munmret != 0){
        perror("lcd munmap failed.");
        return -1;
    }

    return 0;
}

