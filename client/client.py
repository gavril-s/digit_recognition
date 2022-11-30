import requests
import tkinter
from PIL import Image, ImageDraw, ImageTk

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
        imagesprite = self.canvas.create_image(size_x, size_y, image=img)

        self.canvas.bind("<Button-1>", self.on_click)
        self.canvas.bind("<B1-Motion>", self.on_move)
        self.canvas.bind("<space>", self.clear)

    def save(self):
        self.thumbnail = self.image.copy()
        self.thumbnail.thumbnail((64, 64), Image.Resampling.LANCZOS)
        self.thumbnail.save("test.jpg")

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

    def clear(self, event):
        print("d")
        #self = Drawing(self.app, self.size_x, self.size_y, self.color)

def network_request():
    return 1

def main():
    UPDATE_TIME = 100 # in ms

    app = tkinter.Tk()
    app.title("Digit Recognition")
    app.geometry("512x256")
    app.resizable(False, False)

    drawing = Drawing(app, 256, 256)

    ans_canvas = tkinter.Canvas(app, width=256, height=256, bg='black')
    ans_canvas.pack(anchor='nw', side=tkinter.LEFT, expand=False)

    def update_ans():
        ans_canvas.delete("all")
        ans = network_request()
        ans_canvas.create_text(128, 128, text=str(ans), fill="white", font=('Lato 96'))
        app.after(UPDATE_TIME, update_ans)

    app.after(UPDATE_TIME, update_ans)
    app.mainloop()

if __name__ == "__main__":
    main()


