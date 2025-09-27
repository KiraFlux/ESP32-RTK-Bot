from threading import Thread
from time import sleep
from typing import Final

from serial import SerialException

from bytelang.core.protocol import Protocol
from bytelang.impl.serializer.bytevector import ByteVectorSerializer
from bytelang.impl.serializer.primitive import u8, f32, u32, u16
from bytelang.impl.serializer.struct_ import StructSerializer
from bytelang.impl.serializer.void import VoidSerializer
from bytelang.impl.stream.serials import SerialStream


class Robot(Protocol):

    def __init__(self) -> None:
        self._serial: Final = SerialStream(self._get_serial_port(), 115200)

        super().__init__(self._serial, self._serial, u8, u8)

        self.send_millis_request = self.add_sender(VoidSerializer(), "send_millis_request")
        self.execute_task = self.add_sender(StructSerializer((u8, f32)), "execute_task")
        self.set_speed = self.add_sender(f32, "set_speed")

        self.add_receiver(u32, self._on_millis)
        self.add_receiver(ByteVectorSerializer(u16), self._on_log)
        self.add_receiver(u32, self._on_task_completed, "on_task_completed")

        self._task_completed: bool = True
        self._task_result: int = 0

        self.log("Senders: \n" + "\n".join(map(str, self.get_senders())))
        self.log("Receivers: \n" + "\n".join(map(str, self.get_receivers())))

        self.poll_task = Thread(target=self._poll, daemon=True)

    def go_dist(self, mm: float) -> None:
        """
        Прямолинейное перемещение
        :param:
        """
        self._send_task_and_wait(0x00, mm)

    def check_magnetometer(self) -> int:
        """
        Сделать проверку ряда магнитометром
        :returns: кол-во найденных магнитов в ряду
        """
        self._send_task_and_wait(0x02, 0.0)
        return self._task_result

    def turn(self, turns: float) -> None:
        """
        Разворот на месте
        :param: turns кол-во оборотов. Положительный угол - поворот по часовой
        """
        self._send_task_and_wait(0x01, turns)

    @staticmethod
    def log(message: str) -> None:
        """Записать лог"""
        print(f"Robot: {message}")

    def start_poll_task(self):
        """Запустить задачу опроса порта"""
        self.poll_task.start()

    def _poll(self) -> None:
        while True:
            try:
                while True:
                    self.poll()
                    sleep(0.001)

            except SerialException as e:
                ports = self._get_serial_port()
                self.log(f"Ошибка соединения: {e}. Подключение к {ports}")
                self._serial.reconnect(ports)

            except KeyboardInterrupt:
                self.log("Завершение работы")
                break

    def _get_serial_port(self) -> str:
        while True:
            ports = tuple(
                p
                for p in SerialStream.getPorts()
                if "USB" in p
            )

            if ports:
                self.log(f"Обнаружен порты {ports}")
                return ports[0]

            sleep(2)
            self.log("Не найдены порты")

    def _on_millis(self, ms: int) -> None:
        self.log(f"ms: {ms}")

    def _on_log(self, data: bytes) -> None:
        log = data.decode(errors="replace").strip('\n')
        self.log(f"ESP: {log}")

    def _on_task_completed(self, result: int) -> None:
        self._task_completed = True
        self._task_result = result
        self.log(f"task completed: {result=}")

    def _send_task_and_wait(self, task_code: int, arg: float) -> None:
        self.log(f"Выполнение задачи: {task_code=} {arg=}")

        self._task_completed = False
        self.execute_task((task_code, arg))

        while not self._task_completed:
            sleep(0.01)

        self.log(f"Задача : {task_code=} {arg=} выполнена {self._task_result=}")
