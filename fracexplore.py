from tkinter import *
from PIL import Image, ImageTk
from tempfile import TemporaryFile

import subprocess 

class fracExplorer:

    def __init__(self):        
           
        self.x=-0.5
        self.y=0.0
        self.zoom=1.0
        self.xsize=1920/4.
        self.ysize=1080/4.
 
        self.setXYMinMax()

    def run_viewer(self):

        #set up canvas      
        self.root = Tk()
        self.frame = Frame(self.root, bd=2, relief=SUNKEN)
        self.canvas = Canvas(self.frame, width=self.xsize, height=self.ysize, bd=0 )
        self.canvas.grid(row=0, column=0, sticky=N+S+E+W)
        self.frame.pack(fill=BOTH,expand=YES)
        self.canvas.pack(fill=BOTH,expand=YES)
        
        #setup initial image
        self.image_data=TemporaryFile()
        self.write_fractal()
        self.img = ImageTk.PhotoImage(Image.open(self.image_data))
        self.img_on_canvas=self.canvas.create_image(0,0,image=self.img,anchor="nw")
        
        #mouseclick event
        self.canvas.focus_set()
        self.canvas.bind("<Button 1>",self.zoomin)
        self.canvas.bind("r",self.render)
        self.root.mainloop()

    def setXYMinMax(self):
        aspect=self.xsize/float(self.ysize)
        self.xmin = self.x-1.5/self.zoom
        self.xmax = self.x+1.5/self.zoom
        self.ymin = self.y-1.5/aspect/self.zoom
        self.ymax = self.y+1.5/aspect/self.zoom


    def render(self, event):
        
        args=["./fracpgm3","-xpos",str(self.x),"-ypos",str(self.y),"-zoom",str(self.zoom)]
        args+=["-height",str(self.ysize*4.)]
        args+=["-width",str(self.xsize*4.)]
        args+=["-over","3"]
        args+=["-iter","700"]
        
        fn=""
        fn="frac"+fn.join(args[1:])+".pgm"

        print(fn)
        
        with open(fn,"w+") as f:
            ##subprocess.run(args,stdout=f)
            subprocess.Popen(args, stdout=f, close_fds=True)


    def zoomin(self, event):
         
        self.x=event.x/self.xsize*(self.xmax-self.xmin)+self.xmin
        self.y=(self.ysize-event.y)/self.ysize*(self.ymax-self.ymin)+self.ymin        
        self.zoom*=1.1

        self.setXYMinMax()
        
        print(event.x,event.y,self.x,self.y,self.zoom)
        
        self.image_data.close()
        self.image_data=TemporaryFile()
        self.write_fractal()

        self.img = ImageTk.PhotoImage(Image.open(self.image_data))
        self.label=Label(image=self.img)
        self.label.image=self.img
        self.canvas.itemconfig(self.img_on_canvas, image=self.img)
    
        
    def write_fractal(self):

        args=["./fracpgm3","-xpos",str(self.x),"-ypos",str(self.y),"-zoom",str(self.zoom)]
        args+=["-height",str(self.ysize)]
        args+=["-width",str(self.xsize)]
        subprocess.run(args,stdout=self.image_data)

if __name__ == "__main__":


    frac=fracExplorer()
    frac.run_viewer()
