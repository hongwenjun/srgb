// Group By Distance (Adobe Illustrator Script)
// ============================================
//
// Adobe Illustrator script to group selected items by distance value.
//
// JavaScript Script for Adobe Illustrator.
// Tested with Adobe Illustrator CC 2014 & CC 2015, Windows 10 (64-bit) & OSX Yosemite.
// This script provided "as is" without warranty of any kind.
// Free to use and distribute.
//
// Version History
// 1.0.0 - 2015-08-15
//
// Copyright(c) 2016 Creatide / Sakari Niittymaa
// http://www.creatide.com
// hello@creatide.com
//
// The MIT License (MIT).
// Full licence can be found from end of script.


// Adobe Illustrator 脚本: 按距离值对所选项目进行分组。
// 源码网址: https://github.com/Creatide/AI_Group-By-Distance

#target illustrator

if (app.documents.length > 0) {


/* -------- */
/* SETTINGS */
/* -------- */
var distance = 0.0;
var expand = false;
var separatedLayers = false;


/* ---- */
/* INIT */
/* ---- */
var scriptName = "按距离分组";
var doc = app.activeDocument;
var selection = doc.selection;
var selectedPathItems = [];
var layerNameMaxLength = 40;
var newLayers = null;
var processedItemsNum = 0;
var newGroupsNum = 0;
var groupsBeforeNum = 0;
var newLayersNum = 0;
var warningValue = 100;

if ( selection.length > warningValue ) {
    alert("警示!\n您当前选择 " + selection.length + " 个物件. 对于数百个物件而言，脚本可能确实很慢，但肯定比手动速度还要快。 脚本也可能在此过程中崩溃，因此请记住先保存您的工作! 警示已经告知您。");
}


/* -------- */
/* GUI INIT */
/* -------- */
var win = null;
var processTxt = null;
var startTime = null;
var processTimeMs = null;

// Update loadinbar
function updateLoadingbar( text ) {
    var lbText = text.toString();
    processTxt.text = (processTxt.text.length > layerNameMaxLength) ? "处理中: " + lbText.substring(0, layerNameMaxLength) + "..." : "处理中: " + lbText;
    processTimeMs = formatTime( Date.now() - startTime );
    //processTime.text = "Time: " + Math.round( ((processTimeMs / (1000*60)) % 60 ) ) + ":" + Math.round( (( processTimeMs ) / 1000) % 60 ) + ":" +  Math.round(processTimeMs / 10.0) % 100;
    processTime.text = "时间: " + processTimeMs;
    processInfoItems.text = "处理物件: " + processedItemsNum;
    processInfoNewGroups.text = "新增群组: " + newGroupsNum + " (" + groupsBeforeNum + ")";
    processInfoNewLayers.text = "新增图层: " + newLayersNum;
    win.update();
}


/* ------------------------------- */
/* FORMAT MILLISECONDS TO READABLE */
/* ------------------------------- */
function formatTime ( ms ) {
    var days, hours, minutes, seconds, milliseconds;
    milliseconds = Math.floor((ms / 10) % 100);
    seconds = Math.floor(((ms / 1000) % 60));
    minutes = Math.floor((((ms / 1000) / 60) % 60));
    hours = Math.floor(((((ms / 1000) / 60) / 60) % 24));

    if (hours < 10) { hours   = "0" + hours; }
    if (minutes < 10) { minutes = "0" + minutes; }
    if (seconds < 10) { seconds = "0" + seconds; }
    if (milliseconds < 10) { milliseconds = "0" + milliseconds; }
    var time = hours + ':' + minutes + ':' + seconds + ':' + milliseconds;
    return time;
}


/* ------------- */
/* RANGE NUMBERS */
/* ------------- */
Number.prototype.between = function (min, max) {
    return this > min && this < max;
};


/* ------------- */
/* MAIN FUNCTION */
/* ------------- */
function main() {

    startTime = Date.now();

    // Expand items
    if ( expand ) expandPathItems( selection ) ;

    // Get all items to array and break the groups
    getPathItems( selection );

    // Sort items by X & Y position
    //selectedPathItems.sort(sortByX);
    //selectedPathItems.sort(sortByY);

    // Main array for sub-arrays
    var mainArray = [];

    // Loop items for arrays
    for ( k = 0; k < selectedPathItems.length; k++ ) {

        // Update loadingbar
        updateLoadingbar( selectedPathItems[k] );

        // Create sub-array for grouping
        var tempArray = [];

        // Loop through all items and add to array if position test return true
        for ( i = 0; i < selectedPathItems.length; i++ ) {

            // Test if X & Y position match -> Add to array
            if ( comparePos(selectedPathItems[k], selectedPathItems[i]) ) {

                // Create match index number
                var matchIndex = findDuplicates( selectedPathItems[i], mainArray );

                // Check if there is already items in arrays that have almost same positions
                if ( matchIndex ) {
                    mainArray[matchIndex].push(selectedPathItems[i]);
                // If not, then make new sub-array
                } else {
                    tempArray.push(selectedPathItems[i]);
                }
            }
        }

        // Add array to main array
        mainArray.push(tempArray);
    }

    // Loop arrays and make grouping if needed
    for (j = 0; j < mainArray.length; j++) {

        if (mainArray[j].length > 0) {

            //cleanArray(mainArray[j]);
            groupItems(mainArray[j]);

            // Update loadinbar
            updateLoadingbar( mainArray[j] );
        }
    }

    processTxt.text = "完成!";
    app.redraw();
    //win.close();

}


/* --------------- */
/* FIND DUPLICATES */
/* --------------- */
function findDuplicates( item, arr ) {

    var totalmatches = 0;
    var foundIndex = false;

    // Loop main array
    for ( var i = 0; i < arr.length; i++ ) {

        // Loop sub-array
        for ( var j = 0; j < arr[i].length; j++ ) {

            // Test if there is almost same position values
            if ( comparePos(item, arr[i][j]) ) {
                foundIndex = i;
                break;
            }
        }
    }

    return foundIndex;
}


// ---------------------------------- //
// RECURSIVE LOOP ITEMS TO MAIN ARRAY //
// ---------------------------------- //
function getPathItems( item ) {

    for (var i = 0; i < item.length; i++) {

        // Update loadingbar
        updateLoadingbar( "Collecting Items" );

        if (item[i].groupItems) {

            groupsBeforeNum++;
            getPathItems(item[i].groupItems);
            getPathItems(item[i].pathItems);
            getPathItems(item[i].compoundPathItems);
            getPathItems(item[i].textFrames);

        } else if (item[i].typename == "PathItem") {

            selectedPathItems.push(item[i]);

        } else if (item[i].typename == "CompoundPathItem") {

            selectedPathItems.push(item[i]);

        } else if (item[i].typename == "TextFrame") {

            selectedPathItems.push(item[i]);
        }

        processedItemsNum++;
    }
}


/* ---------------- */
/* EXPAND PATHITEMS */
/* ---------------- */
function expandPathItems( arr ) {

    for ( i = 0; i < arr.length; i++ ) {

        app.executeMenuCommand ('Expand3');
        //app.executeMenuCommand ('expandStyle');

        // Update loadingbar
        updateLoadingbar( "Expand " + arr[i] );
    }
}


/* ----------- */
/* CLEAN ARRAY */
/* ----------- */
function cleanArray( arr ){

    var newArray = new Array();

    for(var i = 0; i < arr.length; i++){
        if (arr[i]){
            newArray.push(arr[i]);
        }
    }
    return newArray;
}


/* ------------------------ */
/* SORT ARRAY BY X POSITION */
/* ------------------------ */
function sortByX( a, b ) {

    // Update loadingbar
    updateLoadingbar( "排序中..." );

    var a_posX = a.geometricBounds[0];
    var b_posX = b.geometricBounds[0];

    if (a_posX < b_posX)
        return -1;
    if (a_posX > b_posX)
        return 1;
    return 0;
}


/* ------------------------ */
/* SORT ARRAY BY Y POSITION */
/* ------------------------ */
function sortByY( a, b ) {

    // Update loadingbar
    updateLoadingbar( "排序中..." );

    var a_posY = a.geometricBounds[1];
    var b_posY = b.geometricBounds[1];

    if (a_posY < b_posY)
        return -1;
    if (a_posY > b_posY)
        return 1;
    return 0;
}


/* ----------- */
/* MAKE GROUPS */
/* ----------- */
function groupItems( arr ) {

    if ( arr.length > 0) {

        // Update loadingbar
        updateLoadingbar( "分组中" );

        // Create new layer and group, then move group to new layer
        if ( separatedLayers ) {
            newLayers = doc.layers.add();
            newLayersNum++;
        } else {
            newLayers = newLayers == null ? newLayers = doc.layers.add() : newLayers;
        }

        var newLayer = newLayers;

        var grpItem = newLayer.groupItems.add();
        grpItem.move ( newLayer, ElementPlacement.PLACEATEND );

        newGroupsNum++;

        // Loop all items and place them under the group
        for ( i = 0; i < arr.length; i++ ) {
            arr[i].move(grpItem, ElementPlacement.INSIDE);
        }
    }
}


/* ----------------- */
/* COMPARE POSITIONS */
/* ----------------- */
function comparePos( a, b ) {

    // Get positions and calculate center points
    var a_posX = a.geometricBounds[0] + a.width / 2;
    var a_posY = (a.geometricBounds[1] * -1) + a.height / 2;
    var b_posX = b.geometricBounds[0] + b.width / 2;
    var b_posY = (b.geometricBounds[1] * -1) + b.height / 2;

    // Test X&Y axels
    var xBool = a_posX.between( (b_posX - b.width / 2) - distance, (b_posX + b.width / 2) + distance );
    var yBool = a_posY.between( (b_posY - b.height / 2) - distance, (b_posY + b.height / 2) + distance );

    if ( xBool && yBool ) { return true; } else { return false; }
}


// GUI
startGUI();
function startGUI() {

    win = new Window( "dialog", scriptName, undefined );

    // Style for Main Window
    win.orientation = "column";
    win.alignChildren = ["fill", "fill"];
    win.preferredSize = [250, 50];

    // GROUP: Distance Group
    var distanceGrp = win.add("panel", undefined, "距离");
    distanceGrp.alignChildren = ["fill", "fill"];
    distanceValue = distanceGrp.add( 'edittext { text: 10, characters: 3, justify: "center", active: true }' );

    // GROUP: Options Group
    var optionsGrp = win.add("panel", undefined, "选项");
    optionsGrp.orientation = "column";
    optionsGrp.margins = [10, 15, 10, 6];
    optionsGrp.alignChildren = ["fill", "fill"];

    // Checkboxes
    var cbExpand = optionsGrp.add ("checkbox", undefined, "扩展物件");
    cbExpand.value = expand;

    var cbSeparatedLayers = optionsGrp.add ("checkbox", undefined, "群组分隔图层");
    cbSeparatedLayers.value = separatedLayers;


    // GROUP: Process Group
    var processGrp = win.add("panel", undefined, "处理");
    processGrp.orientation = "column";
    processGrp.margins = [10, 15, 10, 10];
    processGrp.alignChildren = ["fill", "fill"];

    processTxt = processGrp.add ("statictext", undefined, "等待开始...");
    processTime = processGrp.add ("statictext", undefined, "");
    processInfoItems = processGrp.add ("statictext", undefined, "");
    processInfoNewGroups = processGrp.add ("statictext", undefined, "");
    processInfoNewLayers = processGrp.add ("statictext", undefined, "");

    // GROUP: Button Group
    var buttonGrp = win.add('group {orientation: "row"}');
    buttonGrp.alignChildren = ["fill", "fill"];

    // BTN: Close button
    var quitBtn = buttonGrp.add("button", undefined, "关闭");
    quitBtn.helpTip = "Press Esc to Close";
    quitBtn.onClick = function() {
        win.close();
    }

    // BTN: Start button
    var startBtn = buttonGrp.add("button", undefined, "开始");
    startBtn.helpTip = "Start grouping process";
    startBtn.onClick = function() {
        distance = Math.round(parseInt(distanceValue.text));
        expand = cbExpand.value;
        separatedLayers = cbSeparatedLayers.value;
        startBtn.enabled = false;
        main();
    }

    // Centering & Show Window
    win.center();
    win.show();
}

} else {
    alert("您没有打开任何文档!");
}

// The MIT License (MIT)
// =====================
//
// Copyright (c) 2016 Creatide / Sakari Niittymaa
// http://www.creatide.com
// hello@creatide.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
