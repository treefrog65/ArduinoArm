
import serial
import unittest
import time

class TestCommands(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self.controller_connection = serial.Serial('/dev/tnt1',115200, dsrdtr=True) # dsrdtr needed to prevent arduino from resetting
        self.board_connection = serial.Serial('/dev/tnt3',115200, timeout=1)

    def test_joint0(self):
        command = b"move 0 0 600\n"
        result = bytes.fromhex("55 55 00 07 01 00 00 58 02 9D")    
        self.assertEqual(len(command), self.controller_connection.write(command))
        board_command = self.board_connection.read(len(result))
        self.assertEqual(result, board_command)

    def test_readVoltage(self):
        command = b"read_voltage 0"
        result = bytes.fromhex("55 55 ")



if __name__ == "__main__":
    unittest.main()
