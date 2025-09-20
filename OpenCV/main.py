import time
from typing import Sequence

import cv2
import imutils
from imutils.video import VideoStream


def get_available_cameras() -> Sequence[int]:
    """Тестирует все доступные камеры и позволяет выбрать нужную"""
    cameras = [0]

    for i in range(1, 10):
        cap = cv2.VideoCapture(i)

        if cap.isOpened():
            ret, _ = cap.read()

            if ret:
                cameras.append(i)
                print(f"Camera {i}: Found")

            cap.release()

    return cameras


def _start():
    # Выбираем камеру
    camera_index = 0
    print(f"Using camera index: {camera_index}")
    qr_code_detector = cv2.QRCodeDetector()

    video_stream = VideoStream(src=camera_index).start()
    time.sleep(2.0)

    while True:
        frame = video_stream.read()
        # frame = imutils.resize(frame, width=400)

        # Детектирование QR-кодов с помощью OpenCV
        data, bbox, _ = qr_code_detector.detectAndDecode(frame)

        if data and bbox is not None:
            bbox = bbox[0].astype(int)
            x, y = bbox[0]
            w = bbox[2][0] - bbox[0][0]
            h = bbox[2][1] - bbox[0][1]

            # Рисуем прямоугольник вокруг QR-кода
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)

            barcode_data = data
            text = f'{barcode_data}'
            print(text)
            cv2.putText(frame, text, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)

        cv2.imshow("Barcode Reader", frame)
        key = cv2.waitKey(1) & 0xFF

        if key == ord("s"):
            break

    cv2.destroyAllWindows()
    video_stream.stop()


if __name__ == '__main__':
    _start()
