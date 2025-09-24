import time
import cv2
from imutils.video import VideoStream

import csv
import datetime
import os
import re

# Глобальные переменные для протоколирования
PROTOCOL_FILE = 'short_circuit_protocol.csv'
scanned_codes = set()  # Для избежания дублирования записей


def initialize_protocol():
    """Создает файл протокола с заголовками если его нет"""
    if not os.path.exists(PROTOCOL_FILE):
        with open(PROTOCOL_FILE, 'w', newline='', encoding='utf-8') as file:
            writer = csv.writer(file, delimiter=';')
            writer.writerow(['Номер ванны', 'Номер пары анод-катод с замыканием'])



def parse_bath_pair_data(qr_data):
    """Парсит данные из QR-кода для извлечения номера ванны и пары"""
    # Различные форматы, которые могут быть в QR-коде
    patterns = [
        r'Ванна(\d+)[-_]Пара(\d+)',
        r'Bath(\d+)[-_]Pair(\d+)',
        r'(\d+)[-_](\d+)',
        r'В(\d+)[-_]П(\d+)',
        r'ванна\s*(\d+).*пара\s*(\d+)',
        r'bath\s*(\d+).*pair\s*(\d+)'
    ]

    for pattern in patterns:
        match = re.search(pattern, qr_data, re.IGNORECASE)
        if match:
            bath_number = match.group(1)
            pair_number = match.group(2)
            return bath_number, pair_number

    # Если стандартные форматы не подошли, ищем любые два числа
    numbers = re.findall(r'\d+', qr_data)
    if len(numbers) >= 2:
        return numbers[0], numbers[1]

    # Если только одно число - считаем его номером ванны, пару = 1
    if len(numbers) == 1:
        return numbers[0], "1"

    return None, None


def log_to_protocol(bath_number, pair_number):
    """Записывает данные в протокол"""
    try:
        with open(PROTOCOL_FILE, 'a', newline='', encoding='utf-8') as file:
            writer = csv.writer(file, delimiter=';')
            writer.writerow([bath_number, pair_number])

        print(f"Записано в протокол: Ванна {bath_number}, Пара {pair_number}")
        return True
    except Exception as e:
        print(f"Ошибка записи в протокол: {e}")
        return False


def _start():
    # Выбираем камеру
    camera_index = 1

    qr_code_detector = cv2.QRCodeDetector()

    video_stream = VideoStream(src=camera_index).start()
    time.sleep(2.0)

    while True:
        frame = video_stream.read()
        data, bbox, _ = qr_code_detector.detectAndDecode(frame)

        if data and bbox is not None:
            bbox = bbox[0].astype(int)
            x, y = bbox[0]
            w = bbox[2][0] - bbox[0][0]
            h = bbox[2][1] - bbox[0][1]

            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)

            barcode_data = data

            text = str(barcode_data)

            print(text)
            cv2.putText(frame, text, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)

            if text not in scanned_codes:
                scanned_codes.add(text)

                bath_number, pair_number = parse_bath_pair_data(text)

                if bath_number and pair_number:
                    # Записываем в протокол
                    log_to_protocol(bath_number, pair_number)

                    # Выводим информацию о распознавании
                    info_text = f"Ванна: {bath_number}, Пара: {pair_number}"
                    cv2.putText(frame, info_text, (x, y - 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

                else:
                    warning_text = "Формат не распознан"
                    cv2.putText(frame, warning_text, (x, y - 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 255), 2)
                    print(f"Предупреждение: {warning_text} - {text}")

        cv2.imshow("Barcode Reader", frame)
        key = cv2.waitKey(1) & 0xFF

        if key == ord("q"):
            break

    cv2.destroyAllWindows()
    video_stream.stop()

    # Выводим итоговую информацию
    print(f"\nПротокол сохранен в файле: {PROTOCOL_FILE}")
    try:
        with open(PROTOCOL_FILE, 'r', encoding='utf-8') as file:
            records = list(csv.reader(file, delimiter=';'))
            print(f"Всего записей в протоколе: {len(records) - 1}")  # -1 для заголовка
    except:
        pass


if __name__ == '__main__':
    _start()
