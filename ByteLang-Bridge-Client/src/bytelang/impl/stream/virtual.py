from __future__ import annotations

from threading import Condition
from threading import Lock

from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream


class VirtualStream(InputStream, OutputStream):
    """Простой поток-заглушка с блокирующим чтением"""

    def __init__(self,
                 rx_buffer: bytearray,
                 tx_buffer: bytearray,
                 rx_lock: Lock,
                 tx_lock: Lock):
        self.rx = rx_buffer
        self.tx = tx_buffer
        self.rx_lock = rx_lock
        self.tx_lock = tx_lock
        self.rx_condition = Condition(rx_lock)

    def write(self, data: bytes) -> None:
        """Запись данных в буфер передачи"""
        with self.tx_lock:
            self.tx.extend(data)

            with self.rx_condition:
                self.rx_condition.notify_all()

            return

    def read(self, size: int) -> bytes:
        """Блокирующее чтение данных из буфера приема"""
        with self.rx_condition:
            while len(self.rx) < size:
                self.rx_condition.wait()

            data = bytes(self.rx[:size])
            del self.rx[:size]
            return data

    @classmethod
    def create_pair(cls) -> tuple[VirtualStream, VirtualStream]:
        """Создает пару связанных пир-потоков с блокирующим чтением"""
        a_lock = Lock()
        b_lock = Lock()

        a_to_b = bytearray()
        b_to_a = bytearray()

        return (
            cls(
                rx_buffer=b_to_a,
                tx_buffer=a_to_b,
                rx_lock=b_lock,
                tx_lock=a_lock
            ),
            cls(
                rx_buffer=a_to_b,
                tx_buffer=b_to_a,
                rx_lock=a_lock,
                tx_lock=b_lock
            )
        )
