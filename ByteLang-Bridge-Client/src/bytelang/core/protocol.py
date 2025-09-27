from typing import Callable
from typing import Final
from typing import Iterable

from bytelang.abc.serializer import Serializable
from bytelang.abc.serializer import Serializer
from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream
from bytelang.core.instruction import Instruction
from bytelang.impl.serializer.primitive import PrimitiveSerializer

type OnReceiveFunction[T] = Callable[[T], None]
"""Вид обработчика приёма"""


class Protocol:
    """Протокол P2P общения по потоку с указанной структурой полей"""

    def __init__(
            self,
            input_stream: InputStream,
            output_stream: OutputStream,
            local_code: PrimitiveSerializer[int],
            remote_code: PrimitiveSerializer[int]
    ) -> None:
        self._input_stream: Final = input_stream
        self._output_stream: Final = output_stream

        self._local_instruction_code: Final = local_code
        self._remote_instruction_code: Final = remote_code

        self._receive_handlers: Final = dict[bytes, tuple[Instruction, OnReceiveFunction]]()
        self._send_handlers: Final = dict[bytes, Instruction]()

    def get_senders(self) -> Iterable[Instruction]:
        """Получить все обработчики на отправку"""
        return self._send_handlers.values()

    def get_receivers(self) -> Iterable[Instruction]:
        """Получить все обработчики на приём"""
        return (i for i, j in self._receive_handlers.values())

    def add_receiver[T: Serializable](self, /, result: Serializer[T], handler: OnReceiveFunction[T], name: str = None) -> None:
        """Зарегистрировать обработчик входящих сообщений"""
        index = len(self._receive_handlers)
        code = self._local_instruction_code.pack(index)
        instruction = Instruction(code, result, name)
        self._receive_handlers[code] = (instruction, handler)

    def add_sender[T: Serializable](self, /, signature: Serializer[T], name: str = None) -> Callable[[T], None]:
        """Зарегистрировать исходящую инструкцию"""
        index = len(self._send_handlers)
        code = self._remote_instruction_code.pack(index)
        instruction = Instruction(code, signature, name)
        self._send_handlers[code] = instruction

        def _wrapper(value: T):
            return instruction.send(self._output_stream, value)

        return _wrapper

    def poll(self) -> None:
        """Обработать входящее сообщение"""
        code_size = self._remote_instruction_code.getSize()
        code = self._input_stream.read(code_size)

        if code not in self._receive_handlers:
            print(f"Unknown instruction code: {code.hex()}")
            return

        instruction, handler = self._receive_handlers[code]
        args_result = instruction.receive(self._input_stream)

        handler(args_result)
