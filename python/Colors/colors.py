''' Python Class for Coloring Python Command Line (CLI) Scripts Output '''
class Colors:
    colorize = ''
    text = ''
    def __init__(self, text='', color = 0, bg_color = 0):
        self.text = text
        if bg_color and color :
            self.colorize = f'\033[{bg_color}m\033[{color}m{text}\033[0m'    
        elif color:
            self.colorize = f'\033[{color}m{text}\033[0m'
        elif bg_color:
            self.colorize = f'\033[{bg_color}m{text}\033[0m'

class foreground_colors:
    black = 30; dark_gray = '1;30'; blue = 34; light_blue = '1;34'
    green = 32; light_green = '1;32'; cyan = 36; light_cyan = '1;36'
    red = 31; light_red = '1;31'; purple = 35; light_purple = '1;35'
    brown = 33; yellow = '1;33'; light_gray = 37; white = '1;37'

    dir = ['black', 'blue', 'brown', 'cyan', 'dark_gray', 'green', 'light_blue', 'light_cyan', 'light_gray', 
            'light_green', 'light_purple', 'light_red', 'purple', 'red', 'white', 'yellow']

class background_colors:
    black = 40;    red = 41; green = 42; yellow = 43
    blue = 44; magenta = 45; cyan = 46; light_gray = 47
    SUCCESS = 42; FAILURE = 41; WARNING = 43; NOTE = 44

    dir = ['FAILURE', 'NOTE', 'SUCCESS', 'WARNING', 'black', 'blue', 'cyan', 'green', 'light_gray', 'magenta', 'red', 'yellow']

col = foreground_colors    # 文字颜色表         Text color table
bg = background_colors     # 背景色颜色表       background color table
color = Colors             # 初始化文本着色类   Initialize the text coloring class

##  Python 命令行(CLI)脚本使用彩色着色类 Python Class for Coloring Python Command Line (CLI) Scripts Output
def test_colorize_class():
    for i in range(len(col.dir)):
        text = color('Test Foreground colors: ' + col.dir[i], vars(col)[col.dir[i]] )
        print(text.colorize)

    for i in range(len(bg.dir)):
        text = color('Test Background colors: ' + bg.dir[i],  bg_color = vars(bg)[bg.dir[i]] )
        print(text.colorize)

    for i in range(len(col.dir)):
        for j in range(len(bg.dir)):
            text = color('Test Colors', vars(col)[col.dir[i]], bg_color = vars(bg)[bg.dir[j]] )
            print(text.colorize, end = ' ')
        print()

if __name__ == '__main__':
    test_colorize_class()
    text = color("Python Class for Coloring Python Command Line (CLI) Scripts Output", col.green, bg.blue)
    print(text.text, text.colorize)
