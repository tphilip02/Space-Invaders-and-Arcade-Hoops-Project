from ECE16Lib.Communication import Communication
from ECE16Lib.CircularList import CircularList
import numpy as np
import time
from csv import writer


def load_data(file_name):  # to read data from the CSV because I save scores to a CSV to return the high scores
    return np.genfromtxt(file_name, delimiter=",")


if __name__ == "__main__":
    port_name = "/dev/cu.SLAB_USBtoUART"
    name_BT = "/dev/cu.TristanArduinoBT"
    comms = Communication(name_BT, 115200)
    file_name = "file.csv"
    comms.send_message("stop")
    comms.clear()  # just in case any junk is in the pipes
    myData = load_data(file_name)
    myData.sort()  # sorts scores in numerical order
    # print(myData)
    myData = myData[-3:]  # slices the last 3 scores aka the high scores
    the_time = 0
    final_message = "High Scores:," + str(int(myData[2])) + "," + str(int(myData[1])) + "," + str(int(myData[0]))
    comms.send_message("                ,                ,                ,                ")
    comms.send_message(final_message)
    input("Ready to start? Hit enter to begin.\n")
    comms.send_message("start")
    print("Use <CTRL+C> to exit the program.\n")
    points = 0

    try:
        previous_time = time.time()
        last_time = time.time()
        clearFlag = False
        while True:
            message = comms.receive_message()
            if (message != None):
                try:
                    (m1, ppg, debug_messages) = message.split(',')
                except ValueError:
                    continue

                current_time = time.time()
                point_time = time.time()
                ppg = int(ppg)
                debug_messages = int(debug_messages)
                # time = int(m1)
                if (debug_messages != 0):
                    print(debug_messages)
                # print(ppg)

                if (ppg < 32400):
                    if (point_time - last_time >= 1.5):
                        comms.send_message("POINT")
                        points = points + 1
                        print("point scored!")
                        last_time = point_time

                if (current_time - previous_time >= 1):
                    the_time = the_time + 1
                    msg = "seconds passed:," + str(the_time) + ",Points:," + str(points)
                    comms.send_message(msg)
                    previous_time = current_time
                    if the_time > 60:
                        break

    except(Exception, KeyboardInterrupt) as e:
        print(e)

    finally:
        with open(file_name, 'a') as f:
            writer_object = writer(f)
            writer_object.writerow([points])
            f.close()
        myData = load_data(file_name)
        myData.sort()
        # print(myData)
        myData = myData[-3:]
        final_message = "High Scores:," + str(int(myData[2])) + "," + str(int(myData[1])) + "," + str(int(myData[0]))
        print("Exiting the program.")
        comms.send_message("                    ,                ,                ,                ")
        comms.send_message("GAME OVER!!")
        comms.send_message("                    ,                ,                ,                ")
        comms.send_message(final_message)
        comms.close()

    input("[Press ENTER to finish.]")
