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
    robot = Robot()
    robot.start_poll_task()
    sleep(5)

    # Начало алгоритма

    robot.send_millis_request(None)

    sleep(1)

    for i in range(5):
        robot.go_dist(50)
        robot.go_dist(-50)

    robot.poll_task.join()


_launch()
