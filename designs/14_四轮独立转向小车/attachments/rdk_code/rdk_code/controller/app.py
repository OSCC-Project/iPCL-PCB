__author__ = "bliysfyt"
__version__ = "2.0"




import sys 
import time
import serial
import serial.tools.list_ports
from flask import Flask, Response, render_template, request
from hobot_vio import libsrcampy
import threading
import atexit

app = Flask(__name__)

# 初始化串口通信
def init_serial():
    #uart_dev = '/dev/ttyS1'  # 适配你的串口设备
    uart_dev = '/dev/ttyUSB0'
    baudrate = 115200
    try:
        ser = serial.Serial(uart_dev, int(baudrate), timeout=1)  # 1秒超时
        return ser
    except Exception as e:
        print("Open serial failed:", e)
        return None

ser = init_serial()
debug_info = []

# 写入调试信息的函数
def log_debug_info(message):
    debug_info.append(message)

# 串口接收数据并打印
def read_from_serial():
    while True:
        if ser:
            try:
                message = ser.read(ser.in_waiting)
                message_u=message.decode('UTF-8')
                if message:
                    print(f"Received from serial: {message}")
                    print(f"Received from serial: {message_u}")
            except Exception as e:
                print(f"Error reading from serial: {e}")
        time.sleep(0.1)

# 启动串口读取线程
def start_serial_read_thread():
    serial_thread = threading.Thread(target=read_from_serial, daemon=True)
    serial_thread.start()

# 控制面板的主页面
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/send_command', methods=['POST'])
def send_command():
    if not request.data:
        return "No command provided", 400
        
    data = request.data
    if len(data) != 6 or data[0] != 0xFF:
        return "Invalid command format", 400
    

    byte_data = bytearray(data)
    x = byte_data[2] if byte_data[2] <= 127 else byte_data[2] - 256
    y = byte_data[3] if byte_data[3] <= 127 else byte_data[3] - 256
    z = byte_data[4] if byte_data[4] <= 127 else byte_data[4] - 256
    
    print(f"Decoded values - X:{x} Y:{y} Z:{z}")
    
    if ser:
        ser.write(data)
        print(data)
        return f"Sent: {data.hex()} (X:{x} Y:{y} Z:{z})"
    else:
        print('Serial not connected')
        return "Serial not connected", 500

# 注册应用退出时的清理工作
def cleanup():
    if ser:
        ser.close()

atexit.register(cleanup)

if __name__ == '__main__':
    # 启动串口读取线程
    start_serial_read_thread()
    app.run(host='0.0.0.0', port=5000, debug=True)
