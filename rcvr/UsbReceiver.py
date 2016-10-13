import threading
import Queue
from UsbReceiver import UsbReceiver

class Cmds():
    def __init__(self, normalizedVoltage, secondCommand):
        self.NormalizedVoltage = normalizedVoltage
        self.SecondCommand = secondCommand

class Tlm():
    def __init__(self):
        self.Positon = 0.0
        self.Velocity = 0.0
        self.NewTelemetry = 0.0

class UsbReceiver():

    def __init__(self, portName, commFrequency):
        self.usbReceiver = UsbReceiver()
        self.usbReceiver.PortName = portName
        self.usbReceiver.CommFrequency = commFrequency
        self.Connected = False
        self.Active = False
        self.CommThread = threading.Thread(target=self.__usb_comm_thread__)
        self.CommandDataQueue = Queue.LifoQueue(maxsize=5)
        self.TelemetryDataQueue = Queue.LifoQueue(maxsize=5)

    def Connect(self):
        self.usbReceiver.Connect()
        self.Connected = self.usbReceiver.Connected
        self.Active = self.usbReceiver.Active
        return self.Connected and self.Active

    def Start(self):
        if (self.Connected and self.Active):
            self.CommThread.start()
            print "Comm started."
            return True
        else:
            print "Can't start comm. Not connected."
            return False

    def Stop(self):
        self.Active = False
        self.CommThread.join()

    def SendCommands(self, cmds):
        if(isinstance(cmds, Cmds)):
            if (self.CommandDataQueue.full()):
                self.CommandDataQueue.get()
            self.CommandDataQueue.put(cmds)
            return True
        else:
            print "Invalid command. Not sending."
            return False

    def RcvTlm(self):
        return self.TelemetryDataQueue.get()

    def __load__(self):
        if (self.CommandDataQueue.empty()):
            return
        cmd = self.CommandDataQueue.get()
        self.usbReceiver.NormalizedVoltage = cmd.NormalizedVoltage
        self.usbReceiver.SecondCommand = cmd.SecondCommand

    def __unload__(self):
        tlm = Tlm()
        tlm.Position = self.usbReceiver.Position
        tlm.Velocity = self.usbReceiver.Velocity
        tlm.NewTelemetry = self.usbReceiver.NewTelemetry
        if (self.TelemetryDataQueue.full()):
            self.TelemetryDataQueue.get()
        self.TelemetryDataQueue.put(tlm)

    def __usb_comm_thread__(self):
    	while(self.Connected and self.Active):
            try:
                self.__load__()
                self.usbReceiver.RunComm()
                self.__unload__()
            except KeyboardInterrupt:
                print 'Closing connection ... \n'
                return
