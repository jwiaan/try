from selenium import webdriver
from selenium.webdriver.common.by import By

driver = webdriver.Edge()
driver.get('https://www.douyin.com')
input('登录成功后敲下回车')
driver.get('https://live.douyin.com/211202467730')

last = ''
word = {
    '主播': '你好主播',
    '榜一': '你好榜一',
    '大哥': '你好大哥',
    '吃瓜': '你好吃瓜',
    '美女': '你好美女',
    '欢迎': '你好欢迎',
    }

while True:
    try:
        text = driver.find_elements(By.CLASS_NAME, 'webcast-chatroom___content-with-emoji-text')[-1].text
        if text == last:
            continue

        if text in word.values():
            continue

        for key in word:
            if key in text:
                driver.find_element(By.CLASS_NAME, 'webcast-chatroom___textarea').send_keys(word[key])
                driver.find_element(By.CLASS_NAME, 'webcast-chatroom___send-btn').click()
                last = text
                print(text, word[key])
    except:
        print("except")