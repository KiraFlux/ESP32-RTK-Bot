from dataclasses import dataclass

from bytelang.abc.serializer import Serializer
from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream
from bytelang.impl.serializer.primitive import PrimitiveSerializer


@dataclass(frozen=True)
class ByteVectorSerializer(Serializer[bytes]):
    """Вектор байт"""

    length: PrimitiveSerializer[int]
    """Примитив длины"""

    def write(self, stream: OutputStream, value: bytes) -> None:
        self.length.write(stream, len(value))
        stream.write(value)

    def read(self, stream: InputStream) -> bytes:
        length = self.length.read(stream)
        return stream.read(length)

    def __repr__(self) -> str:
        return f"[{self.length}]bytes"
