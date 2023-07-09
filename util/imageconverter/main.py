Import("env")
env.Execute("$PYTHONEXE -m pip --version")

from pathlib import Path
from inspect import getsourcefile
from os.path import abspath

try:
    from PIL import Image
except ImportError:
    env.Execute("$PYTHONEXE -m pip install Pillow")
    from PIL import Image

here = Path(abspath(getsourcefile(lambda:0))).parent

def RGBto565(r, g, b):
    # return bytes.fromhex("%0.4X" % ((int(b / 255 * 31) << 11) | (int(r / 255 * 63) << 5) | (int(g / 255 * 31))))
    return bytes.fromhex(f"{(((r & 0b11111000)<<8) + ((g & 0b11111100)<<3)+(b>>3)):04x}")


# if __name__ == "__main__":
def main(source, target, env):
    print("Converting input pngs to 565 format")

    # Ensure out directory exists
    out_dir = (here / '../../data/565').resolve()
    out_dir.mkdir(exists_ok=True, parents=True)
    print("out dir:", out_dir)
    assert out_dir.is_dir()

    # Ensure in directory exists
    in_dir = (here / Path("in")).resolve()
    print("in dir:", in_dir)
    assert in_dir.is_dir()

    # Loop over all files in input directory
    for path in [x for x in (here / Path("in")).glob("*.png") if x.is_file()]:
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
        for x in range(image.width):
            for y in range(image.height):
                r, g, b, *_ = image.getpixel((x, y))
                out_bytes += RGBto565(r, g, b)

        # write to out file
        out_path = out_dir / Path(f"{path.stem}")
        with out_path.open('wb') as file:
            file.write(out_bytes)

env.AddPreAction("uploadfs", main)