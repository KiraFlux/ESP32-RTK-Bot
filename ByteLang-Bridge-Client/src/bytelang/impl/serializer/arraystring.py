from bytelang.abc.serializer import Serializer
from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream
from bytelang.impl.serializer.bytearray_ import ByteArraySerializer


class ArrayStringSerializer(Serializer[str]):
    """Сериализатор строк фиксированной длины с UTF-8 кодировкой"""

    def __init__(self, length: int) -> None:
        self._byte_array_serializer = ByteArraySerializer(length)

    def write(self, stream: OutputStream, value: str) -> None:
        """Записать строку как байты фиксированной длины"""
        encoded = value.encode('utf-8')

        if len(encoded) > self._byte_array_serializer.length:
            raise Exception(f"String too long ({len(encoded)} > {self._byte_array_serializer.length} bytes)")

        padded = encoded + b'\x00' * (self._byte_array_serializer.length - len(encoded))
        return self._byte_array_serializer.write(stream, padded)

    def read(self, stream: InputStream) -> str:
        """Прочитать строку из байтов фиксированной длины"""
        data = self._byte_array_serializer.read(stream)
        return data.rstrip(b'\x00').decode('utf-8')

    def __repr__(self) -> str:
        return f"[{self._byte_array_serializer.length}]str"
