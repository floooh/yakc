/** helpers */
function id(id) {
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

// drag-n-drop functions
function initDragAndDrop() {
    // add a drag'n'drop handler to the WebGL canvas
    id('canvas').addEventListener('dragover', onDragOver, false);
    id('canvas').addEventListener('drop', onDrop, false);
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
    id('nav').classList.toggle('toggle');
}

// boot into system UI functions
function toggle_systems_panel() {
    id('systems_panel').classList.toggle('hidden');
}
function boot_system(self, sys, os) {
    id('systems_panel').classList.add('hidden');
    yakc_boot(sys, os);
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
    [ 'Tetris', 'tetris.kcc', 'kcc', 'kc85_3'],
    [ 'Ladder', 'ladder-3.kcc', 'kcc', 'kc85_3'],
    [ 'Enterprise', 'enterpri.tap', 'kc_tap', 'kc85_3'],
    [ 'Chess', 'chess.kcc', 'kcc', 'kc85_3'],
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
    [ 'Exolon', 'exolon.z80', 'zx_z80', 'zxspectrum48'],
    [ 'Cyclone', 'cyclone.z80', 'zx_z80', 'zxspectrum48'],
    [ 'Boulderdash', 'boulderdash_zx.z80', 'zx_z80', 'zxspectrum48'],
    [ 'Bomb Jack', 'bombjack_zx.z80', 'zx_z80', 'zxspectrum48'],
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