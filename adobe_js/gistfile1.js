// https://gist.github.com/moluapple/1148519/raw/0ce5e59eeec6a67e264afd91a866139d4476f489/gistfile1.js
// 导出 PSD 或者 分层TIF文件 中文本到 文本文件

(function () {
	function layersText(oDoc) {
		var oLayers = oDoc.layers, text = [], i, j, file;
		function getTextLayers(oLayers, i) {
			for (i = 0; i < oLayers.length; i++) {
				oLayers[i].layers ? getTextLayers(oLayers[i].layers, j) : oLayers[i].kind == LayerKind.TEXT && text.push(oLayers[i].textItem.contents);
			}
		}
		getTextLayers(oLayers, i);
		file = new File(String(oDoc.fullName).replace(/(psd|tiff?)$/i, 'txt'));
		file.open('w');
		file.write(text.join('\n'));
		file.close();
	}
	var oDocs = app.documents, len = oDocs.length, d = 0;
	for (; d < len; d++) layersText(oDocs[d]);
})()
