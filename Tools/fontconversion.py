import sys
from PIL import Image, ImageDraw, ImageFont
CHAR_W, CHAR_H = 8, 16

def main():
    if len(sys.argv) < 4: sys.exit(1)
    font_path = sys.argv[1]
    array_name = sys.argv[2]
    out_path = sys.argv[3]
    size = int(sys.argv[4]) if len(sys.argv) > 4 else 14
    y_offset = int(sys.argv[5]) if len(sys.argv) > 5 else -1
    font = ImageFont.truetype(font_path, size)
    rows = []
    for code in range(256):
        ch = chr(code) if 32 <= code <= 126 else ' '
        img = Image.new("L", (CHAR_W, CHAR_H), 0)
        draw = ImageDraw.Draw(img)
        draw.text((0, y_offset), ch, font = font, fill = 255)
        px = img.load()
        for y in range(CHAR_H):
            byte = 0
            for x in range(CHAR_W):
                if px[x, y] > 100:
                    byte |= (1 << (7 - x))
            rows.append(byte)
    with open(out_path, "w") as f:
        f.write("#ifndef %s_H\n" % (array_name.upper()))
        f.write("#define %s_H\n" % (array_name.upper()))
        f.write("#include <stdint.h>\n\n")
        f.write("static const uint8_t %s[256 * 16] = {" % array_name)
        f.write(", ".join(str(b) for b in rows))
        f.write("};\n\n")
        f.write("#endif")

if __name__ == "__main__": main()
