"""
@author: Ramsin Khoshabeh
"""

from ECE16Lib.Communication import Communication
from time import sleep
import socket, pygame
from csv import writer
import numpy as np

# Setup the Socket connection to the Space Invaders game
host = "127.0.0.1"
port = 65432
mySocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
mySocket.connect((host, port))
mySocket.setblocking(False)


def load_data(file_name): # to read data from the CSV because I save scores to a CSV to return the high scores
    return np.genfromtxt(file_name, delimiter=",")


class PygameController:
    comms = None

    def __init__(self, serial_name, baud_rate):
        self.comms = Communication(serial_name, baud_rate)
        self.gameScore = 0
        self.file_name = "file.csv" # file that has a CSV of all the scores
        self.set_lives = 3


    def run(self):
        # 1. make sure data sending is stopped by ending streaming
        self.comms.send_message("stop")
        self.comms.clear()
        # ''' The code below opens the CSV and sends the top 3 high scores to the mcu to display
        with open(controller.file_name, 'a') as f:
            writer_object = writer(f)
            writer_object.writerow([controller.gameScore])
            f.close()
        myData = load_data(controller.file_name)
        myData.sort() # sorts scores in numerical order
        # print(myData)
        myData = myData[-3:] # slices the last 3 scores aka the high scores
        final_message = "High Scores:," + str(int(myData[2])) + "," + str(int(myData[1])) + "," + str(int(myData[0]))
        controller.comms.send_message("                ,                ,                ,                ")
        controller.comms.send_message(final_message)
        # '''
        # 2. start streaming orientation data
        input("Ready to start? Hit enter to begin.\n")
        self.comms.send_message("start")

        # 3. Forever collect orientation and send to PyGame until user exits
        print("Use <CTRL+C> to exit the program.\n")
        clearFlag = False
        while True:
            message = self.comms.receive_message()
            if (message != None):
                try:
                    # receive the orientation, fire_state, and an miscellaneous message from mcu
                    # lives_left was just something that I could send to python to print for debugging purposes
                    (orientation, fire_state, lives_left) = message.split(',')
                except ValueError:
                    continue
                command = None
                shoot_command = None
                # print(orientation)
                # print(fire_state)
                orientation = int(orientation)
                fire_state = int(fire_state)
                # print(lives_left)
                if fire_state == 1:
                    shoot_command = "FIRE"
                    print(shoot_command)
                if orientation == 3:
                    command = "LEFT"
                elif orientation == 4:
                    command = "RIGHT"

                if command is not None and shoot_command is not None: # if there is both commands, concatenate and send
                    command = command + "," + shoot_command
                    mySocket.send(command.encode("UTF-8"))

                elif command is not None and shoot_command is None: # if there is only a move command, send that
                    mySocket.send(command.encode("UTF-8"))

                elif command is None and shoot_command is not None: # if there is only a shoot command, send that
                    mySocket.send(shoot_command.encode("UTF-8"))

            try:
                data = mySocket.recv(1024) # receive from server
                data = data.decode("utf-8")
                (lives, score) = data.split(',') # string parse
                self.gameScore = score
                game_details = "Lives:" + lives + "," + "Score:" + score + ", " + ", "
                if clearFlag is False: # clear OLED once
                    clearFlag = True
                    self.comms.send_message("                ,                ,                ,                ")
                self.comms.send_message(game_details) # send game score and num of lives
                # print(data)
            except BlockingIOError:
                pass


if __name__ == "__main__":
    serial_name = "/dev/cu.TristanArduino-BT"
    baud_rate = 115200
    controller = PygameController(serial_name, baud_rate)

    try:
        controller.run()
    except(Exception, KeyboardInterrupt) as e:
        print(e)
    finally:
        # report high scores after game finishes
        with open(controller.file_name, 'a') as f:
            writer_object = writer(f)
            writer_object.writerow([controller.gameScore])
            f.close()
        myData = load_data(controller.file_name)
        myData.sort()
        # print(myData)
        myData = myData[-3:]
        final_message = "High Scores:," + str(int(myData[2])) + "," + str(int(myData[1])) + "," + str(int(myData[0]))
        print("Exiting the program.")
        controller.comms.send_message("                ,                ,                ,                ")
        controller.comms.send_message(final_message)
        controller.comms.close()
        mySocket.send("QUIT".encode("UTF-8"))
        mySocket.close()

    input("[Press ENTER to finish.]")
