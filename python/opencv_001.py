import cv2
img = cv2.imread('test.png')
cv2.imwrite('test.jpg', img)
cv2.imwrite('test.webp', img)


import cv2
img = cv2.imread('123.png')
cv2.imshow('', img)
cv2.waitKey()
cv2.destroyAllWidows()
