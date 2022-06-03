if __name__ == "__main__":
    clear_flag = "ECW"
    clear_flag = "ECW{EFI_Byt3_c0d3_rul3z}"
    rot = 4
    encoded_flag = []
    for c in clear_flag:
        val = ord(c)
        val -= rot
        char = chr(val)
        encoded_flag.append(char)
        #print(char, end="")

    encoded_flag = "".join(encoded_flag)
    encoded_flag_len = hex(len(encoded_flag))
    rot_str = hex(rot)
    print("encoded_flag:", encoded_flag)
    print("encoded_flag_len:", encoded_flag_len)
    print("rot:", rot_str)

    with open("template.asm", "rt") as fh:
        data = fh.read()

    data = data.replace("__FLAG__", encoded_flag)
    data = data.replace("__FLAG_LENGTH__", encoded_flag_len)
    data = data.replace("__FLAG_ROT__", rot_str)

    with open("main.asm", "w") as fh:
        fh.write(data)