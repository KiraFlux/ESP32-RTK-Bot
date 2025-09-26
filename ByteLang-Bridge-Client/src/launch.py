from time import sleep

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
        super().__init__(
            input_stream,
            output_stream,
            u8,
            u8
        )

        self.send_millis_request = self.addSender(VoidSerializer(), "send_millis_request")

        self.addReceiver(u32, lambda x: print(f"ms: {x}"), "on_millis")
        self.addReceiver(ByteVectorSerializer(u16), lambda b: print(b.decode()), "on_log")


def _getIOStreams() -> tuple[InputStream, OutputStream]:
    ports = SerialStream.getPorts()

    serial = SerialStream(ports[0], 115200)
    return serial, serial


def _launch():
    i, o = _getIOStreams()
    p = RobotBytelangBridgeProtocol(i, o)

    print("Senders: \n" + "\n".join(map(str, p.getSenders())))
    print("Receivers: \n" + "\n".join(map(str, p.getReceivers())))

    try:
        while True:
            sleep(0.001)

            # p.send_millis_request(None)

            p.poll()

    except KeyboardInterrupt:
        return


_launch()
