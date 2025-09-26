from dataclasses import dataclass
from typing import Sequence

from bytelang.abc.serializer import Serializer
from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream
from bytelang.impl.serializer.primitive import PrimitiveSerializer


@dataclass(frozen=True)
class VectorSerializer[T](Serializer[Sequence[T]]):
    """Сериализатор динамического массива"""

    item: Serializer[T]
    """Сериализатор элемента"""
    length: PrimitiveSerializer[int]
    """Примитив описывающий длину"""

    def read(self, stream: InputStream) -> Sequence[T]:
        length = self.length.read(stream)
        items = list()

        # Чтение элементов
        for i in range(length):
            item = self.item.read(stream)
            items.append(item)

        return items

    def write(self, stream: OutputStream, value: Sequence[T]) -> None:
        self.length.write(stream, len(value))

        for i, item in enumerate(value):
            self.item.write(stream, item)

    def __repr__(self) -> str:
        return f"[{self.length}]{self.item}"
