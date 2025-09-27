from dataclasses import dataclass
from typing import Sequence
from typing import TypeVar

from bytelang.abc.serializer import Serializable
from bytelang.abc.serializer import Serializer
from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream

_T = TypeVar("_T", bound=Serializable)


@dataclass(frozen=True)
class StructSerializer(Serializer[Sequence[_T]]):
    """Объединение нескольких Serializer"""

    fields: Sequence[Serializer]

    def __repr__(self) -> str:
        return f"{{ {', '.join(map(str, self.fields))} }}"

    def read(self, stream: InputStream) -> Sequence[_T]:
        values = list()

        for field in self.fields:
            value = field.read(stream)
            values.append(value)

        return values

    def write(self, stream: OutputStream, value: Sequence[_T]) -> None:
        if len(value) != len(self.fields):
            raise Exception(f"Value/fields count mismatch: {len(value)} vs {len(self.fields)}")

        for field, item in zip(self.fields, value):
            field.write(stream, item)
