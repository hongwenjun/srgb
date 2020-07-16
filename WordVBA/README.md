### Word 宏 批量修改表格  和 表格自动调整窗口宽

```vb
Sub 批量修改表格()

    Dim tempTable As Table
    
    Application.ScreenUpdating = False
    
    If ActiveDocument.ProtectionType = wdAllowOnlyFormFields Then
    
        MsgBox "文档已保护，此时不能选中多个表格！"
        
        Exit Sub
    
    End If
    
    ActiveDocument.DeleteAllEditableRanges wdEditorEveryone
    
    For Each tempTable In ActiveDocument.Tables
    
        tempTable.Range.Editors.Add wdEditorEveryone
    
    Next
    
    ActiveDocument.SelectAllEditableRanges wdEditorEveryone
    
    ActiveDocument.DeleteAllEditableRanges wdEditorEveryone
    
    Application.ScreenUpdating = True


End Sub

Sub 表格自动窗口宽()

        Selection.Tables(1).AutoFitBehavior (wdAutoFitWindow)
End Sub


```

### Excel VBA 行列转换从文件读取单元格数据
```vb
Function find_range_paste(cell As String)

'  行列转换粘贴 函数

    Range(cell).Select
    Application.CutCopyMode = False
    Selection.Copy
    
    ' 选择区选一个单元格，偏移1列
    Range(cell).Resize(1).Offset(0, 1).Select
    Selection.PasteSpecial Paste:=xlPasteValues, Operation:=xlNone, SkipBlanks:= _
        False, Transpose:=True

End Function


Function readText(filepath As String) As String
    Dim fso
    Dim f
    Set fso = CreateObject("Scripting.FileSystemObject")
    Set f = fso.OpenTextFile(filepath)
    readText = f.ReadAll
End Function

Sub 单元格集合操作()
    Dim input_data As String
    ' 定义需要转换操作的单元格集合
    
    input_data = readText("R:\input.txt")
  
 
    ' 单元格集合切割成字符串数组，
    arr = Split(input_data, ",")
    
    ' 单元格调用函数
    find_range_paste (arr(4))
    
    ' 遍历数组，调用函数
    For Each i In arr
        find_range_paste (i)
    Next
    
End Sub

```
### Excel VBA 行列转换

```vb
Sub line_paste()


'  行列转换粘贴

    Range("E2:E7").Select
    Application.CutCopyMode = False
    Selection.Copy
    Range("F2").Select
    Selection.PasteSpecial Paste:=xlPasteValues, Operation:=xlNone, SkipBlanks:= _
        False, Transpose:=True
   
    Range("E8:E14").Select
    Application.CutCopyMode = False
    Selection.Copy
    Range("F8").Select
    Selection.PasteSpecial Paste:=xlPasteValues, Operation:=xlNone, SkipBlanks:= _
        False, Transpose:=True
        
End Sub


Sub line_paste2()


'  行列转换粘贴
    
    
    cell = "E2:E7"

    Range(cell).Select
    Application.CutCopyMode = False
    Selection.Copy
    
    ' 选择区选一个单元格，偏移1列
    Range(cell).Resize(1).Offset(0, 1).Select
    Selection.PasteSpecial Paste:=xlPasteValues, Operation:=xlNone, SkipBlanks:= _
        False, Transpose:=True

End Sub

Function find_range_paste(cell As String)

'  行列转换粘贴 函数

    Range(cell).Select
    Application.CutCopyMode = False
    Selection.Copy
    
    ' 选择区选一个单元格，偏移1列
    Range(cell).Resize(1).Offset(0, 1).Select
    Selection.PasteSpecial Paste:=xlPasteValues, Operation:=xlNone, SkipBlanks:= _
        False, Transpose:=True

End Function


Sub 单元格集合操作()
    Dim input_data As String
    ' 定义需要转换操作的单元格集合
    
    input_data = "E1:E1,E2:E7,E8:E14,E15:E19,E20:E27"
    
    ' 单元格集合切割成字符串数组，
    arr = Split(input_data, ",")
    
    ' 单元格调用函数
    find_range_paste (arr(4))
    
    ' 遍历数组，调用函数
    For Each i In arr
        find_range_paste (i)
    Next
    
End Sub




Sub range_test()
    Dim input_data As String
    
    input_data = "E1:E1,E2:E7,E8:E14,E15:E19,E20:E27"
    arr = Split(input_data, ",")
    
    
    Range(arr(4)).Select
    Range(arr(4)) = "Range单元格"
    
End Sub


```
