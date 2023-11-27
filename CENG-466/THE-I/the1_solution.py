# Ayberk Gökmen - 2380442
# Mert Kaan YILMAZ - 2381093

import os
from PIL import Image, ImageOps
from matplotlib import pyplot as plt
import numpy as np

INPUT_PATH = "./THE1-Images/"
OUTPUT_PATH = "./Outputs/"

def read_image(img_path, rgb = True):
    img = Image.open(img_path)
    if rgb:
        img = img.convert("RGB")
    else:
        img = img.convert("L")
    return img

def write_image(img, output_path, rgb = True):
    if rgb:
        img.convert("RGB").save(output_path, "PNG")
    else:
        img.convert("L").save(output_path, "PNG")

def extract_save_histogram(img, path):
    counts, bins = np.histogram(img, bins=25)
    plt.hist(bins[:-1], bins, weights=counts)
    plt.savefig(path)
    plt.clf()


def rotate_image(img,  degree = 0, interpolation_type = "linear"):
    #interpolation type: "linear" or "cubic"
    #degree: 45 or 90
    if(interpolation_type == "linear"):
        return img.rotate(degree, resample = Image.Resampling.BILINEAR)
    else:       
        return img.rotate(degree, resample = Image.Resampling.BICUBIC)

def histogram_equalization(img):
    img = ImageOps.equalize(img)
    return img

if __name__ == '__main__':
    if not os.path.exists(OUTPUT_PATH):
        os.makedirs(OUTPUT_PATH)
    #PART1
    img = read_image(INPUT_PATH + "a1.png")
    output = rotate_image(img, 45, "linear")
    write_image(output, OUTPUT_PATH + "a1_45_linear.png")

    img = read_image(INPUT_PATH + "a1.png")
    output = rotate_image(img, 45, "cubic")
    write_image(output, OUTPUT_PATH + "a1_45_cubic.png")

    img = read_image(INPUT_PATH + "a1.png")
    output = rotate_image(img, 90, "linear")
    write_image(output, OUTPUT_PATH + "a1_90_linear.png")

    img = read_image(INPUT_PATH + "a1.png")
    output = rotate_image(img, 90, "cubic")
    write_image(output, OUTPUT_PATH + "a1_90_cubic.png")

    img = read_image(INPUT_PATH + "a2.png")
    output = rotate_image(img, 45, "linear")
    write_image(output, OUTPUT_PATH + "a2_45_linear.png")

    img = read_image(INPUT_PATH + "a2.png")
    output = rotate_image(img, 45, "cubic")
    write_image(output, OUTPUT_PATH + "a2_45_cubic.png")

    #PART2
    img = read_image(INPUT_PATH + "b1.png", rgb = False)
    extract_save_histogram(img, OUTPUT_PATH + "original_histogram.png")
    equalized = histogram_equalization(img)
    extract_save_histogram(equalized, OUTPUT_PATH + "equalized_histogram.png")
    write_image(equalized, OUTPUT_PATH + "enhanced_image.png")

    # BONUS
    # Define the following function
    # equalized = adaptive_histogram_equalization(img)
    # extract_save_histogram(equalized, OUTPUT_PATH + "adaptive_equalized_histogram.png")
    # write_image(output, OUTPUT_PATH + "adaptive_enhanced_image.png")







