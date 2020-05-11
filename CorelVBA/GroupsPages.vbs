#If VBA7 Then

    Private Declare PtrSafe Function ShellExecute Lib "shell32.dll" Alias "ShellExecuteA" (ByVal hwnd As Long, ByVal lpOperation As String, ByVal lpFile As String, ByVal lpParameters As String, ByVal lpDirectory As String, ByVal nShowCmd As Long) As Long
    Private Declare PtrSafe Function FindWindow Lib "user32" Alias "FindWindowA" (ByVal lpClassName As String, ByVal lpWindowName As String) As Long

#Else

    Private Declare Function ShellExecute Lib "shell32.dll" Alias "ShellExecuteA" (ByVal hwnd As Long, ByVal lpOperation As String, ByVal lpFile As String, ByVal lpParameters As String, ByVal lpDirectory As String, ByVal nShowCmd As Long) As Long
    Private Declare Function FindWindow Lib "user32" Alias "FindWindowA" (ByVal lpClassName As String, ByVal lpWindowName As String) As Long

#End If

Option Explicit


 Dim iHang, iLie, iPages As Integer     '定义行数(Y) 列数(X)
 Dim iYouyi, iXiayi As Single   '右移(R) 下移(B)
                                'txtHang, txtLie, txtYouyi, txtXiayi ,txtInfo
 Dim LogoFile As String         'Logo
 
 Dim s(1 To 1024) As Shape   '定义对象用于存放每页的群组
 Dim p As Page          '定义多页
 

'**** 主程序  执行
Private Sub cmdRun_Click()

 Dim x_M, y_M
 ActiveDocument.Unit = cdrMillimeter
 ActiveDocument.EditAcrossLayers = False    '跨图层编辑禁止
 
 For Each p In ActiveDocument.Pages
    p.Activate                    '激活每页
    p.Shapes.All.CreateSelection          '每页全选
    Set s(p.Index) = ActiveSelection.Group    '存放每页的群组
 Next p
 
 ActiveDocument.EditAcrossLayers = True     '跨图层编辑开启
 
  x_M = y_M = 0
  
  For Each p In ActiveDocument.Pages
    p.Activate
       
    s(p.Index).MoveToLayer ActivePage.DesktopLayer    '每页对象移动到桌面层
    s(p.Index).Move (iYouyi * x_M), -(300 + iXiayi * y_M) '排列对象  右偏移，下偏移
  
  y_M = y_M + 1
  
  If y_M = iLie Then
  x_M = x_M + 1
  y_M = 0
  End If
  
 Next p
 
 Unload Me '执行完成关闭
End Sub


'**** 主程序 副本 横排序
Private Sub cmdRunX_Click()

 Dim x_M, y_M
 ActiveDocument.Unit = cdrMillimeter
 ActiveDocument.EditAcrossLayers = False    '跨图层编辑禁止
 
 For Each p In ActiveDocument.Pages
    p.Activate                    '激活每页
    p.Shapes.All.CreateSelection          '每页全选
    Set s(p.Index) = ActiveSelection.Group    '存放每页的群组
 Next p
 
 ActiveDocument.EditAcrossLayers = True     '跨图层编辑开启
 
  x_M = y_M = 0
  
  For Each p In ActiveDocument.Pages
    p.Activate
       
    s(p.Index).MoveToLayer ActivePage.DesktopLayer    '每页对象移动到桌面层
    s(p.Index).Move (iYouyi * y_M), -(300 + iXiayi * x_M) '排列对象  右偏移，下偏移
  
  y_M = y_M + 1
  
  If y_M = iHang Then
  x_M = x_M + 1
  y_M = 0
  End If
  
 Next p
 
 Unload Me '执行完成关闭
End Sub


'*********** 初始化程序 ***************
Private Sub UserForm_Initialize()

 Dim s As Shape
ActiveDocument.Unit = cdrMillimeter '本文档单位为mm

 For Each p In ActiveDocument.Pages
 iPages = p.Index
 If iPages = 1 Then
  p.Activate
  p.Shapes.All.CreateSelection

 Set s = ActiveDocument.Selection
        If s.Shapes.Count = 0 Then
            MsgBox "当前文件第一页空白没有物件！"
            Exit Sub
        End If
 
 End If
 Next p
 

 txtLie.Text = 5
 txtHang.Text = Int(iPages / CInt(txtLie.Text) + 0.9)
 txtLie.Text = Int(iPages / CInt(txtHang.Text) + 0.9)
 
 iHang = CInt(txtHang.Text)
 iLie = CInt(txtLie.Text)
 
 
 iYouyi = Int(s.SizeWidth + 0.6)
 iXiayi = Int(s.SizeHeight + 0.6)
 
 txtYouyi.Text = iYouyi
 txtXiayi.Text = iXiayi
 
 
 
  LogoFile = Path & "GMS\sRGB.cz.cc"
  

 txtInfo.Text = "本文档共 " & iPages & " 页，首页物件尺寸(mm):" & s.SizeWidth & "×" & s.SizeHeight
  
End Sub



'帮助

Private Sub cmdHelp_Click()

WebHelp

txtInfo.Text = "点击访问 Http://sRGB.vicp.net 详细帮助,寻找更多的视频教程！"
txtInfo.ForeColor = &HFF0000
cmdHelp.Caption = "在线帮助"
cmdHelp.ForeColor = &HFF0000


End Sub


Private Sub LogoPic_MouseDown(ByVal Button As Integer, ByVal Shift As Integer, ByVal X As Single, ByVal Y As Single)
  
  If ExistsFile_UseFso(LogoFile) Then
    LogoPic.Picture = LoadPicture(LogoFile)   '换LOGO图
  End If
  
txtInfo.Text = "好靓的美女呀！想知道她的QQ号吗？欢迎访问 Http://sRGB.vicp.net "
 
UniteOne.Repaint
End Sub

Private Sub txtInfo_MouseDown(ByVal Button As Integer, ByVal Shift As Integer, ByVal X As Single, ByVal Y As Single)

 WebHelp

End Sub

'关闭
Private Sub cmdClose_Click()
Unload Me
End Sub


'VB限制文本框只能输入数字和小数点
Private Sub txtHang_KeyPress(ByVal KeyAscii As MSForms.ReturnInteger)
Dim Numbers As String
Numbers = "1234567890"
If InStr(Numbers, Chr(KeyAscii)) = 0 Then
KeyAscii = 0
End If
End Sub

Private Sub txtLie_KeyPress(ByVal KeyAscii As MSForms.ReturnInteger)
Dim Numbers As String
Numbers = "1234567890"
If InStr(Numbers, Chr(KeyAscii)) = 0 Then
KeyAscii = 0
End If
End Sub

Private Sub txtXiayi_KeyPress(ByVal KeyAscii As MSForms.ReturnInteger)
Dim Numbers As String
Numbers = "1234567890" + Chr(8) + Chr(46)
If InStr(Numbers, Chr(KeyAscii)) = 0 Then
KeyAscii = 0
End If
End Sub

Private Sub txtYouyi_KeyPress(ByVal KeyAscii As MSForms.ReturnInteger)
Dim Numbers As String
Numbers = "1234567890" + Chr(8) + Chr(46)
If InStr(Numbers, Chr(KeyAscii)) = 0 Then
KeyAscii = 0
End If
End Sub

Private Sub txtHang_Change()
    Dim n As Single
    n = Val(txtHang.Text)
    If n > 0 And n < 1001 Then
        HangSpin.Value = n
        iHang = n
    End If
 
 txtHang.Text = iHang
 txtLie.Text = Int(iPages / iHang + 0.9)
 
  
  iLie = CInt(txtLie.Text)
    
End Sub

Private Sub HangSpin_Change()
    txtHang.Text = CStr(HangSpin.Value)
End Sub

Private Sub txtLie_Change()
    Dim n As Single
    n = Val(txtLie.Text)
    If n > 0 And n < 1001 Then
        LieSpin.Value = n
        iLie = n
    End If
    
    txtLie.Text = iLie
    txtHang.Text = Int(iPages / iLie + 0.9)
    
    iHang = CInt(txtHang.Text)
End Sub

Private Sub LieSpin_Change()
    txtLie.Text = CStr(LieSpin.Value)
End Sub


Private Sub txtXiayi_Change()
    Dim n As Single
    n = Val(txtXiayi.Text)
    If n > 0 And n < 1001 Then
        iXiayi = n
    End If
End Sub

Private Sub txtYouyi_Change()
    Dim n As Single
    n = Val(txtYouyi.Text)
    If n > 0 And n < 1001 Then
        iYouyi = n
    End If
End Sub






' ************* 函数模块 ************* '
Function ExistsFile_UseFso(strPath As String) As Boolean

     Dim fso

     Set fso = CreateObject("Scripting.FileSystemObject")

     ExistsFile_UseFso = fso.FileExists(strPath)

     Set fso = Nothing

End Function

Function WebHelp()
 Dim h As Long, r As Long
 
 If cmdHelp.Caption = "在线帮助" Then
 h = FindWindow(vbNullString, "CorelDRAW 合并多页为一页 蘭雅sRGB 2010-2020")
 r = ShellExecute(h, "", "http://sRGB.vicp.net", "", "", 1)
 End If
End Function


