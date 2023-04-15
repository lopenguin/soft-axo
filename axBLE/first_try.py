import asyncio, logging, csv
from ble_serial.bluetooth.ble_interface import BLE_interface

# BLE_PORT = '35D683ED-8E27-43EF-8AAB-919996092ACB'
BLE_PORT = r'FF:2C:2A:10:3B:77'

class SerialMonitor:

    def __init__(self, ADAPTER, DEVICE, FILENAME):
        self.ADAPTER = ADAPTER
        self.DEVICE = DEVICE
        self.FILENAME = FILENAME

    
    def receive_callback(self, value: bytes):
        msg = value.decode()
        if self.output: print(value.decode())
        self.file.write(msg + '\n')


    async def main(self):
        # None uses default/autodetection, insert values if needed
        SERVICE_UUID = None
        WRITE_UUID = None
        READ_UUID = None

        self.ble = BLE_interface(self.ADAPTER, SERVICE_UUID)
        self.ble.set_receiver(self.receive_callback)

        try:
            await self.ble.connect(self.DEVICE, "public", 10.0)
            await self.ble.setup_chars(WRITE_UUID, READ_UUID, "rw")

            await asyncio.gather(self.ble.send_loop())
        finally:
            self.file.close()
            await self.ble.disconnect()


    def spin(self, output=True):
        self.output = output
        self.file = open(self.FILENAME, 'w')
        logging.basicConfig(level=logging.INFO)
        asyncio.run(self.main())

    
    def shutdown(self):
        self.file.close()
        self.ble.disconnect()



sm = SerialMonitor(ADAPTER=None, DEVICE=BLE_PORT, FILENAME='save.csv')
sm.spin()