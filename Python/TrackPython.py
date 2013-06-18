import sys
sys.path.append("/Users/sorennielsen/opencv/build/lib")

import cv2
import numpy as np

import matplotlib
import numpy as np
from matplotlib.lines import Line2D
import matplotlib.pyplot as plt
import matplotlib.animation as animation

import wx




class Scope:
    def __init__(self, ax, maxt=2, dt=0.02):
        self.ax = ax
        self.dt = dt
        self.maxt = maxt
        self.tdata = [0]
        self.ydata = [0]
        self.line = Line2D(self.tdata, self.ydata)
        self.ax.add_line(self.line)
        self.ax.set_ylim(-.1, 1.1)
        self.ax.set_xlim(0, self.maxt)

    def update(self, y):
	
		self.line.set_data(self.tdata, self.ydata)
		
		        # 
		        # lastt = self.tdata[-1]
		        # if lastt > self.tdata[0] + self.maxt: # reset the arrays
		        #     self.tdata = [self.tdata[-1]]
		        #     self.ydata = [self.ydata[-1]]
		        #     self.ax.set_xlim(self.tdata[0], self.tdata[0] + self.maxt)
		        #     self.ax.figure.canvas.draw()
		        # 
		        # t = self.tdata[-1] + self.dt
		        # self.tdata.append(t)
		        # self.ydata.append(y)
		        # self.line.set_data(self.tdata, self.ydata)
        
		return self.line,


def emitter(p=0.03):
    'return a random value with probability p, else 0'

    while True:
        v = np.random.rand(1)
        if v > p:
            yield 0.
        else:
            yield np.random.rand(1)

#fig = plt.figure()
#ax = fig.add_subplot(111)
#scope = Scope(ax)

# pass a generator in "emitter" to produce data for the update func
#ani = animation.FuncAnimation(fig, scope.update, emitter, interval=10,
#    blit=True)
#plt.show()


class MoviePanel(wx.Panel):
	
	def __init__(self, parent):
		wx.Panel.__init__(self, parent)
		
		self.mid_line_idx = 120
		self.plug_threshold = -40
		
		self.track_idx = 0
		
		self.background = np.genfromtxt( 'example.txt', delimiter=',' )
		self.background = self.background[ :-1 ] #??????
		
		#self.vc = cv2.VideoCapture(0)
		self.vc = cv2.VideoCapture( "sample_plug.avi" )

		fps = self.vc.get(cv2.cv.CV_CAP_PROP_FPS)
		#fps = 15

		retv, frame = self.vc.read()
		
		#frame = cv2.resize(frame, (600,300)) #width, height
		#frame = cv2.cvtColor( frame, cv2.cv.CV_BGR2RGB )

		self.bmp = wx.BitmapFromBuffer(frame.shape[1], frame.shape[0], frame)

		self.Bind(wx.EVT_PAINT, self.onPaint)

		self.playTimer = wx.Timer(self)

		self.Bind(wx.EVT_TIMER, self.onNextFrame)
		self.playTimer.Start( 1000 / fps )
	
	def onPaint(self, evt):
		if self.bmp:
			dc = wx.BufferedPaintDC(self)
			self.PrepareDC(dc)
			dc.DrawBitmap(self.bmp, 0, 0, True)

		evt.Skip()
		
	def onNextFrame(self, evt):
		rval, frame = self.vc.read()
		frame = cv2.cvtColor( frame, cv2.cv.CV_BGR2RGB )

		#frame = cv2.resize(frame, (600,300))

		frame_width, frame_height, frame_channels = frame.shape

		#Convert to grayscale
		gray = cv2.cvtColor( frame, cv2.cv.CV_BGR2GRAY )

		#Cut out middle line:
		mid_line = gray[ self.mid_line_idx, : ]	
		bgsubbed = mid_line - self.background

		#bgsubbed = 0

		#Threshold the background subbed mid_line
		over_threshold = np.where( bgsubbed < self.plug_threshold )[0]

		if over_threshold.size > 0:
			#Get the first point (from right) that exceeds the threshold
			x = over_threshold[ -1 ]
			x2 = over_threshold[ 0 ]
			
			self.track_idx = x

			#Draw tracking line
			pt1 = ( x, 0 )
			pt2 = ( x, frame_height )
			cv2.line(frame, pt1, pt2, color = ( 255, 255, 255 ) )

		cv2.line( frame, (180, 0), (180, 250), color = ( 127, 127, 0 ))
		cv2.line( frame, (160, 0), (160, 250), color = ( 0, 127, 127 ))

		if rval:
			self.bmp.CopyFromBuffer(frame)
			self.Refresh()

		evt.Skip()


class CvMovieFrame(wx.Frame):
	def __init__(self, parent):
		wx.Frame.__init__(self, parent)
		
		self.displayPanel = MoviePanel(self)
		self.SetSize((320, 240))
		self.CenterOnScreen()
		#self.Fit()

if __name__ == "__main__":
	app = wx.App(False)
	frame = CvMovieFrame(None)
	frame.Show()
	app.MainLoop()
		
	# background = np.genfromtxt( 'example.txt', delimiter=',' )
	# background = background[ :-1 ] #??????
	# 
	# cv2.namedWindow( "preview" )
	# vc = cv2.VideoCapture( "sample_plug.avi" )
	# 
	# #Get Frames Per Second
	# fps = vc.get(cv2.cv.CV_CAP_PROP_FPS)
	# frame_width = int(vc.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH))
	# frame_height = int(vc.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT))
	# print "Frame Size (HxW): ", frame_height, frame_width 
	# print "FPS : ", fps
	# 
	# mid_line_idx = 120
	# plug_threshold = -40
	# 
	# if vc.isOpened(): # try to get the first frame
	#     rval, frame = vc.read() #rval = False at last frame
	# else:
	#     rval = False
	# 
	# while rval:
	# 	#Convert frame to grayscale
	# 	gray = cv2.cvtColor( frame, cv2.cv.CV_BGR2GRAY )
	# 
	# 	#Cut out middle line:
	# 	mid_line = gray[ mid_line_idx, : ]	
	# 	bgsubbed = mid_line - background
	# 
	# 	#Threshold the background subbed mid_line
	# 	over_threshold = np.where( bgsubbed < plug_threshold )[0]
	# 
	# 	if over_threshold.size > 0:
	# 		#Get the first point (from right) that exceeds the threshold
	# 		x = over_threshold[ -1 ]
	# 		x2 = over_threshold[ 0 ]
	# 
	# 		#Draw tracking line
	# 		pt1 = ( x, 0 )
	# 		pt2 = ( x, frame_height )
	# 		cv2.line( gray, pt1, pt2, color = ( 255, 255, 255 ) )
	# 
	# 		# if x!=x2 and (x-x2>40):
	# 		# 			#Draw tracking line
	# 		# 			pt1 = ( x2, 0 )
	# 		# 			pt2 = ( x2, 250 )
	# 		# 			cv2.line( gray, pt1, pt2, color = ( 255, 255, 255 ) )
	# 		
	# 
	# 	cv2.line( gray, (180, 0), (180, frame_height), color = ( 127, 127, 127 ))
	# 	cv2.line( gray, (160, 0), (160, frame_height), color = ( 127, 127, 127 ))
	# 
	# 	#Show image
	# 	cv2.imshow( "preview", gray )
	# 
	# 	#Get new frame
	# 	rval, frame = vc.read()
	# 
	# 	#Wait for next frame
	# 	key = cv2.waitKey( int( 1000.0 / fps ) )
	# 	if key == 27: # exit on ESC
	# 		break
