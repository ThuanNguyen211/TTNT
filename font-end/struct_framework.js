let structs = {};

function registerStruct(name, structFormat) {
    structs[name] = structFormat;
}

function decodeStruct(name, memory, offset = 0) {
    // console.log(memory);
    
    var ret = {};
    let cursor = offset;
    var oldType = 'int';
    for (const [key, type] of Object.entries(structs[name])) {

        if (type == 'int') ret[key] = memory[cursor++];
        else {

            if(oldType != type) cursor += 3;

            let arr = [];
            for(var i = 0; i < ret[`${key}Length`]; i++){
                arr.push(memory[cursor + i]);
            }

            ret[key] = arr;

            cursor += arr.length;
            while(!memory[cursor++]);
        }
        oldType = type;

    }

    return ret;
}

function startDecodeStruct(name, ptr, memory) {
    return decodeStruct(name, new Uint32Array(memory.buffer, ptr, 100));
}

function encodeArray(arr, len){
    var ptr = exports.wasmmalloc(len * 4);
    var out = new Uint32Array(memory.buffer, ptr);
    for(var i = 0; i < len; i++){
        out[i] = arr[i];
    }
    return ptr;
}

var memory = new WebAssembly.Memory({
    initial: 512,
    maximum: 1024
});

var exports;
WebAssembly.instantiateStreaming(fetch('../algorithm/algorithm.wasm'), {
    js: { mem: memory },
    env: {
        emscripten_resize_heap: memory.grow,
        _emscripten_memcpy_js: function(dest, src, num) {
            try {
                const srcBytes = new Uint32Array(memory.buffer, src, num);
                const destBytes = new Uint32Array(memory.buffer, dest, num);
                destBytes.set(srcBytes);
            } catch (error) {
                console.error("Error copying memory:", error);
            }
        }
    }
}).then(result => {
    exports = result.instance.exports;
    memory = exports.memory;
}).catch(error => console.error("WebAssembly instantiation failed:", error));

registerStruct("outputStruct", {
    "pathToGoalLength": "int",
    "traversalOrderLength": "int",
    "cost": "int",
    "pathToGoal": "int*",
    "traversalOrder": "int*"
});

function run(nodes, edges){

    if(nodes.length == 0 || edges.length == 0){
        alert('Vui lòng nhập bản đồ');
        return;
    }

    var node_arr = [];
    var h_arr = [];
    var u_arr = [];
    var v_arr = [];
    var g_arr = [];

    var start, goal;
    for(var i = 1; i <= nodes.length; i++){
        node_arr.push(nodes.get(i).id);
        h_arr.push(nodes.get(i).h);
    }

    for(var i = 1; i <= edges.length; i++){
        u_arr.push(edges.get(i).from);
        v_arr.push(edges.get(i).to);
        g_arr.push(edges.get(i).label);
    }

    start = 1;
    goal = node_arr.length;

    console.log(node_arr);

    const node_ptr = encodeArray(node_arr, node_arr.length);
    const h_ptr = encodeArray(h_arr, h_arr.length);
    const u_ptr = encodeArray(u_arr, u_arr.length)
    const v_ptr = encodeArray(v_arr, v_arr.length)
    const g_ptr = encodeArray(g_arr, g_arr.length)

    const res_ptr = exports.AStarAlgorithm(
        node_ptr, h_ptr, node_arr.length,
        u_ptr, v_ptr, g_ptr, u_arr.length,
        start, goal
    );

    const res = startDecodeStruct('outputStruct', res_ptr, memory);

    exports.wasmfree(node_ptr);
    exports.wasmfree(h_ptr);
    exports.wasmfree(u_ptr);
    exports.wasmfree(v_ptr);
    exports.wasmfree(g_ptr);
    exports.wasmfree(res_ptr);

    return res;
}