import socket
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import threading

IP = "127.0.0.1"
PORT = 5001
X_LIM = 1000


class Plot:
    def __init__(self):
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.server_socket.bind((IP, PORT))
        # self.server_socket.setblocking(0)  # Non-blocking socket

        self.t = [0 for _ in range(X_LIM)]
        self.u = [0 for _ in range(X_LIM)]
        self.ref = [0 for _ in range(X_LIM)]
        self.val = [0 for _ in range(X_LIM)]

        self.receive_thread = threading.Thread(target=self.read_socket)
        self.receive_thread.daemon = True
        self.receive_thread.start()

        self.fig, (self.ax1, self.ax2) = plt.subplots(2, 1)
        (self.line1,) = self.ax1.plot([], [], "r-", animated=True)
        (self.line2,) = self.ax1.plot([], [], "g-", animated=True)

        (self.line3,) = self.ax2.plot([], [], "b-", animated=True)

        self.ani = FuncAnimation(
            self.fig, self.update_plot, init_func=self.init_plot, interval=50, blit=True
        )

        plt.show()

    def init_plot(self):
        self.ax1.set_ylim(0, 800)
        self.ax1.grid()
        self.ax1.legend(["Target", "Measured"])

        self.ax2.set_ylim(0, 260)
        self.ax2.grid()
        self.ax2.legend(["u"])

        return self.line1, self.line2, self.line3

    def update_plot(self, frame):
        if len(self.t) > X_LIM:
            self.ax1.set_xlim(self.t[-X_LIM], self.t[-1])
            self.ax2.set_xlim(self.t[-X_LIM], self.t[-1])

        self.line1.set_data(self.t, self.ref)
        self.line2.set_data(self.t, self.val)
        self.line3.set_data(self.t, self.u)

        return self.line1, self.line2, self.line3

    def read_socket(self):
        while True:
            msg, _ = self.server_socket.recvfrom(1024)
            msg = msg.decode()

            t, u, val, ref = map(float, msg.split())
            self.t.append(t)
            self.u.append(u)
            self.val.append(val)
            self.ref.append(ref)
            print("Val: ", val, "Ref: ", ref)


if __name__ == "__main__":
    Plot()
