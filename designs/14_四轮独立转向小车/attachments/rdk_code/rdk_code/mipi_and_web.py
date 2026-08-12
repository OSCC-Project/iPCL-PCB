__author__ = "bliysfyt"
__version__ = "2.0"


import sys
import time
import numpy as np
import cv2
from hobot_vio import libsrcampy
from flask import Flask, Response, render_template_string

app = Flask(__name__)

# 创建相机和编码器对象
camera = libsrcampy.Camera()
encoder = libsrcampy.Encoder()

# 设置绑定（将相机与编码器绑定）
# ret = libsrcampy.bind(camera, encoder)
# if ret != 0:
#     print("Failed to set up bind.")
# camera.open_cam(0, 2, 30, 1920, 1080)

# encoder.encode(0, 1, 1920, 1080)


# 用于存储调试信息
debug_info = []

# 写入调试信息的函数
def log_debug_info(message):
    debug_info.append(message)

# 设置相机并打开它
def open_camera():#1,2
    ret = camera.open_cam(0, -1, 30, 1920, 1088,1080,1920)
    if ret != 0:
        log_debug_info("Failed to open camera.")
        return False
    log_debug_info("Camera opened successfully.")
    return True

# 配置编码器
def setup_encoder():
    ret = encoder.encode(0, 3, 1920, 1088)  # MJPEG编码
    if ret != 0:
        log_debug_info("Failed to set up encoder.")
        return False
    log_debug_info("Encoder set up successfully.")
    return True

# 获取并编码图像
def get_encoded_frame():
    # 获取图像
    img = camera.get_img(2,1920, 1088)
    if img is None:
        print("Failed to get image from camera.")
        return None

    # 编码图像
    ret = encoder.encode_file(img)
    if ret != 0:
        print("Failed to encode image.")
        return None

    # 获取编码后的图像
    encoded_img = encoder.get_img()
    if encoded_img is None:
        print("Failed to get encoded image.")
        return None

    return encoded_img

# 视频流生成器
# 视频流生成器
def generate():
    boundary = b'--frame\r\n'
    while True:
        encoded_frame = get_encoded_frame()
        if encoded_frame is not None:
            # MJPEG流传输，需要将编码图像转换为字节流并传输
            yield (boundary +
                   b'Content-Type: image/jpeg\r\n\r\n' + encoded_frame + b'\r\n\r\n')
        time.sleep(0.1)  # 每100ms获取一次新图像

# 定义视频流路由
@app.route('/video_feed')
def video_feed():
    return Response(generate(), mimetype='multipart/x-mixed-replace; boundary=frame')




# 定义调试信息路由
@app.route('/debug_info')
def show_debug_info():
    # 显示调试信息
    log_debug_info("Debug info page requested.")  # 添加请求调试信息的记录
    debug_output = "<br>".join(debug_info)  # 将调试信息格式化为HTML
    return render_template_string(f"""
    <html>
    <head><title>Debug Info</title></head>
    <body>
        <h1>Debug Information</h1>
        <pre>{debug_output}</pre>
    </body>
    </html>
    """)

# 启动Flask服务器
if __name__ == '__main__':
    if not open_camera() or not setup_encoder():
        sys.exit("Failed to initialize camera or encoder.")
    
    app.run(host='0.0.0.0', port=5001, debug=False)
