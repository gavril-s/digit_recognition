import time
from turtle import screensize
import requests
import tkinter
from PIL import Image, ImageDraw, ImageTk
from threading import Thread

class MousePos:
    def __init__(self, x, y):
        self.x = x
        self.y = y

class Drawing:
    def __init__(self, app, draw_size, save_size, color="white"):
        self.app = app
        self.size_x = draw_size[0]
        self.size_y = draw_size[1]
        self.save_size = save_size
        self.color = color
        self.image = Image.new("RGB", (self.size_x, self.size_y), color)
        self.last_mouse_pos = MousePos(0, 0)
        self.current_mouse_pos = MousePos(0, 0)

        self.canvas = tkinter.Canvas(app, width=self.size_x, height=self.size_y, bg=color)
        self.canvas.pack(anchor='nw', side=tkinter.LEFT, expand=False)
        img = ImageTk.PhotoImage(self.image)
        self.canvas.create_image(self.size_x, self.size_y, image=img)

        self.canvas.bind("<Button-1>", self.on_click)
        self.canvas.bind("<B1-Motion>", self.on_move)
        self.app.bind("<space>", self.clear)

    def save(self):
        self.thumbnail = self.image.copy()
        self.thumbnail.thumbnail(self.save_size)
        self.thumbnail.save("drawing.bmp")

    def draw_line(self, pos1, pos2):
        screen_width = 8;
        img_width = 4;

        draw = ImageDraw.Draw(self.image)

        self.canvas.create_line(
            (pos1.x, pos1.y, pos2.x, pos2.y),
            fill="black",
            width=screen_width)

        offset = (screen_width - 1) / 2
        self.canvas.create_oval(
            (pos2.x - offset, pos2.y - offset,
             pos2.x + offset, pos2.y + offset), 
            fill="black"
            )

        draw.line(
            [(pos1.x, pos1.y), (pos2.x, pos2.y)],
            fill="black",
            width=img_width,
            joint="curve")

        offset = (img_width - 1) / 2
        draw.ellipse(
            (pos2.x - offset, pos2.y - offset,
             pos2.x + offset, pos2.y + offset), 
            fill="black")

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
        try:
            response = requests.get(f"http://{self.url}:{self.port}", data=open('drawing.bmp', 'rb').read()).json()
            return response["result"]
        except:
            return "NO CONNECTION"

    def run(self):
        while not self.stopped:
            res = self.make_request()
            self.res_canvas.delete("all")
            if str(res) == res:
                self.res_canvas.create_text(128, 128, text=res, fill="white", font=('Lato 20'))
            else:
                self.res_canvas.create_text(128, 128, text=str(res), fill="white", font=('Lato 96'))
            time.sleep(self.update_time)

    def stop(self):
        self.stopped = True

def main():
    SERVER_URL = "127.0.0.1"
    SERVER_PORT = 8000
    UPDATE_TIME = 0.1 # in seconds
    RES_SIZE  = (256, 256)
    DRAW_SIZE = (256, 256)
    SEND_SIZE = (28, 28)

    app = tkinter.Tk()
    app.protocol("WM_DELETE_WINDOW", app.destroy)
    app.title("Digit Recognition")
    app.geometry("512x256")
    app.resizable(False, False)

    drawing = Drawing(app, DRAW_SIZE, SEND_SIZE)

    res_canvas = tkinter.Canvas(app, width=RES_SIZE[0], height=RES_SIZE[1], bg='black')
    res_canvas.pack(anchor='nw', side=tkinter.LEFT, expand=False)

    res_upd = ResUpdater(res_canvas, SERVER_URL, SERVER_PORT, UPDATE_TIME)
    res_upd.daemon = True

    res_upd.start()
    app.mainloop()
    res_upd.stop()

if __name__ == "__main__":
    main()
