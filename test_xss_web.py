from flask import Flask,session, render_template, request
import os
folder = os.getcwd()
app  = Flask(__name__, static_folder=folder, template_folder=folder)
app.secret_key = 'hello'

@app.route('/', methods=['GET', 'POST'])
def index():
    session['login'] = 'admin'
    if request.method == "GET" and request.args.get('qq'):
        a = request.args.get('qq')
        print(a)
        return f'''<form action="/" method="get">
                            <input type="text" name="qq">
                            <input type="submit" value="Отправить">
                        </form>
                        <h1>Вы ввели: {a}</h1>'''
    return '''
            <form action="/" method="get">
                <input type="text" name="qq">
                <input type="submit" value="Отправить">
            </form>
            <h1>Вы ввели: </h1>'''

if __name__ == '__main__':
    app.run(debug=True)