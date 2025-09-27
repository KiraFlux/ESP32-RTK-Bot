from time import sleep
from typing import Mapping

from robot import Robot


def _walk_bath(robot: Robot) -> Mapping[int, int]:
    ret = dict()
    bars_in_bath = 10

    for row in range(bars_in_bath):
        robot.go_dist(100)
        magnets_in_row = robot.check_magnetometer()

        if magnets_in_row > 0:
            ret[row] = magnets_in_row

    return ret


def _launch():
    data_csv = tuple(dict() for _ in range(8))

    robot = Robot()
    robot.start_poll_task()
    sleep(2)

    # Начало алгоритма

    robot.send_millis_request(None)
    robot.go_dist(1000)

    robot.turn(0.5)
    robot.go_dist(1000)

    bath_data = _walk_bath(robot)
    data_csv[0].update(bath_data)

    # стоим над кнопкой

    # Алгоритм завершен.

    robot.poll_task.join()


_launch()
