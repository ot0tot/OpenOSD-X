import serial
import sys, glob, logging

# Configure logging
logging.basicConfig(
    level=logging.DEBUG,  # Change to INFO for normal use
    format="%(asctime)s [%(levelname)s] %(message)s",
    datefmt="%H:%M:%S"
)
logger = logging.getLogger(__name__)


def serial_ports():
    """List available serial port names.

    :raises Exception:
        On unsupported or unknown platforms
    :returns:
        A list of the serial ports available on the system
    """
    result = []
    ports = []

    try:
        from serial.tools.list_ports import comports
        if comports:
            logger.info("** Searching flight controllers **")
            __ports = list(comports())
            for port in __ports:
                if (port.manufacturer and port.manufacturer in ['FTDI', 'Betaflight']) or \
                   (port.product and "STM32" in port.product) or \
                   (port.vid and port.vid == 0x0483):
                    logger.info("  > FC found from '%s'", port.device)
                    ports.append(port.device)
    except ImportError:
        pass

    if not ports:
        logger.info("** No FC found, scanning all ports **")

        platform = sys.platform.lower()
        if platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
            ports.reverse()
        elif platform.startswith('linux') or platform.startswith('cygwin'):
            # Only list ttyACM* and ttyUSB* ports
            ports = glob.glob('/dev/ttyACM*')
            ports.extend(glob.glob('/dev/ttyUSB*'))
        elif platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.usbmodem*')
        else:
            raise Exception('Unsupported platform')

    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException) as error:
            if "permission denied" in str(error).lower():
                raise Exception("You don't have permission to use serial port!")
            # Ignore ports that cannot be opened
            pass

    return result


def get_serial_port(debug=True):
    """Return the first available serial port."""
    result = serial_ports()

    if debug:
        logger.info("Detected the following serial ports on this system:")
        for port in result:
            logger.info("  %s", port)

    if len(result) == 0:
        raise Exception('No valid serial port detected or port already open')

    return result[0]


if __name__ == '__main__':
    results = get_serial_port(True)
    logger.info("Found: %s", results)

