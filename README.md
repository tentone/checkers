# Checkers
 - Checkers game board detection vision system.

## Implementation

We started by drawing a checkerboard to use during the work, we chose to use a tray with blue and yellow quandiculas in order to facilitate the discontinuity between pieces and tray, the pieces used are black and white, additionally we included in the tray a green mark in order to be able to identify the orientation of the board, being this green mark also used by the user to know which side of the board in a possible version of human game vs computer, initially we had included more marks on the board in order to facilitate this task to The final version of the board is shown below.

The implementation of our project was divided into several phases:
  - Collect camera images and filter noise
  - Filter the various colors of interest
  - Identify quadrilaterals
  - Identify game board
  - Apply inverse deformation on the board playing area
  - Identify parts on the board
  - Graphically represent board

Collecting images from a camera in OpenCV is a relatively easy task being possible to do it using objects of the type VideoCapture, to filter noise we use method fastNlMeansDenoisingColored, however in the final version we concluded that the noise would be irrelevant to the operation of the program so this option is disabled by default.
To filter the various colors we need to identify the various components iterates over all the pixels of the image collected by the camera and we select all the pixels that have the same color as the one we are looking for, so we create a filterColor method that receives an image, and 3 colors a first being the color we are looking for the second one color with tolerance values ​​for the RGB components within which we can accept the pixel as being of the first color and a third that will be the color used to fill a new image with the same size of the original in which case the pixel is similar to the original image takes this value, otherwise it takes black value.

To facilitate operations and color representation, we also create the ColorRGB class. After detecting the various colors of interest, we merged the final result into an image as represented above.

Detection of squares We created the Static class SquareFinder and the class Quadrilater and Triangle as auxiliaries for processing the values found. For this task, we use one of the examples available in the OpenCV code repository housed in GitHub, we start by filtering by reduction which has been removed because it has no advantage in our case since we will always pass images with two to three colors with filtering previously done, then several threshold values are tested to pass the image to monochrome so that it is possible to do research of contours and then polynomial approximation of the same in order to identify groups of contours that form quadrilaterals.

Some changes were necessary to achieve effectiveness using this method. This method allows easy undercutting of the tray from the previously created white mask and the playing area therein by resorting to mixing the blue and yellow masks created in the previous step. The result obtained is represented in the figure below after identifying elements and removing all information external to the board.

After detecting the points for which the quadrilatero that represents the game zone in the 2D plane is simple to apply deformation to correct the camera's prespective distortion, it is possible to do the same using the getPrepectiveTransform method that creates a transformation matrix from 2 point vectors being the first our quadrilatero and the second a perfect square conscious with our image of said, the transformation is later applied by the method warpPrepective, the result is represented below.

To detect the position of the pieces in this tray already detected we chose to slice the image obtained in the previous step in 8x8 images and verify the amount of white and black in these images if there is more than 50% of one of these colors in the image we assume the existence of a piece in that space . This technique allows to identify parts position in cases where it would not be possible to do the same efficiently through shape detection, as is the case of the image above.

Finally, in order to orient the board in the right direction, we calculate the midpoint of the green open area and the midpoint of the quadrilateral of the board, and calculate the angle between these 2 points relative to normal, so that the orientation of the quadrilateral is correct , if it is not, it is done rearranging the points that make up the same and it is enough to correct the orientation of the board.

## Results

A aplicação final é capaz de detectar de forma eficaz localização de peças num tabuleiro de damas, existem apenas opções para calibração de cor e ligar/desligar remoção de ruido não existindo adicionalmente qualquer interação da parte do utilizador.
