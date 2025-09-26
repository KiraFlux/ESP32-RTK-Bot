from threading import Thread
from time import sleep
from typing import Final

from serial.serialutil import SerialException

from bytelang.core.protocol import Protocol
from bytelang.impl.serializer.bytevector import ByteVectorSerializer
from bytelang.impl.serializer.primitive import f32
from bytelang.impl.serializer.primitive import u16
from bytelang.impl.serializer.primitive import u32
from bytelang.impl.serializer.primitive import u8
from bytelang.impl.serializer.struct_ import StructSerializer
from bytelang.impl.serializer.void import VoidSerializer
from bytelang.impl.stream.serials import SerialStream


class Robot(Protocol):

    def __init__(self) -> None:
        self._serial: Final = SerialStream(self._get_serial_port(), 115200)

        super().__init__(self._serial, self._serial, u8, u8)

        self.send_millis_request = self.addSender(VoidSerializer(), "send_millis_request")
        self.execute_task = self.addSender(StructSerializer((u8, f32)), "execute_task")
        self.set_speed = self.addSender(f32, "set_speed")

        self.addReceiver(u32, self._on_millis)
        self.addReceiver(ByteVectorSerializer(u16), self._on_log)
        self.addReceiver(u32, self._on_task_completed, "on_task_completed")

        self._task_completed: bool = True
        self._task_result: int = 0

        print("Senders: \n" + "\n".join(map(str, self.getSenders())))
        print("Receivers: \n" + "\n".join(map(str, self.getReceivers())))

        Thread(target=self._poll, daemon=True).start()

    def _poll(self) -> None:
        while True:
            try:
                ports = self._get_serial_port()

                print(f"Подключение к {ports}")
                self._serial.reconnect(ports)

                while True:
                    sleep(0.001)
                    self.poll()

            except SerialException as e:
                print(f"Ошибка соединения: {e}. Переподключение...")
                sleep(1)

            except KeyboardInterrupt:
                print("Завершение работы")
                break

    @staticmethod
    def _get_serial_port() -> str:
        while True:
            ports = SerialStream.getPorts()

            if ports:
                print("Порты не обнаружены. Ожидание...")
                return ports[0]

            sleep(2)

    @staticmethod
    def _on_millis(ms: int) -> None:
        print(f"ms: {ms}")

    @staticmethod
    def _on_log(data: bytes) -> None:
        log = data.decode(errors="replace").strip('\n')
        print(f"ESP: {log}")

    def _on_task_completed(self, result: int) -> None:
        self._task_completed = True
        self._task_result = result
        print(f"task completed: {result=}")

    def _send_task_and_wait(self, task_code: int, arg: float) -> None:
        self._task_completed = False
        self.execute_task((task_code, arg))

        print(f"Ожидание задачи : {task_code=} {arg=}")

        while not self._task_completed:
            sleep(0.01)

        print(f"Задача : {task_code=} {arg=} выполнена {self._task_result=}")

    def go_dist(self, mm: float) -> None:
        """Прямолинейное перемещение"""
        self._send_task_and_wait(0x00, mm)

    def turn(self, turns: float) -> None:
        """Разворот на месте (>0 - по часовой)"""
        self._send_task_and_wait(0x01, turns)


def _launch():
    robot = Robot()

    robot.go_dist(1000)
    robot.go_dist(-1000)
    robot.turn(1.0)
    robot.turn(-1.0)


_launch()
