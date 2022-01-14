from operator import imod


import colors

col = colors.foreground_colors    # 文字颜色表         Text color table
bg = colors.background_colors     # 背景色颜色表       background color table
color = colors.Colors             # 初始化文本着色类   Initialize the text coloring class

for i in range(len(col.dir)):
    text = color('Test Foreground colors: ' + col.dir[i], vars(col)[col.dir[i]] )
    print(text.colorize)

for i in range(len(bg.dir)):
    text = color('Test Background colors: ' + bg.dir[i],  bg_color = vars(bg)[bg.dir[i]] )
    print(text.colorize)

for i in range(len(col.dir)):
    for j in range(8):
        text = color(col.dir[i] + bg.dir[j], vars(col)[col.dir[i]], bg_color = vars(bg)[bg.dir[j]] )
        print(text.colorize, end = ' ')
    print()