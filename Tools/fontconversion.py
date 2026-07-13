import sys
from PIL import Image, ImageDraw, ImageFont
CHAR_W = 8
CHAR_H = 16
SUPERSAMPLE = 8

def main():
    if len(sys.argv) < 4: sys.exit(1)
    font_path = sys.argv[1]
    array_name = sys.argv[2]
    out_path = sys.argv[3]
    size = int(sys.argv[4]) if len(sys.argv) > 4 else 14
    y_offset = int(sys.argv[5]) if len(sys.argv) > 5 else -1
    big_font = ImageFont.truetype(font_path, size * SUPERSAMPLE)
    ascent, descent = big_font.getmetrics()
    baseline = ascent
    packed_bytes = []
    for code in range(256):
        ch = chr(code) if 32 <= code <= 126 else ' '
        big_img = Image.new("L", (CHAR_W * SUPERSAMPLE, CHAR_H * SUPERSAMPLE), 0)
        draw = ImageDraw.Draw(big_img)
        glyph_size = CHAR_W * CHAR_H // 2
        baseline = CHAR_H * SUPERSAMPLE - descent
        draw.text((CHAR_W * SUPERSAMPLE // 2, baseline + y_offset * SUPERSAMPLE), ch, font = big_font, fill = 255, anchor = "ms")
        small_img = big_img.resize((CHAR_W, CHAR_H), Image.LANCZOS)
        px = small_img.load()
        coverage = [max(0, px[x, y]) for y in range(CHAR_H) for x in range(CHAR_W)]
        nibbles = [c >> 4 for c in coverage]
        for i in range(0, len(nibbles), 2): packed_bytes.append((nibbles[i] << 4) | nibbles[i + 1])
    with open(out_path, "w") as f:
        f.write("#ifndef %s_H\n" % (array_name.upper()))
        f.write("#define %s_H\n" % (array_name.upper()))
        f.write("#include <stdint.h>\n\n")
        f.write("static const uint8_t %s[%d] = {" % (array_name, 256 * glyph_size))
        for i, b in enumerate(packed_bytes):
            if i % 16 == 0: f.write("   ")
            f.write(str(b))
            if i != len(packed_bytes) - 1: f.write(", ")
            if i % 16 == 15: f.write("\n")
            if len(packed_bytes) % 16 != 0: f.write("\n")
        f.write("};\n\n")
        f.write("#endif")

if __name__ == "__main__": main()
