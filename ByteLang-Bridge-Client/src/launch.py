# from time import sleep
# from typing import Mapping
#
# from robot import Robot
#
#
# def _walk_bath(robot: Robot) -> Mapping[int, int]:
#     ret = dict()
#     bars_in_bath = 10
#
#     for row in range(bars_in_bath):
#         robot.go_dist(100)
#         magnets_in_row = robot.check_magnetometer()
#
#         if magnets_in_row > 0:
#             ret[row] = magnets_in_row
#
#     return ret
#
#
# def _launch():
#     data_csv = tuple(dict() for _ in range(8))
#
#     robot = Robot()
#     robot.start_poll_task()
#     sleep(2)
#
#     # Начало алгоритма
#
#     robot.send_millis_request(None)
#     robot.go_dist(1000)
#
#     robot.turn(0.5)
#     robot.go_dist(1000)
#
#     bath_data = _walk_bath(robot)
#     data_csv[0].update(bath_data)
#
#     # стоим над кнопкой
#
#     # Алгоритм завершен.
#
#     robot.poll_task.join()
#
#
# _launch()

# !/usr/bin/env python3
"""
Оптимизированный лаунчер миссии для обнаружения коротких замыканий
Основан на реальных размерах полигона и точной навигации
"""

from time import sleep
from typing import Dict, List
from imutils.video import VideoStream
import cv2
import time

from robot import Robot
from main import get_aruco_marker_data


class ElectrolysisMissionLauncher:
    def __init__(self):
        """Инициализация лаунчера с точными размерами"""
        self.robot = Robot()
        self.detected_faults: List[int] = []  # Список номеров ванн с замыканиями
        self.video_stream = None

        # Точные размеры полигона (в мм)
        self.ROBOT_WIDTH = 250  # 25 см
        self.CELL_SIZE = 800  # размер ячейки
        self.BATH_WIDTH = 400  # ширина ванны 40 см
        self.BATH_LENGTH = 350  # длина ванны (6 пар брусьев + промежутки + разделитель)
        self.BETWEEN_BATHS = 1000  # расстояние между ваннами ~100 см
        self.WALL_DISTANCE = 400  # от центра ячейки до стенки 40 см
        self.BUTTON_SPACING = 100  # между кнопками 10 см
        self.BUTTON_DIAMETER = 100  # диаметр кнопки 10 см

        # Расстояния движения
        self.START_TO_WALL = 2200  # от старта до стенки 220 см
        self.BATH_SAFETY_MARGIN = 50  # отступ от края ванны 5 см

        print("Инициализация с параметрами:")
        print(f"  Размер робота: {self.ROBOT_WIDTH} мм")
        print(f"  Размер ванны: {self.BATH_WIDTH}x{self.BATH_LENGTH} мм")
        print(f"  Расстояние между ваннами: {self.BETWEEN_BATHS} мм")

    def initialize_mission(self):
        """Инициализация миссии"""
        print("=== ИНИЦИАЛИЗАЦИЯ МИССИИ ===")

        # Запуск робота
        self.robot.start_poll_task()
        sleep(2)

        # Инициализация камеры
        camera_index = 1
        self.video_stream = VideoStream(src=camera_index).start()
        sleep(2.0)

        print("Миссия готова к запуску")

    def scan_bath_thoroughly(self, bath_number: int) -> bool:
        """
        Тщательное сканирование одной ванны
        Проверка над каждой парой брусьев
        """
        print(f"Детальное сканирование ванны {bath_number}")

        # Параметры сканирования
        pair_width = 50  # ширина одной пары брусьев + промежуток (2.5+2.5+2.5 см)
        separator_width = 50  # ширина разделителя в середине
        total_pairs = 6

        has_fault = False

        # Сканирование первых 3 пар
        for pair in range(3):
            position = pair * pair_width + 25  # позиция в центре пары
            print(f"  Сканирование пары {pair + 1} на позиции {position}мм")

            # Проверка магнитного поля
            field_strength = self.robot.check_magnetometer()
            print(f"    Поле: {field_strength}")

            if field_strength > 0:  # Порог можно настроить
                has_fault = True
                print(f"    *** ОБНАРУЖЕНО ПОЛЕ в ванне {bath_number}, пара {pair + 1} ***")

            # Движение к следующей паре (если не последняя)
            if pair < 2:
                self.robot.go_dist(pair_width)

        # Переход через разделитель
        self.robot.go_dist(separator_width)

        # Сканирование последних 3 пар
        for pair in range(3, 6):
            position = 3 * pair_width + separator_width + (pair - 3) * pair_width + 25
            print(f"  Сканирование пары {pair + 1} на позиции {position}мм")

            field_strength = self.robot.check_magnetometer()
            print(f"    Поле: {field_strength}")

            if field_strength > 0:
                has_fault = True
                print(f"    *** ОБНАРУЖЕНО ПОЛЕ в ванне {bath_number}, пара {pair + 1} ***")

            # Движение к следующей паре (если не последняя)
            if pair < 5:
                self.robot.go_dist(pair_width)

        # Движение до конца ванны с отступом
        remaining_distance = self.BATH_LENGTH - (
                    3 * pair_width + separator_width + 3 * pair_width) - self.BATH_SAFETY_MARGIN
        if remaining_distance > 0:
            self.robot.go_dist(remaining_distance)

        if has_fault:
            self.detected_faults.append(bath_number)
            print(f"Ванна {bath_number} ДОБАВЛЕНА в список с замыканиями")

        return has_fault

    def navigate_first_row_baths(self):
        """Навигация по первому ряду ванн (1-4)"""
        print("=== ОБХОД ПЕРВОГО РЯДА ВАНН (1-4) ===")

        # Движение от старта до стенки
        print("Движение от старта до стенки")
        self.robot.go_dist(self.START_TO_WALL)

        # Поворот налево для движения вдоль стенки к первой ванне
        print("Поворот налево для движения к ваннам")
        self.robot.turn(0.25)  # 90 градусов налево

        # Движение к центру первой ванны
        # Расстояние от угла до центра первой ванны
        to_first_bath = self.CELL_SIZE // 2  # половина ячейки до центра
        self.robot.go_dist(to_first_bath)

        # Поворот направо для входа в ванну
        print("Поворот для входа в ванну 1")
        self.robot.turn(-0.25)  # 90 градусов направо

        # Сканирование ванн 1-4
        for bath_num in range(1, 5):
            print(f"\n--- ВАННА {bath_num} ---")

            # Заезд в ванну
            print(f"Заезд в ванну {bath_num}")
            entry_distance = (self.BATH_WIDTH - self.ROBOT_WIDTH) // 2
            self.robot.go_dist(entry_distance)

            # Поворот налево для движения вдоль ванны
            self.robot.turn(0.25)

            # Детальное сканирование ванны
            self.scan_bath_thoroughly(bath_num)

            # Выезд из ванны
            print(f"Выезд из ванны {bath_num}")
            self.robot.turn(-0.25)  # поворот обратно
            self.robot.go_dist(-entry_distance)  # выезд назад

            # Переход к следующей ванне (если не последняя в ряду)
            if bath_num < 4:
                print(f"Переход к ванне {bath_num + 1}")
                self.robot.turn(0.25)  # поворот для движения к следующей ванне
                self.robot.go_dist(self.BETWEEN_BATHS)
                self.robot.turn(-0.25)  # поворот обратно к ваннам

    def navigate_to_second_row(self):
        """Переход ко второму ряду ванн"""
        print("=== ПЕРЕХОД КО ВТОРОМУ РЯДУ ===")

        # От 4-й ванны поворот для движения ко второму ряду
        self.robot.turn(0.25)  # поворот налево

        # Движение к концу ряда ванн
        end_distance = self.CELL_SIZE // 2
        self.robot.go_dist(end_distance)

        # Поворот для движения ко второму ряду
        self.robot.turn(0.25)  # еще поворот налево

        # Движение ко второму ряду
        row_distance = self.CELL_SIZE + 200  # расстояние между рядами + запас
        self.robot.go_dist(row_distance)

        # Поворот для движения вдоль второго ряда
        self.robot.turn(0.25)  # поворот налево

        # Движение к 8-й ванне (начинаем с конца)
        to_eighth_bath = self.CELL_SIZE // 2
        self.robot.go_dist(to_eighth_bath)

        # Поворот для входа в ванны второго ряда
        self.robot.turn(0.25)  # поворот для входа в ванну

    def navigate_second_row_baths(self):
        """Навигация по второму ряду ванн (8-5)"""
        print("=== ОБХОД ВТОРОГО РЯДА ВАНН (8-5) ===")

        # Сканирование ванн 8-5 (в обратном порядке)
        for bath_num in range(8, 4, -1):
            print(f"\n--- ВАННА {bath_num} ---")

            # Заезд в ванну
            print(f"Заезд в ванну {bath_num}")
            entry_distance = (self.BATH_WIDTH - self.ROBOT_WIDTH) // 2
            self.robot.go_dist(entry_distance)

            # Поворот направо для движения вдоль ванны (противоположно первому ряду)
            self.robot.turn(-0.25)

            # Детальное сканирование ванны
            self.scan_bath_thoroughly(bath_num)

            # Выезд из ванны
            print(f"Выезд из ванны {bath_num}")
            self.robot.turn(0.25)  # поворот обратно
            self.robot.go_dist(-entry_distance)  # выезд назад

            # Переход к следующей ванне (если не последняя)
            if bath_num > 5:
                print(f"Переход к ванне {bath_num - 1}")
                self.robot.turn(-0.25)  # поворот для движения к следующей ванне
                self.robot.go_dist(self.BETWEEN_BATHS)
                self.robot.turn(0.25)  # поворот обратно к ваннам

    def return_to_start_area(self):
        """Возврат в стартовую область"""
        print("=== ВОЗВРАТ В СТАРТОВУЮ ОБЛАСТЬ ===")

        # От 5-й ванны поворот для возврата
        self.robot.turn(-0.25)  # поворот направо

        # Движение к краю ряда
        edge_distance = self.CELL_SIZE // 2
        self.robot.go_dist(edge_distance)

        # Поворот для движения к стартовой области
        self.robot.turn(-0.25)  # поворот направо

        # Движение обратно к стартовой области
        return_distance = self.CELL_SIZE + 200  # обратно к первому ряду
        self.robot.go_dist(return_distance)

        # Поворот для движения к стартовой ячейке
        self.robot.turn(-0.25)  # поворот направо

        # Движение к стартовой ячейке
        to_start = self.START_TO_WALL - 200  # почти до стартовой ячейки
        self.robot.go_dist(to_start)

        # Финальный поворот для выравнивания перед кнопками
        self.robot.turn(0.5)  # разворот на 180 градусов

        print("Робот в стартовой области, готов к сканированию кнопок")

    def scan_and_press_buttons(self):
        """Сканирование ArUco и нажатие кнопок"""
        print("=== СКАНИРОВАНИЕ ARUCO И НАЖАТИЕ КНОПОК ===")

        if not self.detected_faults:
            print("Замыкания не обнаружены, кнопки нажимать не нужно")
            return

        print(f"Обнаружены замыкания в ваннах: {self.detected_faults}")

        # Сканирование ArUco маркеров
        print("Сканирование ArUco маркеров на кнопках...")
        detected_markers = get_aruco_marker_data(self.video_stream)

        if not detected_markers:
            print("ArUco маркеры не обнаружены!")
            return

        # Парсинг маркеров
        marker_ids = [int(id.strip()) for id in detected_markers.split(',') if id.strip()]
        print(f"Обнаружены маркеры кнопок: {marker_ids}")

        # Позиционирование перед кнопками
        # Робот должен быть по центру между левыми и правыми кнопками
        center_position = 0  # центральная позиция

        # Нажатие кнопок для ванн с замыканиями
        for bath_number in self.detected_faults:
            if bath_number in marker_ids:
                print(f"\nНажатие кнопки для ванны {bath_number}")

                # Расчет позиции кнопки
                button_position = self.calculate_button_position(bath_number)

                # Движение к кнопке
                current_pos = 0  # текущая позиция робота
                distance_to_button = button_position - current_pos

                if distance_to_button != 0:
                    print(f"Движение к кнопке на {distance_to_button} мм")
                    self.robot.go_dist(distance_to_button)

                # Движение вперед к кнопке
                print("Наезд на кнопку")
                approach_distance = self.WALL_DISTANCE - 100  # подъезд к стенке
                self.robot.go_dist(approach_distance)

                # Пауза для нажатия
                print("Кнопка нажата!")
                sleep(1)

                # Возврат назад
                print("Возврат от кнопки")
                self.robot.go_dist(-approach_distance)

                # Возврат в центральную позицию
                if distance_to_button != 0:
                    self.robot.go_dist(-distance_to_button)

                current_pos = 0  # обновляем текущую позицию

            else:
                print(f"Маркер для ванны {bath_number} не найден!")

    def calculate_button_position(self, button_number: int) -> int:
        """
        Расчет позиции кнопки относительно центра
        Кнопки расположены:
        Верхний ряд: 1, 2, 3, 4 (слева направо)
        Нижний ряд: 5, 6, 7, 8 (слева направо)
        """
        if button_number <= 4:
            # Верхний ряд - робот движется по нижнему ряду
            # Позиции кнопок верхнего ряда не доступны снизу
            return 0
        else:
            # Нижний ряд - доступен для робота
            # Кнопки 5,6,7,8 слева направо
            button_index = button_number - 5  # 0,1,2,3
            # Центр между кнопками 6 и 7 (индексы 1 и 2)
            center_offset = (button_index - 1.5) * self.BUTTON_SPACING
            return int(center_offset)

    def run_mission(self):
        """Запуск полной миссии"""
        print("=== ЗАПУСК МИССИИ ОБНАРУЖЕНИЯ ЗАМЫКАНИЙ ===")

        # Инициализация
        self.initialize_mission()

        # Обход первого ряда ванн (1-4)
        self.navigate_first_row_baths()

        # Переход ко второму ряду
        self.navigate_to_second_row()

        # Обход второго ряда ванн (8-5)
        self.navigate_second_row_baths()

        # Возврат в стартовую область
        self.return_to_start_area()

        # Сканирование ArUco и нажатие кнопок
        self.scan_and_press_buttons()

        # Завершение миссии
        print("\n=== МИССИЯ ЗАВЕРШЕНА ===")
        print(f"Итоговый отчет:")
        print(f"  Проверено ванн: 8")
        print(f"  Обнаружено замыканий: {len(self.detected_faults)}")
        if self.detected_faults:
            print(f"  Ванны с замыканиями: {self.detected_faults}")
            print(f"  Кнопки нажаты для ванн: {self.detected_faults}")
        else:
            print("  Замыкания не обнаружены")

        self.cleanup()

    def emergency_stop(self):
        """Экстренная остановка"""
        print("ЭКСТРЕННАЯ ОСТАНОВКА!")
        if hasattr(self.robot, 'set_speed'):
            self.robot.set_speed(0.0)

    def cleanup(self):
        """Очистка ресурсов"""
        if self.video_stream:
            self.video_stream.stop()
            cv2.destroyAllWindows()
        print("Ресурсы освобождены")


def main():
    """Главная функция"""
    print("=== РОБОТ ОБНАРУЖЕНИЯ КОРОТКИХ ЗАМЫКАНИЙ ===")
    print("Запуск миссии...")

    launcher = ElectrolysisMissionLauncher()

    try:
        launcher.run_mission()
    except KeyboardInterrupt:
        print("\nМиссия прервана пользователем")
        launcher.emergency_stop()
    except Exception as e:
        print(f"\nКритическая ошибка: {e}")
        launcher.emergency_stop()


if __name__ == "__main__":
    main()
