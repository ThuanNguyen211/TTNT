var nodes = new vis.DataSet();
var edges = new vis.DataSet();

const container = document.getElementById('network');
const data = { nodes: nodes, edges: edges };
const options = {
    nodes: {
        shape: 'circle',
        size: 30,  // Kích thước nút
        color: {
            border: '#e8e8e8', // Đặt màu viền thành màu xám nhạt
            background: '#e8e8e8', // Màu nền giống như nút
            hover: {
                border: '#e8e8e8', // Màu viền khi hover
                background: '#D2E5FF' // Màu nền khi hover
            }
        },
        font: {
            vadjust: 0,
            size: 14,
            multi: true,
            color: '#090909' // Màu chữ tối
        },
        margin: {
            top: 15,
            bottom: 15,
            left: 8
        },
    },
    edges: {
        font: { color: '#090909', size: 16, align: 'horizontal' }, // Màu và kích thước chữ
        color: {
            color: '#848484',  // Màu cạnh
            hover: '#2B7CE9'   // Màu khi hover
        },
        width: 2              // Độ dày của cạnh
    },
    physics: {
        enabled: true,
        solver: 'forceAtlas2Based',
        stabilization: { iterations: 100 }
    },
    interaction: {
        hover: true,
        dragNodes: true,
        zoomView: true,
        dragView: false,
        zoomView: false
    },
    layout: {
        randomSeed: 2,
        improvedLayout: true
    }
};


const network = new vis.Network(container, data, options);

var newNodeList = [];
var newEdgeList = [];
function draw(){

    const inputForm = document.getElementById('inputform');
    const edges_input = inputForm.getElementsByClassName('edge');

    for (let i = 0; i < edges_input.length; i++) {
        const edge = edges_input[i];

        const source1 = edge.querySelector('input[name="node1"]').value;
        const source2 = edge.querySelector('input[name="node2"]').value;
        const weight = edge.querySelector('input[name="weight"]').value;

        if([source1, source2, weight].includes('')){
            alert('Nhap cung khong hop le');
            return null;
        }
        
        let h1 = 0, h2 = 0;
        if(document.querySelector('input[name="number-selector"]:checked').value == 1) {
            h1 = -1;
            h2 = -1;
        } else {
            h1 = document.querySelector(`#h-${source1}`).value;
            h2 = document.querySelector(`#h-${source2}`).value;
            if(h1 == '' || h2 == ''){
                alert('Nhap heuristic khong hop le');
                return null;
            }
        }
        
        const node1 = {name: source1, h: h1};
        const node2 = {name: source2, h: h2};

        if(!newNodeList.find(node => node1.name === node.name))
            newNodeList.push(node1);

        if(!newNodeList.find(node => node2.name === node.name))
            newNodeList.push(node2);

        const newEdge = {u: source1, v: source2, w: weight};
        if(!newEdgeList.find(edge => (newEdge.u == edge.u && newEdge.v == edge.v) || (newEdge.v == edge.u && newEdge.u == edge.v)))
            newEdgeList.push(newEdge);

    }

    newNodeList.forEach(newNode => addNode(newNode));
    newEdgeList.forEach(newEdge => addEdge(newEdge));

    return {nodelist: nodes, edgelist: edges};

}

function createNode(name, h){
    return {id: nodes.length + 1, label: `${name}${h != -1 ? `\n${h}` : ''}`, name: name, h: h};
}

function getNode(name){
    let res = null; 
    nodes.forEach(node => {
        if(node.name == name)
            res = node;
    });
    
    return res;
}

function getEdge(node1_name, node2_name){

    const idNode1 = getNode(node1_name).id;
    const idNode2 = getNode(node2_name).id;

    for(let i = 0; i < edges.length; i++){
        const edge = edges.get(i + 1);
        if(edge.from == idNode1 && edge.to == idNode2)
            return edge;
        if(edge.from == idNode2 && edge.to == idNode1)
            return edge;
    }

    return null;
}

function createEdge(u, v, w){
    return {from: getNode(u).id, to: getNode(v).id, label: w, id: edges.length + 1}
}

function addNode(newNode){
    if(getNode(newNode.name) == null){
        nodes.add(createNode(newNode.name, newNode.h))
    }
}

function addEdge(edge){
    if(getEdge(edge.u, edge.v) == null){
        edges.add(createEdge(edge.u, edge.v, edge.w));
    }
}

function highlightNodesAndEdges(nodeIds) {
    // Đặt màu sắc mặc định cho tất cả các nút và cung
    const defaultNodeColor = '#e8e8e8';
    const defaultEdgeColor = '#848484';

    // Cập nhật màu sắc cho tất cả các nút
    const allNodes = nodes.get();
    allNodes.forEach(node => {
        nodes.update({ id: node.id, color: { background: defaultNodeColor, border: '#e8e8e8' } });
    });

    // Cập nhật màu sắc cho tất cả các cung
    const allEdges = edges.get();
    allEdges.forEach(edge => {
        edges.update({ id: edge.id, color: { color: defaultEdgeColor } });
    });

    // Làm nổi bật các nút và cung được chỉ định
    nodeIds.forEach(id => {
        // Làm nổi bật nút
        nodes.update({
            id: id,
            color: {
                background: '#FFD700', // Màu nền nổi bật (vàng)
                border: '#FFD700' // Màu viền nổi bật (vàng)
            }
        });

        // Làm nổi bật các cung tương ứng
        allEdges.forEach(edge => {
            if (edge.from === id || edge.to === id) {
                edges.update({
                    id: edge.id,
                    color: {
                        color: '#FFD700' // Màu cung nổi bật (vàng)
                    }
                });
            }
        });
    });
}
