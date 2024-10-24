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
            highlight: {
                border: '#2B7CE9',
                background: '#D2E5FF'
            },
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
            highlight: '#848484',
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
        zoomView: true         // Enable zooming with mouse wheel
    },
    layout: {
        randomSeed: 2,         // Seed for layout randomness (helps keep consistent layout)
        improvedLayout: true   // Uses an improved algorithm for better layout
    }
};

const network = new vis.Network(container, data, options);

function draw(){

    const inputForm = document.getElementById('inputform');
    const edges_input = inputForm.getElementsByClassName('edge');

    for (let i = 0; i < edges_input.length; i++) {
        const edge = edges_input[i];

        // Lấy các giá trị input trong phần tử con 'edge'
        const source1 = edge.querySelector('input[name="node1"]').value;
        const h1 = edge.querySelector('input[name="h1"]').value;
        const source2 = edge.querySelector('input[name="node2"]').value;
        const h2 = edge.querySelector('input[name="h2"]').value;
        const weight = edge.querySelector('input[name="weight"]').value;

        const node1 = getNode(source1, h1);
        const node2 = getNode(source2, h2);

        addEdge(node1, node2, weight);
    }
    
    return {nodelist: nodes, edgelist: edges};

}

function getNode(name, h){

    for(var i = 1; i <= nodes.length; i++){
        if(nodes.get(i).name == name)
            return nodes.get(i);
    }

    const newNode = {id: nodes.length + 1, label: `${name}\n${h}`, name: name, h: h};
    nodes.add(newNode);

    return newNode;
}

function addEdge(node1, node2, g){

    for(var i = 1; i <= edges.length; i++){
        if(edges.get(i).from == node1.id && edges.get(i).to == node2.id)
            return;
        if(edges.get(i).from == node2.id && edges.get(i).to == node1.id)
            return;
    }

    const newEdge = {from: node1.id, to: node2.id, label: g, id: edges.length + 1};
    edges.add(newEdge);
}