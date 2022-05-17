def uint_to_float(num, args):
    (bits, power) = args
    if num > pow(2, bits):
        num -= pow(2, bits)
    return num / pow(10, power)

def uint_to_int(num, args):
    bits = args
    uint_to_float(num, (bits, 0))

def uint_to_uint(num, _):
    return num