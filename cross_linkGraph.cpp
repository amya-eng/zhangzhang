#include <stdio.h>                 
#include <stdlib.h>
#define MAXV 10
#define INF 32767
//定义十字链表节点结构体
typedef struct node {
    int tail;   //该弧尾部的顶点编号
    int head;   //该弧头部的顶点编号
    int weight; //该弧的权重
    struct node *hlink;  //指向下一个具有相同head的弧（在同一列），              与邻接表的nextarc指针同
    struct node *tlink;  //指向下一个具有相同tail的弧（在同一行），                  与逆邻接表的指针同
} node_t, *node_ptr;

//定义十字链表头节点结构体
typedef struct {
    int vertex;     //顶点编号
    node_ptr first_in;  //入边链表的第一个节点            这里的firstin, firstout 皆是针对heads数组中的！！！,first_in入该结点
    node_ptr first_out; //出边链表的第一个节点
} head_t, *head_ptr;

//定义十字链表表示的图结构体
typedef struct {
    int size;       //图中顶点个数
    head_ptr heads; //指向所有顶点的头节点数组
} graph_t, *graph_ptr;

//由二维数组创建图
graph_ptr create_graph(graph_ptr &g, int A[][MAXV], int size) {
    g = (graph_ptr)malloc(sizeof(graph_t));
    g -> heads = (head_ptr)malloc(size*sizeof(head_t));
    for (int i = 0; i < size; i++) {
    	g-> heads[i].vertex = i;
        g->heads[i].first_in = NULL;
        g->heads[i].first_out = NULL;
    }
    for(int i = 0; i < size; i ++)
    {
    	for(int j = size -1; j >= 0; j --)
    	{
    		if(A[i][j] != 0 && A[i][j] != INF)
    		{
    			node_ptr s = (node_ptr)malloc(sizeof(node_t));
    			s -> hlink = s -> tlink = NULL;
    			s -> head = i;
    			s -> tail = j;
    			s -> weight = A[i][j];
    			s -> hlink = g -> heads[i].first_out;
    			g -> heads[i].first_out = s ;
    			s -> tlink = g-> heads[j].first_in;
    			g -> heads[j].first_in = s;
			}
		}
	}
	g -> size = size;
    return g;
}

//添加顶点
void add_vertex(graph_ptr g) {
    int new_size = g->size + 1;
    g->heads = (head_ptr)realloc(g->heads, new_size * sizeof(head_t));
    head_ptr new_head = &g->heads[new_size - 1];
    new_head->vertex = new_size - 1;
    new_head->first_in = NULL;
    new_head->first_out = NULL;
    g->size = new_size;
}

//查找具有指定tail和head的弧
node_ptr find_arc(graph_ptr g, int tail, int head) {
    node_ptr arc = g->heads[head].first_out;
    while (arc != NULL && arc->tail != tail) {     //跳出结果为arc == NULL,则说明没有这样的边，返回NULL
        arc = arc->hlink;
    }
    return arc;
}

//添加边
void add_edge(graph_ptr g, int tail, int head, int weight) {
    node_ptr arc = find_arc(g, tail, head);      //如果弧存在，则更新权重
    if (arc == NULL) {                              //如果该弧不存在，则创建一个新的弧
        arc = (node_ptr)malloc(sizeof(node_t));
        arc->tail = tail;
        arc->head = head;
        arc->weight = weight;
        arc->hlink = g->heads[head].first_out;
        arc->tlink = g->heads[tail].first_in;
        g->heads[head].first_out = arc;
        g->heads[tail].first_in = arc;
    } else {                        //如果该弧已经存在，则更新其权重
        arc->weight = weight;
    }
}

//删除十字链表中的结点
void delete_vertex(graph_ptr g, int v)
{
	node_ptr pre = g-> heads[v].first_out, p;
	if(pre)
	{
		p = pre -> hlink;
		while(p)
		{
			free(pre);
			pre = p;
			p = p -> hlink;
		}
		free(pre);
	}
	pre = g -> heads[v].first_in, p;
	if(pre)
	{
		p = pre -> tlink;
		while(p)
		{
			free(pre);
			pre = p;
			p = p -> tlink;
		}
		free(pre);
	}
	for(int i = v; i < g -> size - 1; i ++)                  //用后继的头结点元素覆盖要删除的头结点
	{
		g -> heads[i] = g -> heads[i + 1]; 
	}
	g -> size --;
}
//在十字链表中删除从u到v的边
void delete_edge(graph_ptr g, int u, int v) {
    node_ptr pre, p, q;
	pre = g -> heads[u].first_out;
	p = pre -> hlink;
	if(pre -> tail != v)
	{
		while(p && p -> tail != v)
		{
			pre = p;
			p = p -> hlink;
		}
		pre -> hlink = p -> hlink;
		pre = p;                                 //使pre指向要释放的结点
	}
	else
	{
		g -> heads[u].first_out = pre -> hlink;               //这里pre指向要释放的结点了
	}
	
	q = g -> heads[u].first_in;
	p = q -> tlink;
	if(q -> head != u)
	{
		while(p && p -> head != v)
		{
			q = p;
			p = p -> tlink;
		}
		q -> tlink = p -> tlink;
		q = p;
	}
	else
	{
		g -> heads[u].first_in = q -> tlink;
	}
	free(pre);
}

//释放整个十字链表及头结点数组的内存空间
void destroy_graph(graph_ptr g) {
    int i;
    node_ptr p, q;

    //遍历整个十字链表释放每一个边的内存空间
    for (i = 0; i < g->size; i++) {
        for (p = g->heads[i].first_out; p != NULL; p = q) {
            q = p->tlink;
            free(p);
        }
    }

    //释放头结点数组的内存空间
    free(g->heads);

    //释放图数据结构的内存空间
    free(g);
}

void print(graph_ptr g)
{
	for(int i = 0; i < g -> size; i ++)
	{
		printf("%d:", g -> heads[i].vertex);
		node_ptr p = g -> heads[i].first_out;
		while(p)
		{
			printf("->[%d]%d", p -> weight, p -> tail);
			p = p -> hlink;
		}
		printf("\n");
	}
	printf("\n");
}
int main()
{
	int A[MAXV][MAXV] = {{0, INF, 9, 10, INF}, {5, 0, INF, 6, INF},{INF, INF, 0, INF, 8},{INF, INF, INF, 0, 7},{INF, INF, INF, INF, 0}};
	graph_ptr g = NULL;
	g = create_graph(g, A, 5);
	print(g);
	add_edge(g, 4, 1, 3);                    //添加一条边，<1, 4>权重为3
	print(g);
	delete_vertex(g, 1);                     //删除一个结点1及与1有关的所有边
	print(g);
	delete_edge(g,0, 2);                  //删除一条边<0, 2>
	print(g);
	destroy_graph(g);
	return 0;
	
}
