/** helpers */
function id(id) {
    return document.getElementById(id);
}

/** C interface wrappers */
function yakc_boot(sys, os) {
    Module.ccall('yakc_boot', null, ['string','string'], [sys, os]);
}

function yakc_toggle_ui() {
    return Module.ccall('yakc_toggle_ui', 'int', null, null);
}

function yakc_toggle_keyboard() {
    return Module.ccall('yakc_toggle_keyboard', 'int', null, null); 
}

function yakc_toggle_joystick() {
    return Module.ccall('yakc_toggle_joystick', 'int', null, null);
}

function yakc_toggle_crt() {
    return Module.ccall('yakc_toggle_crt', 'int', null, null);
}

function yakc_power() {
    Module.ccall('yakc_power', null, null, null);
}

function yakc_reset() {
    Module.ccall('yakc_reset', null, null, null);
}

function yakc_get_system() {
    return Module.ccall('yakc_get_system', 'string', null, null);
}

function yakc_quickload(name, filename, filetype, system) {
    Module.ccall('yakc_quickload', null,
        ['string','string','string','string'],
        [name, filename, filetype, system]);
}

/** emscripten wrapper page Javascript functions **/

var loaded = false;
var Module = {
    preRun: [],
    postRun: [
        function() {
            initDragAndDrop();
            initWebAudio();
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
        var canvas = id('canvas');
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

// in some browsers, WebAudio starts suspended and must be activated in
// an user input event handler
function initWebAudio() {
    id('canvas').addEventListener('click', resume_webaudio);
    id('canvas').addEventListener('touchstart', resume_webaudio);
    document.addEventListener('keydown', resume_webaudio);
}

function resume_webaudio() {
    if (SDL.audioContext) {
        if (SDL.audioContext.state === 'suspended') {
            SDL.audioContext.resume();
        }
    }
}

// drag-n-drop functions
function initDragAndDrop() {
    // add a drag'n'drop handler to the WebGL canvas
    id('load_panel_container').addEventListener('dragenter', load_dragenter, false);
    id('load_panel_container').addEventListener('dragleave', load_dragleave, false);
    id('load_panel_container').addEventListener('dragover', load_dragover, false);
    id('load_panel_container').addEventListener('drop', load_drop, false);
}

function load_dragenter(e) {
    e.stopPropagation();
    e.preventDefault();
}

function load_dragleave(e) {
    e.stopPropagation();
    e.preventDefault();
    id('load_panel_container').classList.remove('highlight');
}

function load_dragover(e) {
    id('load_panel_container').classList.add('highlight');
    e.stopPropagation();
    e.preventDefault();
}

function load_drop(e) {
    e.stopPropagation();
    e.preventDefault();
    id('load_panel_container').classList.remove('highlight');
    toggle_load_panel();
    load_file(e.dataTransfer.files);
}

// load a file from the filepicker 
function load_file(files) {
    if (files.length > 0) {
        var file = files[0];
        console.log('--- load file:');
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
                    var uint8Array = new Uint8Array(content);
                    var res = Module.ccall('yakc_loadfile',  // C function name
                        'int',
                        ['string', 'array', 'number'],  // name, data, size
                        [file.name, uint8Array, uint8Array.length]);
                    if (res == 0) {
                        console.warn('yakc_loadfile() failed!');
                    } 
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
}

// toggle the nav bar
function nav_toggle() {
    id('nav').classList.toggle('toggle');
}

// show/hide panels
function toggle_systems_panel() {
    id('systems_panel').classList.toggle('hidden');
    id('quickload_panel').classList.add('hidden')
    id('load_panel').classList.add('hidden');
}
function toggle_quickload_panel() {
    id('quickload_panel').classList.toggle('hidden');
    id('systems_panel').classList.add('hidden');
    id('load_panel').classList.add('hidden');
    clear_quickload_panel();
    populate_quickload_panel();
}

function toggle_load_panel() {
    id('load_panel').classList.toggle('hidden');
    id('quickload_panel').classList.add('hidden');
    id('systems_panel').classList.add('hidden');
    id('load_panel_container').classList.remove('highlight');
}

function boot_system(self, sys, os) {
    id('systems_panel').classList.add('hidden');
    yakc_boot(sys, os);
}

function match_system(sys, pattern) {
    if (pattern == 'any_kc85') {
        return ['kc85_2','kc85_3','kc85_4'].indexOf(sys) != -1;
    }
    else if (pattern == 'any_z1013') {
        return ['z1013_01','z1013_16','z1013_64'].indexOf(sys) != -1;
    }
    else if (pattern == 'any_z9001') {
        return ['z9001', 'kc87'].indexOf(sys) != -1;
    }
    else if (pattern == 'any_zx') {
        return ['zxspectrum48k', 'zxspectrum128k'].indexOf(sys) != -1;
    }
    else if (pattern == 'any_cpc') {
        return ['cpc464', 'cpc6128', 'kccompact'].indexOf(sys) != -1;
    }
    else {
        return sys == pattern;
    }
}

function company_by_system(sys) {
    if (['kc85_2','kc85_3','kc85_4','kccompact','any_kc85'].indexOf(sys) != -1) {
        return 'mpm';
    }
    else if (['z9001','kc87','any_z9001'].indexOf(sys) != -1) {
        return 'robotron-dresden';
    }
    else if (['z1013_01','z1013_16','z1013_64','any_z1013'].indexOf(sys) != -1) {
        return 'robotron-riesa';
    }
    else if (['zxspectrum48k','zxspectrum128k','any_zx'].indexOf(sys) != -1) {
        return 'sinclair';
    }
    else if (['cpc464','cpc6128','any_cpc'].indexOf(sys) != -1) {
        // FIXME: not quite right, might trigger on KC Compact
        return 'amstrad';
    }
    else if (['acorn_atom','any_acorn'].indexOf(sys) != -1) {
        return 'acorn';
    }
    else {
        return 'unknown';
    }
}

function clear_quickload_panel() {
    var list = id('quickload-list');    
    while (list.hasChildNodes()) {
        list.removeChild(list.lastChild);
    }
}

function create_quickload_handler(name, filename, filetype, system) {
    return function() { 
        id('quickload_panel').classList.add('hidden');
        yakc_quickload(name, filename, filetype, system); };
}

function populate_quickload_panel() {
    var sys = yakc_get_system();
    var list = id('quickload-list');
    // a close button
    var li = document.createElement('li');
    var text = document.createTextNode('Back...');
    li.appendChild(text);
    li.className = 'panel-list-item';
    li.onclick = toggle_quickload_panel;
    list.appendChild(li);
    for (var i = 0; i < software.length; i++) {
        var item = software[i];
        if (match_system(sys, item[3])) {
            var li = document.createElement('li');
            var text = document.createTextNode(item[0]);
            li.appendChild(text);
            li.className = 'panel-list-item ' + company_by_system(item[3]);
            li.onclick = create_quickload_handler(item[0], item[1], item[2], item[3]);
            list.appendChild(li);
        }
    }
}

// quick toggle handlers
function ui_toggle_ui() {
    if (yakc_toggle_ui()) {
        id('ui_btn').classList.add('enabled');
    }
    else {
        id('ui_btn').classList.remove('enabled');
    }
}
function ui_toggle_keyboard() {
    if (yakc_toggle_keyboard()) {
        id('kbd_btn').classList.add('enabled');
    }
    else {
        id('kbd_btn').classList.remove('enabled');
    }
}
function ui_toggle_joystick() {
    if (yakc_toggle_joystick()) {
        id('joy_btn').classList.add('enabled');
    }
    else {
        id('joy_btn').classList.remove('enabled');
    }
}
function ui_toggle_crt() {
    if (yakc_toggle_crt()) {
        id('crt_btn').classList.add('enabled');
    }
    else {
        id('crt_btn').classList.remove('enabled');
    }
}
function ui_open_filepicker() {
    id('filepicker').click();
}

// the software browser classList
var software = [
    // name, filename, filetype, system
    [ 'Pengo', 'pengo.kcc', 'kcc', 'kc85_3'],
    [ 'Pengo', 'pengo4.kcc', 'kcc', 'kc85_4'],
    [ 'Cave', 'cave.kcc', 'kcc', 'kc85_3'],
    [ 'House', 'house.kcc', 'kcc', 'kc85_3'],
    [ 'House', 'house4.tap', 'kc_tap', 'kc85_4'],
    [ 'Jungle', 'jungle.kcc', 'kcc', 'kc85_3'],
    [ 'Jungle', 'jungle4.tap', 'kc_tap', 'kc85_4'],
    [ 'Pacman', 'pacman.kcc', 'kcc', 'kc85_3'],
    [ 'Breakout', 'breakout.kcc', 'kcc', 'kc85_3'],
    [ 'Mad Breakin', 'breakin.853', 'kcc', 'kc85_3'],
    [ 'Boulderdash', 'boulder3.tap', 'kc_tap', 'kc85_3'],
    [ 'Boulderdash', 'boulder4.tap', 'kc_tap', 'kc85_4'],
    [ 'Digger', 'digger3.tap', 'kc_tap', 'kc85_3'],
    [ 'Digger', 'digger4.tap', 'kc_tap', 'kc85_4'],
    [ 'Tetris', 'tetris.kcc', 'kcc', 'kc85_4'],
    [ 'Ladder', 'ladder-3.kcc', 'kcc', 'kc85_3'],
    [ 'Enterprise', 'enterpri.tap', 'kc_tap', 'any_kc85'],
    [ 'Chess', 'chess.kcc', 'kcc', 'any_kc85'],
    [ 'Testbild', 'testbild.kcc', 'kcc', 'kc85_3'],
    [ 'Demo1', 'demo1.kcc', 'kcc', 'kc85_4'],
    [ 'Demo2', 'demo2.kcc', 'kcc', 'kc85_4'],
    [ 'Demo3', 'demo3.kcc', 'kcc', 'kc85_4'],
    [ 'Tiny-Basic 3.01', 'tinybasic-3.01.z80', 'kc_z80', 'z1013_01'],
    [ 'KC-Basic', 'kc_basic.z80', 'kc_z80', 'any_z1013'],
    [ 'Z1013 Forth', 'z1013_forth.z80', 'kc_z80', 'any_z1013'],
    [ 'Boulderdash', 'boulderdash_1_0.z80', 'kc_z80', 'z1013_16'],
    [ 'Boulderdash', 'boulderdash_1_0.z80', 'kc_z80', 'z1013_64'],
    [ 'Demolation', 'demolation.z80', 'kc_z80', 'any_z1013'],
    [ 'Cosmic Ball', 'cosmic_ball.z80', 'kc_z80', 'z1013_01'],
    [ 'Galactica', 'galactica.z80', 'kc_z80', 'any_z1013'],
    [ 'Mazogs', 'mazog_deutsch.z80', 'kc_z80', 'any_z1013'],
    [ 'Monitor ZM30', 'zm30.kcc', 'kcc', 'any_z9001'],
    [ 'Forth F83', 'F83_COM.TAP', 'kc_tap', 'any_z9001'],
    [ 'Arkanoid', 'arkanoid.sna', 'cpc_sna', 'any_cpc'],
    [ "Ghosts'n'Goblins", 'ghosts_n_goblins.sna', 'cpc_sna', 'any_cpc'],
    [ 'Gryzor', 'gryzor.sna', 'cpc_sna', 'cpc6128'],
    [ 'Dragon Ninja', 'dragon_ninja.sna', 'cpc_sna', 'any_cpc'],
    [ 'Head over Heels', 'head_over_heels.sna', 'cpc_sna', 'any_cpc'],
    [ 'Boulderdash', 'boulder_dash.sna', 'cpc_sna', 'any_cpc'],
    [ 'Bomb Jack', 'bomb_jack.sna', 'cpc_sna', 'any_cpc'],
    [ 'Chase HQ', 'chase_hq.sna', 'cpc_sna', 'any_cpc'],
    [ 'Cybernoid', 'cybernoid.sna', 'cpc_sna', 'any_cpc'],
    [ 'Fruity Frank', 'fruity_frank.sna', 'cpc_sna', 'kccompact'],
    [ 'Fruity Frank', 'fruity_frank.sna', 'cpc_sna', 'cpc6128'],
    [ 'Ikari Warriors', 'ikari_warriors.sna', 'cpc_sna', 'any_cpc'],
    [ '1943', "1943.sna", 'cpc_sna', 'any_cpc'],
    [ '1942', '1942.tap', 'cpc_tap', 'any_cpc'],
    [ "Ghosts'n'Goblins", 'ghostsng.tap', 'cpc_tap', 'any_cpc'],
    [ 'Tir Na Nog', 'tirnanog.tap', 'cpc_tap', 'any_cpc'],
    [ 'Back to Reality', 'backtore.tap', 'cpc_tap', 'any_cpc'],
    [ 'ASSMON', 'assmon.tap', 'cpc_tap', 'any_cpc'],
    [ 'KC Pascal', 'kcpascal.tap', 'cpc_tap', 'any_cpc'],
    [ 'Bombjack 2', 'bombjac1.tap', 'cpc_tap', 'any_cpc'],
    [ 'Beverly Hills Cop', 'beverlyh.tap', 'cpc_tap', 'any_cpc'],
    [ 'Biff', 'biff.tap', 'cpc_tap', 'any_cpc'],
    [ 'Bubbler', 'bubbler.tap', 'cpc_tap', 'any_cpc'],
    [ 'Boulderdash 4', 'boulder1.tap', 'cpc_tap', 'any_cpc'],
    [ 'Combat Zone', 'combatzo.tap', 'cpc_tap', 'any_cpc'],
    [ 'Commandos', 'commando.tap', 'cpc_tap', 'any_cpc'],
    [ 'Curse of Sherwood', 'curseofs.tap', 'cpc_tap', 'any_cpc'],
    [ 'Kingdom of Speldom', 'kingdomo.tap', 'cpc_tap', 'any_cpc'],
    [ 'Haunted Hedges', 'hauntedh.tap', 'cpc_tap', 'any_cpc'],
    [ '2088', '2088.tap', 'cpc_tap', 'any_cpc'],
    [ 'Exolon', 'exolon.z80', 'zx_z80', 'zxspectrum48k'],
    [ 'Cyclone', 'cyclone.z80', 'zx_z80', 'zxspectrum48k'],
    [ 'Boulderdash', 'boulderdash_zx.z80', 'zx_z80', 'zxspectrum48k'],
    [ 'Bomb Jack', 'bombjack_zx.z80', 'zx_z80', 'zxspectrum48k'],
    [ 'Arkanoid (broken)', 'arkanoid_zx128k.z80', 'zx_z80', 'zxspectrum128k'],
    [ 'Silkworm', 'silkworm_zx128k.z80', 'zx_zx80', 'zxspectrum128k'],
    [ 'Hello World!', 'atom_hello.txt', 'text', 'acorn_atom'],
    [ 'Text Mode Test', 'atom_alnum_test.txt', 'text', 'acorn_atom'],
    [ 'Graphics Mode Test', 'atom_graphics_test.txt', 'text', 'acorn_atom'],
    [ 'Atomic Memory Checker', 'atom_memcheck.txt', 'text', 'acorn_atom'],
    [ 'Keyboard Joystick Test', 'atom_kbdjoytest.txt', 'text', 'acorn_atom'],
    [ 'AtoMMC Joystick Test', 'atom_mmcjoytest.txt', 'text', 'acorn_atom'],
    [ 'Jet Set Willy', 'JSW.TAP', 'atom_tap', 'acorn_atom'],
    [ 'Atomic Chuckie Egg', 'CCHUCK.TAP', 'atom_tap', 'acorn_atom'],
    [ 'Hard Hat Harry', 'hardhatharry.tap', 'atom_tap', 'acorn_atom'],
    [ 'Jet Set Miner', 'cjetsetminer.tap', 'atom_tap', 'acorn_atom'],
    [ 'Dormann 6502 Test', 'dormann6502.tap', 'atom_tap', 'acorn_atom']
];