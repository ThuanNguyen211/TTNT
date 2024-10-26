var nodes = new vis.DataSet();
var edges = new vis.DataSet();

const container = document.getElementById('network');
const data = { nodes: nodes, edges: edges };
const options = {
    nodes: {
        shape: 'circle',       // Node shape ('circle', 'square', 'triangle', etc.)
        size: 25,              // Node size
        color: {
            border: '#2B7CE9', // Border color of the node
            background: '#97C2FC', // Background color of the node
            hover: {
                border: '#2B7CE9',
                background: '#D2E5FF'
            }
        },font: {
            vadjust: 0, // Moves the label above the node
            size: 14,
            multi: true,
            color: '#333'
        },
        margin: {
            top: 15,
            bottom: 15,
            left: 8
        },
    },
    edges: {
        font: { color: '#000000', size: 18, align: 'horizontal' },
        color: {
            color: '#848484',  // Edge color
            hover: '#848484'
        },
        width: 2              // Edge thicknes
    },
    physics: {
        enabled: true,         // Enable or disable physics animation
        solver: 'forceAtlas2Based', // Physics solver ('barnesHut', 'forceAtlas2Based', etc.)
        stabilization: { iterations: 100 } // Iterations for physics stabilization
    },
    interaction: {
        hover: true,           // Enable hover effect on nodes
        dragNodes: true,       // Allow nodes to be dragged
        zoomView: true,        // Enable zooming with mouse wheel
        dragView: false, // Ngăn kéo nền
        zoomView: false  // Ngăn phóng to/thu nhỏ
    },
    layout: {
        randomSeed: 2,         // Seed for layout randomness (helps keep consistent layout)
        improvedLayout: true   // Uses an improved algorithm for better layout
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
        if(document.querySelector('#algorithm').value == '1') {
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