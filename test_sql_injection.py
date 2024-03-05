import sqlite3

sqliteConnection = sqlite3.connect('twist.sqlite')
cursor = sqliteConnection.cursor()
login = input("input login ")
password = input("input password ")
query = "select login, password FROM users WHERE login ==  '" + login + "' and password =='" + password + "';"
print(query)
cursor.execute(query)
result = cursor.fetchall()
if result != None and len(result) > 0:
    print("Успешно!")
else:
    print('Не угадал пароль давай попробуем еще раз. Подсказка, я админ')