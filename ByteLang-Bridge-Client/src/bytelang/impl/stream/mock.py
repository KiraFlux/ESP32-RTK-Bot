from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream


class MockStream(InputStream, OutputStream):

    def write(self, data: bytes) -> None:
        return

    def read(self, size: int) -> bytes:
        return bytes(size)
