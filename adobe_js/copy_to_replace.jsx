var docRef = activeDocument;
// 判断选择物件2个以上
if (docRef.selection.length > 1) {
    // 定义选择物件
    mySelection = docRef.selection;

    // 最上层物件为替换源
    var sourceObj = docRef.selection[0];

    // 定义数组用来保存选择物件的左上角坐标
    var alterObjectArray = new Array(); 
    for (var i=0; i < mySelection.length; i++) {
        // PageItem.position  获得物件群组左上角坐标
        var sel_xy = mySelection[i].position
        alterObjectArray.push(sel_xy);
    }
    // 删除用来定位的下层物件
    for (var i=1; i < mySelection.length; i++) {
            mySelection[i].remove();
    }
    // PageItem.duplicate 复制对象, 需要一个相对对象定位
    var newGroup = sourceObj.parent.groupItems.add();
    for (var i=1; i < alterObjectArray.length; i++) {
        sourceObj.position = alterObjectArray[i];     // 设置替换物的左上角位置，达到替换目的
        sourceObj.duplicate(newGroup, ElementPlacement.PLACEATEND);
    }
  sourceObj.remove();
  
}