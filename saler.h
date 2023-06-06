
/****************************************************************************************
*      @文件名称: saler.h
*      @实现功能: 售货机页面相关处理接口声明，例如主菜单、购物车页面的展示等
* 
*      @文件作者: 廖杰文
*      @创建日期: 2023-06-04
*      @修改版本: V1.0
*      @注意事项: 无
*      @版权声明: 无
* ***************************************************************************************/
#ifndef _SALER_H_
#define _SALER_H_

#include "list.h"


#define CHECKTOUCHPOS(tx,ty,pos_x,pos_y,w,h) \
(((tx)>(pos_x))&&((tx)<((pos_x)+(w)))&&((ty)>(pos_y))&&((ty)<((pos_y)+(h))))


/*进入菜单页面*/
void showmenu(man_t *manager, int mpage);

/*按页码,展示购物车列表*/
void showcartlist(cman_t *pcart, int cartpage);

/*显示某商品已达最大购买数量*/
void showmaxcount(char *pname, int step);

/*按页码显示支付列表*/
float showpaylist(cman_t *cmanager, int paypage);

/*进入结算支付页面*/
int showpay(cman_t *cmanager, man_t *manager);

/*进入购物车页面*/
void showcart(cman_t *pCartlist, man_t *manager);

/*显示某商品已成功添加到购物车*/
void showaddpd(char *pname, int count, man_t *manager);

/*进入管理员补货界面*/
void showproduct(man_t *manager, cman_t *cmanager);

/*商品展示(管理员模式|未使用)*/
void showproductlist(man_t *manager, int spage);


char *getkey(man_t *manager);

#endif
