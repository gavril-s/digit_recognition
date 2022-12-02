import time
import requests
import tkinter
from PIL import Image, ImageDraw, ImageTk
from threading import Thread

class MousePos:
    def __init__(self, x, y):
        self.x = x
        self.y = y

class Drawing:
    def __init__(self, app, size_x, size_y, color="white"):
        self.app = app
        self.size_x = size_x
        self.size_y = size_y
        self.color = color
        self.image = Image.new("RGB", (size_x, size_y), color)
        self.last_mouse_pos = MousePos(0, 0)
        self.current_mouse_pos = MousePos(0, 0)

        self.canvas = tkinter.Canvas(app, width=size_x, height=size_y, bg=color)
        self.canvas.pack(anchor='nw', side=tkinter.LEFT, expand=False)
        img = ImageTk.PhotoImage(self.image)
        self.canvas.create_image(size_x, size_y, image=img)

        self.canvas.bind("<Button-1>", self.on_click)
        self.canvas.bind("<B1-Motion>", self.on_move)
        self.app.bind("<space>", self.clear)

    def save(self):
        self.thumbnail = self.image.copy()
        self.thumbnail.thumbnail((64, 64), Image.Resampling.LANCZOS)
        self.thumbnail.save("drawing.bmp")

    def draw_line(self, pos1, pos2):
        draw = ImageDraw.Draw(self.image)
        self.canvas.create_line(
            (pos1.x, pos1.y, pos2.x, pos2.y),
            fill="black",
            width=8)
        draw.line(
            [(pos1.x, pos1.y), (pos2.x, pos2.y)],
            fill="black",
            width=4)
        self.save()

    def on_click(self, event):
        self.current_mouse_pos = MousePos(event.x, event.y)

    def on_move(self, event):
        self.last_mouse_pos = self.current_mouse_pos
        self.current_mouse_pos = MousePos(event.x, event.y)
        self.draw_line(self.last_mouse_pos, self.current_mouse_pos)

    def clear(self, event=None):
        self.canvas.delete("all")
        self.image = Image.new("RGB", (self.size_x, self.size_y), self.color)
        img = ImageTk.PhotoImage(self.image)
        self.canvas.create_image(self.size_x, self.size_y, image=img)

class ResUpdater(Thread):
    def __init__(self, res_canvas, url, port, update_time):
        Thread.__init__(self)
        self.res_canvas = res_canvas
        self.url = url
        self.port = port
        self.update_time = update_time
        self.stopped = False

    def make_request(self):
        response = requests.get(f"http://{self.url}:{self.port}").json()
        return response["result"]

    def run(self):
        while not self.stopped:
            res = self.make_request()
            self.res_canvas.delete("all")
            self.res_canvas.create_text(128, 128, text=str(res), fill="white", font=('Lato 96'))
            time.sleep(self.update_time)

    def stop(self):
        self.stopped = True

def main():
    SERVER_URL = "127.0.0.1"
    SERVER_PORT = 8000
    UPDATE_TIME = 0.1

    app = tkinter.Tk()
    app.protocol("WM_DELETE_WINDOW", app.destroy)
    app.title("Digit Recognition")
    app.geometry("512x256")
    app.resizable(False, False)

    drawing = Drawing(app, 256, 256)

    res_canvas = tkinter.Canvas(app, width=256, height=256, bg='black')
    res_canvas.pack(anchor='nw', side=tkinter.LEFT, expand=False)

    res_upd = ResUpdater(res_canvas, SERVER_URL, SERVER_PORT, UPDATE_TIME)
    res_upd.daemon = True

    res_upd.start()
    app.mainloop()
    res_upd.stop()

if __name__ == "__main__":
    main()
