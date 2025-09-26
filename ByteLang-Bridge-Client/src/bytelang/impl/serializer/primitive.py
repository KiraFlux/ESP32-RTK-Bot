import struct
from itertools import chain
from typing import Final
from typing import Iterable

from bytelang.abc.serializer import Serializer
from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream


class _Format:
    I8: Final[str] = "b"
    I16: Final[str] = "h"
    I32: Final[str] = "l"
    I64: Final[str] = "q"

    U8: Final[str] = "B"
    U16: Final[str] = "H"
    U32: Final[str] = "L"
    U64: Final[str] = "Q"

    F32: Final[str] = "f"
    F64: Final[str] = "d"

    @classmethod
    def getAllSigned(cls) -> Iterable[str]:
        """Все знаковые форматы"""
        return cls.I8, cls.I16, cls.I32, cls.I64

    @classmethod
    def getAllUnsigned(cls) -> Iterable[str]:
        """Все форматы без знака"""
        return cls.U8, cls.U16, cls.U32, cls.U64

    @classmethod
    def getAllExponential(cls) -> Iterable[str]:
        """Все экспоненциальные форматы"""
        return cls.F32, cls.F64

    @classmethod
    def getAll(cls) -> Iterable[str]:
        """Все типы"""
        return chain(cls.getAllExponential(), cls.getAllSigned(), cls.getAllUnsigned())

    @classmethod
    def matchPrefix(cls, fmt: str) -> str:
        """Подобрать префикс"""
        match fmt:
            case _ if fmt in cls.getAllExponential():
                return "f"

            case _ if fmt in cls.getAllSigned():
                return "i"

            case _ if fmt in cls.getAllUnsigned():
                return "u"

        raise ValueError(fmt)


class PrimitiveSerializer[T](Serializer[T]):
    """Сериализатор примитивных типов с фиксированным размером"""

    def __init__(self, _format: str):
        self._struct = struct.Struct(f"<{_format}")

    def __repr__(self) -> str:
        return f"{_Format.matchPrefix(self._struct.format.strip("<>"))}{self.getSize() * 8}"

    def read(self, stream: InputStream) -> T:
        data = stream.read(self.getSize())
        return self.unpack(data)

    def write(self, stream: OutputStream, value: T) -> None:
        data = self.pack(value)
        stream.write(data)

    def unpack(self, data: bytes) -> T:
        """Распаковать данные"""
        return self._struct.unpack(data)[0]

    def pack(self, value: T) -> bytes:
        """Упаковать данные"""
        return self._struct.pack(value)

    def getSize(self) -> int:
        """Узнать размер примитива"""
        return self._struct.size


u8 = PrimitiveSerializer[int | bool](_Format.U8)
u16 = PrimitiveSerializer[int](_Format.U16)
u32 = PrimitiveSerializer[int](_Format.U32)
u64 = PrimitiveSerializer[int](_Format.U64)

i8 = PrimitiveSerializer[int](_Format.I8)
i16 = PrimitiveSerializer[int](_Format.I16)
i32 = PrimitiveSerializer[int](_Format.I32)
i64 = PrimitiveSerializer[int](_Format.I64)

f32 = PrimitiveSerializer[float](_Format.F32)
f64 = PrimitiveSerializer[float](_Format.F64)
