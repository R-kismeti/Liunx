/*************************************************************************
	> File Name: Redenvelope.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月22日 星期五 19时58分02秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>

#define MAX 200         //最大红包个数

double red[MAX];        //红包数组 
pthread_mutex_t lock;   //申请一个互斥锁
pthread_t threads[MAX]; //线程数组

struct RedEnvelo        //定义红包 包括钱和数量
{
    double money;
    int num;
}re;

struct Person           //定义抢红包人信息
{
    int id;
    double m;           //取到的钱
    int flag;           //是否领取过红包
}per[MAX];


void CreatRedenve(double m,int n)   //创建红包
{
    pthread_mutex_lock(&lock);
    re.money = m;
    re.num = n;
    pthread_mutex_unlock(&lock);
    printf("红包已放入！\n");
}

void* DistrubuteRedenve(void *j) //分配红包
{
    srand((int)time(0));           //随机数种子

    double m =((struct RedEnvelo*)j)->money;
    int n =((struct RedEnvelo*)j)->num;
    double restmoney = m;
    int restnum = n;
    double redmax = m / n * 2;
    
    for(int i=0; i < n-1; i++)
    {
        red[i] = (rand() % (int)(redmax * 100) + 1) / 100.0;
        restmoney -= red[i];
        redmax = restmoney / (n - i - 1) * 2;
    }
    red[n-1] = restmoney;

    return (void*)2;
}

void* Getrede(void* j)
{
    int i = *(int*)j;
    pthread_mutex_lock(&lock);
    if(per[i].flag == 0)
    {
        printf("第%d个人拿走的金额是：%.2lf\n",i+1,red[i]);
        re.money = re.money - red[i];
        re.num = re.num - 1;
        printf("剩余金额：%.2lf  剩余红包个数：%d\n",re.money,re.num);
        per[i].id = i;
        per[i].m = red[i];
        per[i].flag =1;
    }
    pthread_mutex_unlock(&lock);

    return (void*)1;
}

int main()
{
    struct RedEnvelo* p;
    p = &re;
    pthread_mutex_init(&lock,0); 
    pthread_t t;
    double money = 0;              //红包放入的钱
    int num = 0;                   //要发的红包个数

    printf("请输入红包金额：\n");
    scanf("%lf",&money);
    printf("请输入红包个数：\n");    
    scanf("%d",&num);

    CreatRedenve(money,num);       //创建红包

    memset(per,0,sizeof(struct Person)*(num));      //初始化抢红包信息

    
    pthread_create(&t,NULL,DistrubuteRedenve,p);
    pthread_join(t,NULL);

    for(int i=0; i < num; i++)
    {
        pthread_create(&threads[i],NULL,Getrede,(void*)&i);
        pthread_join(threads[i],NULL);
    }

    pthread_mutex_destroy(&lock);


    return 0;
}
