import socket
import pyautogui

s = socket.socket()
s.bind(('0.0.0.0', 8090))
s.listen(0)

while True:
    client, addr = s.accept()
    # client handling code
    while True:
        content = client.recv(32)

        if len(content) == 0:
            break

        else:
            content = content.decode("utf-8")
            if 'Play/Pause' in content:
                pyautogui.typewrite(['space'], 0.2)

            if 'Rewind' in content:
                pyautogui.hotkey('ctrl', 'left')

            if 'Forward' in content:
                pyautogui.hotkey('ctrl', 'right')

            if 'Vdown' in content:
                pyautogui.hotkey('ctrl', 'down')

            if 'Vup' in content:
                pyautogui.hotkey('ctrl', 'up')
            print(content)
    client.close()
