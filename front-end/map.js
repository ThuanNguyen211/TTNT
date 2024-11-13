var nodes = new vis.DataSet();
var edges = new vis.DataSet();

const container = document.getElementById('network');
const data = { nodes: nodes, edges: edges };
const options = {
    nodes: {
        shape: 'circle',
        size: 30,
        color: {
            border: '#e8e8e8',
            background: '#e8e8e8',
            hover: { border: '#e8e8e8', background: '#e8e8e8' },
            highlight: { border: '#e8e8e8', background: '#e8e8e8' }
        },
        font: {
            face: 'Consolas',
            vadjust: 0,
            size: 14,
            multi: true,
            color: '#000000',
            align: 'center'
        },
        margin: {
            top: 20,
            bottom: 20,
            left: 20,
            right: 20
        },
    },
    edges: {
        font: {
            face: 'Consolas',
            color: '#000000',
            size: 14,
            align: 'horizontal'
        },
        color: {
            color: '#333',
        },
        width: 2
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
        
        let h1 = 0, h2 = 0;
        if(document.querySelector('input[name="number-selector"]:checked').value == 1) {
            h1 = -1;
            h2 = -1;
        } else {
            h1 = document.querySelector(`#h-${source1}`) == null ? -2 : document.querySelector(`#h-${source1}`).value;
            h2 = document.querySelector(`#h-${source2}`) == null ? -2 : document.querySelector(`#h-${source2}`).value;
            if(h1 == '' || h2 == ''){
                alert('Nhap heuristic khong hop le');
                return null;
            }
        }
        
        if(source1 && h1 != -2){
            const node1 = {name: source1, h: parseInt(h1)};
            if(!newNodeList.find(node => node1.name === node.name))
                newNodeList.push(node1);
            else {
                const index = newNodeList.findIndex(newNode => newNode.name === node1.name);
                newNodeList[index] = node1;
            }
        }

        if(source2 && h2 != -2){
            const node2 = {name: source2, h: parseInt(h2)};
            if(!newNodeList.find(node => node2.name === node.name))
                newNodeList.push(node2);
            else {
                const index = newNodeList.findIndex(newNode => newNode.name === node2.name);
                newNodeList[index] = node2;
            }
        }

        if(source1 != '' && source2 != '' && weight == ''){
            alert('Nhap cung khong hop le');
            return null;
        } else if (![source1, source2, weight].includes('')) {
            const newEdge = {u: source1, v: source2, w: weight};
            if(!newEdgeList.find(edge => (newEdge.u == edge.u && newEdge.v == edge.v) || (newEdge.v == edge.u && newEdge.u == edge.v)))
                newEdgeList.push(newEdge);
        }

    }

    newNodeList.forEach(newNode => addNode(newNode));
    newEdgeList.forEach(newEdge => addEdge(newEdge));

    return {nodelist: nodes, edgelist: edges};

}

function createNode(name, h){
    let newNodeId = 1;
    const allNodes = nodes.get();
    if(allNodes.length > 0)
        newNodeId = allNodes[allNodes.length - 1].id + 1;

    return {id: newNodeId, label: `${name}${h != -1 ? `\n${h}` : ''}`, name: name, h: h};
}

function upateNode(node){
    return {id: getNode(node.name).id, label: `${node.name}${node.h != -1 ? `\n${node.h}` : ''}`, h: node.h}
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

    let res = null;

    edges.get().forEach(edge => {
        if(edge.from == idNode1 && edge.to == idNode2)
            res = edge;
        if(edge.from == idNode2 && edge.to == idNode1)
            res = edge;
    });

    return res;
}

function createEdge(u, v, w){
    let newEdgeId = 1;
    const allEdges = edges.get();
    if(allEdges.length > 0)
        newEdgeId = allEdges[allEdges.length - 1].id + 1;
    return {from: getNode(u).id, to: getNode(v).id, label: w, id: newEdgeId}
}

function addNode(newNode){
    if(getNode(newNode.name) == null){
        nodes.add(createNode(newNode.name, newNode.h))
    } else nodes.update(upateNode(newNode));
}

function addEdge(edge){
    if(getEdge(edge.u, edge.v) == null){
        edges.add(createEdge(edge.u, edge.v, edge.w));
    }
}

function highlightNodesAndEdges(nodeIds, newColor) {

    // Đặt màu sắc mặc định cho tất cả các nút và cung
    const allNodes = nodes.get();
    const allEdges = edges.get();

    nodes.update({
        id: nodeIds[0],
        color: {
            background: newColor,
            border: newColor
        }
    });

    for(let i = 1; i < nodeIds.length; i++){

        nodes.update({
            id: nodeIds[i],
            color: {
                background: newColor,
                border: newColor
            }
        });

        allEdges.forEach(edge => {
            if (edge.from === nodeIds[i] && edge.to === nodeIds[i - 1] || edge.from === nodeIds[i - 1] && edge.to === nodeIds[i]) {
                edges.update({
                    id: edge.id,
                    color: {
                        color: newColor
                    }
                });
            }
        });

    }
}

function resetHighlightMap(){
    const allEdges = edges.get();
    const defaultColor = '#e8e8e8';

    allEdges.forEach(edge => {
        nodes.update({
            id: edge.from,
            color: {
                background: defaultColor,
                border: defaultColor
            }
        });

        nodes.update({
            id: edge.to,
            color: {
                background: defaultColor,
                border: defaultColor
            }
        });  

        edges.update({
            id: edge.id,
            color: {
                color: defaultColor
            }
        });
    });
}

function getNodeNameById(id){
    let res = '';
    nodes.get().forEach(node => {
        if(node.id == id){
            res = node.name;
            return;
        }
    });

    return res;
}

function deleteNode(nodeName){
    const inputForm = document.getElementById('inputform');
    const edges_input = inputForm.getElementsByClassName('edge');
    let sourceList = [];

    for (let i = 0; i < edges_input.length; i++) {
        const edge = edges_input[i];
        const source1 = edge.querySelector('input[name="node1"]').value;
        const source2 = edge.querySelector('input[name="node2"]').value;
        sourceList.push(source1);
        sourceList.push(source2);
    }

    if(sourceList.includes(nodeName)) return;
    
    const node = getNode(nodeName);
    if(node != null){
        nodes.remove(node.id);
        newNodeList = newNodeList.filter(newNode => newNode.name != nodeName);
        deleteEdgesFrom(node.id);
    }
}


function deleteEdgesFrom(nodeId) {
    const edgeListTmp = edges.get();
    edgeListTmp.forEach(edge => {
        if(edge.from == nodeId || edge.to == nodeId) { 
            const nodeNameFrom = getNodeNameById(edge.from);
            const nodeNameTo = getNodeNameById(edge.to);
                       
            edges.remove(edge.id);
            // Cập nhật newEdgeList để xóa tất cả các edge liên quan đến nodeId
            newEdgeList = newEdgeList.filter(newEdge => 
                newEdge.u != nodeNameFrom && newEdge.v != nodeNameTo &&
                newEdge.u != nodeNameTo && newEdge.v != nodeNameFrom
            );
        }
    });
}