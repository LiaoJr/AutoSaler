
/*
arm-linux-gcc main.c list.c showjpg.c touch.c font.c -L./lib -ljpeg -lfont -lm -o main
*/

#include <time.h>
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


#define CHECKTOUCHPOS(tx,ty,pos_x,pos_y,w,h) \
(((tx)>(pos_x))&&((tx)<((pos_x)+(w)))&&((ty)>(pos_y))&&((ty)<((pos_y)+(h))))


int *lcd = NULL;  //全局变量，lcd设备内存映射首地址
unsigned int *origincount;
int page = 1;


void showcart(cman_t *pCartlist, man_t *manager);


/*从商品文档中读取信息并将其添加到链表，完成后将链表返回*/
man_t * ProductRead(char *fp)
{
    /*从商品信息文档中读取信息*/
    FILE *f = fopen(fp, "r");
    if(NULL == f){
        perror("file fopen failed.");
        return NULL;
    }
    else{
        printf("file fopen success.\n");
    }

    /*创建一个商品新链表*/
    man_t *pManager = ListCreat();

    /*从商品信息文档中读取现有商品信息，并将*/
    int R;
    node_t *NodeRead = pManager->head;
    while(1)
    {
        data_t DataTmp;
        R = fscanf(f, "%s %u %s %u %u %u %u %f %u\n", 
                DataTmp.fp,
                &DataTmp.ID,
                DataTmp.name,
                &DataTmp.pos_x,
                &DataTmp.pos_y,
                &DataTmp.w,
                &DataTmp.h,
                &DataTmp.price,
                &DataTmp.count);
        
        if(R == EOF){
            printf("read finish\n");
            break;
        }

        /*将读到的数据打印出来*/
        printf("%u %s %u %u %u %u %f %u\n",
                DataTmp.ID,
                DataTmp.name,
                DataTmp.pos_x,
                DataTmp.pos_y,
                DataTmp.w,
                DataTmp.h,
                DataTmp.price,
                DataTmp.count);
        
        ListTailInsert(pManager, &DataTmp);
    }

    fclose(f);
    return pManager;
}


void showmenu(man_t *manager, int page)
{
    imginfo_t menu = {"menu.jpeg", 0, 0};
    showjpg(&menu);

    int num = (page-1) * 6;
    node_t *pdis = manager->head;
    for(int i=0;i<num;i++){
        pdis = pdis->next;
    }

    //---------->这时拿到该页的开头结点

    int numdis = 0;
    while(pdis != NULL)
    {
        imginfo_t pimage;
        switch(numdis){
            case 0:
            strcpy(pimage.imgpath, pdis->pd.fp);
            pimage.pos_x = 50;
            pimage.pos_y = 102;
            showjpg(&pimage);
            break;

            case 1:
            strcpy(pimage.imgpath, pdis->pd.fp);
            pimage.pos_x = 300;
            pimage.pos_y = 102;
            showjpg(&pimage);
            break;

            case 2:
            strcpy(pimage.imgpath, pdis->pd.fp);
            pimage.pos_x = 550;
            pimage.pos_y = 102;
            showjpg(&pimage);
            break;

            case 3:
            strcpy(pimage.imgpath, pdis->pd.fp);
            pimage.pos_x = 50;
            pimage.pos_y = 262;
            showjpg(&pimage);
            break;

            case 4:
            strcpy(pimage.imgpath, pdis->pd.fp);
            pimage.pos_x = 300;
            pimage.pos_y = 262;
            showjpg(&pimage);
            break;

            case 5:
            strcpy(pimage.imgpath, pdis->pd.fp);
            pimage.pos_x = 550;
            pimage.pos_y = 262;
            showjpg(&pimage);
            break;

            default:
            break;
        }

        font_t pfont = {"", 30, pdis->pd.pos_x+75, pdis->pd.pos_y, 0x1034a2, 125, 125, 0xffffff, 125};
        sprintf(pfont.content, "名称:%s\n价格:%.1f元\n数量:%u", pdis->pd.name, pdis->pd.price, pdis->pd.count);
        showfont(&pfont);
        pdis = pdis->next;
        numdis++;
        if(numdis > 5){
            break;
        }
    }
    

    char str1[] = "欢迎光临";
    font_t myfont = {"", 50, 0, 0, 0xffffff, 100, 100, 0x0000DD, 100};
    strcpy(myfont.content, str1);
    showfont(&myfont);

    char str2[] = "管理员模式";
    font_t font2 = {"", 40, 600, 0, 0xffff00, 200, 40, 0xae86fc, 200};
    strcpy(font2.content, str2);
    showfont(&font2);

    font_t font3 = {"", 50, 0, 430, 0xffffff, 120, 50, 0x86fcfd, 150};
    sprintf(font3.content,  "第%d页", page);
    showfont(&font3);

}

void showproductlist(man_t *manager)
{
    /*展示购物车信息(后面加上"+"和"-"按钮)*/
    node_t *NodeShow = manager->head;
    font_t plusf, minorsf;
    int stepposy = 0;
    while(NodeShow != NULL){
        font_t buyfont = {"", 50, 200, 100+stepposy, 0xffffff, 500, 50, 0x115599, 500};
        strcpy(buyfont.content, NodeShow->pd.name);
        showfont(&buyfont);

        font_t countf = {"", 50, 380, 100+stepposy, 0xffffff, 250, 50, 0x115599, 250};
        sprintf(countf.content, "货存数量: %u", NodeShow->pd.count);
        showfont(&countf);

        strcpy(plusf.content, " +");
        plusf.size = 50;
        plusf.pos_x = 600;
        plusf.pos_y = 108+stepposy;
        plusf.fcolor = 0xffee66;
        plusf.bgw = 40;
        plusf.bgh = 40;
        plusf.bgcolor = 0x44ee22;
        plusf.linew = 50;
        showfont(&plusf);

        strcpy(minorsf.content, " -");
        minorsf.size = 50;
        minorsf.pos_x = 650;
        minorsf.pos_y = 108+stepposy;
        minorsf.fcolor = 0xffee66;
        minorsf.bgw = 40;
        minorsf.bgh = 40;
        minorsf.bgcolor = 0x44ee22;
        minorsf.linew = 50;
        showfont(&minorsf);

        stepposy += 50;
        NodeShow = NodeShow->next;
    }

}

/*展示购物车信息*/
void showcartlist(cman_t *pcart, int cartpage)
{
    /*展示购物车信息(后面加上"+"和"-"按钮)*/
    cnode_t *CNodeShow = pcart->head;
    
    /*找该页的第一个*/
    for(int i = 0;i<(cartpage-1)*6;i++){
        CNodeShow = CNodeShow->next;
    }

    //----->这时CNodeShow是该页的第一项

    font_t plusf, minorsf;
    int stepposy = 0;
    int pagelimit = 0;
    while(CNodeShow != NULL){
        font_t buyfont = {"", 50, 200, 100+stepposy, 0xffffff, 500, 50, 0x115599, 500};
        strcpy(buyfont.content, CNodeShow->cd.name);
        showfont(&buyfont);

        font_t countf = {"", 50, 380, 100+stepposy, 0xffffff, 250, 50, 0x115599, 250};
        sprintf(countf.content, "已购数量: %u", CNodeShow->cd.count);
        showfont(&countf);

        strcpy(plusf.content, " +");
        plusf.size = 50;
        plusf.pos_x = 600;
        plusf.pos_y = 108+stepposy;
        plusf.fcolor = 0xffee66;
        plusf.bgw = 40;
        plusf.bgh = 40;
        plusf.bgcolor = 0x44ee22;
        plusf.linew = 50;
        showfont(&plusf);

        strcpy(minorsf.content, " -");
        minorsf.size = 50;
        minorsf.pos_x = 650;
        minorsf.pos_y = 108+stepposy;
        minorsf.fcolor = 0xffee66;
        minorsf.bgw = 40;
        minorsf.bgh = 40;
        minorsf.bgcolor = 0x44ee22;
        minorsf.linew = 50;
        showfont(&minorsf);

        stepposy += 50;
        CNodeShow = CNodeShow->next;
        pagelimit++;
        if(pagelimit==6){
            break;
        }
    }

    font_t font3 = {"", 50, 0, 430, 0xffffff, 120, 50, 0x86fcfd, 150};
    sprintf(font3.content,  "第%d页", cartpage);
    showfont(&font3);
}

/*显示某商品一大最大购买数量*/
void showmaxcount(char *pname, int step)
{
    font_t fs = {"", 50, 600, 108+step, 0xffffff, 200, 100, 0xff0000, 200};
    sprintf(fs.content, "%s已达最大数量", pname);
    showfont(&fs);

    sleep(1);
}


float showpaylist(cman_t *cmanager, int paypage)
{
    printf("paypage = %d\n", paypage);

    float sum = 0;
    int step = 0;
    //font_t fontpay;
    cnode_t *nodepaylist = cmanager->head;
    /*找该页头结点*/
    for(int i=0;i<(paypage-1)*6;i++){
        nodepaylist = nodepaylist->next;
    }

    printf("paylisy first item: %s\n", nodepaylist->cd.name);
    //----->这时nodepaylist为该页第一项

    int pagemax = 0;
    while(nodepaylist != NULL)
    {
        font_t fontpay = {"", 50, 50, 100+step, 0xffffff, 250, 50, 0x11ee11, 300};
        sprintf(fontpay.content, "%s * %u", nodepaylist->cd.name, nodepaylist->cd.count);
        showfont(&fontpay);

        font_t fontprice = {"", 50, 300, 100+step, 0xffffff, 200, 50, 0x11ee11, 200};
        sprintf(fontprice.content, "价格:%.2f", nodepaylist->cd.price * nodepaylist->cd.count);
        showfont(&fontprice);

        nodepaylist = nodepaylist->next;
        step += 50;
        pagemax++;
        if(pagemax == 6){
            break;
        }
    }


    nodepaylist = cmanager->head;
    while(nodepaylist != NULL){
        sum += nodepaylist->cd.price * nodepaylist->cd.count;
        nodepaylist = nodepaylist->next;
    }
    /*显示总支付价格*/
    font_t fontsum = {"", 50, 20, 400, 0xffffff, 280, 80, 0x11ee11, 280};
    sprintf(fontsum.content, "总计支付 :%.2f", sum);
    showfont(&fontsum);

    font_t font3 = {"", 50, 150, 0, 0xffffff, 120, 50, 0x86fcfd, 150};
    sprintf(font3.content,  "第%d页", paypage);
    showfont(&font3);


    return sum;
}


/*进入结算页面*/
int showpay(cman_t *cmanager, man_t *manager)
{
    unsigned int tx,ty;
    bool flagconti = false;
    int ret;
    int ppage = 1;

    while(1)
    {
        imginfo_t background = {"background.jpeg", 0, 0};
        showjpg(&background);

        /*显示返回和确认支付按钮*/
        font_t fontback = {"<返回", 80, 0, 0, 0xffffff, 150, 80, 0x0000cc, 240};
        showfont(&fontback);
        font_t fontpay = {"确认支付", 80, 300, 400, 0xffffff, 250, 80, 0x0066cc, 320};
        showfont(&fontpay);
        /*显示取消支付按钮*/
        font_t fontcancel = {"取消支付", 80, 550, 400, 0xffffff, 250, 80, 0xff0000, 320};
        showfont(&fontcancel);

        /*显示商品支付列表*/
        float sum;
        sum = showpaylist(cmanager, ppage);  //返回支付总计价格

        /*显示支付二维码*/
        imginfo_t imgcode = {"paycode.jpeg", 500, 0};
        showjpg(&imgcode);

        ret = touchpanel(&tx, &ty);  //检测是否有触碰到屏幕

        if(ret == 0){
            /*如果点到返回按钮*/
            if(CHECKTOUCHPOS(tx, ty, 0, 0, 150, 80)){
                /*返回购物车页面*/
                printf("-----返回购物车页面-----\n");
                // showcart(cmanager, manager);
                return 1;
                // break;
            }
            /*如果点到确认支付按钮*/
            else if(CHECKTOUCHPOS(tx, ty, 300, 400, 250, 80)){
                printf("正在结算\n");
                font_t fontfinish = {"", 80, 50, 100, 0xffffff, 450, 300, 0x11eecc, 450};
                sprintf(fontfinish.content, "已成功支付:\n%.2f元\n欢迎再次光临\n       !== :)  ==!", sum);
                showfont(&fontfinish);
                sleep(1);

                /*清空购物车链表结点*/
                CListDestroy(cmanager);

                printf("结算完成\n");
                break;
            }
            else if(CHECKTOUCHPOS(tx, ty, 550, 400, 250, 80)){
                printf("取消支付\n");
                font_t fontcf = {"确认放弃支付?\n(将会清空购物车)", 50, 200, 100, 0xffffff, 400, 150, 0xff0000, 400};
                showfont(&fontcf);
                font_t font1 = {"取消", 50, 200, 200, 0xffffff, 80, 50, 0x0000ff, 100};
                showfont(&font1);
                font_t font2 = {"确认", 50, 520, 200, 0xffffff, 80, 50, 0xee3380, 100};
                showfont(&font2);
                int tx1, ty1;
                while(1){
                    touchpanel(&tx1, &ty1);
                    /*如果点击取消*/
                    if(CHECKTOUCHPOS(tx1,ty1,200, 200, 80, 50)){
                        flagconti = true;
                        break;
                    }
                    /*如果点击确认*/
                    if(CHECKTOUCHPOS(tx1, ty1, 520, 200, 80, 50)){
                        /*还原商品链表*/
                        cnode_t *cnoderet = cmanager->head;
                        node_t *noderet;
                        while(cnoderet != NULL)
                        {
                            noderet = ListFind(manager, cnoderet->cd.ID);
                            noderet->pd.count += cnoderet->cd.count;

                            cnoderet = cnoderet->next;
                        }
                        /*清空商品链表*/
                        CListDestroy(cmanager);
                        
                        flagconti = false;
                        break;
                    }
                }
                if(flagconti){
                    continue;
                }
                
                break;
            }
        }

        if(ret == 1){  //如果是在屏幕上滑
            printf("pay列表上滑\n");
            if((ppage == (cmanager->num)/6) && (cmanager->num)%6==0){
                printf("购物车已在最后一页\n");
                /*提示用户已经在最后一页啦*/
                font_t fontbot = {"已经在最后一页啦!", 80, 100, 0, 0xffffff, 600, 80, 0x998800, 600};
                showfont(&fontbot);
                continue;
            }
            else if(ppage == (cmanager->num)/6 + 1){
                printf("-paylist已在最后一页-\n");
                /*提示用户已经在最后一页啦*/
                font_t fontbot = {"已经在最后一页啦!", 80, 100, 0, 0xffffff, 600, 80, 0x998800, 600};
                showfont(&fontbot);
                continue;
            }
            else{
                printf("paylist下一页\n");
                ppage++;
            }
        }

        if(ret == 2){  //如果是在屏幕下滑
            printf("pay列表下滑\n");
            if(ppage>1){
                printf("paylist上一页\n");
                ppage--;
            }
            else{
                /*提示用户已经在第一页啦*/
                font_t fontbot = {"已经在第一页啦!", 80, 100, 0, 0xffffff, 600, 80, 0x998800, 600};
                showfont(&fontbot);
                continue;
            }
        }
    }
    return 0;
}


void showcart(cman_t *pCartlist, man_t *manager)
{

    unsigned int tx,ty;
    bool flagconti = false;
    int ret;
    int cpage = 1;
    while(1){
        imginfo_t background = {"background.jpeg", 0, 0};
        showjpg(&background);

        /*显示返回和结算按钮*/
        font_t fontback = {"<返回", 80, 0, 0, 0xffffff, 150, 80, 0x0000cc, 240};
        showfont(&fontback);
        font_t fontpay = {"结算支付", 80, 300, 400, 0xffffff, 250, 80, 0x0066cc, 320};
        showfont(&fontpay);

        showcartlist(pCartlist, cpage);  //显示购物车列表
        ret = touchpanel(&tx, &ty);  //等到屏幕点击事件

        if(ret == 0){  //如果是屏幕点击
            node_t *pcount;
            cnode_t *jcount = pCartlist->head;
            int steplist = 0;  //购物车列表项步长
            while(jcount != NULL){
                // printf("steplist = %d\n", steplist);
                /*如果点击到了加号*/
                if(CHECKTOUCHPOS(tx,ty,600,108+steplist,40,40)){
                    jcount->cd.count++;
                    printf("%s 数量加1后: %u\n", jcount->cd.name, jcount->cd.count);
                    pcount = ListFind(manager, jcount->cd.ID);
                    if(jcount->cd.count > *(origincount+(pcount->pd.ID-1))){
                        // printf("该商品已达最大购买数量\n");
                        jcount->cd.count--;
                        showmaxcount(jcount->cd.name, steplist);  //展示商品达到最大购买数量页面
                        flagconti = true;
                        break;
                    }

                    /*购物车的数量加1，那商品链表对应商品结点数量减1*/
                    if(pcount!=NULL && pcount->pd.count>0){
                        pcount->pd.count--;
                    }
                    flagconti = true;
                    break;
                }
                else if(CHECKTOUCHPOS(tx,ty,650,108+steplist,40,40)){
                    jcount->cd.count--;
                    printf("%s 数量减1后: %u\n", jcount->cd.name, jcount->cd.count);
                    /*购物车的数量减1，那商品链表对应商品结点数量加1*/
                    pcount = ListFind(manager, jcount->cd.ID);
                    if(pcount!=NULL && pcount->pd.count>=0){
                        pcount->pd.count++;
                    }
                    flagconti = true;
                    break;
                }
                else{
                    steplist += 50;
                    jcount = jcount->next;
                }
            }

            if(jcount != NULL && jcount->cd.count == 0){
                CListDelNode(pCartlist, jcount->cd.ID);
            }

            if(flagconti){
                flagconti = false;
                continue;
            }

            /*如果点击结算但是购物车为空*/
            if((pCartlist->head == NULL) && (CHECKTOUCHPOS(tx,ty,fontpay.pos_x,fontpay.pos_y,fontpay.bgw,fontpay.bgh))){  //如果购物车为空
                font_t fontnp = {"购物车为空\n无法结算", 50, 100, 100, 0xffffff, 240, 100, 0xff0000, 250};
                showfont(&fontnp);
                sleep(1);
                continue;
            }
            /*如果点击结算但是购物车不为空*/
            else if((CHECKTOUCHPOS(tx,ty,fontpay.pos_x,fontpay.pos_y,fontpay.bgw,fontpay.bgh))){

                /*进入结算页面*/
                int ret = showpay(pCartlist, manager);
                if(ret == 1){  //如果我在结算支付页面点击了返回
                    continue;
                }
                // if(ret == 2){  //如果在结算支付页面点击了取消支付并确认

                // }
                showmenu(manager, page);
                break;  //结算完成后返回主界面

            }
            /*如果点击了返回*/
            else if(CHECKTOUCHPOS(tx,ty,fontback.pos_x,fontback.pos_y,fontback.bgw,fontback.bgw)){
                printf("-----返回主页面-----\n");
                showmenu(manager, page);
                break;
            }
            else{
                // printf(">>>>>>>>>>>>>>>>>>>>无效点击\n");
            }
        }

        if(ret == 1){  //如果是在屏幕上滑
            printf("购物车列表上滑\n");
            if((cpage == (pCartlist->num)/6) && (pCartlist->num)%6==0){
                printf("购物车已在最后一页\n");
                /*提示用户已经在最后一页啦*/
                font_t fontbot = {"已经在最后一页啦!", 80, 100, 0, 0xffffff, 600, 80, 0x998800, 600};
                showfont(&fontbot);
                continue;
            }
            else if(cpage == (pCartlist->num)/6 + 1){
                printf("-cart已在最后一页-\n");
                /*提示用户已经在最后一页啦*/
                font_t fontbot = {"已经在最后一页啦!", 80, 100, 0, 0xffffff, 600, 80, 0x998800, 600};
                showfont(&fontbot);
                continue;
            }
            else{
                printf("cart下一页\n");
                cpage++;
            }
        }

        if(ret == 2){  //如果是在屏幕下滑
            printf("购物车链表下滑\n");
            if(cpage>1){
                printf("cart上一页\n");
                cpage--;
            }
            else{
                /*提示用户已经在第一页啦*/
                font_t fontbot = {"已经在第一页啦!", 80, 100, 0, 0xffffff, 600, 80, 0x998800, 600};
                showfont(&fontbot);
                continue;
            }
        }
    }
}

/*传入点击的商品名称*/
void showaddpd(char *pname, int count, man_t *manager)
{
    /*展示文字图片*/
    font_t f1 = {"", 50, 50, 400, 0x00ff00, 550, 50, 0x001188, 800};
    sprintf(f1.content, "%.14s已添加到购物车,数量%d", pname, count);
    showfont(&f1);

    /*延时1秒*/
    // sleep(1);

    /*回到主菜单界面*/
    showmenu(manager, page);
}


void showmanager(man_t *manager)
{
    unsigned int tx,ty;
    bool flagconti = false;
    while(1)
    {
        /*显示背景*/
        imginfo_t background = {"background.jpeg", 0, 0};
        showjpg(&background);

        /*显示管理员模式*/
        font_t font2 = {"管理员模式", 40, 600, 0, 0xffff00, 200, 40, 0xae86fc, 200};
        showfont(&font2);

        char fp[256] = "product.txt";
        manager = ProductRead(fp);

        /*显示商品货存列表*/
        showproductlist(manager);

        touchpanel(&tx, &ty);

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

    showmenu(pManager, page);  //显示主界面

    /*创建一个购物车链表*/
    cman_t *pCart = CListCreat();

    int tx,ty;
    int ReTouch;
    while (1)
    {
        ListPrint(pManager);
        CListPrint(pCart);
        // int page = 1;
        ReTouch = touchpanel(&tx, &ty);  //等待触屏事件，检测到才往下响应

        //检测到触屏事件
        if(ReTouch == 0){
            node_t *NodeTouch = pManager->head;
            for(int i=0;i<(page-1)*6;i++){
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
                    showmenu(pManager, page);
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
            if(page*6<pManager->num){
                page++;
            }
            else{
                /*提示用户已经在最后一页啦*/
                font_t fontbot = {"已经在最后一页啦!", 80, 100, 0, 0xffffff, 600, 80, 0x998800, 600};
                showfont(&fontbot);
            }
            showmenu(pManager, page);
        }

        /*如果是在主菜单下滑*/
        if(ReTouch == 2){
            if(page>1){
                page--;
            }
            else{
                /*提示用户已经在第一页啦*/
                font_t fonttop = {"已经在第一页啦!", 80, 100, 0, 0xffffff, 600, 80, 0x998800, 600};
                showfont(&fonttop);
            }

            showmenu(pManager, page);
        }





    }


    /*释放LCD映射的内存*/
    int munmret = munmap(lcd, 800 * 480 * 4);
    if(munmret != 0){
        perror("lcd munmap failed.");
        return -1;
    }

    return 0;
}




