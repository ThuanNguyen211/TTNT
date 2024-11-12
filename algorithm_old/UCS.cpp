#include <stdio.h>
#include <string>
#include <vector>
#include <malloc.h>
#include <algorithm>
#include <stack>

using namespace std;

// Khai báo cấu trúc Edge
typedef struct{
    char u, v; // Điểm đầu và điểm cuối của cung
    int g; // Trong số cung (chi phí của đường đi)
} Edge;

// Khai báo cấu trúc bản đồ (Map)
typedef struct{
    vector<char> point_list; // Danh sách các điểm
    vector<int> h_list; // Danh sách heuristic của từng điểm (sẽ không sử dụng trong UCS)
    vector<Edge> edge_list; // Danh sách các đường đi
} Map;

// Khởi tạo Map
void init_map(Map *map){
    map->point_list.clear();
    map->h_list.clear();
    map->edge_list.clear();
}

// Đọc dữ liệu từ file cấu hình
void readFileTxt(string filename, Map *map, char *start, char *goal){
    freopen(filename.c_str(),"r", stdin);

    int n, m;
    scanf("%d %d\n", &n, &m);

    char node;
    int h;
    for(int i = 0; i < n; i++){
        scanf("%c %d\n", &node, &h);
        map->point_list.push_back(node);
        map->h_list.push_back(h);
    }

    char u, v;
    int g;
    for(int i = 0; i < m; i++){
        scanf("%c %c %d\n", &u, &v, &g);
        Edge edge = {u, v, g};
        map->edge_list.push_back(edge);
    }

    scanf("%c %c", start, goal);
}

// Khai báo kiểu cấu trúc Node - lưu trữ trạng thái 
typedef struct Node{
    char point;
    struct Node* parent;
    int g; // Chỉ dùng chi phí hiện tại `g` trong UCS
} Node;

// Lấy chi phí của một đường đi
int get_cost(int ut, int vt, Map map){
    for(size_t i = 0; i < map.edge_list.size(); i++){
        int u = map.edge_list.at(i).u;
        int v = map.edge_list.at(i).v;
        if(ut == u && vt == v || ut == v && vt == u)
            return map.edge_list.at(i).g;
    }
    return -1;
}

// Lấy các điểm láng giềng (liền kề với điểm đang xét)
vector<char> get_neighbors(char point, Map map){
    vector<char> neighbors;
    for(size_t i = 0; i < map.edge_list.size(); i++){
        char u = map.edge_list.at(i).u;
        char v = map.edge_list.at(i).v;
        if(point == u) neighbors.push_back(v);
        else if (point == v) neighbors.push_back(u);
    }
    return neighbors;
}

// Tìm một node chứa điểm đang xét trong một danh sách các node, lưu lại vị trí tìm thấy
Node* find_point_in_list(char point, vector<Node*>& list, vector<Node*>::iterator *pos){
    if(list.size() == 0) return NULL;

    vector<Node*>::iterator it = list.begin();
    while(it != list.end()){
        if((*it)->point == point){
            *pos = it;
            return *it;
        }
        ++it;
    }
    return NULL;
}

// Hàm so sánh theo chi phí g -> cung cấp cho hàm sort()
bool compare_g(Node* a, Node* b){
    return a->g > b->g;
}

// Giải thuật UCS
Node* UCSAlgorithm(Map map, char start, char goal, vector<char> *traversing_order){

    // Khai báo 2 danh sách open và close
    vector<Node*> openList;
    vector<Node*> closeList;

    // Tạo một root node với điểm start
    Node* root = (Node*)malloc(sizeof(Node));
    root->point = start;
    root->parent = NULL;
    root->g = 0;

    // Đưa root node vào open
    openList.push_back(root);

    // Lặp khi open list còn phần tử
    while(!openList.empty()){

        // Lấy node cuối cùng trong open
        Node* node = openList.back();
        // Xóa node này ra khỏi open và đưa vào close
        openList.pop_back();
        closeList.push_back(node);

        // Thêm node đàn xét vào danh sách lưu thứ tự duyệt
        traversing_order->push_back(node->point);

        // Kiểm tra: nếu node đang xét chứa điểm goal -> tìm được lời giải
        if(node->point == goal) return node;

        // Lấy danh sách và duyệt qua các láng giềng
        vector<char> neighbors = get_neighbors(node->point, map);
        for(size_t v = 0; v < neighbors.size(); v++){
            Node* newNode = (Node*)malloc(sizeof(Node));
            newNode->point = neighbors.at(v);
            newNode->parent = node;
            newNode->g = newNode->parent->g + get_cost(newNode->parent->point, newNode->point, map);

            vector<Node*>::iterator posOpen, posClose;
            Node* nodeFoundOpen = find_point_in_list(newNode->point, openList, &posOpen);
            Node* nodeFoundClose = find_point_in_list(newNode->point, closeList, &posClose);

            // Nếu chưa có trong cả 2 danh sách => thêm vào open
            if(nodeFoundOpen == NULL && nodeFoundClose == NULL){
                openList.push_back(newNode);
            }
            // Nếu đã có trong open và chi phí tệ hơn -> xóa node tìm thấy và thêm node hiện tại vào open
            else if(nodeFoundOpen != NULL && nodeFoundOpen->g > newNode->g){
                openList.erase(posOpen);
                openList.push_back(newNode);
            }
            // Nếu đã có trong close và chi phí tệ hơn -> xóa node tìm thấy và thêm node hiện tại vào open
            else if(nodeFoundClose != NULL && nodeFoundClose->g > newNode->g){
                closeList.erase(posClose);
                openList.push_back(newNode);
            }
            else free(newNode);

            // Sắp xếp lại open theo chiều giảm dần của g
            sort(openList.begin(), openList.end(), compare_g);
        }
    }

    return NULL;
}

// Hiển thị đường đi lời giải
void print_way_to_goal(Node *node){
    stack<Node*> path;

    while(node->parent != NULL){
        path.push(node);
        node = node->parent;
    }
    path.push(node);

    printf("\nPath to goal: ");
    while(path.size() > 1){
        printf("%c -> ", path.top()->point);
        path.pop();
    }
    printf("%c", path.top()->point);
    path.pop();
}

int main(){

    // Khai báo và khởi tạo Map
    Map map;
    init_map(&map);

    // Đọc dữ liệu từ file
    char start, goal;
    readFileTxt("map_data.txt", &map, &start, &goal);

    // Khai báo danh sách chứa thứ tự duyệt
    vector<char> traversing_order;

    // Thực hiện giải thuật
    Node* node = UCSAlgorithm(map, start, goal, &traversing_order);

    if(node == NULL){
		printf("Khong tim thay duong di tu %c den %c", start, goal);
		return 0;
	}

    // Hiển thị thứ tự duyệt
    printf("Traversing Order: ");
    for(size_t i = 0; i < traversing_order.size() - 1; i++){
        printf("%c -> ", traversing_order.at(i));
    }
    printf("%c", traversing_order.back());

    // Hiển thị đường đi lời giải
    print_way_to_goal(node);

    // Hiển thị tổng chi phí
    printf("\nTotal cost: %d", node->g);

    return 0;
}