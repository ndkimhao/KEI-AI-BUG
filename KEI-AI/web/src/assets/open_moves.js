var board_size = 15;

// === BOARD ===
// Preset graphics
var svg_lines = [
    '<line x1="50%" y1="0" x2="50%" y2="50%" style="stroke:rgb(110,110,110); stroke-width:2px;" />',
    '<line x1="0" y1="50%" x2="50%" y2="50%" style="stroke:rgb(110,110,110); stroke-width:2px;" />',
    '<line x1="50%" y1="50%" x2="50%" y2="100%" style="stroke:rgb(110,110,110); stroke-width:2px;" />',
    '<line x1="50%" y1="50%" x2="100%" y2="50%" style="stroke:rgb(110,110,110); stroke-width:2px;" />',

    '<line x1="35%" y1="50%" x2="65%" y2="50%" style="stroke:rgb(150,150,150); stroke-width:2px;" />' +
    '<line x1="50%" y1="35%" x2="50%" y2="65%" style="stroke:rgb(150,150,150); stroke-width:2px;" />',

    '<line x1="35%" y1="50%" x2="65%" y2="50%" style="stroke:rgb(255,0,0); stroke-width:2px;" />' +
    '<line x1="50%" y1="35%" x2="50%" y2="65%" style="stroke:rgb(255,0,0); stroke-width:2px;" />'];
var svg_circles = [
    // '<circle cx="50%" cy="50%" r="32%" fill="#fff" />',
    '<use xlink:href="#white_piece" />',
    //'<circle cx="50%" cy="50%" r="32%" fill="#333" />'
    '<use xlink:href="#black_piece" />'
];
var svg_number = [
    '<text x="50%" y="52%" fill="#333" alignment-baseline="middle" text-anchor="middle" font-weight="bold">',
    '<text x="50%" y="52%" fill="#fff" alignment-baseline="middle" text-anchor="middle" font-weight="bold">'
];
var colors = ['#bbb', '#777', '#eee'];

var tbl_board = document.getElementById('tbl_board');
var div_gamearea = document.getElementById('div_gamearea');

function init_board() {
    // Automatically adjust display size
    var container_size = panel_gamearea.offsetWidth - 32;

    // Control table size with cell size
    var cell_width = Math.floor(container_size / board_size);
    var cell_height = Math.floor(container_size / board_size);

    // No more blurry
    cell_width -= cell_width % 2;
    cell_height -= cell_height % 2;

    // Set containing div size
    div_gamearea.style.width = cell_width * board_size + 'px';
    div_gamearea.style.height = cell_height * board_size + 'px';

    // Initialize tbl_board
    tbl_board.innerHTML = "";
    for (var r = 0; r < board_size; r++) {
        var row = tbl_board.insertRow();
        for (var c = 0; c < board_size; c++) {
            // New cell and data
            var cell = row.insertCell();
            cell.r = r; cell.c = c;
            cell.board_data = 0;

            // Style
            cell.width = cell_width; cell.height = cell_height;
            cell.style.padding = '0';
            cell.style.verticalAlign = 'bottom';

            // Handle click event
            cell.addEventListener("click", tblBoardOnClick);
        }
    }
}

function tblBoardOnClick(e) {
    render_next();
}
// END BOARD

var board;
var cnt = 1;
function render_next() {
    init_board();
    board = [];
    $("#stt").text(cnt + " / 80"); cnt++;
    for (var i = 0; i < board_size; i++) {
        board.push([]);
        for (var j = 0; j < board_size; j++)
            board[i].push({});
    }

    // var colors = ['#bbb', '#777', '#eee'];
    var center = 4;
    var len1 = data[p++], len2 = data[p++];
    if (len1 == 0) {
        alert("hết cmnr");
        return;
    }
    for (var i = 0; i < len1; i++) {
        var x = data[p++], y = data[p++];
        board[center + x][center + y] = {
            color: 1 - i % 2,
            no: i + 1
        };
    }
    for (var i = 0; i < len2; i++) {
        var x = data[p++], y = data[p++];
        board[center + x][center + y] = {
            color: 0,
            no: len1 + 1,
            last: true
        };
    }

    for (var r = 0; r < board_size; r++) {
        for (var c = 0; c < board_size; c++) {
            var d = board[r][c];

            var cell = tbl_board.rows[r].cells[c];
            var svg = '<svg style="display: block; width: 100%; height: 100%;" xmlns:svg="http://www.w3.org/2000/svg" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">';

            // Grid (lines)
            if (r > 0) svg += svg_lines[0];
            if (c > 0) svg += svg_lines[1];
            if (r < board_size - 1) svg += svg_lines[2];
            if (c < board_size - 1) svg += svg_lines[3];

            // Pieces
            if (d.color !== undefined) {
                svg += svg_circles[d.color];
                if (d.last) svg += svg_lines[5];
                else svg += svg_number[d.color] + d.no + '</text>';
            }

            // Update node
            cell.innerHTML = svg + '</svg>';
        }
    }
}

var p = 0;
var data = [
    1, 8, 0, 0, -1, 0, 1, 0, 0, 1, 0, -1, 1, 1, -1, 1, 1, -1, -1, -1,
    2, 2, 0, 0, 2, 2, 1, 3, 3, 1,
    2, 8, 0, 0, 1, 0, -1, -1, 0, -1, 1, -1, 2, -1, -1, 1, 0, 1, 1, 1, 2, 1,
    2, 3, 0, 0, 1, 1, 0, 2, 2, 0, 1, 2,
    3, 3, 0, 0, 3, 2, 2, 1, 1, 0, 1, 1, 0, 1,
    3, 6, 0, 0, 1, 0, 2, 0, 1, 1, 1, -1, 0, 2, 0, -2, 2, 2, 2, -2,
    3, 1, 0, 0, 3, 1, 2, 2, 3, 3,
    3, 1, 0, 0, 3, 2, 2, 2, 1, 1,
    3, 1, 0, 0, 3, 3, 2, 2, 1, 1,
    3, 1, 0, 0, 2, 2, 1, 2, 0, 1,
    3, 1, 0, 0, 2, 1, 1, 2, 0, 2,
    3, 1, 1, 0, 0, 2, 1, 2, 1, 1,
    3, 1, 0, 0, 0, 3, 1, 2, 1, 1,
    3, 2, 0, 0, 0, 3, 0, 2, 1, 3, -1, 3,
    3, 2, 1, 0, 0, 3, 1, 2, 2, 3, 1, 1,
    3, 1, 0, 0, 0, 2, 1, 2, 1, 0,
    3, 4, 0, 0, 1, 1, 2, 2, 1, -1, -1, 1, 3, 1, 1, 3,
    3, 1, 0, 0, 2, 1, 2, 2, 1, 1,
    3, 1, 0, 1, 3, 0, 2, 0, 1, 1,
    3, 2, 0, 0, 2, 2, 1, 1, 2, 1, 1, 2,
    3, 1, 0, 0, 0, 1, 1, 2, 1, 1,
    3, 1, 0, 0, 2, 1, 1, 1, 2, 2,
    3, 1, 0, 0, 0, 2, 0, 1, 0, -1,
    3, 1, 1, 0, 0, 0, 1, 1, 1, 2,
    3, 1, 1, 2, 0, 0, 1, 1, 1, 3,
    3, 4, 1, 0, 0, 1, 1, 2, 0, 2, 0, 0, 2, 0, 2, 2,
    3, 1, 0, 0, 2, 0, 1, 1, 2, 2,
    3, 1, 0, 1, 0, 0, 1, 0, 1, 1,
    4, 1, 0, 0, 1, 1, 2, 1, 2, 2, 3, 3,
    4, 1, 1, 0, 0, 1, 2, 2, 1, 2, -1, 0,
    4, 1, 1, 0, 0, 0, 0, 2, 1, 1, -1, -1,
    4, 1, 1, 0, 0, 1, 2, 0, 1, 2, -1, 0,
    4, 1, 2, 0, 0, 0, 2, 1, 1, 1, 2, 2,
    4, 2, 2, 0, 0, 0, 2, 2, 1, 1, 0, -1, 1, -1,
    4, 1, 0, 0, 1, 1, 3, 1, 2, 2, 3, 3,
    4, 1, 2, 0, 0, 0, 0, 2, 1, 1, 2, 2,
    4, 1, 1, 0, 1, 1, 0, 2, 2, 2, 0, 0,
    4, 1, 1, 0, 0, 1, 1, 1, 1, 2, -1, 0,
    4, 1, 1, 0, 0, 1, 1, 3, 1, 2, -1, 0,
    5, 1, 0, 0, 1, 1, 1, 0, 1, 2, 0, 2, 0, 1,
    5, 1, 2, 0, 0, 2, 1, 1, 1, 2, 2, 2, 0, 0,
    5, 2, 0, 1, 1, 2, 2, 1, 3, 0, 0, 3, 1, 1, 0, 2,
    5, 1, 1, 0, 1, 2, 0, 1, 3, 2, 2, 1, 2, -1,
    5, 2, 1, 0, 0, 0, 0, 1, 2, 1, 1, 1, 1, 2, -1, 2,
    5, 2, 1, 1, 0, 0, 2, 1, 1, 3, 1, 2, 0, 1, 3, 1,
    5, 1, 1, 1, 0, 0, 2, 1, 0, 2, 3, 1, 0, 1,
    5, 1, 1, 0, 1, 1, 0, 1, 2, 1, 3, 2, 2, -1,
    5, 1, 0, 2, 1, 0, 2, 0, 2, 1, 1, 1, -1, 3,
    5, 2, 0, 0, 1, 1, 1, 2, 0, 1, 2, 1, 2, 0, 3, 0,
    5, 1, 1, 0, 0, 0, 0, 1, 1, 1, 2, 3, 2, 2,
    5, 1, 1, 1, 1, 0, 0, 2, 1, 3, 1, 2, 2, 2,
    5, 1, 1, 0, 0, 0, 1, 1, 1, 2, 2, 1, 1, -1,
    5, 1, 0, 0, 2, 1, 0, 1, 2, 3, 1, 2, 1, 3,
    5, 2, 2, 2, 0, 0, 1, 0, 1, 1, 0, 1, 2, 1, 1, 2,
    6, 1, 2, 1, 0, 0, 1, 3, 0, 1, 2, 3, 1, 2, 2, 0,
    6, 1, 2, 1, 0, 0, 2, 2, 0, 1, 2, 3, 1, 2, 2, 0,
    6, 1, 0, 2, 0, 0, 1, 1, 1, 2, 2, 2, 2, 1, 3, 0,
    6, 1, 0, 0, 1, 1, 3, 1, 2, 2, 4, 3, 3, 3, 2, 4,
    6, 1, 3, 0, 0, 0, 1, 1, 2, 1, 2, 2, 1, 2, -1, 0,
    6, 1, 0, 0, 1, 0, 2, 1, 3, 1, 3, 2, 2, 2, 1, -1,
    6, 1, 1, 0, 0, 0, 1, 1, 2, 1, 2, 2, 1, 2, 0, -1,
    6, 1, 1, 0, 0, 0, 0, 2, 1, 1, 3, 3, 2, 2, 1, -1,
    7, 1, 0, 1, 1, 1, 2, 0, 0, 2, 1, 2, 3, 4, 2, 3, 2, 1,
    7, 1, 1, 0, 0, 0, 0, 1, 1, 1, 2, 2, 1, 2, 1, 3, 2, 3,
    7, 1, 0, 0, 0, 1, 2, 0, 0, 3, 2, 1, 1, 3, 1, 2, 2, 3,
    7, 1, 1, 0, 0, 1, 0, 2, 2, 2, 2, 1, 3, 2, 5, 2, 3, 0,
    7, 1, 0, 0, 0, 1, 1, 0, 0, 3, 2, 1, 3, 2, 1, 2, 1, -1,
    7, 1, 3, 2, 2, 1, 2, 2, 1, 1, 1, 0, 0, 0, 0, 1, 1, 2,
    7, 1, 0, 0, 1, 1, 0, 1, 2, 2, 3, 2, 3, 3, 2, 3, 0, 2,
    8, 1, 1, 1, 3, 0, 0, 2, 3, 1, 1, 3, 2, 2, 2, 3, 1, 4, 3, 3,
    8, 1, 0, 2, 1, 1, 1, 2, 2, 0, 2, 2, 3, 0, 3, 1, 3, 2, 4, 0,
    9, 1, 0, 0, 2, 0, 1, 1, 1, 2, 2, 1, 1, 4, 3, 2, 4, 3, 2, 3, 0, 1,
    9, 1, 1, 1, 0, 0, 2, 2, 0, 2, 2, 1, 3, 1, 1, 2, 1, 3, 0, 3, 3, 0,
    9, 1, 0, 0, 2, 0, 1, 0, 1, 1, 0, 1, 2, 2, 3, 2, 3, 3, 2, 3, 0, 2,
    9, 1, 1, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 1, 2, 3, 1, 2, 0, 3, 1, 4,
    9, 1, 1, 1, 0, 0, 2, 2, 3, 2, 3, 3, 2, 3, 1, 4, 1, 3, 1, 5, 2, 4,
    9, 1, 0, 0, 0, 3, 0, 1, 1, 2, 0, 2, 2, 3, 3, 3, 3, 4, 2, 4, 1, 1,
    11, 1, 1, 0, 0, 0, 1, 1, 3, 2, 2, 2, 2, 3, 3, 3, 2, 4, 2, 5, 1, 5, 1, 4, 0, 3,
    11, 1, 1, 1, 0, 0, 2, 2, 3, 2, 3, 3, 1, 3, 2, 4, 1, 5, 3, 3, 3, 2, 5, 1, 4, 3,
    15, 1, 4, 0, 0, 0, 1, 1, 2, 3, 2, 2, 4, 2, 5, 3, 3, 4, 3, 3, 5, 5, 4, 3, 3, 6, 4, 5, 6, 5, 5, 4, 3, 1,
    0, 0
];

$(render_next);