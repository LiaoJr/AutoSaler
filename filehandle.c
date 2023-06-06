/*该文件仅用于写入和读取product.txt文档中的商品信息*/


#include <stdio.h>
#include "list.h"

int main()
{
#if 0
    /*从商品信息文档中读取信息*/
    char fp[256] = "product.txt";
    FILE *f = fopen(fp, "r");
    if(NULL == f){
        perror("file fopen failed.");
        return -1;
    }
    else{
        printf("file fopen success.\n");
    }

    /*创建一个商品新链表*/
    man_t *pManager = ListCreat();

    /*从商品信息文档中读取现有商品信息，并将*/
    int R;
    node_t *NodeRead = pManager->head;
    while(1){
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
        printf("%s %u %s %u %u %u %u %f %u\n",
            DataTmp.fp,
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
#endif    

#if 1
    /*往商品信息文档中写入商品信息*/
    char fp[256] = "product.txt";
    FILE *f = fopen(fp, "w+");
    if(NULL == f){
        perror("file fopen failed.");
        return -1;
    }

    /*创建一个商品新链表*/
    man_t *pManager = ListCreat();

    /*从商品信息文档中读取现有商品信息，并将*/


    data_t b1 = { "fenda.jpeg", 1,"芬达",50,102,180,120,2.50,15 };
    data_t b2 = { "cola.jpeg", 2,"可口可乐",300,102,180,120,2.50, 8 };
    data_t b3 = { "lemontea.jpeg", 3,"柠檬茶", 550,102,180,120,2.00, 2 };
    data_t b4 = { "water.jpeg", 4,"农夫山泉",50,262,180,120,3.00,6 };
    data_t b5 = { "spring.jpeg", 5, "雪碧",300,262,180,120,2.50,21 };
    data_t b6 = { "bwater.jpeg", 6, "元气森林",550,262,180,120,3.00,17 };
    data_t b7 = { "sars.jpeg", 7,"沙士",50,102,180,120,3.50,11 };
    data_t b8 = { "weita.jpeg", 8,"维他奶",300,102,180,120,3.50, 9 };
    data_t b9 = { "icetea.jpeg", 9,"冰红茶", 550,102,180,120,4.00, 6 };
    data_t b10 = { "pulse.jpeg", 10,"脉动",50,262,180,120,5.00,6 };
    data_t b11 = { "cafe.jpeg", 11,"雀巢咖啡",300,262,180,120,8.00,28 };
    data_t b12 = { "nutri.jpeg", 12,"营养快线",550,262,180,120,4.00,5 };
    data_t b13 = { "coconut.jpeg", 13,"椰子汁",50,102,180,120,3.50,61 };
    data_t b14 = { "yoga.jpeg", 14,"安慕希",300,102,180,120,4.50,19 };
    data_t b15 = { "wang.jpeg", 15,"王老吉",550,102,180,120,4.00,46 };
    /*向链表中插入结点(尾插)*/
    ListTailInsert(pManager, &b1);
    ListTailInsert(pManager, &b2);
    ListTailInsert(pManager, &b3);
    ListTailInsert(pManager, &b4);
    ListTailInsert(pManager, &b5);
    ListTailInsert(pManager, &b6);
    ListTailInsert(pManager, &b7);
    ListTailInsert(pManager, &b8);
    ListTailInsert(pManager, &b9);
    ListTailInsert(pManager, &b10);
    ListTailInsert(pManager, &b11);
    ListTailInsert(pManager, &b12);
    ListTailInsert(pManager, &b13);
    ListTailInsert(pManager, &b14);
    ListTailInsert(pManager, &b15);

    node_t *NodeWrite = pManager->head;
    while(NodeWrite != NULL){
        fprintf(f, "%s %u %s %u %u %u %u %f %u\n",
            NodeWrite->pd.fp,
            NodeWrite->pd.ID,
            NodeWrite->pd.name,
            NodeWrite->pd.pos_x,
            NodeWrite->pd.pos_y,
            NodeWrite->pd.w,
            NodeWrite->pd.h,
            NodeWrite->pd.price,
            NodeWrite->pd.count);

        NodeWrite = NodeWrite->next;
    }
#endif

    fclose(f);

    return 0;
}
