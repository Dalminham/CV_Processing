import cv2 as cv
import os 
import numpy as np


img = cv.imread("InImage/gauss_2.bmp",1)
gray_img = cv.cvtColor(img, cv.COLOR_RGB2GRAY)
cv.imwrite("gray2.bmp",gray_img)
