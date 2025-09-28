
from imutils.video import VideoStream


import cv2
import time
from typing import Optional

import cv2
import numpy as np


def detect_red_in_frame_debug(_video_stream, min_area=500, show_windows=False):

    while True:
        # Чтение кадра
        frame = _video_stream.read()


        # Конвертация в HSV
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        # Диапазоны красного цвета (в HSV)
        lower_red1 = np.array([0, 100, 100])
        upper_red1 = np.array([10, 255, 255])
        lower_red2 = np.array([160, 100, 100])
        upper_red2 = np.array([180, 255, 255])

        # Создание масок
        mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
        mask2 = cv2.inRange(hsv, lower_red2, upper_red2)
        red_mask = cv2.bitwise_or(mask1, mask2)


        kernel = np.ones((5, 5), np.uint8)
        red_mask_cleaned = cv2.morphologyEx(red_mask, cv2.MORPH_OPEN, kernel)
        red_mask_cleaned = cv2.morphologyEx(red_mask_cleaned, cv2.MORPH_CLOSE, kernel)

        # Поиск контуров
        contours, _ = cv2.findContours(red_mask_cleaned, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        red_detected = False
        for contour in contours:
            area = cv2.contourArea(contour)
            if area > min_area:
                red_detected = True
                if show_windows:
                    # Рисуем контур на исходном кадре
                    cv2.drawContours(frame, [contour], -1, (0, 255, 0), 2)

            # Выход по клавише 'q'
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        # Возвращаем результат для текущего кадра, но не выходим из цикла
        return red_detected

    if show_windows:
        cv2.destroyAllWindows()


def get_aruco_marker_data(_video_stream: VideoStream) -> Optional[str]:
    try:
        aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_5X5_250)
        parameters = cv2.aruco.DetectorParameters()
        detector = cv2.aruco.ArucoDetector(aruco_dict, parameters)
        use_new_api = True
    except AttributeError:

        try:
            aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_5X5_250)
            parameters = cv2.aruco.DetectorParameters()
            use_new_api = False
        except AttributeError:

            aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_5X5_250)
            detector = cv2.aruco.ArucoDetector()
            use_new_api = True

    end = time.time() + 10

    while time.time() < end:
        frame = _video_stream.read()



        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        try:
            if use_new_api:
                corners, ids, rejected = detector.detectMarkers(gray)
            else:
                corners, ids, rejected = cv2.aruco.detectMarkers(
                    gray, aruco_dict, parameters=parameters
                )
        except Exception as e:
            print(f"Ошибка обнаружения маркеров: {e}")
            continue


        if ids is not None:
            cv2.aruco.drawDetectedMarkers(frame, corners, ids)

        cv2.imshow('main', frame)


        if ids is not None:
            ids_str = ','.join(str(int(id)) for id in ids.flatten())
            return ids_str

        if cv2.waitKey(1) & 0xff == ord('q'):
            break

    return None



if __name__ == '__main__':
    camera_index = 0
    video_stream = VideoStream(src=camera_index).start()
    time.sleep(2.0)

    data = get_aruco_marker_data(video_stream)
    result = detect_red_in_frame_debug(video_stream)
    print(data)
    print(result)
    video_stream.stop()
    cv2.destroyAllWindows()
