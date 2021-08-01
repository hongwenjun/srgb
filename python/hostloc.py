import time
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

import sys

option = webdriver.ChromeOptions()
option.add_argument('headless')
option.add_argument('no-sandbox')
driver = webdriver.Chrome(options=option)
# 浏览器地址跳到登录页
driver.get('https://www.hostloc.com/member.php?mod=logging&action=login')
# 给点时间加载页面元素
print(driver.title)
# 填写帐号密码并点登录
driver.find_element_by_name("username").send_keys(sys.argv[1])
driver.find_element_by_name("password").send_keys(sys.argv[2])
driver.find_element_by_name("loginsubmit").click()
# 登录需要一定的跳转时间，根据网速来设定合适的值
WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.CLASS_NAME, "showmenu")))
print(driver.title)
driver.get('https://www.hostloc.com/home.php?mod=spacecp')
# 打开个人中心检查标题,知道登录是否成功
print(driver.title)

for num in range(1,20):
    driver.get('https://www.hostloc.com/?' + str(num))
    # 打开个人中心检查标题,知道登录是否成功
    print(driver.title)

driver.close()

