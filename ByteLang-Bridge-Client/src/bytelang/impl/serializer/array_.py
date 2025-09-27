from __future__ import annotations

from dataclasses import dataclass
from typing import Sequence
from typing import TypeVar

from bytelang.abc.serializer import Serializable
from bytelang.abc.serializer import Serializer
from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream

_T = TypeVar("_T", bound=Serializable)


@dataclass(frozen=True)
class ArraySerializer(Serializer[Sequence[_T]]):
    """Сериализатор массивов фиксированной длины"""

    item: Serializer[_T]
    """Сериализатор элемента массива"""
    length: int
    """Длинна массива"""

    def __post_init__(self):
        assert self.length >= 1

    def __repr__(self) -> str:
        return f"[{self.length}]{self.item}"

    def read(self, stream: InputStream) -> list[_T]:
        items = list()

        for i in range(self.length):
            item_result = self.item.read(stream)
            items.append(item_result)

        return items

    def write(self, stream: OutputStream, value: list) -> None:
        if len(value) != self.length:
            raise Exception(f"Array length mismatch: expected {self.length}, got {len(value)}")

        for item in value:
            self.item.write(stream, item)
