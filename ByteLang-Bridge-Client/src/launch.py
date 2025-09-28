#!/usr/bin/env python3
"""
Лаунчер для обхода первых 4 ванн
"""

from time import sleep
from imutils.video import VideoStream
import cv2

from robot import Robot
from main import get_aruco_marker_data


class MissionLauncher:
    def __init__(self):
        self.robot = Robot()
        self.detected_faults = []
        self.video_stream = None

        print("Робот готов к миссии")

    def start_mission(self):
        print("=== ЗАПУСК ===")

        self.robot.start_poll_task()
        sleep(2)

        self.video_stream = VideoStream(src=1).start()
        sleep(2)

        print("Система готова")

    def check_bath(self, bath_number):
        print(f"Проверка ванны {bath_number}")

        sleep(0.5)

        field_value = self.robot.check_magnetometer()
        print(f"  Поле: {field_value}")

        if field_value > 0:
            self.detected_faults.append(bath_number)
            print(f"  *** ЗАМЫКАНИЕ В ВАННЕ {bath_number} ***")
            return True
        else:
            print(f"  Ванна {bath_number} в норме")
            return False

    def patrol_baths(self):
        print("=== ОБХОД ВАНН ===")

        # к первой ванне
        print("Движение к ваннам")
        self.robot.go_dist(1000)

        # поворот к ваннам
        self.robot.turn(0.25)

        # к центру первой ванны
        self.robot.go_dist(400)

        # обход ванн 1-4
        for bath_num in range(1, 5):
            print(f"\n--- ВАННА {bath_num} ---")

            self.check_bath(bath_num)

            if bath_num < 4:
                print(f"К ванне {bath_num + 1}")
                self.robot.go_dist(800)

        print("Обход завершен")

    def return_to_start(self):
        print("=== ВОЗВРАТ ===")

        # разворот
        self.robot.turn(0.5)

        # обратно вдоль ванн
        self.robot.go_dist(2200)

        # к стартовой области
        self.robot.turn(0.25)

        # к стартовой ячейке
        self.robot.go_dist(1400)

        print("Возврат завершен")

    def scan_and_press(self):
        print("=== КНОПКИ ===")

        if not self.detected_faults:
            print("Замыкания не найдены")
            return

        print(f"Нужно нажать: {self.detected_faults}")

        # сканирование арuco
        markers_data = get_aruco_marker_data(self.video_stream)

        if not markers_data:
            print("Маркеры не найдены!")
            return

        found_markers = [int(id.strip()) for id in markers_data.split(',') if id.strip()]
        print(f"Найдены маркеры: {found_markers}")

        # к левому краю кнопок
        self.robot.go_dist(-150)

        current_position = 1

        for bath_num in self.detected_faults:
            if bath_num in found_markers and bath_num <= 4:
                print(f"\nНажатие кнопки {bath_num}")

                # движение к кнопке
                if current_position < bath_num:
                    distance = (bath_num - current_position) * 100
                    self.robot.go_dist(distance)
                elif current_position > bath_num:
                    distance = (current_position - bath_num) * 100
                    self.robot.go_dist(-distance)

                current_position = bath_num

                # нажатие
                self.robot.go_dist(300)
                sleep(1)
                print("  Нажата!")
                self.robot.go_dist(-300)

            else:
                print(f"Кнопка {bath_num} недоступна")

    def run_mission(self):
        print("=== МИССИЯ: 4 ВАННЫ ===")

        self.start_mission()

        self.patrol_baths()

        self.return_to_start()

        self.scan_and_press()

        print("\n=== ЗАВЕРШЕНО ===")
        print(f"Проверено: 4 ванны")
        print(f"Замыканий: {len(self.detected_faults)}")
        if self.detected_faults:
            print(f"Ванны: {self.detected_faults}")

        if self.video_stream:
            self.video_stream.stop()
            cv2.destroyAllWindows()


def main():
    print("=== РОБОТ ДЛЯ 4 ВАНН ===")

    launcher = MissionLauncher()

    try:
        launcher.run_mission()
    except KeyboardInterrupt:
        print("\nОстановлено")
    except Exception as e:
        print(f"Ошибка: {e}")


if __name__ == "__main__":
    main()
