import asyncio, logging, csv, aioconsole, sys
from ble_serial.bluetooth.ble_interface import BLE_interface
from colorama import Fore

# BLE_PORT = '35D683ED-8E27-43EF-8AAB-919996092ACB'
BLE_PORT = r'FF:2C:2A:10:3B:77'

class SerialMonitor:

    def __init__(self, ADAPTER, DEVICE, FILENAME):
        self.ADAPTER = ADAPTER
        self.DEVICE = DEVICE
        self.FILENAME = FILENAME

    
    def receive_callback(self, value: bytes):
        msg = value.decode()
        if self.output: print('Recieved value:\n' + value.decode())
        self.file.write(msg + '\n')

    async def send(self):
        while True:
            msg = await aioconsole.ainput('> ')
            tosend = bytearray(msg + '\n', 'UTF-8')
            for i in range(0, len(tosend) // 10 + 1):
                self.ble.queue_send(tosend[10 * i:min(len(tosend), 10 * i + 10)])


    async def main(self):
        # None uses default/autodetection, insert values if needed
        SERVICE_UUID = None
        WRITE_UUID = None
        READ_UUID = None

        self.ble = BLE_interface(self.ADAPTER, SERVICE_UUID)
        self.ble.set_receiver(self.receive_callback)

        try:
            print(Fore.CYAN + 'Connecting to device...')
            await self.ble.connect(self.DEVICE, "public", 10.0)
            await self.ble.setup_chars(WRITE_UUID, READ_UUID, "rw")
            print(Fore.GREEN + f'Connected to {self.ble.dev.address}!')

            print(Fore.WHITE + '+' * 10 + 'BEGIN SERIAL MONITOR' + '+' * 10)
            await asyncio.gather(self.ble.send_loop(), self.send())
        finally:
            
            self.file.close()
            await self.ble.disconnect()
            print(Fore.RED + 'Process finished!')


    def spin(self, output=True):
        try:
            self.output = output
            self.file = open(self.FILENAME, 'w')
            logging.basicConfig(level=logging.WARNING)
            asyncio.run(self.main())
        except KeyboardInterrupt:
            print('-' * 40)
            print(Fore.RED + 'User interruption.')
            pass

    
    def shutdown(self):
        self.file.close()
        self.ble.disconnect()


sm = SerialMonitor(ADAPTER=None, DEVICE=BLE_PORT, FILENAME='save.txt')
sm.spin(output=False)
