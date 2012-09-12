#define DRAWOBJECT "CorelDRAW.Automation.9"


DECLARE SUB COLORMARK
DECLARE SUB JX4J
DECLARE SUB ZX4B
DECLARE SUB CMYKMARK


CALL COLORMARK

SUB COLORMARK ' 导入颜色套准标记文件， 格式化MarkNameID&(5) 数组
WITHOBJECT DRAWOBJECT
    CurDir$ = GetCurrFolder()
    IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
        CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
    ENDIF
    BJ_File$ = CurDir$+"\ColorMark.cdr"
    .GetPageSize PXSize&, PYSize& '得到页面大小
    .FileImport BJ_File$, 1796, FALSE  '导入文件
    .AlignToCenterOfPage 3, 1
    .MoveObject 0, -(PYSize&/2+300000) '页面左下(3cm)定位
    REM .ZoomToSelection
    .Ungroup

	DIM MarkNameID&(5)   ' 存储颜色套准标记Id号
	.SelectAllObjects   ' 先全选，就会从最后导入的物件选择了

	FOR ix = 1 TO 10 STEP 1	     ' 存储颜色套准标记 到 MarkNameID&(5) 数组中
		.SelectNextObject 0
		MarkName$ = .GetObjectData( "MarkName" )

		IF MarkName$ = "CenterLine" THEN
			MarkNameID&(1) = .GetObjectID(0,TRUE,TRUE)
			.ClearObjectData ( "MarkName" )

		ELSEIF 	MarkName$ = "TargetLine" THEN
			MarkNameID&(2) = .GetObjectID(0,TRUE,TRUE)
			.ClearObjectData ( "MarkName" )

		ELSEIF 	MarkName$ = "ColorStrip" THEN
			MarkNameID&(3) = .GetObjectID(0,TRUE,TRUE)
			.ClearObjectData ( "MarkName" )

		ELSEIF 	MarkName$ = "ColorMark" THEN
			MarkNameID&(4) = .GetObjectID(0,TRUE,TRUE)
			.ClearObjectData ( "MarkName" )

		ENDIF

	NEXT ix
	

	FOR ix = 1 TO 5 STEP 1
		Id& =MarkNameID&(ix)
		.SelectObjectOfCDRStaticID Id&

     	SELECT CASE ix
            case 1
                CALL  ZX4B
            case 2
                CALL  JX4J
            case 3
                CALL  JX4J
            case 4
                CAll  CMYKMARK
            case else
            '没有操作
        END SELECT

	NEXT ix


END WITHOBJECT
END SUB



SUB JX4J ' "把角线放置到页面四角(请先选择角线)"
WITHOBJECT DRAWOBJECT
    Id&=.GetObjectID(0,TRUE,TRUE)
    .SelectObjectOfCDRStaticID Id&
    .GetSize XSize&, YSize& '得到物件大小
    .GetPageSize PXSize&, PYSize& '得到页面大小
    .SelectObjectOfCDRStaticID Id&
    .AlignToCenterOfPage 2, 1 '左上对齐到页面中心
    .MoveObject -PXSize&/2,  PYSize&/2 '左上移
    .DuplicateObject 0, 0
    .RotateObject -90000000, FALSE, 0, 0
    .AlignToCenterOfPage 1, 1
    .MoveObject  PXSize&/2,  PYSize&/2 '右上移
    .DuplicateObject 0, 0
    .RotateObject -180000000, FALSE, 0, 0
    .AlignToCenterOfPage 2, 2
    .MoveObject -PXSize&/2, -PYSize&/2 '左下移
    .DuplicateObject 0, 0
    .RotateObject 90000000, FALSE, 0, 0
    .AlignToCenterOfPage 1, 2
    .MoveObject  PXSize&/2, -PYSize&/2 '右下移
END WITHOBJECT
END SUB


SUB ZX4B ' "把中线放置到页边中间(请先选择中线)"
WITHOBJECT DRAWOBJECT
    Id&=.GetObjectID(0,TRUE,TRUE)
    .SelectObjectOfCDRStaticID Id&
    .GetSize XSize&, YSize& '得到物件大小
    .GetPageSize PXSize&, PYSize& '得到页面大小
    .SelectObjectOfCDRStaticID Id&
    .AlignToCenterOfPage 3, 1 '上对齐到页面中心
    .MoveObject 0,  PYSize&/2 '上移

    .DuplicateObject 0, 0
    .RotateObject 180000000, FALSE, 0, 0
    .AlignToCenterOfPage 3, 2
    .MoveObject  0,  -PYSize&/2 '下移

    .DuplicateObject 0, 0
    .RotateObject -90000000, FALSE, 0, 0
    .AlignToCenterOfPage 2, 3
    .MoveObject -PXSize&/2, 0 '左移

    .DuplicateObject 0, 0
    .RotateObject 180000000, FALSE, 0, 0
    .AlignToCenterOfPage 1, 3
    .MoveObject  PXSize&/2, 0 '右移
END WITHOBJECT
END SUB

SUB CMYKMARK
WITHOBJECT DRAWOBJECT
	Id&=.GetObjectID(0,TRUE,TRUE)
    	.SelectObjectOfCDRStaticID Id&
	.ZoomToSelection

END WITHOBJECT
END SUB

