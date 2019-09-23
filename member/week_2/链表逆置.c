#include<stdio.h>
#include<stdlib.h>
typedef struct node{
    int data;
    struct node *next;
}node;
node *creat(int n){
    node *head,*p,*q;
    int num;
    head = (node*)malloc(sizeof(node));
    head->next = NULL;
    q = head;
    while(n--){
        scanf("%d",&num);
        p = (node*)malloc(sizeof(node));
        p->data = num;
        q->next = p;
        q = p;
    }
    q->next = NULL;
    return head;
}

void print(node *head){
    node *p = head->next;
    while(p != NULL){
        printf("%d ",p->data);
        p = p->next;
    }
    printf("\n");
    return;
}

//头插
void reverse_head(node *head){
    node *p,*q;
    p = head->next; //使用 p 依次向后遍历
    head->next = NULL;
    while(p){
        //前一负责指向插入结点的指针后移，指向将要插入头指针后的结点
        //在第二次 while 循环时看这个操作更清楚
        q = p;
        //向后遍历
        p = p->next;    
        //将 q 结点插入到头结点后面
        q->next = head->next;
        head->next = q;
    }
    return;
}

//就地逆置
node *reverse_another(node *head){  //这里传入的是头结点的下一个有效结点
    if(head == NULL || head->next == NULL)  //判定返回
        return head;
    
    node *rev,*t,*cur;
    rev = NULL;
    cur = head;
    while(cur != NULL){
        //指针后移
        t = cur;
        //向后遍历
        cur = cur->next;
        //与头插法类似
        t->next = rev;
        rev = t;
    }
    return rev;
}

int main(){
    node *head;
    int n;
    printf("请输入数字个数:");
    scanf("%d",&n);
    head = creat(n);
    print(head);
    reverse_head(head);             //头插输出
    //head->next = reverse_another(head->next); //就地逆置输出
    print(head);
    return 0;
}
