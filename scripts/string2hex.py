import sys

# Parse arguments
# argv[1]: start address (hex)
# argv[2]: ASCII string
# argv[3]: fixed length (bytes)
addr = int(sys.argv[1], 16)
text = sys.argv[2]
length = int(sys.argv[3])

# Encode string as ASCII, add NULL terminator, and pad with NULLs
data = text.encode("ascii")[:length - 1] + b'\0'
data = data.ljust(length, b'\0')


def ihex_record(addr16, rec_type, data):
    """
    Generate a single Intel HEX record line.

    addr16   : 16-bit address field
    rec_type : record type (00=data, 04=extended linear address, 01=EOF)
    data     : list or bytes of payload data
    """
    ln = len(data)
    s = [ln, (addr16 >> 8) & 0xFF, addr16 & 0xFF, rec_type] + list(data)
    chk = ((~sum(s) + 1) & 0xFF)
    return ":" + "".join(f"{x:02X}" for x in s) + f"{chk:02X}"


# Split 32-bit address into upper and lower 16 bits
upper = (addr >> 16) & 0xFFFF
lower = addr & 0xFFFF

# Emit Extended Linear Address record (type 04)
# This sets the upper 16 bits of the address
ela_data = [(upper >> 8) & 0xFF, upper & 0xFF]
print(ihex_record(0x0000, 0x04, ela_data))

# Emit Data record (type 00) with the lower 16-bit address
print(ihex_record(lower, 0x00, data))

# Emit End Of File record
print(":00000001FF")

