from selenium import webdriver
from selenium.webdriver.common.keys import Keys
import time

LOGIN_URL = 'https://www.paypal.com/signin/'

USER = "pkgoingout@gmail.com"
PASS = "Youtuberocks132"

chrome_options = webdriver.ChromeOptions()
chrome_options.add_argument('--user-agent="Mozilla/5.0 (Windows Phone 10.0; Android 4.2.1; Microsoft; Lumia 640 XL LTE) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.135 Mobile Safari/537.36 Edge/12.10166"')
chrome_options.add_argument('--headless')

driver = webdriver.Chrome(executable_path="chromedriver.exe", chrome_options=chrome_options)
driver.execute_script("Object.defineProperty(navigator, 'webdriver', {get: () => undefined})")

driver.get(LOGIN_URL)

elem = driver.find_element_by_id('email')
elem.send_keys(USER)

elem = driver.find_element_by_id('btnNext')
elem.click()

time.sleep(2)

elem = driver.find_element_by_id('password')
elem.send_keys(PASS)
elem.send_keys(Keys.RETURN)

time.sleep(4)

driver.save_screenshot("screenshot.png")

driver.close()

