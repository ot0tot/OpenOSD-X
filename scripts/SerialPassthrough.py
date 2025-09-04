import serial, time, sys, re, logging
import serials_find
import SerialHelper

# Configure logging
logging.basicConfig(
    level=logging.DEBUG,  # Change to INFO for less verbosity
    format="%(asctime)s [%(levelname)s] %(message)s",
    datefmt="%H:%M:%S"
)
logger = logging.getLogger(__name__)

class PassthroughEnabled(Exception):
    pass

class PassthroughFailed(Exception):
    pass


def msp_crc8_d5(data: bytes) -> int:
    """Calculate CRC8 with polynomial 0xD5."""
    crc = 0
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x80:
                crc = ((crc << 1) ^ 0xD5) & 0xFF
            else:
                crc = (crc << 1) & 0xFF
    return crc


def bootloader_start(port):
    """Send MSP_REBOOT command and close the port."""
    logger.info("MSP:REBOOT")
    logger.info("  Trying to reboot %s", port)

    s = serial.Serial(
        port=port, baudrate=115200,
        bytesize=8, parity='N', stopbits=1,
        timeout=1, xonxoff=0, rtscts=0
    )

    rl = SerialHelper.SerialHelper(s, 3., ['CCC', "# "])
    rl.clear()

    # Build MSP_REBOOT packet
    data = [0x00] * 9
    data[0] = ord('$')
    data[1] = ord('X')
    data[2] = ord('<')
    data[3] = 0x00
    data[4] = 68      # MSP_REBOOT
    data[5] = 0x00
    data[6] = 0x00
    data[7] = 0x00
    data[8] = msp_crc8_d5(data[3:8])

    rl.write(bytes(data))
    time.sleep(.5)
    s.close()
    time.sleep(.5)

    logger.info("MSP:REBOOT DONE")


def serialpassthrough_start(port):
    """Send MSP_SET_PASSTHROUGH command and close the port."""
    logger.info("MSP:MSP_SET_PASSTHROUGH")

    s = serial.Serial(
        port=port, baudrate=115200,
        bytesize=8, parity='N', stopbits=1,
        timeout=1, xonxoff=0, rtscts=0
    )

    rl = SerialHelper.SerialHelper(s, 3.)
    rl.clear()

    # Build MSP_SET_PASSTHROUGH packet
    data = [0x00] * 12
    data[0] = ord('$')
    data[1] = ord('X')
    data[2] = ord('<')
    data[3] = 0x00
    data[4] = 245       # MSP_SET_PASSTHROUGH
    data[5] = 0x00
    data[6] = 0x02      # LEN
    data[7] = 0x00
    data[8] = 0xFE      # MSP_PASSTHROUGH_SERIAL_FUNCTION_ID
    data[9] = 0x11      # FUNCTION_VTX_MSP
    data[10] = msp_crc8_d5(data[3:10])

    rl.write(bytes(data))
    time.sleep(.5)
    s.close()
    time.sleep(.5)

    logger.info("MSP:MSP_SET_PASSTHROUGH DONE")


if __name__ == '__main__':
    port = serials_find.get_serial_port()

    try:
        serialpassthrough_start(port)
        bootloader_start(port)
    except PassthroughEnabled as err:
        logger.error("PassthroughEnabled: %s", err)

