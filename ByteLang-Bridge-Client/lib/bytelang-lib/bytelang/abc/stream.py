from abc import ABC
from abc import abstractmethod


class InputStream(ABC):
    """Абстрактный поток ввода (чтения)"""

    @abstractmethod
    def read(self, size: int) -> bytes:
        """Считать данные из потока ввода"""


class OutputStream(ABC):
    """Абстрактный поток вывода (записи)"""

    @abstractmethod
    def write(self, data: bytes) -> None:
        """Записать данные в поток вывода"""
