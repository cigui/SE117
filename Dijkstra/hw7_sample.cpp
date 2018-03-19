#include <iostream>
#include <queue>
#include <vector>

using namespace std;


//定义边的数据结构
struct EdgeList{
	EdgeList *next = nullptr;
	int dest = 0;
	int dist = 0;
};

//定义图的邻接表表示方法
class AdjGraph{
public:
	//构造函数。每个点对应的邻接表初始为空。
	AdjGraph(int n) :size(n){
		edge = new EdgeList*[n];
		for (int i = 0; i < n; i++)
			edge[i] = nullptr;
	}

	//析构函数
	~AdjGraph(){
		for (int i = 0; i < size; i++){
			EdgeList *cur = edge[i];
			//释放邻接表的空间
			while (cur != nullptr){
				EdgeList *next = cur->next;
				delete cur;
				cur = next;
			}
			delete cur;
		}
	}

	//addEdge函数向图中加入一条边
	void addEdge(int from, int to, int dis){
		//当邻接表为空时，以给定边创建一个新的邻接表
		if (edge[from] == nullptr){
			edge[from] = new EdgeList;
			edge[from]->dest = to;
			edge[from]->dist = dis;
			edge[from]->next = nullptr;
		}
		//当邻接表不为空时，迭代找到邻接表的最后一条边
		else{
			EdgeList *copy(edge[from]);
			//迭代找到邻接表的最后一条边
			while (copy->next != nullptr)
				copy = copy->next;
			EdgeList *next = new EdgeList;
			next->dest = to;
			next->dist = dis;
			next->next = nullptr;
			copy->next = next;
		}
	}

	//返回指定点对应的邻接表
	EdgeList *operator[](int index){
		return this->edge[index];
	}

private:
	int size;
	EdgeList **edge;
};

//定义点的数据结构
struct Node{
	int name;
	int dis;
	//点的访问状态
	int state;      //1=UNVISITED   2=will VISITED  3=REACHED
};

//用数组实现优先队列
class PriorityQueue{
public:
	PriorityQueue() :capacity(0), size(0){}

	//添加新元素
	void insert(Node node){
		//内存的自动增长机制
		if (size == capacity){
			capacity = 1 > 2 * capacity ? 1 : 2 * capacity;
			Node *resizedQ = new Node[capacity];
			for (int i = 0; i < size; i++)
				resizedQ[i] = pQueue[i];
			pQueue = resizedQ;
		}
		pQueue[size] = node;
		size++;
		siftUp(size);
	}

	//返回并删除队列中的最小值
	Node extractMin(){
		Node min = pQueue[0];
		pQueue[0] = pQueue[size - 1];
		size--;
		siftDown(1);
		return min;
	}

	//更新队列中指定元素的值
	void decreaseKey(int name, int value){
		int key;
		for (int i = 0; i < size; i++){
			if (pQueue[i].name == name)
				key = i + 1;
		}
		pQueue[key - 1].dis = value;
		siftUp(key);
	}

	//判断队列是否为空，若为空返回true，否则返回false
	bool empty(){
		return size == 0 ? true : false;
	}

private:
	//辅助函数
	void siftUp(int key){
		while (key > 1 &&
			pQueue[key - 1].dis < pQueue[key / 2 - 1].dis){
			Node temp = pQueue[key - 1];
			pQueue[key - 1] = pQueue[key / 2 - 1];
			pQueue[key / 2 - 1] = temp;
			key = key / 2;
		}
	}

	//辅助函数
	void siftDown(int key){
		while (2 * key <= size){
			bool stop = true;
			if (2 * key + 1 <= size){
				Node temp = pQueue[key - 1];
				if (pQueue[key - 1].dis
			> pQueue[minIndex(2 * key, 2 * key - 1)].dis){
					pQueue[key - 1] = pQueue[minIndex(2 * key, 2 * key - 1)];
					pQueue[minIndex(2 * key, 2 * key - 1)] = temp;
					key = minIndex(2 * key, 2 * key - 1) + 1;
					stop = false;
				}
			}
			else
				if (pQueue[key - 1].dis > pQueue[2 * key - 1].dis) {
				Node temp = pQueue[key - 1];
				pQueue[key - 1] = pQueue[2 * key - 1];
				pQueue[2 * key - 1] = temp;
				key = 2 * key;
				stop = false;
				}
			if (stop)
				break;
		}
	}

	//辅助函数
	int minIndex(int x, int y){
		return  pQueue[x].dis >= pQueue[y].dis ? y : x;
	}

	int capacity;
	int size;
	Node *pQueue;
};

//单源最短路径的迪杰斯特拉图算法
void ssspDijkstra(AdjGraph &graph, Node *nodes){
	nodes[0].dis = 0;
	nodes[0].state = 2;
	PriorityQueue Q;
	Q.insert(nodes[0]);
	while (!Q.empty()){
		Node cur = Q.extractMin();
		EdgeList *list = graph[cur.name];
		while (list != nullptr){
			//如果目标点已在队列中，则更新它的值
			if (nodes[list->dest].state == 2){
				if (cur.dis + list->dist < nodes[list->dest].dis){
					nodes[list->dest].dis = cur.dis + list->dist;
					Q.decreaseKey(list->dest, cur.dis + list->dist);
				}
			}
			//如果目标点不在队列中，则将其加入队列
			if (nodes[list->dest].state == 1){
				if (cur.dis + list->dist < nodes[list->dest].dis){
					nodes[list->dest].dis = cur.dis + list->dist;
					//将该点标记为将要访问
					nodes[list->dest].state = 2;
					Q.insert(nodes[list->dest]);
				}
			}
			list = list->next;
		}
		//将该点标记为已到达
		nodes[cur.name].state = 3;
	}
}

int main(){

	int nodeSize(0), edgeSize(0);
	cin >> nodeSize >> edgeSize;
	nodeSize++;

	//从给定文件中读入图
	AdjGraph graph(nodeSize);
	for (int i = 0; i < edgeSize; i++){
		int start, dest, dist;
		cin >> start >> dest >> dist;
		graph.addEdge(start, dest, dist);
	}

	//初始化
	Node *nodes = new Node[nodeSize];
	for (int i = 0; i < nodeSize; i++){
		nodes[i].name = i;
		nodes[i].dis = INT_MAX;
		nodes[i].state = 1;
	}

	//计算单源最短路径
	ssspDijkstra(graph, nodes);

	//输出结果
	for (int i = 0; i < nodeSize; i++){
		if (nodes[i].dis == INT_MAX)
			cout << -1 << " ";
		else
			cout << nodes[i].dis << " ";
	}
	return 0;
}
