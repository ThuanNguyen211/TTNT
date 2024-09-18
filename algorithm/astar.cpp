#include <stdio.h>
#include <string>
#include <vector>
#include <malloc.h>
#include <algorithm>
#include <stack>

using namespace std;

// Khai báo kiểu cấu trúc Edge
// - Điểm đầu và cuối của cung
// - Trọng số cung: chi phí của đường đi
typedef struct {
	char u, v;
	int g;
} Edge;

// Khai báo kiểu cấu trúc Map
// - Danh sách các điểm
// - Danh sách hueristic của từng điểm
// - Danh sách đường đi
typedef struct {
	vector<char> point_list;
	vector<int> h_list;
	vector<Edge> edge_list;
} Map;

// Khởi tạo Map
void init_map(Map *map){
	map->point_list.clear();
	map->h_list.clear();
	map->edge_list.clear();
}

// Đọc dữ liệu từ file cấu hình
// - Dòng đầu tiên chưa n và m (n - số đỉnh, m - số cung)
// - n dòng tiếp theo: tên các đỉnh (kiểu char) và heristic của đỉnh đó
// - m dòng tiếp theo: danh sách cung với trọng số (g)
// - dòng cuối cùng chứa 2 điểm: start và goal - cho biết điểm bắt đầu và kết thúc
void readFileTxt(string filename, Map *map, char *start, char *goal){ 
	freopen(filename.c_str(), "r", stdin);

	// Đọc số điểm và số đường đi
	int n, m;
	scanf("%d %d\n", &n, &m);

	// Đọc n điểm với heuristic
	char node;
	int h;
	for(int i = 0; i < n; i++){
		scanf("%c %d\n", &node, &h);
		map->point_list.push_back(node);
		map->h_list.push_back(h);
	}

	// Đọc m đường đi
	char u, v;
	int g;
	for(int i = 0; i < m; i++){
		scanf("%c %c %d\n", &u, &v, &g);
		Edge edge = {u, v, g};
		map->edge_list.push_back(edge);
	}

	// Đọc 2 điểm start và goal
	scanf("%c %c", start, goal);
}

// Khai báo kiểu cấu trúc Node - lưu trữ trạng thái 
// - Điểm
// - Node cha
// - hueristic và chi phí hiện tại
typedef struct Node {
	char point;
	struct Node* parent;
	int h, g;
} Node;

// Lấy chỉ số heuristic của một điểm
int get_heuristic(char point, Map map){
	for(int i = 0; i < map.point_list.size(); i++){
		if(point == map.point_list.at(i))
			return map.h_list.at(i);
	}
	return -1;
}

// Lấy chi phí của một đường đi
int get_cost(int ut, int vt, Map map){
	for(int i = 0; i < map.edge_list.size(); i++){
		int u = map.edge_list.at(i).u;
		int v = map.edge_list.at(i).v;
		if(ut == u && vt == v || ut == v && vt == u)
			return map.edge_list.at(i).g;
	}
	return -1;
}

// Lấy các điểm láng giềng - có đường đi tới điểm đang xét
vector<char> get_neighbors(char point, Map map){
	vector<char> neighbors;
	for(int i = 0; i < map.edge_list.size(); i++){
		char u = map.edge_list.at(i).u;
		char v = map.edge_list.at(i).v;
		if(point == u) neighbors.push_back(v);
		else if(point == v) neighbors.push_back(u);
	}
	return neighbors;
}

// Tìm một node chứa điểm đang xét trong một danh sách node - có tham số để lưu vị trí tìm thấy
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

// Hàm so sánh -> nhằm mục đích cung cấp cho hàm sort()
bool compare_f(Node* a, Node* b){
	return a->g + a->h > b->g + b->h;
}

// Giải thuật A-Star
Node* AStarAlgorithm(Map map, char start, char goal, vector<char> *traversing_order){

	// Khai báo 2 danh sách open và close
	vector<Node*> openList;
	vector<Node*> closeList;

	// Tạo một root node với điểm start
 	Node* root = (Node*)malloc(sizeof(Node));
	root->point = start;
	root->parent = NULL;
	root->h = get_heuristic(start, map);
	root->g = 0;

	if(root->h == -1) {
		printf("Hueristic of root is -1");
		return NULL;
	}

	// Đưa root node vào open
	openList.push_back(root);

	// Lặp khi open list còn phần tử
	while(!openList.empty()){

		// Lấy node cuối cùng trong open (open được sắp xếp giảm dần theo f = g + h)
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
		for(int v = 0; v < neighbors.size(); v++){

			// Tạo node mới với từng láng giềng
			Node* newNode = (Node*)malloc(sizeof(Node));
			newNode->point = neighbors.at(v);
			newNode->parent = node;
			newNode->h = get_heuristic(newNode->point, map);
			newNode->g = newNode->parent->g + get_cost(newNode->parent->point, newNode->point, map);

			if(newNode->h == -1) {
				printf("Hueristic of new node is -1");
				return NULL;
			}

			if(newNode->g == -1) {
				printf("Cost of new node is -1");
				return NULL;
			}

			// Kiểm tra sự hiện diện của láng giềng trong open và close
			vector<Node*>::iterator posOpen, posClose; // Chứa vị trí nếu tìm được
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

			// Sắp xếp lại open theo chiều giảm dần
			sort(openList.begin(), openList.end(), compare_f);
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

	// Đọc dư liệu từ file
	char start, goal;
	readFileTxt("map_data.txt", &map, &start, &goal);

	// Khai báo danh sách chứa thứ tự duyệt
	vector<char> traversing_order;

	// Thực hiện giải thuật
	Node* node = AStarAlgorithm(map, start, goal, &traversing_order);
	
	// Hiển thị thứ tự duyệt
	printf("Traversing Order: ");
	for(int i = 0; i < traversing_order.size() - 1; i++){
		printf("%c -> ", traversing_order.at(i));
	}
	printf("%c", traversing_order.back());

	// Hiển thị đường đi lời giải
	print_way_to_goal(node);

	// Hiển thị tổng chi phí
	printf("\nTotal cost: %d", node->g);
	
	return 0;
}