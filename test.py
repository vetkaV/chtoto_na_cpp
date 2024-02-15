import sqlite3

sqliteConnection = sqlite3.connect('twist.sqlite')
cursor = sqliteConnection.cursor()
cursor.execute('''
CREATE TABLE IF NOT EXISTS users (
    id INTEGER,
            login VARCHAR,
            password VARCHAR
);        ''')
sqliteConnection.commit()

login = input("input login ")
query = "select login, password FROM users WHERE login ==  '" + login + "';"
print(query)
cursor.execute(query)
result = cursor.fetchall()
print(result)
