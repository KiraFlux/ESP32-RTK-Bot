from dataclasses import dataclass
from typing import Sequence
from typing import TypeVar

from bytelang.abc.serializer import Serializable
from bytelang.abc.serializer import Serializer
from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream
from bytelang.impl.serializer.primitive import PrimitiveSerializer


_T = TypeVar("_T", bound=Serializable)


@dataclass(frozen=True)
class VectorSerializer(Serializer[Sequence[_T]]):
    """Сериализатор динамического массива"""

    item: Serializer[_T]
    """Сериализатор элемента"""
    length: PrimitiveSerializer[int]
    """Примитив описывающий длину"""

    def read(self, stream: InputStream) -> Sequence[_T]:
        length = self.length.read(stream)
        items = list()

        # Чтение элементов
        for i in range(length):
            item = self.item.read(stream)
            items.append(item)

        return items

    def write(self, stream: OutputStream, value: Sequence[_T]) -> None:
        self.length.write(stream, len(value))

        for i, item in enumerate(value):
            self.item.write(stream, item)

    def __repr__(self) -> str:
        return f"[{self.length}]{self.item}"
