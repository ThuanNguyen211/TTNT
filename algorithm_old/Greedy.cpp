#include <stdio.h>
#include <string>
#include <vector>
#include <malloc.h>
#include <algorithm>
#include <stack>

using namespace std;

//Khai báo cấu trúc Edge
typedef struct{
    char u, v; //Điểm đầu và điểm cuối của cung
    int g; //Trong số cung (chi phí của đường đi)
} Edge;

//Khai báo cấu trúc bản đồ (Map)
typedef struct{
    vector<char> point_list; //Danh sách các điểm
    vector<int> h_list; //Danh sách heuristic của từng điểm
    vector<Edge> edge_list; //Danh sách các đường đi
} Map;

//Khởi tạo Map
void init_map(Map *map){
    map->point_list.clear();
    map->h_list.clear();
    map->edge_list.clear();
}

//Đọc dữ liệu từ file cấu hình
// - Dòng đầu tiên chưa n và m (n - số đỉnh, m - số cung)
// - n dòng tiếp theo: tên các đỉnh (kiểu char) và heuristic của đỉnh đó
// - m dòng tiếp theo: danh sách cung với trọng số (g)
// - dòng cuối cùng chứa 2 điểm: start và goal - cho biết điểm bắt đầu và kết thúc
void readFileTxt(string filename, Map *map, char *start, char *goal){
    freopen(filename.c_str(),"r", stdin);

    //Đọc số điểm và số đường đi
    int n, m;
    scanf("%d %d\n", &n, &m);

    //Đọc n điểm với Heuristic
    char node;
    int h;
    for(int i = 0; i < n; i++){
        scanf("%c %d\n", &node, &h);
        map->point_list.push_back(node);
        map->h_list.push_back(h);
    }

    //Đọc m đường đi
    char u, v;
    int g;
    for(int i = 0; i < m; i++){
        scanf("%c %c %d\n", &u, &v, &g);
        Edge edge = {u, v, g};
        map->edge_list.push_back(edge);
    }

    //Đọc 2 điểm start & goal
    scanf("%c %c", start, goal);

}

//Khai báo kiểu cấu trúc Node - lưu trữ trạng thái 
typedef struct Node{
    char point; //Điểm
    struct Node* parent; // Node parent
    int h,g; // heuristic & chi phí hiện tại
} Node;

//Lấy chỉ số heuristic của một điểm
int get_heuristic(char point, Map map){
    for(size_t i = 0; i < map.point_list.size(); i++){
        if(point == map.point_list.at(i))
            return map.h_list.at(i);
    }
    return -1;
}

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
// Lấy các điểm láng giềng (Liền kề với điểm đang xét)
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

//Tìm một node chứa điểm đang xét trong một danh sách các node, lưu lại vị trí tìm thấy
Node* find_point_in_list(char point, vector<Node*> list, vector<Node*>::iterator *pos){
    if(list.size() == 0) return NULL;

    vector<Node*>::iterator it = list.begin();
    while(it != list.end()){
        if((*it)->point == point){
            *pos = it;
            return *it;
        }
        it = list.erase(it);
    }
    return NULL;
}

//Hàm so sánh -> nhằm mục đích cung cấp cho hàm sort
bool compare_h(Node *a, Node *b){
    return a->h > b->h;
}

//Giải thuật Greedy
Node* GreedyAlgorithm(Map map, char start, char goal, vector<char> *traversing_order){
    //Khai báo 2 danh sách open & close
    vector<Node*> openList;
    vector<Node*> closeList;

    //Tạo một root node với điểm start
    Node *root = (Node*)malloc(sizeof(Node));
    root->point = start;
    root->parent = NULL;
    root->h = get_heuristic(start, map);
    root->g = 0;

    if(root->h == -1){
        printf("Heuristic of root is -1");
        return NULL;
    }

    //Đưa root node vài open
    openList.push_back(root);

    //Lặp khi Open List còn phần tử
    while(!openList.empty()){
        //Sắp xếp Openlist theo  giá trị giảm dần của h
        sort(openList.begin(),openList.end(),compare_h);

        //Lấy Node cuối cùng trong open
        Node* node = openList.back();

        //Xóa node này ra khỏi Open và đưa vào Close
        openList.pop_back();
        closeList.push_back(node);

        //Thêm node đang xét vào danh sách lưu thứ tự duyệt
        traversing_order->push_back(node->point);

        //Kiểm tra: Nếu node đang xét là goal
        if(node->point == goal) return node;

        //Lấy danh sách các láng giềng của node đang xét
        vector<char> neighbors = get_neighbors(node->point, map);
        
        for(size_t v = 0; v < neighbors.size(); v++){
            //Tạo node mới với từng láng giềng
            Node* newNode = (Node*)malloc(sizeof(Node));
            newNode->point = neighbors.at(v);
            newNode->parent = node;
            newNode->h = get_heuristic(newNode->point, map);
            newNode->g = newNode->parent->g +get_cost(newNode->parent->point, newNode->point, map);

            //Kiểm tra sự hiện diện của node láng giềng trong close
            vector<Node*>::iterator pos;
            if(find_point_in_list(newNode->point,closeList,&pos) != NULL){
                free(newNode); //Nếu tìm thấy trong close -> Xóa node láng giềng vừa được tìm thấy
                continue;
            }

            //Kiểm tra sự hiện diện của node láng giềng trong open
            Node* found = find_point_in_list(newNode->point,openList,&pos);
            if(found == NULL){
                openList.push_back(newNode); //Nếu không tìm thấy trong open -> Thêm nút láng giềng vừa được tìm thấy vào open
            } else {
                free(newNode); //Nếu tìm thấy trong open -> Xóa node láng giềng vừa được tìm thấy
            }
        }
    }

    return NULL;
}

//In đường đi lời giải
void prin_way_to_goal(Node* node){
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
    printf("%c",path.top()->point);
    path.pop();
}

int main(){
    //Khởi tạo map
    Map map;
    init_map(&map);

    //Đọc dữ liệu từ file cấu hình
    char start, goal;
    readFileTxt("map_data.txt", &map, &start, &goal);

    //Khai báo danh sách chứa thứ tuyệt duyệt
    vector<char> traversing_order;

    //Triển khai giải thuật
    Node* node = GreedyAlgorithm(map, start, goal, &traversing_order);

    if(node == NULL){
		printf("Khong tim thay duong di tu %c den %c", start, goal);
		return 0;
	}

    //In thứ tự duyệt
    printf("Traversing order: ");
    for(size_t i = 0; i < traversing_order.size() - 1; i++){
        printf("%c -> ", traversing_order.at(i));
    }
    printf("%c", traversing_order.back());

    //In đường đi lời giải
    prin_way_to_goal(node);

    //In tổng chi phí
    printf("\nTotal cost: %d", node->g);

    return 0;
}