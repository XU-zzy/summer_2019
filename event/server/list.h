#ifndef LIST_H_
#define LIST_H_
#include <stdlib.h>
#include <assert.h>
/*初始化链表。将链表 list 初始化为空的带头节点的双向循环列表*/
#define List_Init(list,list_node_t) {					\
		list=(list_node_t*)malloc(sizeof(list_node_t)); \
		(list)->next=(list)->prev=list;					\
	}

//释放链表list中所有数据结点空间
#define List_Free(list,list_node_t) {			\
		assert(NULL!=list);						\
		list_node_t *tmpPtr;					\
		(list)->prev->next=NULL; 				\
		while(NULL!=(tmpPtr=(list)->next)){ 	\
			(list)->next=tmpPtr->next;			\
			free(tmpPtr);						\
		}										\
		(list)->next=(list)->prev=list;			\
	}

//销毁链表list，释放所有数据节点以及头结点
#define List_Destroy(list,list_node_t) {		\
		assert(NULL!=list);						\
		List_Free(list, list_node_t);			\
		free(list);								\
		(list)=NULL;							\
	}

//头插法，将结点newhead 插入到链表list的开头
#define List_AddHead(list,newNode) {			\
		(newNode)->next=(list)->next;		 	\
		(list)->next->prev=newNode;			 	\
		(newNode)->prev=(list);				 	\
		(list)->next=newNode;				 	\
	}

//尾插法，将newNode插入到list末尾
#define List_AddTail(list,newNode) {			\
		(newNode)->prev=(list)->prev; 		 	\
		(list)->prev->next=newNode;			 	\
		(newNode)->next=list;				 	\
		(list)->prev=newNode;				 	\
	}

//将结点new插入到链表结点node前
#define List_InsertBefore(node,newNode) {		\
		(newNode)->prev=(node)->prev; 		 	\
		(newNode)->next=node;			 		\
		(newNode)->prev->next=newNode;			\
		(newNode)->next->prev=newNode;			\
	}

//将结点newNode插入到链表结点node֮之后
#define List_InsertAfter(node, newNode) {		\
		(newNode)->next=node->next;			 	\
		(newNode)->prev=node; 				 	\
		(newNode)->next->prev=newNode;			\
		(newNode)->prev->next=newNode;			\
	}

//判断list是否为空，为空时返回true，否则为false
#define List_IsEmpty(list)  ((list != NULL)	\
	&& ((list)->next == list)				\
	&& (list == (list)->prev))

//将数据结点node从链表中删除（不释放空间）
#define List_DelNode(node) {\
			assert(NULL!=node && node!=(node)->next && node!=(node)->prev);				\
			(node)->prev->next=(node)->next; 	\
			(node)->next->prev=(node)->prev;	\
	}

//将数据结点node从链表中删除并释放结点
#define List_FreeNode(node) {	\
		List_DelNode(node);		\
		free(node);				\
	}


//使用链表指针变量curPos逐个遍历list中的每个数据结点
#define List_ForEach(list, curPos) 		\
	 for (   curPos = (list)->next;  	\
		  	  	  curPos != list;       \
		  	  	  curPos=curPos->next	\
	    )


//---------------------------------分页器---------------------------------------


//定义结分页器结构体，简称分页器类型
typedef struct
{
	int totalRecords;	//总数据记录数
	int offset;			//当前分页起始记录相当于第一条记录的偏移记录数
	int pageSize;		//页面大小
	void *curPos;		//当前页起始记录在链表中的结点地址
}Pagination_t;

//根据分页器paging的偏移量offset将分页器定位到链表list的对应位置
#define List_Paging(list, paging, list_node_t) {			\
		if(paging.offset+paging.pageSize>=paging.totalRecords){	\
			Paging_Locate_LastPage(list, paging, list_node_t);	}\
		else {													\
			int i;												\
			list_node_t * pos=(list)->next;						\
			for( i=0; i<paging.offset && pos!=list ; i++) 		\
			   pos=pos->next;		 							\
			paging.curPos=(void*)pos;							\
		}														\
	}

//将分页器paging定位到链表list的第一页
#define Paging_Locate_FirstPage(list, paging) { \
		paging.offset=0;						\
		paging.curPos=(void *)((list)->next);	\
	}

//将分页器paging定位到链表list的最后一页
#define Paging_Locate_LastPage(list, paging, list_node_t) {	\
	int i=paging.totalRecords % paging.pageSize;	\
	if (0==i && paging.totalRecords>0)				\
		i=paging.pageSize;							\
	paging.offset=paging.totalRecords-i;			\
	list_node_t * pos=(list)->prev;					\
	for(;i>1;i--)									\
		pos=pos->prev;								\
	paging.curPos=(void*)pos;						\
													\
}

//根据链表list及分页器paging,使用指针变量curPos依次遍历paging所指向的页面中的每个结点
//这里i为整型变量用作计数器
#define Paging_ViewPage_ForEach(list, paging, list_node_t, pos, i){ 	\
	for (i=0, pos = (list_node_t *) (paging.curPos);	\
			pos != list && i < paging.pageSize; 		\
			i++, pos=pos->next)							\
}														\

//对于链表list，将分页器paging向前（后）移动offsetPage个页面
//当offsetPage<0时，向前（链表头方向）移动offsetPage个页面
//当offsetPage>0时，向后（链表尾方向）移动offsetPage个页面
#define Paging_Locate_OffsetPage(list, paging, offsetPage, list_node_t) {\
	int offset=offsetPage*paging.pageSize;			\
	list_node_t *pos=(list_node_t *)paging.curPos;	\
	int i;											\
	if(offset>0){									\
		if( paging.offset + offset >= paging.totalRecords )	{\
			Paging_Locate_LastPage(list, paging, list_node_t);	\
		}else {												\
			for(i=0; i<offset; i++ )						\
				pos=pos->next;								\
			paging.offset += offset;						\
			paging.curPos= (void *)pos;						\
		}													\
	}else{													\
		if( paging.offset + offset <= 0 ){					\
			Paging_Locate_FirstPage(list, paging);			\
		}else {												\
			for(i=offset; i<0; i++ )						\
				pos	= pos->prev;							\
			paging.offset += offset;						\
			paging.curPos= pos;								\
		}													\
	}														\
}

//根据分页器paging计算当前的页号
#define Paging_CurPage(paging)(0==(paging).totalRecords?0:1+(paging).offset/(paging).pageSize)

//根据分页器paging计算总的页数
#define Paging_TotalPages(paging)(((paging).totalRecords%(paging).pageSize==0)?\
	(paging).totalRecords/(paging).pageSize:\
	(paging).totalRecords/(paging).pageSize+1)

//根据paging判断当前页面是否为第一页，返回true和false
#define Paging_IsFirstPage(paging) (Paging_CurPage(paging)<=1)

//根据paging判断当前页面是否为最后一页，返回true和false
#define Paging_IsLastPage(paging) (Paging_CurPage(paging)>=Paging_TotalPages(paging))
#endif /* LIST_H_ */

#ifndef LIST_H_
#define LIST_H_
#include <stdlib.h>
#include <assert.h>
/*初始化链表。将链表 list 初始化为空的带头节点的双向循环列表*/
#define List_Init(list,list_node_t) {					\
		list=(list_node_t*)malloc(sizeof(list_node_t)); \
		(list)->next=(list)->prev=list;					\
	}

//释放链表list中所有数据结点空间
#define List_Free(list,list_node_t) {			\
		assert(NULL!=list);						\
		list_node_t *tmpPtr;					\
		(list)->prev->next=NULL; 				\
		while(NULL!=(tmpPtr=(list)->next)){ 	\
			(list)->next=tmpPtr->next;			\
			free(tmpPtr);		