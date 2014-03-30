Sub Test()
    ' Recorded 2014/3/29
    Dim s1 As Shape
    
    // 按默认页面大小 A4(210x297建立一个矩形),下行语句的数字单位是英寸 11.692913 x 8.267717 就是A4
    Set s1 = ActiveLayer.CreateRectangle(0#, 11.692913, 8.267717, 0#) 
    
    // 填充颜色无，轮廓颜色 K100，线条粗细0.2mm (0.007874英寸)
    s1.Fill.ApplyNoFill
    s1.Outline.SetProperties 0.007874, OutlineStyles(0), CreateCMYKColor(0, 0, 0, 100), ArrowHeads(0), ArrowHeads(0), cdrFalse, cdrFalse, cdrOutlineButtLineCaps, cdrOutlineMiterLineJoin, 0#, 100, MiterLimit:=5#

    // 物件中心基准, 先把宽度设定为 100mm(英寸3.937008)， 再把高度也设定为100mm  
    ActiveDocument.ReferencePoint = cdrCenter
    s1.SetSize 3.937008, 11.692913
    s1.SetSize 3.937008, 3.937008
    
    AppWindow.WindowState = cdrWindowMinimized
    AppWindow.WindowState = cdrWindowNormal
    
    // 物件旋转 30度，轮廓线1mm ,轮廓颜色 M100Y100
    s1.Rotate 30#
    s1.Outline.SetProperties 0.03937
    s1.Outline.SetProperties Color:=CreateCMYKColor(0, 100, 100, 0)
End Sub
