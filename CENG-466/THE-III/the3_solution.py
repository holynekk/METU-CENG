# Ayberk Gökmen - 2380442
# Mert Kaan YILMAZ - 2381093

import os
import cv2
import numpy as np
from cv2 import IMREAD_GRAYSCALE
import matplotlib.pyplot as plt
import random

INPUT_PATH = "./THE3_Images/"
OUTPUT_PATH = "./Outputs/"

# The function to read the image with cv2 library
def read_image(img_path, grey=False):
    if grey:
        image = cv2.imread(img_path, IMREAD_GRAYSCALE)
        return image
    else:
        image = cv2.imread(img_path)
        b, g, r =  cv2.split(image)
        return r, g, b

    

# The function to merge separated rgb channels and write it with cv2 library
def write_image(r, g, b, output_path):
    image_merge = cv2.merge([r, g, b])
    h, w = image_merge.shape[:2]
    norm = np.zeros((h,w))
    final = cv2.normalize(image_merge,  norm, 0, 255, cv2.NORM_MINMAX)
    cv2.imwrite(output_path, final)

def write_image_one_channel(channel, output_path):
    h, w = channel.shape[:2]
    norm = np.zeros((h,w))
    final = cv2.normalize(channel,  norm, 0, 255, cv2.NORM_MINMAX)
    cv2.imwrite(output_path, final)


# Calculates euclidean distance between two points
def euc_distance(p1, p2):
    return ((p1[0] - p2[0])**2 + (p1[1] - p2[1])**2 + (p1[2] - p2[2])**2)**0.5

# Randomize the centroid
def random_sample(l, h):
    return int((h-l)*random.random() + l)

# Return k many random centroids
def get_random_centroids(data, k):
    # Boundaries are 255 since we use rgb
    r_min = g_min = b_min = 260
    r_max = g_max = b_max = -260

    h, w = data.shape[:2]
    # Iterate through entire image to get boundaries
    for i in range(h):
        for j in range(w):
            r_min = min(data[i][j][0], r_min)
            r_max = max(data[i][j][0], r_max)
            g_min = min(data[i][j][1], g_min)
            g_max = max(data[i][j][1], g_max)
            b_min = min(data[i][j][2], b_min)
            b_max = max(data[i][j][2], b_max)

    centroids = []
    # Randomize k centroid values
    for _ in range(k):
        centroids.append([random_sample(r_min, r_max), random_sample(g_min, g_max), random_sample(b_min, b_max)])
    return centroids


def detect_faces(image, k):
    '''
    Main function to run face detection.
    Firstly, it creates k-mean version of the original one, and then
    do the face detection.
    '''
    # Get random k number of centroids
    centroids = get_random_centroids(image, k)
    h, w, c = image.shape
    labels = np.arange(h * w).reshape(image.shape[:2])
    counts = [0]*k
    ic = 0
    # End the iteration at certain amount of time
    while ic < 10:
        # Loop through the image
        for i in range(h):
            for j in range(w):
                min_dist = float('inf')
                label = None
                # Calculate euc distance for all centroids
                # to get the closest one
                for a in range(k):
                    distance = euc_distance(image[i][j], centroids[a])
                    if distance < min_dist:
                        label = a
                        min_dist = distance
                # Set the label and increase cluster (centroid) count
                counts[label] += 1
                labels[i][j] = label
        # Update centroids
        new_centroids = np.zeros(k * 3).reshape(k, 3)
        for i in range(h):
            for j in range(w):
                new_centroids[labels[i][j]] += image[i][j]
        for i in range(k):
            new_centroids[i] = new_centroids[i]/counts[i]
        centroids = new_centroids[::]
        # Increase the iteration count
        ic += 1
        if ic < 10:
            break
    # Convert the image into its kmeans implemented version.
    for i in range(h):
        for j in range(w):
            image[i][j] = new_centroids[labels[i][j]]
    return image

def color_images(r_source, g_source, b_source, grey_image):
    # get the histogram of the source image
    hist_r, bins_r = np.histogram(r_source.flatten(), 256, [0, 256])
    hist_g, bins_g = np.histogram(g_source.flatten(), 256, [0, 256])
    hist_b, bins_b = np.histogram(b_source.flatten(), 256, [0, 256])
    
    # calculate cdf
    cdf_r = hist_r.cumsum()
    cdf_g = hist_g.cumsum()
    cdf_b = hist_b.cumsum()

    # normalize cdf to 0-255 to get the mapping
    cdf_r = (cdf_r - cdf_r.min()) * 255 / (cdf_r.max() - cdf_r.min())
    cdf_r = np.ma.filled(cdf_r, 0).astype('uint8')

    cdf_g = (cdf_g - cdf_g.min()) * 255 / (cdf_g.max() - cdf_g.min())
    cdf_g = np.ma.filled(cdf_g, 0).astype('uint8')

    cdf_b = (cdf_b - cdf_b.min()) * 255 / (cdf_b.max() - cdf_b.min())
    cdf_b = np.ma.filled(cdf_b, 0).astype('uint8')

    """ plt.plot(cdf_r, color = 'r')
    plt.plot(cdf_g, color = 'g')
    plt.plot(cdf_b, color = 'b')
    plt.show() """

    # use mapping for grey image
    r = cdf_r[grey_image]
    g = cdf_g[grey_image]
    b = cdf_b[grey_image]
    
    return r, g, b

def color_images2(r_source, g_source, b_source, grey_image):

    #downsample the target image
    grey_image = cv2.resize(grey_image, (0,0), fx=0.25, fy=0.25)
    target_h, target_w = grey_image.shape[:2]


    # pad to multiple of 25
    r_source = np.pad(r_source, ((0, 25 - r_source.shape[0] % 25), (0, 25 - r_source.shape[1] % 25)), 'constant')
    g_source = np.pad(g_source, ((0, 25 - g_source.shape[0] % 25), (0, 25 - g_source.shape[1] % 25)), 'constant')
    b_source = np.pad(b_source, ((0, 25 - b_source.shape[0] % 25), (0, 25 - b_source.shape[1] % 25)), 'constant')

    grey_image = np.pad(grey_image, ((0, 25 - grey_image.shape[0] % 25), (0, 25 - grey_image.shape[1] % 25)), 'constant')
  

    # get the variance of 25x25 windows of the source image
    var_r = cv2.boxFilter(r_source**2, -1, (25, 25)) - cv2.boxFilter(r_source, -1, (25, 25))**2
    var_g = cv2.boxFilter(g_source**2, -1, (25, 25)) - cv2.boxFilter(g_source, -1, (25, 25))**2
    var_b = cv2.boxFilter(b_source**2, -1, (25, 25)) - cv2.boxFilter(b_source, -1, (25, 25))**2

    # get the mean of 25x25 windows of the source image
    mean_r = cv2.boxFilter(r_source, -1, (25, 25))
    mean_g = cv2.boxFilter(g_source, -1, (25, 25))
    mean_b = cv2.boxFilter(b_source, -1, (25, 25))

    # get the variance of 25x25 windows of the target image
    var_grey = cv2.boxFilter(grey_image**2, -1, (25, 25)) - cv2.boxFilter(grey_image, -1, (25, 25))**2

    # get the mean of 25x25 windows of the target image
    mean_grey = cv2.boxFilter(grey_image, -1, (25, 25))

    r = np.zeros(grey_image.shape)
    g = np.zeros(grey_image.shape)
    b = np.zeros(grey_image.shape)

    for i in range(target_h):
        for j in range(target_w):
            # get the variance of the current window of the target image
            var_target = var_grey[i, j]
            mean_target = mean_grey[i, j]

            if(mean_target == 0):
                continue

            # get the difference between variances
            diff_r = np.abs(var_r - var_target)
            diff_g = np.abs(var_g - var_target)
            diff_b = np.abs(var_b - var_target)
            
            # get ther difference between means
            diff_mean_r = np.abs(mean_r - mean_target)
            diff_mean_g = np.abs(mean_g - mean_target)
            diff_mean_b = np.abs(mean_b - mean_target)

            # calculate the evaluation function
            evaluation_r = 2*diff_r + diff_mean_r
            evaluation_g = 2*diff_g + diff_mean_g
            evaluation_b = 2*diff_b + diff_mean_b

            plt.plot(evaluation_r.ravel())
            plt.show()
            
            # get the index of the minimum value
            min_index_r = np.unravel_index(np.argmin(evaluation_r), evaluation_r.shape)
            min_index_g = np.unravel_index(np.argmin(evaluation_g), evaluation_g.shape)
            min_index_b = np.unravel_index(np.argmin(evaluation_b), evaluation_b.shape)

            # get the corresponding pixel from the source image
            r[i, j] = r_source[min_index_r]
            g[i, j] = g_source[min_index_g]
            b[i, j] = b_source[min_index_b]

    return r, g, b



            




def plot_rgb_histogram(r, g, b):
    plt.figure()
    plt.hist(r.ravel(), bins=256, color='red', alpha=0.5)
    plt.hist(g.ravel(), bins=256, color='green', alpha=0.5)
    plt.hist(b.ravel(), bins=256, color='blue', alpha=0.5)
    plt.show()

def rgb_to_hsi(r, g, b):
    # first convert to hsv
    hsv = cv2.cvtColor(np.dstack((r, g, b)), cv2.COLOR_RGB2HSV)

    # convert hsv to hsi
    h = hsv[:, :, 0]
    s = hsv[:, :, 1]

    # calculate i
    i = (r + g + b) / 3

    return h, s, i


def plot_hsi_histogram(r, g, b):
    h, s, i = rgb_to_hsi(r, g, b)
    plt.figure()
    plt.hist(h.ravel(), bins=256, color='red', alpha=0.5)
    plt.hist(s.ravel(), bins=256, color='green', alpha=0.5)
    plt.hist(i.ravel(), bins=256, color='blue', alpha=0.5)
    plt.show()




def detect_edges(r, g, b):
    # detect edges using gradient magnitude
    r = np.uint8(r)
    g = np.uint8(g)
    b = np.uint8(b)
    edges_r = cv2.Canny(r, 100, 200)
    edges_g = cv2.Canny(g, 100, 200)
    edges_b = cv2.Canny(b, 100, 200)

    return edges_r, edges_g, edges_b

    


def rotate_image(r, g, b):
    r = np.rot90(r, 1)
    g = np.rot90(g, 1)
    b = np.rot90(b, 1)

    return r, g, b

if __name__ == '__main__':
    if not os.path.exists(OUTPUT_PATH):
        os.makedirs(OUTPUT_PATH)
        os.makedirs(OUTPUT_PATH + "/Channels")

    # Question 1 Face Detection ---------------------
    k = 10

    # 1st Image
    image = cv2.imread(INPUT_PATH + "1_source.png")
    output = detect_faces(image, k)
    cv2.imwrite(OUTPUT_PATH + "1_faces.png", output)
    
    # 2nd Image
    image = cv2.imread(INPUT_PATH + "2_source.png")
    scale_percent = 30
    width = int(image.shape[1] * scale_percent / 100)
    height = int(image.shape[0] * scale_percent / 100)
    dim = (width, height)
    # resize image
    resized = cv2.resize(image, dim, interpolation = cv2.INTER_AREA)
    output = detect_faces(resized, k)
    cv2.imwrite(OUTPUT_PATH + "2_faces.png", output)

    # 3rd Image
    image = cv2.imread(INPUT_PATH + "3_source.png")
    output = detect_faces(image, k)
    cv2.imwrite(OUTPUT_PATH + "3_faces.png", output)
    # -----------------------------------------------

    # Question 2 Pseudo-coloring --------------------

    # 1st Image
    grey_image = read_image(INPUT_PATH + "1.png", grey=True)
    r_source, g_source, b_source = read_image(INPUT_PATH + "1_source.png")

    colored_output = color_images(r_source, g_source, b_source, grey_image)
    #colored_output = rotate_image(*colored_output) # we rotate the image because it was not in the right direction, it seems cv2 can't read EXIF data properly
    write_image(*colored_output, OUTPUT_PATH + "1_colored.png")
    write_image_one_channel(colored_output[0], OUTPUT_PATH + "Channels/1_colored_r.png")
    write_image_one_channel(colored_output[1], OUTPUT_PATH + "Channels/1_colored_g.png")
    write_image_one_channel(colored_output[2], OUTPUT_PATH + "Channels/1_colored_b.png")
    hsi = rgb_to_hsi(*colored_output)
    write_image_one_channel(hsi[0], OUTPUT_PATH + "Channels/1_colored_h.png")
    write_image_one_channel(hsi[1], OUTPUT_PATH + "Channels/1_colored_s.png")
    write_image_one_channel(hsi[2], OUTPUT_PATH + "Channels/1_colored_i.png")


    hsi_source = rgb_to_hsi(r_source, g_source, b_source)

    edge_output = detect_edges(r_source, g_source, b_source)
    write_image(*edge_output, OUTPUT_PATH + "1_colored_edges_rgb.png")
    edge_output = detect_edges(*hsi_source)
    write_image(*edge_output, OUTPUT_PATH + "1_colored_edges_hsi.png")

    # 2nd Image
    grey_image = read_image(INPUT_PATH + "2.png", grey=True)
    r_source, g_source, b_source = read_image(INPUT_PATH + "2_source.png")

    colored_output = color_images(r_source, g_source, b_source, grey_image)
    write_image(*colored_output, OUTPUT_PATH + "2_colored.png")
    write_image_one_channel(colored_output[0], OUTPUT_PATH + "Channels/2_colored_r.png")
    write_image_one_channel(colored_output[1], OUTPUT_PATH + "Channels/2_colored_g.png")
    write_image_one_channel(colored_output[2], OUTPUT_PATH + "Channels/2_colored_b.png")
    hsi = rgb_to_hsi(*colored_output)
    write_image_one_channel(hsi[0], OUTPUT_PATH + "Channels/2_colored_h.png")
    write_image_one_channel(hsi[1], OUTPUT_PATH + "Channels/2_colored_s.png")
    write_image_one_channel(hsi[2], OUTPUT_PATH + "Channels/2_colored_i.png")

    hsi_source = rgb_to_hsi(r_source, g_source, b_source)

    edge_output = detect_edges(r_source, g_source, b_source)
    write_image(*edge_output, OUTPUT_PATH + "2_colored_edges_rgb.png")
    edge_output = detect_edges(*hsi_source)
    write_image(*edge_output, OUTPUT_PATH + "2_colored_edges_hsi.png")

    # 3rd Image
    grey_image = read_image(INPUT_PATH + "3.png", grey=True)
    r_source, g_source, b_source = read_image(INPUT_PATH + "3_source.png")

    colored_output = color_images(r_source, g_source, b_source, grey_image)
    write_image(*colored_output, OUTPUT_PATH + "3_colored.png")
    write_image_one_channel(colored_output[0], OUTPUT_PATH + "Channels/3_colored_r.png")
    write_image_one_channel(colored_output[1], OUTPUT_PATH + "Channels/3_colored_g.png")
    write_image_one_channel(colored_output[2], OUTPUT_PATH + "Channels/3_colored_b.png")
    hsi = rgb_to_hsi(*colored_output)
    write_image_one_channel(hsi[0], OUTPUT_PATH + "Channels/3_colored_h.png")
    write_image_one_channel(hsi[1], OUTPUT_PATH + "Channels/3_colored_s.png")
    write_image_one_channel(hsi[2], OUTPUT_PATH + "Channels/3_colored_i.png")

    hsi_source = rgb_to_hsi(r_source, g_source, b_source)

    edge_output = detect_edges(r_source, g_source, b_source)
    write_image(*edge_output, OUTPUT_PATH + "3_colored_edges_rgb.png")
    edge_output = detect_edges(*hsi_source)
    write_image(*edge_output, OUTPUT_PATH + "3_colored_edges_hsi.png")

    # 4th Image
    grey_image = read_image(INPUT_PATH + "4.png", grey=True)
    r_source, g_source, b_source = read_image(INPUT_PATH + "4_source.png")

    colored_output = color_images(r_source, g_source, b_source, grey_image)
    write_image(*colored_output, OUTPUT_PATH + "4_colored.png")
    write_image_one_channel(colored_output[0], OUTPUT_PATH + "Channels/4_colored_r.png")
    write_image_one_channel(colored_output[1], OUTPUT_PATH + "Channels/4_colored_g.png")
    write_image_one_channel(colored_output[2], OUTPUT_PATH + "Channels/4_colored_b.png")
    hsi = rgb_to_hsi(*colored_output)
    write_image_one_channel(hsi[0], OUTPUT_PATH + "Channels/4_colored_h.png")
    write_image_one_channel(hsi[1], OUTPUT_PATH + "Channels/4_colored_s.png")
    write_image_one_channel(hsi[2], OUTPUT_PATH + "Channels/4_colored_i.png")
    # -----------------------------------------------
    
