// 汉化增强版:蘭公子 2008.07.20  --只留打包图片功能，增快了速度  修正了CS2以上批量收集的问题
// 屏蔽掉文字转曲功能   屏蔽修改后保存功能   屏蔽更新图片
// 修改默认为只打包当前打开的Ai文档的链接图
// 开启文字转曲功能: 把第18行 最前面 "//" 删除就可以

$.level = 1;
ctrImg = 0;

DiyFolder = "R:\\CTP"                 // 可以使用自己定义的常用目录 "d:\\桌面\\输出"

// 有AI文档打开就打包当前文档的链接图

if (app.documents.length != 0) {
myFolder= Folder.selectDialog ("AICS链接图收集打包脚本--汉化增强版:蘭公子2008.07\n现在开始收集当前AI文档的链接图,请指定你要保存链接图的文件夹", DiyFolder);
	ExportLinks();  //收集链接图应用函数
	
	
// ConvertText();  // 开启文字转曲功能: 把本行最前面 "//" 删除就可以


EndInfo = "AICS链接图收集打包脚本--汉化增强版:蘭公子2008.07\n\nAI文档的链接图已经收集到指定的目录\n\nHttp://sRGB.net.cn  sRGB色彩联盟, QQ群:2471313\n用RGB颜色空间矫色，使你的图片色彩更丰富！\n\n①(批量打包链接图,先关闭所有的AI文件执行本脚本)\n②(文字转曲开启，请修改本脚本第18行)\n"
alert(EndInfo);   //显示完成信息
}


// 批量打包AI文件的链接图,请先关闭所有的AI文件
else   {

myScanFolder= Folder.selectDialog ("AICS链接图收集打包脚本--汉化增强版:蘭公子2008.07\n①来源：选择Adobe Illustrator文件(*.ai)的文件夹",DiyFolder);
myFolder= Folder.selectDialog ("AI CS链接图收集打包脚本\n②目标：选择保存打包链接图的文件夹 \n(一般直接点确定,保存图片到AI文件的相同目录里)", myScanFolder);
aiFiles = myScanFolder.getFiles("*.ai");

for (j=0;j<aiFiles.length;j++) {
	myFile = File(aiFiles[j]);
	docRef = open(myFile);
// 屏蔽掉文字转曲功能	ConvertText();
	ExportLinks();
		
// 屏蔽修改后保存功能
//	mySaveAsFile = File(myFolder+"/"+myFile.name);
//	mySaveAsFile.open("w",myFile.type,myFile.creator);
//	mySaveAsFile.close();
//	docRef.saveAs(mySaveAsFile);
//	docRef.close();
        docRef.close(SaveOptions.DONOTSAVECHANGES); //不保存关闭当前文档,解决高版本打开低版本文件更新
	
}
EndInfo = "AICS链接图收集打包脚本--汉化增强版:蘭公子2008.07\n\nAI文档的链接图已经收集到指定的目录\n\nHttp://sRGB.net.cn  sRGB色彩联盟, QQ群:2471313\n用RGB颜色空间矫色，使你的图片色彩更丰富！\n\n(刚才执行的是批量打包链接图功能，已经完成)"
alert(EndInfo);   //显示完成信息
myFolder.execute(); // 浏览目的文件夹
}



//  以下为应用函数

function ConvertText() {
	myDoc = app.activeDocument;
	while (myDoc.textFrames.length != 0) {myDoc.textFrames[0].createOutline();}
}

function ExportLinks() {
	for (i=0;i<app.activeDocument.placedItems.length;i++) {
		linkedFile = app.activeDocument.placedItems[i].file;
		newFile = File(myFolder+"/"+linkedFile.name);
		linkedFile.copy(newFile);
//屏蔽更新图片		app.activeDocument.placedItems[i].file = newFile;
	}

	for (i=0;i<app.activeDocument.rasterItems.length;i++) {
		if (app.activeDocument.rasterItems[i].embedded) continue;
		linkedFile = app.activeDocument.rasterItems[i].file;
		newFile = File(myFolder+"/"+linkedFile.name);
		linkedFile.copy(newFile);
//屏蔽更新图片		app.activeDocument.rasterItems[i].file = newFile;
	}

}
