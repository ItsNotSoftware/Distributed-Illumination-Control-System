import serial
import matplotlib.pyplot as plt
import numpy as np
import sys
import threading
from colorama import Fore, Style, init
from termcolor import colored
import time
import socket

from plot import IP, PORT

# Flags for controlling the verbosity of the logger
ERROR = True
WARNINGS = True
INFO = True
VALUES = True
START_TIME = int(round(time.time() * 1000))


def get_time_ms():
    """Return the current time in milliseconds since the start of the program."""
    return str(int(round(time.time() * 1000)) - START_TIME)


class Interface:
    def __init__(self, serial_port):
        self.port = serial.Serial(serial_port, 115200)  # Init serial port

        # Start a thread to read the serial port
        self.receive_thread = threading.Thread(target=self.read_serial)
        self.receive_thread.daemon = True
        self.receive_thread.start()

        self.values = {}  # Store the values received from VALUE message

        self.t = []
        self.u = []
        self.measured = []
        self.target = []

        # Open log files
        self.info_file = open("logs/info.log", "w")
        self.warning_file = open("logs/warning.log", "w")
        self.error_file = open("logs/error.log", "w")

        self.plot_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def read_serial(self):
        """Read the serial port and process the messages."""
        while True:
            # Reade and decode the serial port
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
                    print(colored(data, "blue"))

            elif words[0] == "[CONTROLLER]:":
                words.pop(0)
                self.t.append(float(words[0]))
                self.u.append(float(words[1]))
                self.measured.append(float(words[2]))
                self.target.append(float(words[3]))

                self.plot_socket.sendto(" ".join(words).encode(), (IP, PORT))

            else:
                print(data)

    def send_message(self, message):
        """Send a message to the serial port."""
        self.port.write(message.encode())

    def gen_plots(self):
        """Generate and save plots for the values received."""

        # Saving the values as plots and numpy arrays
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

                # Save the values as a numpy array
                np.save(f"logs/{key}.npy", numeric_values)
            except ValueError as e:
                print(f"Error converting values for {key}: {e}")

    def on_exit(self):
        """Called on exit."""

        self.port.close()
        self.gen_plots()

        np.save("logs/t.npy", self.t)
        np.save("logs/u.npy", self.u)
        np.save("logs/measured.npy", self.measured)
        np.save("logs/target.npy", self.target)

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
