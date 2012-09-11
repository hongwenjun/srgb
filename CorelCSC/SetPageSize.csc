#define DRAWOBJECT "CorelDRAW.Automation.9"
' 修改第一行指定你的CorelDRAW版本

REM  拼版工序--按物件设定页面大小  创建 2012年9月11日 By 蘭公子

WITHOBJECT  DRAWOBJECT
    .Group                  ' 群组
    .GetSize XSize&, YSize& '得到物件大小

    ' 所选物件居中
    Id&=.GetObjectID(0,TRUE,TRUE)
    .SelectObjectOfCDRStaticID Id&
    .AlignToCenterOfPage 3, 3 ' 居中对齐
    .Ungroup                  ' 解散群组
    .UnSelectAll              ' 取消选择所有物件

    ' 设定页面大小 为物件大小
    XSize& = int(XSize& / 10000 + 0.5) * 10000   ' 尺寸 mm取整
    YSize& = int(YSize& / 10000 + 0.5) * 10000
    .SetCurrentPageSize  XSize&, YSize&      ' 设定页面大小为 物件大小取整

    ' 画一个矩形        Top       Left      Bottom     Right   CornerRadius(圆角半径可选四个参数)
    .CreateRectangle YSize&/2, -XSize&/2, -YSize&/2, XSize&/2,   0   ' 相对于中心的 顶-左-下-右 四个坐标 画矩形
    .ApplyOutline 1500, 1, 0, 0, 100, 0, 0, -1, -1, FALSE, 2, 0, FALSE   ' 0.15mm轮廓线

	.StoreColor 5020, 100, 100, 100, 100, 0, 0, 0, 0                     ' 拼版色
	.SetOutlineColor                                                     ' 设置轮廓颜色

END WITHOBJECT
