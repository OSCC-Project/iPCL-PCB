# noinspection PyShadowingNames
def get_str_guester(up_fingers: object, list_lms: object) -> object:
    if len(up_fingers) == 1 and up_fingers[0] == 8:

        v1 = list_lms[6] - list_lms[7]
        v2 = list_lms[8] - list_lms[7]

        angle = get_angle(v1, v2)

        if angle < 160:
            str_guested = "9"
        else:
            str_guested = "1"

    elif len(up_fingers) == 1 and up_fingers[0] == 4:
        str_guested = "Good"

    elif len(up_fingers) == 1 and up_fingers[0] == 20:
        str_guested = "Bad"

    elif len(up_fingers) == 1 and up_fingers[0] == 12:
        str_guested = "FXXX"

    elif len(up_fingers) == 2 and up_fingers[0] == 8 and up_fingers[1] == 12:
        str_guested = "2"
        #serial.write("2".encode())
    elif len(up_fingers) == 2 and up_fingers[0] == 4 and up_fingers[1] == 20:
        str_guested = "6"

    elif len(up_fingers) == 2 and up_fingers[0] == 4 and up_fingers[1] == 8:
        str_guested = "8"

    elif len(up_fingers) == 3 and up_fingers[0] == 8 and up_fingers[1] == 12 and up_fingers[2] == 16:
        str_guested = "3"
        #serial.write("3".encode())
    elif len(up_fingers) == 3 and up_fingers[0] == 4 and up_fingers[1] == 8 and up_fingers[2] == 12:

        dis_8_12 = list_lms[8, :] - list_lms[12, :]
        dis_8_12 = np.sqrt(np.dot(dis_8_12, dis_8_12))

        dis_4_12 = list_lms[4, :] - list_lms[12, :]
        dis_4_12 = np.sqrt(np.dot(dis_4_12, dis_4_12))

        if dis_4_12 / (dis_8_12 + 1) < 3:
            # noinspection PyShadowingNames
            str_guested = "7"

        elif dis_4_12 / (dis_8_12 + 1) > 5:
            str_guested = "Gun"
        else:
            str_guested = "7"

    elif len(up_fingers) == 3 and up_fingers[0] == 4 and up_fingers[1] == 8 and up_fingers[2] == 20:
        str_guested = "ROCK"
        

    elif len(up_fingers) == 4 and up_fingers[0] == 8 and up_fingers[1] == 12 and up_fingers[2] == 16 and up_fingers[
        3] == 20:
        str_guested = "4"
        #serial.write("4".encode())
    elif len(up_fingers) == 5:
        str_guested = "5"
        #serial.write("5".encode())
    elif len(up_fingers) == 0:
        str_guested = "0"
        #serial.write("0".encode())
    else:
        str_guested = " "

    return str_guested
