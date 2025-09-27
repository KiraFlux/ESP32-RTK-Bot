from abc import ABC
from abc import abstractmethod
from typing import Generic
from typing import Sequence, TypeVar
from typing import Type

from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream

_serializable = Type[int | float | str]
_serializable = Type[Sequence[_serializable] | _serializable]
_serializable = Type[Sequence[_serializable] | _serializable]

Serializable = _serializable

_T = TypeVar("_T", bound=Serializable)


class Serializer(ABC, Generic[_T]):
    """Serializer - упаковка, распаковка данных"""

    @abstractmethod
    def read(self, stream: InputStream) -> _T:
        """Считать значение из потока"""

    @abstractmethod
    def write(self, stream: OutputStream, value: _T) -> None:
        """Записать значение в поток"""
