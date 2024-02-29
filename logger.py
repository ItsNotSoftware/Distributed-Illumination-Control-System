import serial
import matplotlib.pyplot as plt
import numpy as np
import sys
import threading
from colorama import Fore, Style, init
from termcolor import colored

# Print options
ERROR = True
WARNINGS = True
INFO = True
VALUES = False


class Interface:
    def __init__(self, serial_port):
        self.port = serial.Serial(serial_port, 115200)
        self.receive_thread = threading.Thread(target=self.read_serial)
        self.receive_thread.daemon = (
            True  # Daemonize the thread to close with main thread
        )
        self.receive_thread.start()
        self.values = {}

    def read_serial(self):
        while True:
            data = self.port.readline().decode().replace("\n", "")
            words = data.split()

            if len(words) == 0:
                continue
            if words[0] == "[INFO]:" and INFO:
                print(colored(data, "green"))
            elif words[0] == "[WARNING]:" and WARNINGS:
                print(colored(data, "yellow"))
            elif words[0] == "[ERROR]:" and ERROR:
                print(colored(data, "red"))
            elif words[0] == "[VALUE]:":
                if words[1] in self.values:
                    self.values[words[1]].append(words[2])
                else:
                    self.values[words[1]] = [words[2]]

                if VALUES:
                    print("\t", data)

    def send_message(self, message):
        self.port.write(message.encode())

    def gen_plots(self):
        for key, value_list in self.values.items():
            try:
                numeric_values = [
                    float(value) for value in value_list
                ]  # Convert values to float
                plt.figure()
                plt.plot(numeric_values)
                plt.title(key)
                plt.xlabel("Sample Index")
                plt.ylabel("Value")
                plt.savefig(f"plots/{key}.png")  # Save the plot as a PNG file
                plt.close()
            except ValueError as e:
                print(f"Error converting values for {key}: {e}")

    def on_exit(self):
        self.port.close()
        self.gen_plots()
        print("\n", self.values)


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
