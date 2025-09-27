from dataclasses import dataclass
from typing import Generic
from typing import Optional
from typing import TypeVar

from bytelang.abc.serializer import Serializable
from bytelang.abc.serializer import Serializer
from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream

_T = TypeVar("_T", bound=Serializable)


@dataclass(frozen=True)
class Instruction(Generic[_T]):
    """Исполняемая инструкция протокола"""

    code: bytes
    signature: Serializer[_T]
    name: Optional[str]

    def send(self, stream: OutputStream, value: _T) -> None:
        """Отправить инструкцию с аргументами в поток"""
        stream.write(self.code)
        self.signature.write(stream, value)

    def receive(self, stream: InputStream) -> _T:
        """Принять и десериализовать результат инструкции"""
        return self.signature.read(stream)

    def __repr__(self) -> str:
        name = self.name or "anonymous"
        return f"{name}@{self.code.hex()}( {self.signature} )"
