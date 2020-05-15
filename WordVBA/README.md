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
