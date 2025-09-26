from abc import ABC
from abc import abstractmethod
from typing import Sequence

from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream

type _serializable = int | float | str
type _serializable = Sequence[_serializable] | _serializable
type _serializable = Sequence[_serializable] | _serializable

Serializable = _serializable


class Serializer[T: Serializable](ABC):
    """Serializer - упаковка, распаковка данных"""

    @abstractmethod
    def read(self, stream: InputStream) -> T:
        """Считать значение из потока"""

    @abstractmethod
    def write(self, stream: OutputStream, value: T) -> None:
        """Записать значение в поток"""
