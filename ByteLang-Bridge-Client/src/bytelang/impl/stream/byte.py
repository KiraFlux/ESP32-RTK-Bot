from dataclasses import dataclass
from dataclasses import field

from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream


@dataclass
class ByteBufferInputStream(InputStream):
    """Поток ввода, работающий с буфером байтов в памяти."""

    _data: bytes
    _index: int = field(default=0, init=False)

    def read(self, size: int) -> bytes:
        """
        Считывает указанное количество байтов из буфера.

        Args:
            size: Количество байтов для чтения (должно быть положительным)
        """

        if size < 0:
            raise Exception(f"Invalid read size: {size}. Size must be non-negative")

        if self._index >= len(self._data):
            raise Exception("Read beyond end of buffer")

        # Вычисляем доступные данные
        available = len(self._data) - self._index
        read_size = min(size, available)

        # Читаем данные и обновляем позицию
        result = self._data[self._index:self._index + read_size]
        self._index += read_size

        return result

    def available(self) -> int:
        """Возвращает количество доступных для чтения байтов"""
        return len(self._data) - self._index

    def reset(self) -> None:
        """Сбрасывает позицию чтения в начало буфера"""
        self._index = 0


@dataclass(frozen=True)
class ByteBufferOutputStream(OutputStream):
    """
    Поток вывода, записывающий данные в буфер в памяти.

    Реализован как неизменяемый dataclass с прямым доступом к буферу.
    Полезен для тестирования и сбора выходных данных.
    """

    buffer: bytearray = field(default_factory=bytearray, init=False)

    def write(self, data: bytes) -> None:
        """
        Записывает данные в буфер.

        Args:
            data: Байтовые данные для записи
        """

        self.buffer.extend(data)
