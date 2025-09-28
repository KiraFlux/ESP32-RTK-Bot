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
        # убираем - сканирование теперь в движении
        pass

    def patrol_baths(self):
        print("=== ОБХОД ВАНН ===")

        # прямо до ванн
        print("Движение к ваннам")
        self.robot.go_dist(2400)

        # налево до конца первой ванны + сканирование
        print("Ванна 1: движение налево + сканирование")
        self.robot.turn(-0.25)  # поворот налево

        # едем и сканируем ванну 1
        for i in range(4):  # 4 точки сканирования по ходу движения
            self.robot.go_dist(100)  # двигаемся частями
            field = self.robot.check_magnetometer()
            print(f"  Точка {i + 1}: поле {field}")
            if field > 0 and 1 not in self.detected_faults:
                self.detected_faults.append(1)
                print("  *** ЗАМЫКАНИЕ В ВАННЕ 1 ***")

        # разворот для перехода ко второй ванне
        print("Переход к ванне 2")
        self.robot.turn(0.5)  # разворот 180
        self.robot.go_dist(400)  # назад
        self.robot.turn(-0.25)  # поворот к следующей ванне
        self.robot.go_dist(800)  # к ванне 2
        self.robot.turn(-0.25)  # поворот вдоль ванны

        # едем и сканируем ванну 2
        print("Ванна 2: движение + сканирование")
        for i in range(4):
            self.robot.go_dist(100)
            field = self.robot.check_magnetometer()
            print(f"  Точка {i + 1}: поле {field}")
            if field > 0 and 2 not in self.detected_faults:
                self.detected_faults.append(2)
                print("  *** ЗАМЫКАНИЕ В ВАННЕ 2 ***")

        # переход к ванне 3
        print("Переход к ванне 3")
        self.robot.turn(0.5)  # разворот
        self.robot.go_dist(400)  # назад
        self.robot.turn(-0.25)  # к следующей
        self.robot.go_dist(800)  # к ванне 3
        self.robot.turn(-0.25)  # вдоль ванны

        # едем и сканируем ванну 3
        print("Ванна 3: движение + сканирование")
        for i in range(4):
            self.robot.go_dist(100)
            field = self.robot.check_magnetometer()
            print(f"  Точка {i + 1}: поле {field}")
            if field > 0 and 3 not in self.detected_faults:
                self.detected_faults.append(3)
                print("  *** ЗАМЫКАНИЕ В ВАННЕ 3 ***")

        # переход к ванне 4
        print("Переход к ванне 4")
        self.robot.turn(0.5)  # разворот
        self.robot.go_dist(400)  # назад
        self.robot.turn(-0.25)  # к следующей
        self.robot.go_dist(800)  # к ванне 4
        self.robot.turn(-0.25)  # вдоль ванны

        # едем и сканируем ванну 4
        print("Ванна 4: движение + сканирование")
        for i in range(4):
            self.robot.go_dist(100)
            field = self.robot.check_magnetometer()
            print(f"  Точка {i + 1}: поле {field}")
            if field > 0 and 4 not in self.detected_faults:
                self.detected_faults.append(4)
                print("  *** ЗАМЫКАНИЕ В ВАННЕ 4 ***")

        print("Обход завершен")

    def return_to_start(self):
        print("=== ВОЗВРАТ ===")

        # от последней ванны к старту
        self.robot.turn(0.5)  # разворот
        self.robot.go_dist(400)  # от ванны
        self.robot.turn(0.25)  # к старту
        self.robot.go_dist(2400)  # назад к старту

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
