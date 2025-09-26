from bytelang.abc.serializer import Serializer
from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream


class VoidSerializer(Serializer[None]):
    def read(self, stream: InputStream) -> None:
        return

    def write(self, stream: OutputStream, value: None) -> None:
        return

    def __repr__(self) -> str:
        return "void"
