import serial
import matplotlib.pyplot as plt
import numpy as np
import sys
import threading
import time

from colorama import Fore, Style, init
from termcolor import colored
from matplotlib.animation import FuncAnimation
import matplotlib.animation as animation


# Print options
ERROR = True
WARNINGS = True
INFO = True
VALUES = True

X_LIM = 1000


START_TIME = int(round(time.time() * 1000))


def get_time_ms():
    return str(int(round(time.time() * 1000)) - START_TIME)


class Interface:
    def __init__(self, serial_port):
        self.port = serial.Serial(serial_port, 115200)
        self.receive_thread = threading.Thread(target=self.read_serial)
        self.receive_thread.daemon = (
            True  # Daemonize the thread to close with main thread
        )
        self.receive_thread.start()
        self.values = {}

        self.info_file = open("logs/info.log", "w")
        self.warning_file = open("logs/warning.log", "w")
        self.error_file = open("logs/error.log", "w")

        self.x_data = []
        self.u_data = []
        self.y1_data = []
        self.y2_data = []

        # plots
        self.fig, self.ax = plt.subplots()
        (self.ln1,) = plt.plot([], [], "r-", animated=True)
        (self.ln2,) = plt.plot([], [], "g-", animated=True)
        self.ani = FuncAnimation(
            self.fig, self.update_plot, init_func=self.init_plot, interval=50, blit=True
        )
        plt.show()

    def init_plot(self):
        self.ax.set_ylim(0, 3000)
        self.ax.grid()
        self.ax.legend(["traget", "current"])
        # titles
        self.ax.set_xlabel("Time (ms)")
        self.ax.set_ylabel("Volatge (mV)")

        return self.ln1, self.ln2

    def update_plot(self, frame):
        if len(self.x_data) > X_LIM:  # Make plot window slide after filling up
            self.ax.set_xlim(self.x_data[-X_LIM], self.x_data[-1])

        self.ln1.set_data(self.x_data, self.y1_data)
        self.ln2.set_data(self.x_data, self.y2_data)

        return self.ln1, self.ln2

    def read_serial(self):
        time.sleep(1)

        while True:
            data = self.port.readline().decode().replace("\n", "")
            words = data.split()

            if len(words) == 0:
                continue

            if words[0] == "[INFO]:":
                self.info_file.write(data.replace("INFO", get_time_ms()) + "\n")

                if INFO:
                    print(colored(data, "green"))

            elif words[0] == "[WARNING]:":
                self.warning_file.write(data.replace("WARNING", get_time_ms()) + "\n")

                if WARNINGS:
                    print(colored(data, "yellow"))

            elif words[0] == "[ERROR]:":
                self.error_file.write(data.replace("ERROR", get_time_ms()) + "\n")

                if ERROR:
                    print(colored(data, "red"))

            elif words[0] == "[VALUE]:":
                if words[1] in self.values:
                    self.values[words[1]].append(words[2])
                else:
                    self.values[words[1]] = [words[2]]

                if VALUES:
                    print("\t", data)

            elif words[0] == "[CONTROLLER]:":
                self.x_data.append(float(words[1]))
                self.u_data.append(float(words[2]))
                self.y1_data.append(float(words[3]))
                self.y2_data.append(float(words[4]))

            else:
                print(data)

    def send_message(self, message):
        self.port.write(message.encode())

    def gen_plots(self):
        for key, value_list in self.values.items():
            try:
                numeric_values = [float(value) for value in value_list]
                plt.figure()
                plt.plot(numeric_values)
                plt.title(key)
                plt.xlabel("Sample Index")
                plt.ylabel("Value")
                plt.savefig(f"plots/{key}.png")  # Save the plot as a PNG file
                plt.close()

                np.save(f"logs/{key}.npy", numeric_values)
            except ValueError as e:
                print(f"Error converting values for {key}: {e}")

    def on_exit(self):
        self.port.close()
        self.gen_plots()

        np.save("logs/ct.npy", self.x_data)
        np.save("logs/cu.npy", self.u_data)
        np.save("logs/cy1.npy", self.y1_data)
        np.save("logs/cy2.npy", self.y2_data)

        self.info_file.close()
        self.warning_file.close()
        self.error_file.close()
        exit()


def main(interface):
    while True:
        msg = input()
        interface.send_message(msg)


if __name__ == "__main__":
    interface = Interface(sys.argv[1])
    try:
        main(interface)
    except KeyboardInterrupt:
        interface.on_exit()
        exit()
