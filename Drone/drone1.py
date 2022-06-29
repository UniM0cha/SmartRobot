from djitellopy import tello
from time import sleep
import cv2

me = tello.Tello()

global color
color = "undefined"


def start():
    me.move_up(50)
    sleep(0.5)
    me.move_forward(100)
    sleep(0.5)
    me.move_down(70)
    sleep(0.5)


def right():
    me.move_right(100)


def left():
    me.move_left(100)


def back():
    me.move_up(50)
    sleep(0.1)
    me.move_left(100)
    sleep(0.1)
    me.move_back(320)


def colordetect():
    while True:
        global color
        img = me.get_frame_read().frame

        hsv_frame = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        height, width, _ = img.shape

        # 가로 960 픽셀, 세로 720 픽셀
        cx = int(width / 2)
        cy = height - int(height / 25)
        print(height)
        cx_left = 440
        cx_right = 520
        cx_left_left = 400
        cx_right_right = 560

        pixel_center = img[cy, cx]
        pixel_center_left = img[cy, cx_left]
        pixel_center_left_left = img[cy, cx_left_left]
        pixel_center_right = img[cy, cx_right]
        pixel_center_right_right = img[cy, cx_right_right]
        hue_value = pixel_center[0]
        hue_value_left = pixel_center_left[0]
        hue_value_left_left = pixel_center_left_left[0]
        hue_value_right = pixel_center_right[0]
        hue_value_right_right = pixel_center_right_right[0]

        color = "Undefined"
        color_left = "Undefined"
        color_right = "Undefined"
        color_left_left = "Undefined"
        color_right_right = "Undefined"

        if hue_value < 25:
            color = "RED"
        elif hue_value < 33:
            color = "WHITE"
        elif hue_value < 78:
            color = "RED"
        elif hue_value < 170:
            color = "VIOLET"
        else:
            color = "WHITE"

        if hue_value_left < 25:
            color = "RED"
        elif hue_value_left < 78:
            color = "WHITE"
        elif hue_value_left < 131:
            color = "BLUE"
        elif hue_value_left < 170:
            color = "VIOLET"
        else:
            color = "WHITE"

        if hue_value_left < 25:
            color = "RED"
        elif hue_value_left < 78:
            color = "WHITE"
        elif hue_value_left < 131:
            color = "BLUE"
        elif hue_value_left < 170:
            color = "VIOLET"
        else:
            color = "WHITE"

        if hue_value_right < 25:
            color = "RED"
        elif hue_value_right < 78:
            color = "WHITE"
        elif hue_value_right < 131:
            color = "BLUE"
        elif hue_value_right < 170:
            color = "VIOLET"
        else:
            color = "WHITE"

        if hue_value_right < 25:
            color = "RED"
        elif hue_value_right < 78:
            color = "WHITE"
        elif hue_value_right < 131:
            color = "BLUE"
        elif hue_value_right < 170:
            color = "VIOLET"
        else:
            color = "WHITE"

        cv2.putText(img, color_left_left, (10, 50), 0, 1, (255, 0, 0), 2)
        cv2.putText(img, color_left, (200, 50), 0, 1, (255, 0, 0), 2)
        cv2.putText(img, color, (400, 50), 0, 1, (255, 0, 0), 2)
        cv2.putText(img, color_right, (600, 50), 0, 1, (255, 0, 0), 2)
        cv2.putText(img, color_right_right, (800, 50), 0, 1, (255, 0, 0), 2)

        cv2.circle(img, (cx, cy), 5, (255, 0, 0), 3)
        cv2.circle(img, (cx_left, cy), 5, (0, 0, 255), 3)
        cv2.circle(img, (cx_right, cy), 5, (0, 255, 0), 3)
        cv2.circle(img, (cx_left_left, cy), 5, (0, 0, 255), 3)
        cv2.circle(img, (cx_right_right, cy), 5, (0, 255, 0), 3)

        cv2.imshow("Frame", img)
        key = cv2.waitKey(2)

        if key == 27:
            break
        elif color == "RED":
            print(color)
            return color
            break
        elif color == "BLUE":
            print(color)
            return color
            break
        else:
            print(color)
            return color


def main():
    global color
    me.connect()
    print(me.get_battery())
    me.takeoff()
    me.streamoff()

    # start()
    # right()
    i = 1
    me.streamon()
    while True:
        colordetect()
        if color == "RED":
            sleep(0.1)
            me.move_forward(160)
            sleep(0.5)
            me.land()
            break
        elif color == "BLUE":
            sleep(0.1)
            me.move_forward(160)
            sleep(0.5)
            me.land()
            break
        else:
            me.move_forward(20)
            sleep(0.5)
            colordetect()
            i = i + 1
            if i == 5:
                me.land()
            continue
    back()


if __name__ == "__main__":
    main()
