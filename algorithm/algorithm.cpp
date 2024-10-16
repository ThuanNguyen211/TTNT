#include <emscripten.h>
#include <stdlib.h>

int main(){ return 0; }

typedef struct {
    int *data;
    int size;
} IntList;

void initIntList(IntList* list) {
    list->data = NULL;
    list->size = 0;
}

void pushBackIntList(IntList *list, int x) {

    if(list->data == NULL) list->data = (int*)malloc(sizeof(int));
    else list->data = (int*)realloc(list->data, (list->size + 1) * sizeof(int));

    list->data[list->size] = x;
    list->size++;
}

void popBackIntList(IntList *list) {
	list->size--;
    list->data = (int*)realloc(list->data, (list->size) * sizeof(int));
}

int getBackIntList(IntList *list){
	return list->data[list->size - 1];
}

int *intListToArray(IntList list){
	int *ptr = (int*)malloc(list.size * sizeof(int));
	for(int i = 0; i < list.size; i++){
		ptr[i] = list.data[i];
	}
	return ptr;
}

typedef struct {
    int u, v, g;
} Edge;

typedef struct {
    Edge *edges;
    int size;
} EdgeList;

void initEdgeList(EdgeList* edgeList) {
    edgeList->edges = NULL;
    edgeList->size = 0;
}

void pushBackEdgeList(EdgeList *edgeList, Edge edge) {

    if(edgeList->edges == NULL) edgeList->edges = (Edge*)malloc(sizeof(Edge));
    else edgeList->edges = (Edge*)realloc(edgeList->edges, (edgeList->size + 1) * sizeof(Edge));

    edgeList->edges[edgeList->size] = edge;
    edgeList->size++;
}

void popBackEdgeList(EdgeList *list) {
	list->size--;
    list->edges = (Edge*)realloc(list->edges, (list->size) * sizeof(Edge));
}

Edge getBackEdgeList(EdgeList *list){
	return list->edges[list->size - 1];
}

typedef struct {
    IntList point_list;
    IntList h_list;
    EdgeList edge_list;
} Map;

void initMap(Map* map) {
    initIntList(&map->point_list);
    initIntList(&map->h_list);
    initEdgeList(&map->edge_list);
}

Map createMapWithInput(int* node_arr, int* h_arr, int n, int* u_arr, int* v_arr, int* g_arr, int m) {
    Map map;
    initMap(&map);
    for (int i = 0; i < n; i++) {
        pushBackIntList(&map.point_list, node_arr[i]);
        pushBackIntList(&map.h_list, h_arr[i]);
    }
    for (int i = 0; i < m; i++) {
        Edge edge = {u_arr[i], v_arr[i], g_arr[i]};
        pushBackEdgeList(&map.edge_list, edge);
    }
    return map;
}

int getHeuristic(int point, Map map) {
    for (int i = 0; i < map.h_list.size; i++) {
        if (point == map.point_list.data[i])
            return map.h_list.data[i];
    }
    return -1;
}

int getCost(int ut, int vt, Map map) {
    for (int i = 0; i < map.edge_list.size; i++) {
        int u = map.edge_list.edges[i].u;
        int v = map.edge_list.edges[i].v;
        if ((ut == u && vt == v) || (ut == v && vt == u))
            return map.edge_list.edges[i].g;
    }
    return -1;
}

IntList getNeighbors(int point, Map map) {
	IntList neighbors;
    initIntList(&neighbors);
    for (int i = 0; i < map.edge_list.size; i++) {
        int u = map.edge_list.edges[i].u;
        int v = map.edge_list.edges[i].v;
        if (point == u) pushBackIntList(&neighbors, v);
        else if (point == v) pushBackIntList(&neighbors, u);
    }
    return neighbors;
}

typedef struct Node {
    int point;
    struct Node* parent;
    int h, g;
} Node;

typedef struct {
    Node** nodes;
    int size;
} NodeList;

void initNodeList(NodeList* list) {
    list->nodes = NULL;
    list->size = 0;
}

void pushBackNodeList(NodeList* list, Node* node) {

    if(list->nodes == NULL) list->nodes = (Node**)malloc(sizeof(Node*));
    else list->nodes = (Node**)realloc(list->nodes, (list->size + 1) * sizeof(Node*));

    list->nodes[list->size] = node;
    list->size++;
}

Node *getBackNodeList(NodeList list){
	return list.nodes[list.size - 1];
}

void popBackNodeList(NodeList* list) {
	list->size--;
    list->nodes = (Node**)realloc(list->nodes, list->size * sizeof(Node*));
}

void removeNodeAt(NodeList *list, int p){
	for(int i = p - 1; i < list->size - 1; i++){
		list->nodes[i] = list->nodes[i + 1];
	}
	popBackNodeList(list);
}

Node* findNodeInNodeList(NodeList* list, int point, int* index) {
    for (int i = 0; i < list->size; i++) {
        if (list->nodes[i]->point == point) {
            *index = i;
            return list->nodes[i];
        }
    }
    return NULL;
}

void sortNodeListByF(NodeList* list) {
    for (int i = 0; i < list->size - 1; i++) {
        for (int j = i + 1; j < list->size; j++) {
            int f1 = list->nodes[i]->g + list->nodes[i]->h;
            int f2 = list->nodes[j]->g + list->nodes[j]->h;
            if (f1 < f2) {
                Node* temp = list->nodes[i];
                list->nodes[i] = list->nodes[j];
                list->nodes[j] = temp;
            }
        }
    }
}

Node* solveAStar(Map map, int start, int goal, IntList *traversalOrder) {
    NodeList openList, closeList;
    initNodeList(&openList);
    initNodeList(&closeList);

    Node* root = (Node*)malloc(sizeof(Node));
    root->point = start;
    root->parent = NULL;
    root->h = getHeuristic(start, map);
    root->g = 0;
    pushBackNodeList(&openList, root);

    while (openList.size > 0) {
        Node* node = getBackNodeList(openList);
        popBackNodeList(&openList);
        pushBackNodeList(&closeList, node);

        pushBackIntList(traversalOrder, node->point);
        if (node->point == goal) return node;

        IntList neighbors = getNeighbors(node->point, map);
        for (int i = 0; i < neighbors.size; i++) {
            Node* newNode = (Node*)malloc(sizeof(Node));
            newNode->point = neighbors.data[i];
            newNode->parent = node;
            newNode->h = getHeuristic(newNode->point, map);
            newNode->g = node->g + getCost(node->point, newNode->point, map);

            int posOpen, posClose;
            Node* foundInOpen = findNodeInNodeList(&openList, newNode->point, &posOpen);
            Node* foundInClose = findNodeInNodeList(&closeList, newNode->point, &posClose);
            if(!foundInOpen && !foundInClose){
            	pushBackNodeList(&openList, newNode);
            } else if(foundInOpen && foundInOpen->g > newNode->g){
            	removeNodeAt(&openList, posOpen + 1);
                pushBackNodeList(&openList, newNode);
            } else if(foundInClose && foundInClose->g > newNode->g){
            	removeNodeAt(&closeList, posClose + 1);
                pushBackNodeList(&openList, newNode);
            }    
        }
        sortNodeListByF(&openList);
    }
    return NULL;
}

IntList getGoalPath(Node *node){
	IntList path;
	initIntList(&path);
	while(node != NULL){
		pushBackIntList(&path, node->point);
		node = node->parent;
	}

	IntList ret;
	initIntList(&ret);
	for(int i = 0; i < path.size; i++){
		pushBackIntList(&ret, path.data[path.size - 1 - i]);
	}
	return ret;
}

typedef struct {
    int pathToGoalLength;
    int traversalOrderLength;
    int cost;
    int *pathToGoal;
    int *traversalOrder;
} OutputStruct;

OutputStruct *createOutputStruct(IntList pathToGoal, IntList traversalOrder, int cost){
    OutputStruct *res = (OutputStruct*)malloc(sizeof(OutputStruct));

    res->pathToGoal = (int*)malloc(pathToGoal.size * sizeof(int));
    res->pathToGoalLength = pathToGoal.size;
    for (int i = 0; i < pathToGoal.size; i++) {
        res->pathToGoal[i] = pathToGoal.data[i];
    }

    res->traversalOrder = (int*)malloc(traversalOrder.size * sizeof(int));
    res->traversalOrderLength = traversalOrder.size;
    for (int i = 0; i < traversalOrder.size; i++) {
        res->traversalOrder[i] = traversalOrder.data[i];
    }

    res->cost = cost;
    return res;
}

extern "C" {

    EMSCRIPTEN_KEEPALIVE
    OutputStruct *AStarAlgorithm(int* node_arr, int* h_arr, int n, int* u_arr, int* v_arr, int* g_arr, int m, int start, int goal) {
        Map map = createMapWithInput(node_arr, h_arr, n, u_arr, v_arr, g_arr, m);

        IntList traversalOrder;
        initIntList(&traversalOrder);

        Node *node = solveAStar(map, start, goal, &traversalOrder);
        int cost = node->g;

        IntList path = getGoalPath(node);
        return createOutputStruct(path, traversalOrder, cost);
    }

    EMSCRIPTEN_KEEPALIVE
    void* wasmmalloc(size_t size) {
        return malloc(size);
    }

    EMSCRIPTEN_KEEPALIVE
    void wasmfree(void* ptr) {
        free(ptr);
    }

}
