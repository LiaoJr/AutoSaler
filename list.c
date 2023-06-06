
#include "list.h"


man_t * ListCreat(void)
{
    man_t *manager = (man_t *)calloc(1, sizeof(man_t));
    if(NULL == manager){
        perror("manager calloc failed.");
        return NULL;
    }

    /*管理结构体初始化*/
    manager->head = NULL;
    manager->tail = NULL;
    manager->num = 0;

    /*返回管理结构体*/
    return manager;
}


node_t * ListCreatNode(data_t *data)
{
    node_t *NewNode = (node_t *)calloc(1, sizeof(node_t));
    if(NULL == NewNode){
        perror("NewNode calloc failed.");
        return NULL;
    }

    /*初始化新节点*/
    strcpy(NewNode->pd.fp, data->fp);
    NewNode->pd.ID = data->ID;
    strcpy(NewNode->pd.name, data->name);
    NewNode->pd.pos_x = data->pos_x;
    NewNode->pd.pos_y = data->pos_y;
    NewNode->pd.w = data->w;
    NewNode->pd.h = data->h;
    NewNode->pd.price = data->price;
    NewNode->pd.count = data->count;
    NewNode->next = NULL;

    /*返回新节点地址*/
    return NewNode;
}


void ListTailInsert(man_t *manager, data_t *data)
{
    /*先创建新节点*/
    node_t *NewNode = ListCreatNode(data);

    /*将新节点在链表尾部插入*/
    /*如果链表为空*/
    if(NULL == manager->head){
        manager->head = NewNode;
        manager->tail = NewNode;
    }
    else{
        manager->tail->next = NewNode;
        manager->tail = NewNode;
    }
    manager->num++;
}



/*根据商品编号查找商品结点*/
node_t *ListFind(man_t *manager, unsigned int ID)
{
    node_t *NodeFind = manager->head;
    while (NodeFind != NULL)
    {
        if(NodeFind->pd.ID == ID){
            break;
        }
        NodeFind = NodeFind->next;
    }
    if(NULL == NodeFind){
        printf("node not found.\n");
    }

    return NodeFind;
}


/*根据商品ID删除商品结点*/
void ListDelNode(man_t *manager, unsigned int ID)
{
    int i = 0;
    node_t *NodeDel = manager->head;
    while(NodeDel->pd.ID != ID){
        NodeDel = NodeDel->next;
        if(NULL == NodeDel){
            printf("NodeDel not found.\n");
            return ;
        }
    }

    /*如果要删除的结点是头结点*/
    if(NodeDel == manager->head){
        manager->head = NodeDel->next;
        NodeDel->next = NULL;
    }
    else{
        node_t *NodeDelPrev = manager->head;

        /*循环找出要删除结点的前驱结点*/
        while(NodeDelPrev->next != NodeDel){
            NodeDelPrev = NodeDelPrev->next;
        }
        NodeDelPrev->next = NodeDel->next;
        NodeDel->next = NULL;
    }

    free(NodeDel);
    manager->num--;
}




void ListPrint(man_t *manager)
{
    node_t *NodePrint = manager->head;
    printf("=======================================product info=====================================\n");
    while(NodePrint != NULL){
        printf("fp:%s ID:%u, product name:%s, price:%f, count:%u\n",
        NodePrint->pd.fp,
        NodePrint->pd.ID, 
        NodePrint->pd.name, 
        NodePrint->pd.price, 
        NodePrint->pd.count);

        NodePrint = NodePrint->next;
    }
    printf("========================================================================================\n");
}


/*创建购物车链表结构体*/
cman_t * CListCreat()
{
    cman_t *manager = (cman_t *)calloc(1, sizeof(cman_t));
    if(NULL == manager){
        perror("manager calloc failed.");
        return NULL;
    }
    manager->head = NULL;
    manager->tail = NULL;
    manager->num = 0;
    return manager;
}


/*创建购物车商品结点*/
cnode_t * CListCreatNode(cdata_t *cdata)
{
    cnode_t *NewNode = (cnode_t *)calloc(1, sizeof(cnode_t));
    if(NULL == NewNode){
        perror("cart NewNode calloc failed.");
        return NULL;
    }

    NewNode->cd.ID = cdata->ID;
    strcpy(NewNode->cd.name, cdata->name);
    NewNode->cd.price = cdata->price;
    NewNode->cd.count = 1;  //创建购物车商品结点是初始数量要设为1

    return NewNode;
}


/*购物车商品结点尾插*/
void CListTailInsert(cman_t *manager, cdata_t *cdata)
{
    /*先创建新节点*/
    cnode_t *NewNode = CListCreatNode(cdata);

    /*将新节点在链表尾部插入*/
    /*如果链表为空*/
    if(NULL == manager->head){
        manager->head = NewNode;
        manager->tail = NewNode;
    }
    else{
        manager->tail->next = NewNode;
        manager->tail = NewNode;
    }
    manager->num++;
}


/*根据商品编号查找购物车结点*/
cnode_t * CListFind(cman_t *manager, unsigned int ID)
{
    cnode_t *NodeFind = manager->head;
    while (NodeFind != NULL)
    {
        if(NodeFind->cd.ID == ID){
            break;
        }
        NodeFind = NodeFind->next;
    }

    return NodeFind;
}


/*根据商品编号删除购物车商品结点*/
void CListDelNode(cman_t *manager, unsigned int ID)
{
    cnode_t *CNodeDel = manager->head;
    cnode_t *CNodePrev = NULL;
    while(CNodeDel != NULL){
        if(CNodeDel->cd.ID == ID){
            break;
        }
        CNodePrev = CNodeDel;
        CNodeDel = CNodeDel->next;
    }
    if(NULL == CNodeDel){
        printf("CNodeDel not found.\n");
        return ;
    }
    else{
        /*如果要删除的结点是头结点*/
        if(CNodeDel == manager->head){
            manager->head = CNodeDel->next;
            CNodeDel->next = NULL;
        }
        /*如果要删除的加点是尾结点*/
        else if(CNodeDel == manager->tail){
            CNodePrev->next = CNodeDel->next;
            manager->tail = CNodePrev;
        }
        /*如果要删除的结点不是中间结点*/
        else{
            CNodePrev->next = CNodeDel->next;
            CNodeDel->next = NULL;
        }
        free(CNodeDel);
        manager->num--;
    }
}


/*清空购物车链表结点*/
void CListDestroy(cman_t *cmanager)
{
    cnode_t *NodeClean = cmanager->head;
    while(NodeClean != NULL){
        CListDelNode(cmanager, NodeClean->cd.ID);
        NodeClean = cmanager->head;
    }
}


void CListPrint(cman_t *manager)
{
    cnode_t *NodePrint = manager->head;
    printf("=========================================cart info=======================================\n");
    while(NodePrint != NULL){
        printf("ID:%u, product name:%s, price:%f, count:%u\n",
        NodePrint->cd.ID, 
        NodePrint->cd.name, 
        NodePrint->cd.price, 
        NodePrint->cd.count);

        NodePrint = NodePrint->next;
    }
    printf("========================================================================================\n");
}

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
