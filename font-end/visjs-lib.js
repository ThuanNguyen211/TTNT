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

function drawNetwork(){

    const inputForm = document.getElementById('inputform');
    const edges_input = inputForm.getElementsByClassName('edge');

    for (let i = 0; i < edges_input.length; i++) {
        const edge = edges_input[i];

        // Lấy các giá trị input trong phần tử con 'edge'
        const source1 = edge.querySelector('input[name="source1"]').value;
        const h1 = edge.querySelector('input[name="h1"]').value;
        const source2 = edge.querySelector('input[name="source2"]').value;
        const h2 = edge.querySelector('input[name="h2"]').value;
        const weight = edge.querySelector('input[name="weight"]').value;

        console.log(nodes.length);
        let id = 1;
        if(nodes.length > 0)
            id = nodes.get(nodes.length) + 1;

        const node1 = {id: id++, label: `${source1}\n${h1}`, h: h1};
        const node2 = {id: id, label: `${source2}\n${h2}`, h: h2};

        nodes.add(node1);
        nodes.add(node2);
        edges.add({from: node1.id, to: node2.id, label: weight});
    }

}