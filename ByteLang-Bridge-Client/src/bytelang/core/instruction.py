from dataclasses import dataclass
from typing import Optional

from bytelang.abc.serializer import Serializable
from bytelang.abc.serializer import Serializer
from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream


@dataclass(frozen=True)
class Instruction[T: Serializable]:
    """Исполняемая инструкция протокола"""

    code: bytes
    signature: Serializer[T]
    name: Optional[str]

    def send(self, stream: OutputStream, value: T) -> None:
        """Отправить инструкцию с аргументами в поток"""
        stream.write(self.code)
        self.signature.write(stream, value)

    def receive(self, stream: InputStream) -> T:
        """Принять и десериализовать результат инструкции"""
        return self.signature.read(stream)

    def __repr__(self) -> str:
        name = self.name or "anonymous"
        return f"{name}@{self.code.hex()}( {self.signature} )"
