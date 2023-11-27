# Ayberk Gökmen - 2380442
# Mert Kaan YILMAZ - 2381093

import os
import cv2
import numpy as np
from skimage.segmentation import slic, mark_boundaries
from skimage.color import label2rgb
from skimage.future import graph
from skimage.morphology import max_tree
from matplotlib import pyplot as plt

INPUT_PATH = "./THE4_Images/"
OUTPUT_PATH = "./Outputs/"

def read_image(img_path):
    image = cv2.imread(img_path)
    return image


def write_image(image, output_path):
    normalized = cv2.normalize(image, None, 0, 255, cv2.NORM_MINMAX)
    cv2.imwrite(output_path, normalized)


def object_detection(image, name, kernel_size, threshold, color_space="hsv", iter_erode=10, iter_dialate=10):

    # Convert to color space
    converted = cv2.cvtColor(image, cv2.COLOR_BGR2HSV if color_space == "hsv" else (cv2.COLOR_BGR2LAB if color_space == "lab" else cv2.COLOR_BGR2YUV))

    # write_image(converted, OUTPUT_PATH + name + "_converted.png")
    # Convert to grayscale
    gray = cv2.cvtColor(converted, cv2.COLOR_BGR2GRAY)

     # threshold
    _, gray = cv2.threshold(gray, threshold, 255, cv2.THRESH_BINARY)

    # write_image(gray, OUTPUT_PATH + name + "_threshold.png")

    

    # erode and dilate to remove noise  
    kernel = np.ones((kernel_size, kernel_size), np.uint8)

    """ # create a circle kernel
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (kernel_size, kernel_size)) """

    gray = cv2.erode(gray, kernel, iterations=iter_erode)
    gray = cv2.dilate(gray, kernel, iterations=iter_dialate)

    # write_image(gray, OUTPUT_PATH + name + "_morph.png")    

    # Find contours, external only, white on black
    contours, _ = cv2.findContours(gray, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Draw contours
    count = 0
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > 10000: # exclude small areas like noise etc
            (x, y), radius = cv2.minEnclosingCircle(cnt)
            center = (int(x), int(y))
            radius = int(radius)
            cv2.circle(image, center, radius, (0, 255, 0), 2)
            count += 1

    return image, count


def are_diff_colors(c1, c2):
    if c1[0] != c2[0] or c1[1] != c2[1] or c1[2] != c2[2]:
        return True
    return False


def image_segmentation_v2(img, seg_count, compact_val):
    h, w = img.shape[:2]

    original_image = np.copy(img)

    # labels = slic(original_image, n_segments=100, compactness=20)
    labels = slic(original_image, n_segments=seg_count, compactness=compact_val)
    g = graph.rag_mean_color(img, labels)

    fig, ax = plt.subplots(nrows=1, sharex=True, sharey=True, figsize=(3, 4))
    lc = graph.show_rag(labels, g, img, ax=ax)
    ax.axis('off')
    plt.savefig('bum.png', dpi=300, bbox_inches='tight', pad_inches=0)
    region_adjacency_graph = cv2.resize(cv2.imread("bum.png"), (w, h), interpolation = cv2.INTER_AREA)


    segmentation_map = label2rgb(labels, original_image, kind = 'avg')

    boundary_overlay = np.copy(img)    
    for i in range(img.shape[0]-1):
        for j in range(img.shape[1]-1):
            if are_diff_colors(segmentation_map[i][j], segmentation_map[i-1][j]) or are_diff_colors(segmentation_map[i][j], segmentation_map[i][j-1]):
                boundary_overlay[i][j] = (255, 0, 0)


    tree_relationship_structure = np.copy(img)

    # Concat [Original Image - Segmentation map - Boundary overlay - Tree relationship structure - Region adjacency graph]
    result = np.concatenate(
        (original_image, segmentation_map, boundary_overlay, tree_relationship_structure, region_adjacency_graph),
        axis=1
    )

    return result


if __name__ == '__main__':
    if not os.path.exists(OUTPUT_PATH):
        os.makedirs(OUTPUT_PATH)

    
    # Question 1 (40 Points) - Object Counting

    ### IMAGE A1 ###
    img1 = read_image(INPUT_PATH + "A1.png")
    img1, count1 = object_detection(img1, "A1", kernel_size=10, threshold=110) # default color space is hsv
    write_image(img1, OUTPUT_PATH + "A1.png")
    print("The number of flowers in image A1 is", count1)

    ### IMAGE A2 ###
    img2 = read_image(INPUT_PATH + "A2.png")
    img2, count2 = object_detection(img2, "A2", kernel_size=15,threshold=130, color_space="yuv", iter_erode=3, iter_dialate=5)
    write_image(img2, OUTPUT_PATH + "A2.png")
    print("The number of flowers in image A2 is", count2)

    ### IMAGE A3 ###
    img3 = read_image(INPUT_PATH + "A3.png")
    img3, count3 = object_detection(img3, "A3", kernel_size=10, threshold=130, color_space="lab", iter_erode=15, iter_dialate=10)
    write_image(img3, OUTPUT_PATH + "A3.png")
    print("The number of flowers in image A3 is", count3)


    # Question 2 (60 Points) - Segmentation

    # Parameters for segment_counts & compactness values
    p_set_1 = (200, 30)
    p_set_2 = (100, 30)
    p_set_3 = (200, 60)

    ### IMAGE B1 ###
    img4 = read_image(INPUT_PATH + "B1.jpg")
    output = image_segmentation_v2(img4, *p_set_1)
    write_image(output, OUTPUT_PATH + "B1_algorithm_meanshift_parameterset_1.png")
    output = image_segmentation_v2(img4, *p_set_2)
    write_image(output, OUTPUT_PATH + "B1_algorithm_meanshift_parameterset_2.png")
    output = image_segmentation_v2(img4, *p_set_3)
    write_image(output, OUTPUT_PATH + "B1_algorithm_meanshift_parameterset_3.png")

    ### IMAGE B2 ###
    img5 = read_image(INPUT_PATH + "B2.jpg")
    output = image_segmentation_v2(img5, *p_set_1)
    write_image(output, OUTPUT_PATH + "B2_algorithm_meanshift_parameterset_1.png")
    output = image_segmentation_v2(img5, *p_set_2)
    write_image(output, OUTPUT_PATH + "B2_algorithm_meanshift_parameterset_2.png")
    output = image_segmentation_v2(img5, *p_set_3)
    write_image(output, OUTPUT_PATH + "B2_algorithm_meanshift_parameterset_3.png")

    ### IMAGE B3 ###
    img6 = read_image(INPUT_PATH + "B3.jpg")
    output = image_segmentation_v2(img6, *p_set_1)
    write_image(output, OUTPUT_PATH + "B3_algorithm_meanshift_parameterset_1.png")
    output = image_segmentation_v2(img6, *p_set_2)
    write_image(output, OUTPUT_PATH + "B3_algorithm_meanshift_parameterset_2.png")
    output = image_segmentation_v2(img6, *p_set_3)
    write_image(output, OUTPUT_PATH + "B3_algorithm_meanshift_parameterset_3.png")

    ### IMAGE B4 ###
    img7 = read_image(INPUT_PATH + "B4.jpg")
    output = image_segmentation_v2(img7, *p_set_1)
    write_image(output, OUTPUT_PATH + "B4_algorithm_meanshift_parameterset_1.png")
    output = image_segmentation_v2(img7, *p_set_2)
    write_image(output, OUTPUT_PATH + "B4_algorithm_meanshift_parameterset_2.png")
    output = image_segmentation_v2(img7, *p_set_3)
    write_image(output, OUTPUT_PATH + "B4_algorithm_meanshift_parameterset_3.png")

