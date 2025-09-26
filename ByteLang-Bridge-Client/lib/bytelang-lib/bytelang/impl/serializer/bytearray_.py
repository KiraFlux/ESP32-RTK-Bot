from dataclasses import dataclass

from bytelang.abc.serializer import Serializer
from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream


@dataclass(frozen=True)
class ByteArraySerializer(Serializer[bytes]):
    """Сериализатор байтовых строк"""

    length: int
    """Размер"""

    def __post_init__(self):
        assert self.length >= 1

    def write(self, stream: OutputStream, value: bytes) -> None:
        if len(value) != self.length:
            raise Exception(f"Invalid ByteArray size (expected: {self.length}, got: {len(value)})")

        stream.write(value)

    def read(self, stream: InputStream) -> bytes:
        return stream.read(self.length)

    def __repr__(self) -> str:
        return f"[{self.length}]bytes"
