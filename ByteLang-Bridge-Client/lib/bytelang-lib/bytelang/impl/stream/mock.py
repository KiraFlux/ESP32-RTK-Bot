from bytelang.abc.stream import Stream


class MockStream(Stream):

    def write(self, data: bytes) -> None:
        return

    def read(self, size: int) -> bytes:
        return bytes(size)
