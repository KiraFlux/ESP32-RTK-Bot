from imutils.video import VideoStream
import argparse
import datetime
import imutils
import time
import cv2


def test_cameras():
    """Тестирует все доступные камеры и позволяет выбрать нужную"""
    cameras = []
    for i in range(10):
        cap = cv2.VideoCapture(i)
        if cap.isOpened():
            ret, frame = cap.read()
            if ret:
                cameras.append(i)
                print(f"Camera {i}: Found")
            cap.release()
        else:
            print(f"Camera {i}: Not found")

    if not cameras:
        print("No cameras found!")
        return 0

    if len(cameras) == 1:
        return cameras[0]

    print("\nAvailable cameras:", cameras)
    choice = input("Enter camera index to use (default 0): ")
    return int(choice) if choice.isdigit() else cameras[0]


# Выбираем камеру
camera_index = 1
print(f"Using camera index: {camera_index}")
# Инициализация детектора QR-кодов OpenCV
qr_code_detector = cv2.QRCodeDetector()

ap = argparse.ArgumentParser()
ap.add_argument("-o", "--output", type=str, default="barcodes.csv",
                help="path to output CSV file containing barcodes")
args = vars(ap.parse_args())

vs = VideoStream(src=camera_index).start()
time.sleep(2.0)
csv = open(args["output"], "w")
found = set()

while True:
    frame = vs.read()
    frame = imutils.resize(frame, width=400)

    # Детектирование QR-кодов с помощью OpenCV
    data, bbox, _ = qr_code_detector.detectAndDecode(frame)

    if data and bbox is not None:
        # Преобразуем bbox в координаты прямоугольника
        bbox = bbox[0].astype(int)
        x, y = bbox[0]
        w = bbox[2][0] - bbox[0][0]
        h = bbox[2][1] - bbox[0][1]

        # Рисуем прямоугольник вокруг QR-кода
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)

        barcodeData = data
        barcodeType = "QRCODE"
        text = "{} ({})".format(barcodeData, barcodeType)
        print(text)

        cv2.putText(frame, text, (x, y - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)

        # if the barcode text is currently not in our CSV file, write
        if barcodeData not in found:
            try:
                csv.write(f"{datetime.datetime.now()},{barcodeData}\n")
            except TypeError:
                print(f"{bytes.decode(barcodeData)}")
            csv.flush()
            found.add(barcodeData)

    cv2.imshow("Barcode Reader", frame)
    key = cv2.waitKey(1) & 0xFF

    # if the `s` key is pressed, break from the loop
    if key == ord("s"):
        break

print("[INFO] cleaning up...")
csv.close()
cv2.destroyAllWindows()
vs.stop()
