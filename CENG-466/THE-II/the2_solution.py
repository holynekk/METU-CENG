# Ayberk Gökmen - 2380442
# Mert Kaan YILMAZ - 2381093

import os
import cv2
import numpy as np
from scipy.linalg import hadamard

INPUT_PATH = "./THE2_images/"
OUTPUT_PATH = "./Outputs/"

# The function to read the image with cv2 library
def read_image(img_path):
    img = cv2.imread(img_path)
    return cv2.split(img)

# The function to merge separated rgb channels and write it with cv2 library
def write_image(r, g, b, output_path):
    image_merge = cv2.merge([r, g, b])
    h, w = image_merge.shape[:2]
    norm = np.zeros((h,w))
    final = cv2.normalize(image_merge,  norm, 0, 255, cv2.NORM_MINMAX)
    cv2.imwrite(output_path, final)


def fourier_transform(r, g, b):
    # For red channel
    f = np.fft.fft2(r)
    fshift = np.fft.fftshift(f)
    magnitude_spectrum_1 = 20*np.log(np.abs(fshift))

    # For green channel
    f = np.fft.fft2(g)
    fshift = np.fft.fftshift(f)
    magnitude_spectrum_2 = 20*np.log(np.abs(fshift))

    # For blue channel
    f = np.fft.fft2(b)
    fshift = np.fft.fftshift(f)
    magnitude_spectrum_3 = 20*np.log(np.abs(fshift))

    return magnitude_spectrum_1, magnitude_spectrum_2, magnitude_spectrum_3


def hadamard_transform(r, g, b):
    h, w = r.shape #get the shape of the image 
    size = h > w and h or w #get the max size of the image

    # make the size of the image a power of 2
    size = 2 ** int(np.log2(size) + 1)

    # hadamard matrix
    H = hadamard(size)

    # pad the image with zeros
    r = np.pad(r, ((0, size - h), (0, size - w)), 'constant')
    g = np.pad(g, ((0, size - h), (0, size - w)), 'constant')
    b = np.pad(b, ((0, size - h), (0, size - w)), 'constant')

    # hadamard transform
    r = np.matmul(np.matmul(H, r), H)
    g = np.matmul(np.matmul(H, g), H)
    b = np.matmul(np.matmul(H, b), H)

    return r, g, b

def cosine_transform(r, g, b):
    # For blue channel
    imf = np.float32(r)/255.0 # float conversion/scale
    dst = cv2.dct(imf) # the dct
    r = np.uint8(dst)*255.0 # convert back

    # For green channel
    imf = np.float32(g)/255.0 # float conversion/scale
    dst = cv2.dct(imf) # the dct
    g = np.uint8(dst)*255.0 # convert back

    # For blue channel
    imf = np.float32(b)/255.0 # float conversion/scale
    dst = cv2.dct(imf) # the dct
    b = np.uint8(dst)*255.0 # convert back

    return r, g, b


def ideal_low_pass_filter(r, g, b, r_val):
    filtered_channels = []
    # For rgb channels, iterate through all three
    for channel in (r, g, b):
        # Get the frequency domain values
        F = np.fft.fft2(channel)
        Fshift = np.fft.fftshift(F)
        M,N = channel.shape
        H = np.zeros((M,N), dtype=np.float32)
        # Set the cut-off frequency
        D0 = r_val
        for u in range(M):
            for v in range(N):
                D = np.sqrt((u-M/2)**2 + (v-N/2)**2)
                if D <= D0:
                    H[u,v] = 1
                else:
                    H[u,v] = 0
        # Apply inverse fourier transform to get the image
        Gshift = Fshift * H
        G = np.fft.ifftshift(Gshift)
        filtered_channels.append(np.abs(np.fft.ifft2(G)))

    return (*filtered_channels, )


def gaussian_low_pass_filter(r, g, b, r_val):
    filtered_channels = []
    # For rgb channels, iterate through all three
    for channel in (r, g, b):
        # Get the frequency domain values
        F = np.fft.fft2(channel)
        Fshift = np.fft.fftshift(F)
        M,N = channel.shape
        H = np.zeros((M,N), dtype=np.float32)
        # Set the cut-off frequency
        D0 = r_val
        for u in range(M):
            for v in range(N):
                D = np.sqrt((u-M/2)**2 + (v-N/2)**2)
                H[u,v] = np.exp(-D**2/(2*D0*D0))
        # Apply inverse fourier transform to get the image
        Gshift = Fshift * H
        G = np.fft.ifftshift(Gshift)
        filtered_channels.append(np.abs(np.fft.ifft2(G)))

    return (*filtered_channels, )


def butterworh_low_pass_filter(r, g, b, r_val):
    filtered_channels = []
    # For rgb channels, iterate through all three
    for channel in (r, g, b):
        # Get the frequency domain values
        F = np.fft.fft2(channel)
        Fshift = np.fft.fftshift(F)
        M,N = channel.shape
        H = np.zeros((M,N), dtype=np.float32)
        # Set the cut-off frequency
        D0 = r_val
        # Order
        n = 10
        for u in range(M):
            for v in range(N):
                D = np.sqrt((u-M/2)**2 + (v-N/2)**2)
                H[u,v] = 1 / (1 + (D/D0)**n)
        # Apply inverse fourier transform to get the image
        Gshift = Fshift * H
        G = np.fft.ifftshift(Gshift)
        filtered_channels.append(np.abs(np.fft.ifft2(G)))

    return (*filtered_channels, )


def ideal_high_pass_filter(r, g, b, r_val):
    filtered_channels = []
    # For rgb channels, iterate through all three
    for channel in (r, g, b):
        # Get the frequency domain values
        F = np.fft.fft2(channel)
        Fshift = np.fft.fftshift(F)
        M,N = channel.shape
        H = np.zeros((M,N), dtype=np.float32)
        # Set the cut-off frequency
        D0 = r_val
        for u in range(M):
            for v in range(N):
                D = np.sqrt((u-M/2)**2 + (v-N/2)**2)
                if D <= D0:
                    H[u,v] = 1
                else:
                    H[u,v] = 0
        # Apply inverse fourier transform to get the image
        H = 1 - H
        Gshift = Fshift * H
        G = np.fft.ifftshift(Gshift)
        filtered_channels.append(np.abs(np.fft.ifft2(G)))

    return (*filtered_channels, )

def gaussian_high_pass_filter(r, g, b, r_val):
    filtered_channels = []
    # For rgb channels, iterate through all three
    for channel in (r, g, b):
        # Get the frequency domain values
        F = np.fft.fft2(channel)
        Fshift = np.fft.fftshift(F)
        M,N = channel.shape
        H = np.zeros((M,N), dtype=np.float32)
        # Set the cut-off frequency
        D0 = r_val
        for u in range(M):
            for v in range(N):
                D = np.sqrt((u-M/2)**2 + (v-N/2)**2)
                H[u,v] = np.exp(-D**2/(2*D0*D0))
        # Apply inverse fourier transform to get the image
        HPF = 1 - H
        Gshift = Fshift * HPF
        G = np.fft.ifftshift(Gshift)
        filtered_channels.append(np.abs(np.fft.ifft2(G)))

    return (*filtered_channels, )


def butterworh_high_pass_filter(r, g, b, r_val):
    filtered_channels = []
    # For rgb channels, iterate through all three
    for channel in (r, g, b):
        # Get the frequency domain values
        F = np.fft.fft2(channel)
        Fshift = np.fft.fftshift(F)
        M,N = channel.shape
        HPF = np.zeros((M,N), dtype=np.float32)
        # Set the cut-off frequency
        D0 = r_val
        n = 10
        for u in range(M):
            for v in range(N):
                D = np.sqrt((u-M/2)**2 + (v-N/2)**2)
                HPF[u,v] = 1 / (1 + (D0/D)**n)
        # Apply inverse fourier transform to get the image
        Gshift = Fshift * HPF
        G = np.fft.ifftshift(Gshift)
        filtered_channels.append(np.abs(np.fft.ifft2(G)))

    return (*filtered_channels, )



def band_pass_filter(r, g, b):
    filter_size = 100
    filter2_size = 50

    # Adapted from lecture notes.
    freq_image = np.fft.fft2(r)
    freq_image_centered = np.fft.fftshift(freq_image)
    band_pass = np.copy(freq_image_centered)
    band_pass[int(freq_image_centered.shape[0]/2) - filter_size-1:int(freq_image_centered.shape[0]/2) + filter_size, int(freq_image_centered.shape[1]/2) - filter_size-1: int(freq_image_centered.shape[1]/2) + filter_size] = freq_image_centered[int(freq_image_centered.shape[0]/2) - filter_size-1:int(freq_image_centered.shape[0]/2) + filter_size, int(freq_image_centered.shape[1]/2) - filter_size-1: int(freq_image_centered.shape[1]/2) + filter_size]
    band_pass[int(freq_image_centered.shape[0]/2) - filter2_size-1:int(freq_image_centered.shape[0]/2) + filter2_size, int(freq_image_centered.shape[1]/2) - filter2_size-1: int(freq_image_centered.shape[1]/2) + filter2_size] = 0
    filtered_image_r = np.fft.ifft2(np.fft.ifftshift(band_pass)).real

    freq_image = np.fft.fft2(g)
    freq_image_centered = np.fft.fftshift(freq_image)
    band_pass = np.copy(freq_image_centered)
    band_pass[int(freq_image_centered.shape[0]/2) - filter_size-1:int(freq_image_centered.shape[0]/2) + filter_size, int(freq_image_centered.shape[1]/2) - filter_size-1: int(freq_image_centered.shape[1]/2) + filter_size] = freq_image_centered[int(freq_image_centered.shape[0]/2) - filter_size-1:int(freq_image_centered.shape[0]/2) + filter_size, int(freq_image_centered.shape[1]/2) - filter_size-1: int(freq_image_centered.shape[1]/2) + filter_size]
    band_pass[int(freq_image_centered.shape[0]/2) - filter2_size-1:int(freq_image_centered.shape[0]/2) + filter2_size, int(freq_image_centered.shape[1]/2) - filter2_size-1: int(freq_image_centered.shape[1]/2) + filter2_size] = 0
    filtered_image_g = np.fft.ifft2(np.fft.ifftshift(band_pass)).real

    freq_image = np.fft.fft2(b)
    freq_image_centered = np.fft.fftshift(freq_image)
    band_pass = np.copy(freq_image_centered)
    band_pass[int(freq_image_centered.shape[0]/2) - filter_size-1:int(freq_image_centered.shape[0]/2) + filter_size, int(freq_image_centered.shape[1]/2) - filter_size-1: int(freq_image_centered.shape[1]/2) + filter_size] = freq_image_centered[int(freq_image_centered.shape[0]/2) - filter_size-1:int(freq_image_centered.shape[0]/2) + filter_size, int(freq_image_centered.shape[1]/2) - filter_size-1: int(freq_image_centered.shape[1]/2) + filter_size]
    band_pass[int(freq_image_centered.shape[0]/2) - filter2_size-1:int(freq_image_centered.shape[0]/2) + filter2_size, int(freq_image_centered.shape[1]/2) - filter2_size-1: int(freq_image_centered.shape[1]/2) + filter2_size] = 0
    filtered_image_b = np.fft.ifft2(np.fft.ifftshift(band_pass)).real

    return filtered_image_r, filtered_image_g, filtered_image_b

def band_reject_filter(r, g, b):
    filter_size = 100
    filter2_size = 50

    freq_image = np.fft.fft2(r)
    freq_image_centered = np.fft.fftshift(freq_image)
    band_reject = np.copy(freq_image_centered)
    band_reject[int(freq_image_centered.shape[0]/2) - filter_size-1:int(freq_image_centered.shape[0]/2) + filter_size, int(freq_image_centered.shape[1]/2) - filter_size-1: int(freq_image_centered.shape[1]/2) + filter_size] = 0
    band_reject[int(freq_image_centered.shape[0]/2) - filter2_size-1:int(freq_image_centered.shape[0]/2) + filter2_size, int(freq_image_centered.shape[1]/2) - filter2_size-1: int(freq_image_centered.shape[1]/2) + filter2_size] = freq_image_centered[int(freq_image_centered.shape[0]/2) - filter2_size-1:int(freq_image_centered.shape[0]/2) + filter2_size, int(freq_image_centered.shape[1]/2) - filter2_size-1: int(freq_image_centered.shape[1]/2) + filter2_size]
    filtered_image_r = np.fft.ifft2(np.fft.ifftshift(band_reject)).real

    freq_image = np.fft.fft2(g)
    freq_image_centered = np.fft.fftshift(freq_image)
    band_reject = np.copy(freq_image_centered)
    band_reject[int(freq_image_centered.shape[0]/2) - filter_size-1:int(freq_image_centered.shape[0]/2) + filter_size, int(freq_image_centered.shape[1]/2) - filter_size-1: int(freq_image_centered.shape[1]/2) + filter_size] = 0
    band_reject[int(freq_image_centered.shape[0]/2) - filter2_size-1:int(freq_image_centered.shape[0]/2) + filter2_size, int(freq_image_centered.shape[1]/2) - filter2_size-1: int(freq_image_centered.shape[1]/2) + filter2_size] = freq_image_centered[int(freq_image_centered.shape[0]/2) - filter2_size-1:int(freq_image_centered.shape[0]/2) + filter2_size, int(freq_image_centered.shape[1]/2) - filter2_size-1: int(freq_image_centered.shape[1]/2) + filter2_size]
    filtered_image_g = np.fft.ifft2(np.fft.ifftshift(band_reject)).real

    freq_image = np.fft.fft2(b)
    freq_image_centered = np.fft.fftshift(freq_image)
    band_reject = np.copy(freq_image_centered)
    band_reject[int(freq_image_centered.shape[0]/2) - filter_size-1:int(freq_image_centered.shape[0]/2) + filter_size, int(freq_image_centered.shape[1]/2) - filter_size-1: int(freq_image_centered.shape[1]/2) + filter_size] = 0
    band_reject[int(freq_image_centered.shape[0]/2) - filter2_size-1:int(freq_image_centered.shape[0]/2) + filter2_size, int(freq_image_centered.shape[1]/2) - filter2_size-1: int(freq_image_centered.shape[1]/2) + filter2_size] = freq_image_centered[int(freq_image_centered.shape[0]/2) - filter2_size-1:int(freq_image_centered.shape[0]/2) + filter2_size, int(freq_image_centered.shape[1]/2) - filter2_size-1: int(freq_image_centered.shape[1]/2) + filter2_size]
    filtered_image_b = np.fft.ifft2(np.fft.ifftshift(band_reject)).real

    return filtered_image_r, filtered_image_g, filtered_image_b

def improve_the_contrast(r, g, b):
    filtered_channels = []
    # For rgb channels
    for channel in (r, g, b):
        # ---------------------------------------------------------------------------
        # kernel = np.array([[0, 1, 0], [1, -4, 1], [0, 1, 0]])
        # LaplacianImage = cv2.filter2D(src=channel, ddepth=-1, kernel=kernel)
        # c = -1
        # fltrd_channel = channel + c*LaplacianImage
        # filtered_channels.append(fltrd_channel)


        # ---------------------------------------------------------------------------

        x,y = channel.shape
        g = np.zeros((x,y), dtype=np.float32)
        pepper = 0.1
        salt = 0.95  
        for i in range(x):
            for j in range(y):
                rdn = np.random.random()
                if rdn < pepper:
                    g[i][j] = 0
                elif rdn > salt:
                    g[i][j] = 1
                else:
                    g[i][j] = channel[i][j]

        channel_noise = g

        # img_noise_median = np.clip(channel_noise, -1, 1) #pixel value range
        # img_noise_median = img_as_ubyte(img_noise_median) #convert to uint8
        denoise_median = cv2.medianBlur(channel_noise, 5)
        filtered_channels.append(denoise_median)
        # ---------------------------------------------------------------------------
        
        # img_median = cv2.medianBlur(channel, 5)

        # ---------------------------------------------------------------------------

        # noiseless_image_bw = cv2.fastNlMeansDenoising(channel, None, 20, 7, 21) 
        # filtered_channels.append(noiseless_image_bw)

    return (*filtered_channels, )

if __name__ == '__main__':
    if not os.path.exists(OUTPUT_PATH):
        os.makedirs(OUTPUT_PATH)

    # Step 2 --------------------------------------------

    # For the first image
    r, g, b = read_image(INPUT_PATH + "1.png")

    output = fourier_transform(r, g, b)
    write_image(*output, OUTPUT_PATH + "F1.png")

    output = cosine_transform(r, g, b)
    write_image(*output, OUTPUT_PATH + "C1.png")

    # For the second image
    r, g, b = read_image(INPUT_PATH + "2.png")

    output = fourier_transform(r, g, b)
    write_image(*output, OUTPUT_PATH + "F2.png")

    output = cosine_transform(r, g, b)
    write_image(*output, OUTPUT_PATH + "C2.png")


    # Step 3 --------------------------------------------
    r, g, b = read_image(INPUT_PATH + "3.png")

    # Apply Ideal Low pass Filter with 3 different cutoff frequencies r1 , r2 , r3 of your choosing.
    r1, r2, r3 = 10, 40, 95

    # Ideal Low pass Filter
    output = ideal_low_pass_filter(r, g, b, r1)
    write_image(*output, OUTPUT_PATH + "ILP_r1.png")

    output = ideal_low_pass_filter(r, g, b, r2)
    write_image(*output, OUTPUT_PATH + "ILP_r2.png")

    output = ideal_low_pass_filter(r, g, b, r3)
    write_image(*output, OUTPUT_PATH + "ILP_r3.png")

    # Gaussian Low Pass Filter
    output = gaussian_low_pass_filter(r, g, b, r1)
    write_image(*output, OUTPUT_PATH + "GLP_r1.png")

    output = gaussian_low_pass_filter(r, g, b, r2)
    write_image(*output, OUTPUT_PATH + "GLP_r2.png")

    output = gaussian_low_pass_filter(r, g, b, r3)
    write_image(*output, OUTPUT_PATH + "GLP_r3.png")

    # # Butterworh Low Pass Filter
    output = butterworh_low_pass_filter(r, g, b, r1)
    write_image(*output, OUTPUT_PATH + "BLP_r1.png")

    output = butterworh_low_pass_filter(r, g, b, r2)
    write_image(*output, OUTPUT_PATH + "BLP_r2.png")

    output = butterworh_low_pass_filter(r, g, b, r3)
    write_image(*output, OUTPUT_PATH + "BLP_r3.png")


    # Step 4 --------------------------------------------
    r, g, b = read_image(INPUT_PATH + "3.png")

    # # Apply Ideal High pass Filter with 3 different cutoff frequencies r1 , r2 , r3 of your choosing.
    r1, r2, r3 = 10, 40, 95

    # Ideal High pass Filter
    output = ideal_high_pass_filter(r, g, b, r1)
    write_image(*output, OUTPUT_PATH + "IHP_r1.png")

    output = ideal_high_pass_filter(r, g, b, r2)
    write_image(*output, OUTPUT_PATH + "IHP_r2.png")

    output = ideal_high_pass_filter(r, g, b, r3)
    write_image(*output, OUTPUT_PATH + "IHP_r3.png")

    # Gaussian High Pass Filter
    output = gaussian_high_pass_filter(r, g, b, r1)
    write_image(*output, OUTPUT_PATH + "GHP_r1.png")

    output = gaussian_high_pass_filter(r, g, b, r2)
    write_image(*output, OUTPUT_PATH + "GHP_r2.png")

    output = gaussian_high_pass_filter(r, g, b, r3)
    write_image(*output, OUTPUT_PATH + "GHP_r3.png")

    # # Butterworh High Pass Filter
    output = butterworh_high_pass_filter(r, g, b, r1)
    write_image(*output, OUTPUT_PATH + "BHP_r1.png")

    output = butterworh_high_pass_filter(r, g, b, r2)
    write_image(*output, OUTPUT_PATH + "BHP_r2.png")

    output = butterworh_high_pass_filter(r, g, b, r3)
    write_image(*output, OUTPUT_PATH + "BHP_r3.png")
    
    # Step 5 --------------------------------------------
    bandwidht_freq = None

    # Read the image 4 first and process it
    r, g, b = read_image(INPUT_PATH + "4.png")

    output = band_reject_filter(r, g, b)
    write_image(*output, OUTPUT_PATH + "BR1.png")
    output = band_pass_filter(r, g, b)
    write_image(*output, OUTPUT_PATH + "BP1.png")

    # Then read the image 5 5and process it
    r, g, b = read_image(INPUT_PATH + "5.png")

    output = band_reject_filter(r, g, b)
    write_image(*output, OUTPUT_PATH + "BR2.png")
    output = band_pass_filter(r, g, b)
    write_image(*output, OUTPUT_PATH + "BP2.png")


    # Step 6 --------------------------------------------
    r, g, b = read_image(INPUT_PATH + "6.png")
    output = improve_the_contrast(r, g, b)
    write_image(*output, OUTPUT_PATH + "Space6.png")

    r, g, b = read_image(INPUT_PATH + "7.png")
    output = improve_the_contrast(r, g, b)
    write_image(*output, OUTPUT_PATH + "Space7.png")
     
    r, g, b = read_image(INPUT_PATH + "1.png")
    
    output = hadamard_transform(r, g, b)
    write_image(*output, OUTPUT_PATH + "H1.png")
    
    output = hadamard_transform(r, g ,b)
    write_image(*output, OUTPUT_PATH + "H2.png")
