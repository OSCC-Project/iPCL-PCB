import socket  # 引入套接字
import threading  # 引入并行
import time
import json
import requests  # Requests 是⽤Python语⾔编写，基于urllib，采⽤Apache2 Licensed开源协议的 HTTP 库


# 判断JSON是否合法
def is_valid_json(json_str):
    try:
        json.loads(json_str)
        return True
    except ValueError:
        return False


# t2线程,UDP接收程序
def udp_recv(udp_socket):
    global u3_port
    global u3_ip
    global u3_f

    global g_temp
    global g_hum
    global g_lx
    global g_uv
    global g_wd
    global g_ws
    global g_bat
    global g_vin
    global g_gps

    while True:
        recv_data, addr1 = udp_socket.recvfrom(1024)  # recv_data = udp_socket.recv(1024)
        # str_utc = str(int(time.time()))
        # print(str_utc)
        print('\n收到', addr1, end=" ")  # 不带回车
        print('%s' % recv_data)

        str_dat = recv_data.decode('utf-8')
        if is_valid_json(str_dat):
            # print("JSON is valid")
            country_dict = json.loads(str_dat)
            # print(country_dict)
            if "t" in country_dict:
                g_temp = country_dict["t"]
                print("g_temp=%.2f" % g_temp)
            if "h" in country_dict:
                g_hum = country_dict["h"]
                print("g_hum=%.2f" % g_hum)
            if "lx" in country_dict:
                g_lx = country_dict["lx"]
                print("g_lx=%d" % g_lx)
            if "uv" in country_dict:
                g_uv = country_dict["uv"]
                print("g_uv=%d" % g_uv)
            if "b" in country_dict:
                g_bat = country_dict["b"]
                print("g_bat=%.1f" % g_bat)
            if "v" in country_dict:
                g_vin = country_dict["v"]
                print("g_vin=%.1f" % g_vin)
            # if "g" in country_dict:
            #     s_gps = country_dict["g"]
            #     # print(g_gps.find('/'))
            #     d_gps = s_gps.split('/')
            #     print(d_gps)
            #     print("g_gps=%s,%s" % (d_gps[0], d_gps[1]))
            #     g_gps = d_gps
            if "wd" in country_dict:
                g_wd = country_dict["wd"]
                print("g_wd=%s" % g_wd)
            if "ws" in country_dict:
                g_ws = country_dict["ws"]
                print("g_ws=%f" % g_ws)

            if "gps" in country_dict:
                s_gps = country_dict["gps"]
                if s_gps == 'null':
                    g_gps[0] = 0
                    g_gps[1] = 0
                else:
                    d_gps = s_gps.split('/')
                    print(d_gps)

                    if len(d_gps) < 2:
                        g_gps[0] = d_gps[0]
                        g_gps[1] = d_gps[0]
                    else:
                        g_gps[0] = d_gps[0]
                        g_gps[1] = d_gps[1]
                        # print("g_gps=%s,%s" % (d_gps[0], d_gps[1]))
                        # g_gps = d_gps
                print(g_gps)

            if "type" in country_dict:
                u3_ip, u3_port = addr1
                u3_f = 1
        else:
            print("JSON is invalid")


def udp_back(udp_socket):
    """
    t3线程 响应手机端的查询
    """
    global u3_ip  # 客户端 IP
    global u3_port  # 端口
    global u3_f

    global gw_temp  # 天气预报的温度
    global gw_hum  # 天气预报的湿度
    global gw_wea  # 天气,阴晴多云，雨

    global g_temp  # 气象仪上传的 温度数据
    global g_hum  # 温度
    global g_lx  # 环境照度
    global g_uv  # 紫外线强度
    global g_wd  # 实测风向
    global g_ws  # 风速
    global g_bat  # 电池电压
    global g_vin  # 光伏板 输入电压
    global g_gps  # GPS坐标

    gps_num = [22.782523, 108.2965]

    while True:
        if u3_f == 1:
            # print(u3_ip, end=":")
            # print(u3_port)
            u3_f = 0
            print(g_gps)
            gps_num[0] = float('%.7f' % (float(g_gps[0]) / 100))
            gps_num[1] = float('%.7f' % (float(g_gps[1]) / 100))
            # if g_gps != 'null':
            #     # 解析字符串到数组
            #     pass
            back_srt = '{"TEMP":' + str(round(g_temp, 2)) + ',"HUM":' + str(round(g_hum, 2)) + ',"UV":' + str(g_uv) + \
                       ',"LUX":' + str(g_lx) + ',"BAT":' + str(g_bat) + ',"SOLAR":' + str(g_vin) + \
                       ',"WD":"' + g_wd + '","WS":' + str(g_ws) + \
                       ',"GPSs":[' + str(gps_num[0]) + ',' + str(gps_num[1]) + ']' + \
                       ',"WTEMP":' + str(gw_temp) + ',"WHUM":"' + str(gw_hum) + '"}'
            print(back_srt)
            send_data = back_srt.encode('utf-8')
            udp_socket.sendto(send_data, (u3_ip, u3_port))  # sendto（发送数据，发送地址）


def get_weather(udp_socket):
    """
    t4线程 获取天气预报
    """
    global gw_temp  # 天气预报的温度
    global gw_hum  # 天气预报的湿度
    global gw_wea  # 天气,阴晴多云，雨

    while True:
        r = requests.get('http://t.weather.sojson.com/api/weather/city/101300101')
        # r = requests.get('http://wthrcdn.etouch.cn/weather_mini?city=北京')
        r.encoding = 'utf-8'
        print()
        print(r.json())
        d = r.json()
        if d['status'] == 200:
            print("城市：", d["cityInfo"]["parent"], d["cityInfo"]["city"])
            print("时间：", d["time"], d["data"]["forecast"][0]["week"])
            print("温度：", d["data"]["forecast"][0]["high"], d["data"]["forecast"][0]["low"], " 湿度：", d["data"]["shidu"])
            print("天气：", d["data"]["forecast"][0]["type"])

            gw_temp = d["data"]["wendu"]
            gw_hum = d["data"]["shidu"]
            gw_wea = d["data"]["forecast"][0]["type"]
        time.sleep(20)


global u3_ip  # 客户端 IP
global u3_port  # 端口
global u3_f

global gw_temp  # 天气预报的温度
global gw_hum  # 天气预报的湿度
global gw_wea  # 天气,阴晴多云，雨

global g_temp  # 气象仪上传的 温度数据
global g_hum  # 温度
global g_lx  # 环境照度
global g_uv  # 紫外线强度
global g_wd  # 实测风向
global g_ws  # 风速
global g_bat  # 电池电压
global g_vin  # 光伏板 输入电压
global g_gps  # GPS坐标

if __name__ == '__main__':
    # gps_num = [22.782523, 108.2965]
    g_temp = 0
    g_hum = 0
    g_lx = 0
    g_uv = 0
    g_wd = 'nc'
    g_ws = 0
    g_bat = 0
    g_vin = 0
    g_gps = ['2246.23198', '10817.21427']

    gw_temp = 0
    gw_hum = 0

    u3_port = 2000
    u3_ip = ''
    u3_f = 0

    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # 创建套接字
    ip = ''  # 服务器ip和端口
    port = 2048
    udp_socket.bind((ip, port))  # 服务器绑定ip和端口

    # 获取本地气象数据
    t1 = threading.Thread(target=get_weather, args=(udp_socket,))
    # 接收气象仪的数据
    t2 = threading.Thread(target=udp_recv, args=(udp_socket,))
    # 返回气象信息给手机
    t3 = threading.Thread(target=udp_back, args=(udp_socket,))

    t1.start()  # 启动线程序
    t2.start()
    t3.start()
