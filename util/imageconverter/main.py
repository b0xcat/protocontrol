from pathlib import Path
from PIL import Image


def RGBto565(r, g, b):
    return bytes.fromhex("%0.4X" % ((int(r / 255 * 31) << 11) | (int(g / 255 * 63) << 5) | (int(b / 255 * 31))))


if __name__ == "__main__":
    # Ensure out directory exists
    out_dir = Path("out")
    out_dir.mkdir(exist_ok=True)

    # Loop over all files in input directory
    for path in [x for x in Path("in").glob("*") if x.is_file()]:
        print(f"Converting {path}")

        try:
            image = Image.open(path)
        except Exception:
            print(f'Could not open {path}, skipping')
            continue

        out_bytes = bytes()

        # TYPE -> 565 column major
        out_bytes += b'\x00'

        # WIDTH
        print(f"Width: {image.width}")
        out_bytes += image.width.to_bytes(2, "little")

        # HEIGHT
        print(f"Height: {image.height}")
        out_bytes += image.height.to_bytes(2, "little")

        # IMAGE DATA
        for y in range(image.height):
            for x in range(image.width):
                r, g, b, *_ = image.getpixel((x, y))
                out_bytes += RGBto565(r, g, b)

        # write to out file
        out_path = out_dir / Path(f"{path.stem}")
        with out_path.open('wb') as file:
            file.write(out_bytes)
