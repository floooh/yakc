/** helpers */
function $(id) {
    return document.getElementById(id);
}

/** C interface wrappers */
function yakc_boot(sys, os) {
    Module.ccall('yakc_boot', null, ['string','string'], [sys, os]);
}

function yakc_toggle_ui() {
    Module.ccall('yakc_toggle_ui', null, [], []);
}

function yakc_toggle_keyboard() {
    Module.ccall('yakc_toggle_keyboard', null, [], []);
}

function yakc_toggle_joystick() {
    Module.ccall('yakc_toggle_joystick', null, [], []);
}

function yakc_power() {
    Module.ccall('yakc_power', null, [], []);
}

function yakc_reset() {
    Module.ccall('yakc_reset', null, [], []);
}

function yakc_get_system() {
    return Module.ccall('yakc_get_system', 'string', [], []);
}

/** emscripten wrapper page Javascript functions **/

var loaded = false;
var Module = {
    preRun: [],
    postRun: [
        function() {
            initDragAndDrop();    
        },
    ],
    print: (function() {
        return function(text) {
            text = Array.prototype.slice.call(arguments).join(' ');
            console.log(text);
        };
    })(),
    printErr: function(text) {
        text = Array.prototype.slice.call(arguments).join(' ');
        console.error(text);
    },
    canvas: (function() {
        var canvas = $('canvas');
        canvas.addEventListener("webglcontextlost", function(e) { alert('FIXME: WebGL context lost, please reload the page'); e.preventDefault(); }, false);
        return canvas;
    })(),
    setStatus: function(text) {
        console.log("status: " + text);
    },
};

window.onerror = function(event) {
    console.log("onerror: " + event);
};

function callAsEventHandler(func_name) {
    // this is some hackery to make the browser module believe that it
    // is running in an event handler
    var eventHandler = { allowsDeferredCalls: true };
    ++JSEvents.inEventHandler;
    JSEvents.currentEventHandler = eventHandler;
    Module.cwrap(func_name)()
    --JSEvents.inEventHandler;
}

// drag-n-drop functions
function initDragAndDrop() {
    // add a drag'n'drop handler to the WebGL canvas
    $('canvas').addEventListener('dragover', onDragOver, false);
    $('canvas').addEventListener('drop', onDrop, false);
}

function onDragOver(e) {
    e.preventDefault();
}

function onDrop(dropEvent) {
    dropEvent.preventDefault();
    var files = dropEvent.dataTransfer.files;
    if (files.length > 0) {
        var file = files[0]
        console.log('--- dropped');
        console.log('  name: ' + file.name);
        console.log('  type: ' + file.type);
        console.log('  size: ' + file.size);

        // load the file content (ignore big files)
        if (file.size < 256000) {
            var reader = new FileReader();
            reader.onload = function(loadEvent) {
                console.log('file loaded!')
                var content = loadEvent.target.result;
                if (content) {
                    console.log('content length: ' + content.byteLength);
                    var dataView = new DataView(content)
                    for (i = 0; i < 128; i++) {
                        console.log(dataView.getUint8(i).toString(16) + ' ');
                    }
                    var uint8Array = new Uint8Array(content);
                    Module.ccall('emsc_pass_data',  // C function name
                        null,       // return type (void)
                        ['string', 'array', 'number'],  // name, data, size
                        [file.name, uint8Array, uint8Array.length]);
                }
                else {
                    console.warn('load result empty!');
                }
            };
            reader.readAsArrayBuffer(file);
        }
        else {
            console.warn('ignoring dropped file because it is too big')
        }
        
    }
    console.log('onDrop called!')
}

// toggle the nav bar
function nav_toggle() {
    $('nav').classList.toggle('toggle');
}

// boot into system UI functions
function toggle_systems_panel() {
    $('systems_panel').classList.toggle('hidden');
}
function boot_system(self, sys, os) {
    $('systems_panel').classList.add('hidden');
    yakc_boot(sys, os);
}
