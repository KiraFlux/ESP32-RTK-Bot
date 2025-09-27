import time
from typing import Optional

import cv2
from imutils.video import VideoStream
import time
import csv
import datetime
import os
import re
























def get_qr_code_data(_video_stream: VideoStream) -> Optional[str]:
    qr_code_detector = cv2.QRCodeDetector()

    end = time.time() + 5

    while time.time() < end:
        frame = _video_stream.read()
        _data, bbox, _ = qr_code_detector.detectAndDecode(frame)
        cv2.imshow('main', frame)

        if _data and bbox is not None:
            return _data

        if cv2.waitKey(1) & 0xff == ord('q'):
            break

    return None




if __name__ == '__main__':
    camera_index = 1
    video_stream = VideoStream(src=camera_index).start()
    time.sleep(2.0)

    while True:
        data = get_qr_code_data(video_stream)
        print(data)

        if cv2.waitKey(1) & 0xff == ord('q'):
            break

    video_stream.stop()

    cv2.destroyAllWindows()
