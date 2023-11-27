byte64 = (2**64)-1
def hash_djb2(s):
    hash = 5381
    for x in s:
        hash = (((hash << 5) + hash) + ord(x)) & byte64
    return hash

def hash_336(s):
    for i in range(2000):
        s = hash_djb2(s)
        s = f"{s:016x}"
    return s

def bytes_sanity_check(bytess):
    return not ((b'{' in bytess) and (b'}' in bytess))