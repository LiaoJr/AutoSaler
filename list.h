
#ifndef _LIST_H_
#define _LIST_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
// #include <stdbool.h>


typedef struct Data
{
    char fp[256];    //商品图片名称
    unsigned int ID; //商品编号
    char name[32];       //记录商品名称
    unsigned int pos_x;  //记录商品的x坐标
    unsigned int pos_y;  //记录商品的y坐标
    unsigned int w;      //记录商品图片区域的宽度
    unsigned int h;      //记录商品图片区域的高度
    float price;         //记录商品价格
    unsigned int count;        //记录商品数量
}data_t;


/*购物车中商品结点的数据域*/
typedef struct Cdata{
    unsigned int ID; //商品编号
    char name[32];       //商品名称
    float price;  //记录商品价格
    unsigned int count;  //记录购物车中该商品的数量
}cdata_t;


typedef struct Node{
    data_t pd;
    struct Node *next;   //结点的next指针
}node_t;


/*购物车商品结点*/
typedef struct Cnode{
    cdata_t cd;
    struct Cnode *next;
}cnode_t;


typedef struct Man{
    node_t *head;  //记录首节点
    node_t *tail;  //记录尾节点
    unsigned int num;   //记录结点数量
}man_t;


typedef struct CMan{
    cnode_t *head;
    cnode_t *tail;
    unsigned int num;
}cman_t;



/*新建一个空链表，返回管理节后提首地址*/
man_t * ListCreat(void);

/*创建新节点并根据data初始化*/
node_t * ListCreatNode(data_t *data);


/*根据数据data创建新节点后，尾插到链表manager中*/
void ListTailInsert(man_t *manager, data_t *data);


/*链表结点遍历打印相关数据*/
void ListPrint(man_t *manager);


/*根据商品编号查找商品结点*/
node_t *ListFind(man_t *manager, unsigned int ID);


/*根据商品ID删除商品结点*/
void ListDelNode(man_t *manager, unsigned int ID);


cman_t * CListCreat();


cnode_t * CListCreatNode(cdata_t *cdata);


void CListTailInsert(cman_t *manager, cdata_t *cdata);

cnode_t * CListFind(cman_t *manager, unsigned int ID);

void CListDelNode(cman_t *manager, unsigned int ID);

/*清空购物车链表结点*/
void CListDestroy(cman_t *cmanager);

void CListPrint(cman_t *manager);
    



#endif


