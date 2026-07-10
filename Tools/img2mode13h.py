import sys
from PIL import Image
WIDTH = 320
HEIGHT = 200

def main():
    if len(sys.argv) < 3: sys.exit(1)
    src_path = sys.argv[1]
    out_path = sys.argv[2]
    src = Image.open(src_path)
    if src.mode in ("RGBA", "LA") or (src.mode == "P" and "transparency" in src.info):
        src = src.convert("RGBA")
        background = Image.new("RGB", src.size, (0, 0, 0))
        background.paste(src, mask=src.split()[3])
        src = background
    else: src = src.convert("RGB")
    scale = min(WIDTH / src.width, HEIGHT / src.height)
    fitted_w = max(1, round(src.width * scale))
    fitted_h = max(1, round(src.height * scale))
    fitted = src.resize((fitted_w, fitted_h), Image.LANCZOS)
    canvas = Image.new("RGB", (WIDTH, HEIGHT), (0, 0, 0))
    paste_x = (WIDTH - fitted_w) // 2
    paste_y = (HEIGHT - fitted_h) // 2
    canvas.paste(fitted, (paste_x, paste_y))
    quantized = canvas.quantize(colors = 256, method = Image.MEDIANCUT)
    raw_palette = quantized.getpalette()
    pixel_indices = list(quantized.getdata())
    with open(out_path, "w") as f:
        f.write("#ifndef LOGO_MODE13_H\n")
        f.write("#define LOGO_MODE13_H\n")
        f.write("#include <stdint.h>\n")
        f.write("#define LOGO13_WIDTH %d\n" % WIDTH)
        f.write("#define LOGO13_HEIGHT %d\n" % HEIGHT)
        f.write("static const uint8_t logo13_palette[256][3] = {")
        for i in range(256):
            r = raw_palette[i * 3 + 0] >> 2
            g = raw_palette[i * 3 + 1] >> 2
            b = raw_palette[i * 3 + 2] >> 2
            f.write("   {%d, %d, %d}," % (r, g, b))
        f.write("};\n")
        f.write("static const uint8_t logo13_pixels[LOGO13_WIDTH * LOGO13_HEIGHT] = {")
        f.write(", ".join(str(v) for v in pixel_indices))
        f.write("};\n")
        f.write("#endif")
if __name__ == "__main__": main()