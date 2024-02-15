import sqlite3

sqliteConnection = sqlite3.connect('sql.db')
cursor = sqliteConnection.cursor()
login = input("input login")
query = 'select login, password FROM table WHERE login == ' + login + ';'
cursor.execute(query)
