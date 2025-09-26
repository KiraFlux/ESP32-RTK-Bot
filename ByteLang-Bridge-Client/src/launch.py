from time import sleep

from serial.serialutil import SerialException

from bytelang.abc.stream import InputStream
from bytelang.abc.stream import OutputStream
from bytelang.core.protocol import Protocol
from bytelang.impl.serializer.bytevector import ByteVectorSerializer
from bytelang.impl.serializer.primitive import u16
from bytelang.impl.serializer.primitive import u32
from bytelang.impl.serializer.primitive import u8
from bytelang.impl.serializer.void import VoidSerializer
from bytelang.impl.stream.serials import SerialStream


class RobotBytelangBridgeProtocol(Protocol):

    def __init__(self, input_stream: InputStream, output_stream: OutputStream) -> None:
        super().__init__(input_stream, output_stream, u8, u8)

        self.send_millis_request = self.addSender(VoidSerializer(), "send_millis_request")

        self.addReceiver(u32, self._on_millis)
        self.addReceiver(ByteVectorSerializer(u16), self._on_log)

        print("Senders: \n" + "\n".join(map(str, self.getSenders())))
        print("Receivers: \n" + "\n".join(map(str, self.getReceivers())))

    def _on_millis(self, ms: int) -> None:
        print(f"ms: {ms}")

    def _on_log(self, data: bytes) -> None:
        log = data.decode(errors="replace").strip('\n')
        print(f"ESP: {log}")


def _launch():
    while True:
        try:
            ports = SerialStream.getPorts()
            if not ports:
                print("Порты не обнаружены. Ожидание...")
                sleep(2)
                continue

            print(f"Подключение к {ports[0]}")
            serial = SerialStream(ports[0], 115200)
            protocol = RobotBytelangBridgeProtocol(serial, serial)

            while True:
                sleep(0.001)
                protocol.poll()

        except SerialException as e:
            print(f"Ошибка соединения: {e}. Переподключение...")
            sleep(1)

        except KeyboardInterrupt:
            print("Завершение работы")
            break


_launch()
