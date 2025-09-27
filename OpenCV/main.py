
from imutils.video import VideoStream


import cv2
import time
from typing import Optional


def get_aruco_marker_data(_video_stream: cv2.VideoCapture) -> Optional[str]:
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

    end = time.time() + 5

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
    camera_index = 1
    video_stream = VideoStream(src=camera_index).start()
    time.sleep(2.0)

    data = get_aruco_marker_data(video_stream)

    print(data)
    video_stream.stop()

    cv2.destroyAllWindows()
