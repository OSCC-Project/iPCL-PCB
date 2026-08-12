import mediapipe as mp
import cv2
import numpy as np
#import serial

if __name__ == "__main__":
    #serial = serial.Serial('COM4', 115200, timeout=0.5) #此com口根据自己的电脑更改com端口和波特率
    #if serial.isOpen():
    #    print("open success")
    #else:
    #    print("open failed")
    cap = cv2.VideoCapture(0)
    # 定义手 检测对象
    mpHands = mp.solutions.hands
    hands = mpHands.Hands()
    mpDraw = mp.solutions.drawing_utils

    # 创建一个 黑色图像


    while True:
        image = cv2.imread("pic.jpg", 1)
        # 读取一帧图像
        success, img = cap.read()
        if not success:
            continue
        image_height, image_width, _ = np.shape(img)

        # 转换为RGB
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

        # 得到检测结果
        results = hands.process(imgRGB)

        if results.multi_hand_landmarks:
            hand = results.multi_hand_landmarks[0]

            mpDraw.draw_landmarks(image, hand, mpHands.HAND_CONNECTIONS)

            # 采集所有关键点的坐标
            list_lms = []
            for i in range(21):
                pos_x = hand.landmark[i].x * image_width
                pos_y = hand.landmark[i].y * image_height
                list_lms.append([int(pos_x), int(pos_y)])

            # 构造凸包点
            list_lms = np.array(list_lms, dtype=np.int32)
            hull_index = [0, 1, 2, 3, 6, 10, 14, 19, 18, 17, 10]
            hull = cv2.convexHull(list_lms[hull_index, :])
            # 绘制凸包
            #cv2.polylines(image, [hull], True, (0, 255, 0), 2)

            # 查找外部的点数
            n_fig = -1
            ll = [4, 8, 12, 16, 20]
            up_fingers = []

            for i in ll:
                pt = (int(list_lms[i][0]), int(list_lms[i][1]))
                dist = cv2.pointPolygonTest(hull, pt, True)
                if dist < 0:
                    up_fingers.append(i)
            #get_str_guester(up_fingers, list_lms)#这行代码用于使能手势控制
            for i in ll:
                pos_x = hand.landmark[i].x * image_width
                pos_y = hand.landmark[i].y * image_height
                # 画点
                #cv2.circle(image, (int(pos_x), int(pos_y)), 3, (0, 255, 255), -1)

        cv2.imshow("hands", image)

        key = cv2.waitKey(1) & 0xFF

        # 按键 "q" 退出
        if key == ord('q'):
            break
    cap.release()