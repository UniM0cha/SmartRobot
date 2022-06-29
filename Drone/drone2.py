from pickle import NONE
from djitellopy import tello
import cv2

me = tello.Tello()
me.connect()

me.streamoff()
print(me.get_battery())
me.streamon()


while True:
    img = me.get_frame_read().frame

    cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    height, width, _ = img.shape

    for i in range(360, height, 20):
        for j in range(0, width, 20):
            pixel = img[i, j]
            hue = pixel[0]
            if hue < 5:
                cv2.circle(img, (j, i), 3, (0, 0, 255), 1)
            else:
                cv2.circle(img, (j, i), 3, (0, 0, 0), 1)

    # cy = height - int(height / 25)
    # cx = int(width / 2)

    # cx_left = 440
    # cx_right = 520
    # cx_left_left = 400
    # cx_right_right = 560

    # pixel_center = img[cy, cx]
    # pixel_center_left = img[cy, cx_left]
    # pixel_center_left_left = img[cy, cx_left_left]
    # pixel_center_right = img[cy, cx_right]
    # pixel_center_right_right = img[cy, cx_right_right]
    # hue_value = pixel_center[0]
    # hue_value_left = pixel_center_left[0]
    # hue_value_left_left = pixel_center_left_left[0]
    # hue_value_right = pixel_center_right[0]
    # hue_value_right_right = pixel_center_right_right[0]

    # color = "Undefined"
    # color_left = "Undefined"
    # color_right = "Undefined"
    # if hue_value < 25:
    #     color = "RED"
    # elif hue_value < 78:
    #     color = "WHITE"
    # elif hue_value < 131:
    #     color = "BLUE"
    # elif hue_value < 170:
    #     color = "VIOLET"
    # else:
    #     color = "WHITE"

    # if hue_value_left < 25:
    #     color_left = "RED"
    # elif hue_value_left < 78:
    #     color_left = "WHITE"
    # elif hue_value_left < 131:
    #     color_left = "BLUE"
    # elif hue_value_left < 170:
    #     color_left = "VIOLET"
    # else:
    #     color_left = "WHITE"

    # if hue_value_left < 25:
    #     color_left_left = "RED"
    # elif hue_value_left < 78:
    #     color_left_left = "WHITE"
    # elif hue_value_left < 131:
    #     color_left_left = "BLUE"
    # elif hue_value_left < 170:
    #     color_left_left = "VIOLET"
    # else:
    #     color_left_left = "WHITE"

    # if hue_value_right < 25:
    #     color_right = "RED"
    # elif hue_value_right < 78:
    #     color_right = "WHITE"
    # elif hue_value_right < 131:
    #     color_right = "BLUE"
    # elif hue_value_right < 170:
    #     color_right = "VIOLET"
    # else:
    #     color_right = "WHITE"

    # if hue_value_right < 25:
    #     color_right_right = "RED"
    # elif hue_value_right < 78:
    #     color_right_right = "WHITE"
    # elif hue_value_right < 131:
    #     color_right_right = "BLUE"
    # elif hue_value_right < 170:
    #     color_right_right = "VIOLET"
    # else:
    #     color_right_right = "WHITE"

    # pixel_center_bgr = img[cy, cx]
    # cv2.putText(img, color_left_left, (10, 50), 0, 1, (255, 0, 0), 2)
    # cv2.putText(img, color_left, (200, 50), 0, 1, (255, 0, 0), 2)
    # cv2.putText(img, color, (400, 50), 0, 1, (255, 0, 0), 2)
    # cv2.putText(img, color_right, (600, 50), 0, 1, (255, 0, 0), 2)
    # cv2.putText(img, color_right_right, (800, 50), 0, 1, (255, 0, 0), 2)

    # cv2.circle(img, (cx, cy), 5, (255, 0, 0), 3)
    # cv2.circle(img, (cx_left, cy), 5, (0, 0, 255), 3)
    # cv2.circle(img, (cx_right, cy), 5, (0, 255, 0), 3)
    # cv2.circle(img, (cx_left_left, cy), 5, (0, 0, 255), 3)
    # cv2.circle(img, (cx_right_right, cy), 5, (0, 255, 0), 3)

    cv2.imshow("Frame", img)
    key = cv2.waitKey(2)
