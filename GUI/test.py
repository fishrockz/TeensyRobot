import sys

from PyQt4.QtCore import QTimer
from PyQt4.QtGui import QApplication
from PyQt4 import QtGui
from PyQt4 import QtCore


import serial

	


class myOutputText(QtGui.QWidget):
	def __init__(self,parent=None):
		super(myOutputText, self).__init__(parent)
		
		MainLayout = QtGui.QVBoxLayout()
		logOutput = QtGui.QTextEdit(parent)
		logOutput.setReadOnly(True)
		logOutput.setLineWrapMode(QtGui.QTextEdit.NoWrap)

		font = logOutput.font()
		font.setFamily("Courier")
		font.setPointSize(10)

		MainLayout.addWidget(logOutput)
		
		self.setLayout(MainLayout)
		
	def addStuff(self,toAdd):
		logOutput.moveCursor(QTextCursor.End)
		logOutput.setCurrentFont(font)
		logOutput.setTextColor(color)

		logOutput.insertPlainText(text)

		sb = logOutput.verticalScrollBar()
		sb.setValue(sb.maximum())
	def SerialRecive(self,info):
		print 'SerialRecive'
		
class mysillyPics(QtGui.QWidget):
	def __init__(self,parent=None):
		super(mysillyPics, self).__init__(parent)
		self.setMinimumSize(500,300)
		MainLayout = QtGui.QVBoxLayout()
		#self.view = QtGui.QGraphicsView()
		self.scn = QtGui.QGraphicsScene()
		self.view = QtGui.QGraphicsView(self.scn)
		
		self.valves=[]
		
		MainLayout.addWidget(self.view)
		
		
		self.pixmapA = QtGui.QPixmap("valveA.png")#.scaled(16, 16, QtCore.Qt.KeepAspectRatio)
		self.pixmapB = QtGui.QPixmap("valveB.png")
		
		if 0:
			self.pixItem = QtGui.QGraphicsPixmapItem(self.pixmapA)
		
			self.pixItem.translate(20.0, -30)
			self.pixItem.setScale(0.1)
			self.scn.addItem(self.pixItem)
		
		
			#pixmap = QtGui.QPixmap("valveA.png").scaled(16, 16, QtCore.Qt.KeepAspectRatio)
			self.pixItemB = QtGui.QGraphicsPixmapItem(self.pixmapA)
			#pixItem.translate(20.0, -30)
		
			self.scn.addItem(self.pixItemB)
		
		pen = QtGui.QPen()
		pen.setWidth(4)
		pen.setCosmetic(True)
		
		#line1=QtGui.QPolygonF([QtCore.QPointF(50,100),QtCore.QPointF(50,300),QtCore.QPointF(100,300),QtCore.QPointF(-100,400),QtCore.QPointF(-100,300),QtCore.QPointF(-50,300),QtCore.QPointF(50,100)])
		
		
		
		
		line1=QtGui.QPolygonF([QtCore.QPointF(50,25),QtCore.QPointF(200,25),
		
		QtCore.QPointF(200,50),QtCore.QPointF(400,50),
		QtCore.QPointF(400,-50),QtCore.QPointF(200,-50),
		
		QtCore.QPointF(200,-25),QtCore.QPointF(50,-25)])
		
		poly1=self.scn.addPolygon(line1,pen)
		poly1.setFillRule(0)
		
		
		
		line1=self.scn.addLine(200,25,300,25,pen)
		line2=self.scn.addLine(200,-25,300,-25,pen)
		
		line2=self.scn.addLine(310,45,310,-45,pen)
		
		
		lineSpike1=self.scn.addLine(0,0,250,0,pen)
		lineSpike2=self.scn.addLine(250,20,250,-20,pen)
	
		
		#flow
		
		self.scn.addLine(100,25,100,100,pen)
		self.scn.addLine(75,100,125,100,pen)
		
		self.scn.addLine(150,100,325,100,pen)
		#supply
		self.scn.addLine(200,100,200,150,pen)
		
		
		
		self.scn.addLine(350,100,400,100,pen)
		self.scn.addLine(375,100,375,50,pen)
		
		
		self.scn.addLine(275,-50,275,-75,pen)
		#self.scn.addLine(250,20,250,-20,pen)
		
		
		
		self.PenDelt1=QtGui.QPen()
		self.brushDelt1 = QtGui.QBrush()
		self.brushDelt1.setColor(QtGui.QColor(0,50,200))
		self.brushDelt1.setStyle(QtCore.Qt.Dense2Pattern)
		self.pressuerSenceA1=self.scn.addRect(200,25,100,25,	self.PenDelt1,	brush=self.brushDelt1)
		self.pressuerSenceA2=self.scn.addRect(200,-25,100,-25,	self.PenDelt1,	brush=self.brushDelt1)
		
		
		self.brushDelt2 = QtGui.QBrush()
		self.brushDelt2.setColor(QtGui.QColor(0,50,200))
		self.brushDelt2.setStyle(QtCore.Qt.Dense2Pattern)
		self.pressuerSenceB1=self.scn.addRect(60,3,180,20,	self.PenDelt1,	brush=self.brushDelt1)
		self.pressuerSenceB2=self.scn.addRect(60,-3,180,-20,	self.PenDelt1,	brush=self.brushDelt1)
		
		
		#self.pressuerSence1.setFillRule(1)
		
		#self.view.fitInView(pixItem)
		
		#self.view.update()
		
		
		self.addValue(350,100)
		self.addValue(425,100)
		
		
		self.addValue(150,100)
		self.addValue(50,100)
		
		self.addValue(280,-100)
		
		self.setLayout(MainLayout)
		
		
	def addValue(self,x,y):
		self.pixItem = QtGui.QGraphicsPixmapItem(self.pixmapA)
		
		self.pixItem.translate(x-10,y-10)
		self.pixItem.setScale(0.15)
		self.scn.addItem(self.pixItem)
	
		self.valves.append({'Gui':None})
	def SerialRecive(self,info):
		print 'silly GUI SerialRecive',info
		info=str(info)
		#self.pixItemB.setPixmap(self.pixmapB)
		striped=''
		state=None
		#try:
		if 1:
			
			striped='>'+info.split('<TelemPacket>')[1].split('</ TelemPacket>')[0]+'<'
			items=striped.split('><')[1:-1]
			items=map(lambda x: x.split('='), items)
			print 'items',items
			for item in items:
				if item[0].split('-')[0]=='ValveII':
					print item[1]
			
		#except:
		#	pass

		self.brushDelt1.setColor(QtGui.QColor(0,50,100))
		self.pressuerSenceA1.setBrush(self.brushDelt1)
		self.pressuerSenceA2.setBrush(self.brushDelt1)
		
		self.view.updateScene([self.view.sceneRect(),])
		self.view.update()
		
		
class ConnectStuff(QtGui.QWidget):
	SerialDataOut = QtCore.pyqtSignal([str] )
	def __init__(self,parent=None):
		super(ConnectStuff, self).__init__(parent)
		layout = QtGui.QVBoxLayout()
		self.b1 = QtGui.QPushButton("Connect")
		self.b1.clicked.connect(self.setoffTimer)
		self.b2 = QtGui.QPushButton("Go")
		self.b2.clicked.connect(self.setoffTimer)
		#self.b1.clicked.connect(self.btnstate)
		layout.addWidget(self.b1)
		layout.addWidget(self.b2)
		self.timer = QTimer()
		self.setLayout(layout)
		self.console=None
#		try:
##			self.SerialConnn
		
		self.serial = None
		self.timer.timeout.connect(self.tick)
		self.timer.start(1000)
	def setoffTimer(self):
		print "hi there"
		try:
			self.serial = serial.Serial('/dev/ttyACM0')
			self.serial.timeout = 0.001
		except:
			pass
		
		
	def tick(self):
		print 'tick',self.serial
		newline=''
		if self.serial !=None:
			try:
				new = self.serial.read()
				while new != '':
					newline+=new
					new = self.serial.read()
				print 'new: ',new
				
			except:
				pass
		self.SerialDataOut.emit(newline)
		print 'newline :',newline


class myWidget(QtGui.QMainWindow):
	def __init__(self,parent=None):
		super(myWidget, self).__init__(parent)
		self.resize(250, 150)
		self.setWindowTitle('Simple')
		self.widgetsTogetSerial=[]
		self.SerialConection=ConnectStuff()
		
		self.setCentralWidget(self.SerialConection)
		

		self.SerialConection.SerialDataOut.connect(self.handle_Telem)
		
		
		tab1	= myOutputText()	
		tab2	= mysillyPics()
		tab3	= QtGui.QWidget()
		tab4	= QtGui.QWidget()
		
		
		ConsoleTabDock=QtGui.QDockWidget("Console",self)
		ConsoleTabDock.setWidget(tab1)
		self.widgetsTogetSerial.append(tab1)
		
		
		PicTabDock=QtGui.QDockWidget("Pics",self)
		PicTabDock.setWidget(tab2)
		self.widgetsTogetSerial.append(tab2)
			
		self.addDockWidget(QtCore.Qt.BottomDockWidgetArea,ConsoleTabDock)
		self.addDockWidget(QtCore.Qt.BottomDockWidgetArea,PicTabDock)
	
	def handle_Telem(self,info):
		print "brill"
		for widget in self.widgetsTogetSerial:
			widget.SerialRecive(info)

def main():
    
    app = QtGui.QApplication(sys.argv)

    w = myWidget()
   # w.r
    #w.move(300, 300)
  #  w.
    w.show()
    
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
