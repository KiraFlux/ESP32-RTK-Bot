"""
Точка входа
"""

from robot import Robot


def _launch():
    robot = Robot()
    robot.start_poll_task()

    robot.poll_task.join()


if __name__ == '__main__':
    _launch()
