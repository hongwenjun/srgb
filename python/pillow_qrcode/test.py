from PIL import ImageGrab, Image,  PSDraw
im = ImageGrab.grab((0,0,800,200)) #截取屏幕指定区域的图像 
# im = ImageGrab.grab() #不带参数表示全屏幕截图

title = "hopper"



im.save('1.png')
im.show()