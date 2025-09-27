from time import sleep

from robot import Robot


def _launch():
    robot = Robot()
    robot.start_poll_task()
    sleep(2)

    # Начало алгоримта

    robot.send_millis_request(None)
    robot.go_dist(1000)

    robot.turn(0.5)
    robot.go_dist(1000)

    # стоим над кнопкой

    # Алгоритм завершен.

    robot.poll_task.join()


_launch()
