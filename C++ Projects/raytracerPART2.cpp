#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <Eigen/Dense>
#include <cstdlib>
#include <cmath>

using namespace std;

struct point
{

  float verticeX;
  float verticeY;
  float verticeZ;
  float normalX;
  float normalY;
  float normalZ;
  
};


struct polygon
{

  int numberOfSides;
  vector<point> vertices;
  vector<float> fillVariables;
  
};

struct ray{

  Eigen::Vector3d originPoint;
  Eigen::Vector3d direction;

};

struct lightPosition{

  vector<point> positions;

};

struct finalPicture
{

  vector<polygon*> allPolygons;
  float R = 0.0;
  float G = 0.0;
  float B = 0.0;
  float fx;
  float fy;
  float fz;
  vector<float> fillVariables;
  vector<float> fillVariables2;
  vector<float> fillVariables3;
  vector<lightPosition> lightPositions;
  float ax;
  float ay;
  float az;
  float angle;
  float ux;
  float uy;
  float uz;
  float hither;
  int xres;
  int yres;
  int aspectRatio;
  int numLightPositions = 0;
    
};

finalPicture parse();

void writePPM(unsigned char pixels[512][512][3]);

int fieldOfView(finalPicture picture);

bool doesIntersect(double t, double gamma, double beta);

double calculateDeterminant(Eigen::Matrix3d matrix);

void shade(finalPicture picture);

int main(){

  finalPicture picture = parse(); // Calls the parse function which will get all the information from the nff file to create a finalPicture object.
  fieldOfView(picture); // Using the information from the parse function we can call the fieldOfView function.
  
  return 0;

}

double calculateDeterminant(Eigen::Matrix3d matrix){

  double determinant = matrix.determinant(); // Uses the determinant function to give us the determinant of a matrix object.

  return determinant;
  
}

bool doesIntersect(double t, double gamma, double beta){

  if (t < 0){ // If t is negative, it will not intersect, so we return false.

    return false;

  }

  if (gamma < 0 || gamma > 1){ // If gamma is negative or greater than 1, then we also return false.

    return false;

  }

  if (beta < 0 || beta > 1 - gamma){ // If beta is negative, or is greater than 1 - gamma we return false as it will not intersect.

    return false;

  }
  
  return true; // If we pass through and don't trigger any of the above clauses, we know it must intersect a polygon, and we return true.
  
}

int fieldOfView(finalPicture picture){

  const float pi = 3.141593;
  const Eigen::Vector3d bias(1e-6, 1e-6, 1e-6);
  // const int recursionLimit = 5;
  picture.angle*=(pi/180); // Converts the fov angle from degrees to radians.
  
  unsigned char pixels[512][512][3];
  Eigen::Vector3d from(picture.fx, picture.fy, picture.fz); // Creates the eye of the image from the nff file.
  Eigen::Vector3d at(picture.ax, picture.ay, picture.az); // Creates what the eye is looking at from the nff file.
  Eigen::Vector3d up(picture.ux, picture.uy, picture.uz); // Creates the up vector from the eye.

  Eigen::Vector3d w = (from - at).normalized(); // Creates the w vector.
  Eigen::Vector3d u = up.cross(w).normalized(); // Creates the u vector.
  Eigen::Vector3d v = w.cross(u); // Creates the v vector.

  picture.aspectRatio = picture.xres/picture.yres; // Sets the aspect ratio of the picture (for this project it will be 1.)
  float pixelWidth = 2*tan(picture.angle/2)/picture.xres; // Calculates the width of each pixel.
  
  for (int i = 0; i < picture.yres; i++){

    for (int j = 0; j < picture.xres; j++){ // Nested for loop to go through each pixel in the image.

      bool beenHit = false;
      bool beenHit2 = false;
      float minT = 100.00;
            
      ray ray1;
      ray1.originPoint = from;
      float p = -1 * tan(picture.angle/2) + (pixelWidth/2) + (i * pixelWidth);
      float q = (tan(picture.angle/2)) - (pixelWidth/2) - (j * pixelWidth);
      float r = -1;
      
      Eigen::Vector3d pixelDirection = (p * u) + (q * v) + (r * w); // Does the dot product of p q r and u v w to transfer us into world coordinates.
      
      ray1.direction = pixelDirection;
            
      for (long unsigned int k = 0; k < picture.allPolygons.size(); k++){ // We check every polygon to see if our ray intersects them.

	if (picture.allPolygons[k]->numberOfSides == 4){ // Checks if the polygon is a square, if so we will break it up into two triangles.
	  
          Eigen::Vector3d xa(picture.allPolygons[k]->vertices[0].verticeX, picture.allPolygons[k]->vertices[0].verticeY, picture.allPolygons[k]->vertices[0].verticeZ);
	  Eigen::Vector3d xb(picture.allPolygons[k]->vertices[1].verticeX, picture.allPolygons[k]->vertices[1].verticeY, picture.allPolygons[k]->vertices[1].verticeZ);                  
          Eigen::Vector3d xc(picture.allPolygons[k]->vertices[2].verticeX, picture.allPolygons[k]->vertices[2].verticeY, picture.allPolygons[k]->vertices[2].verticeZ);
          Eigen::Vector3d fourthSide(picture.allPolygons[k]->vertices[3].verticeX, picture.allPolygons[k]->vertices[3].verticeY, picture.allPolygons[k]->vertices[3].verticeZ);
          Eigen::Vector3d xd = pixelDirection;

	  xa.normalized();
	  xb.normalized();
	  xc.normalized();
	  fourthSide.normalized();
          Eigen::Matrix3d A; // Creates one triangle matrix.
          Eigen::Matrix3d B; // Creates the second triangle matrix.

          A << xb - xa, xc - xa, -xd; // Creates the matrix A, that we can use to get the values of gamma, beta and T.

          B << xc - fourthSide, xa - fourthSide, -xd; // Creates the matrix B, that we can use to get the values of gamma, beta and T.

          Eigen::Vector3d gammaBetaT(A.inverse()*(from - xa)); // Gives us a vector containing the values of gamma, beta, and T.

          Eigen::Vector3d gammaBetaTB(B.inverse()*(from - fourthSide)); // Gives us a vector containing the values of gamma, beta, and T.

          double finalGamma = gammaBetaT[0];
          double finalBeta = gammaBetaT[1];
          double finalT = gammaBetaT[2];

          double finalGammaB = gammaBetaTB[0];
          double finalBetaB = gammaBetaTB[1];
          double finalTB = gammaBetaTB[2];

	  if ((doesIntersect(finalT, finalGamma, finalBeta) == true && beenHit2 == false) || (doesIntersect(finalT, finalGamma, finalBeta) == true && finalT < minT)){ // If our ray intersects a triangle,or if we find a closer polygon,  we color it the specified color from the nff file.

            float finalR = 0;
            float finalG = 0;
            float finalB = 0;
	    
            float lightIntensity = 1/sqrt(picture.numLightPositions); // Light intensity equation.

	    for (int f = 0; f < picture.numLightPositions; f++){ // For each of the lights, calculate the diffuse, specular, and shadows.

              bool shadowed = false; // Initializes that a polygon is not in a shadow.
              Eigen::Vector3d intersectionPoint = (from + (gammaBetaT[2] * pixelDirection)); // Finds the intersectionPoint of the ray to the polygon.
              Eigen::Vector3d nVector = (xb - xa).cross(xc - xa); // Creates the nVector.

              Eigen::Vector3d lightVector(picture.lightPositions[f].positions[0].verticeX, picture.lightPositions[f].positions[0].verticeY, picture.lightPositions[f].positions[0].verticeZ); // Creates the light vector

              Eigen::Vector3d lVector = (lightVector - intersectionPoint).normalized(); // Creates the lVector.

              Eigen::Vector3d vVector = (from-intersectionPoint).normalized(); // Creates the vVector.

              long unsigned int counter = 0; // Works as an index for our while loop.

              intersectionPoint += bias; // Adds shadow bias to our intersection point so we don't get shadow acne.

              while (counter < picture.allPolygons.size() && shadowed == false){ // While a pixel isn't shadowed and we haven't gone through every polygon, check for a shadow.

                Eigen::Vector3d one(picture.allPolygons[counter]->vertices[0].verticeX, picture.allPolygons[counter]->vertices[0].verticeY, picture.allPolygons[counter]->vertices[0].verticeZ);
                Eigen::Vector3d two(picture.allPolygons[counter]->vertices[1].verticeX, picture.allPolygons[counter]->vertices[1].verticeY, picture.allPolygons[counter]->vertices[1].verticeZ);
                Eigen::Vector3d three(picture.allPolygons[counter]->vertices[2].verticeX, picture.allPolygons[counter]->vertices[2].verticeY, picture.allPolygons[counter]->vertices[2].verticeZ);
                Eigen::Vector3d four = lVector;

                Eigen::Matrix3d C; // Creates a matrix C, that we can use to find gamma, beta, and T.

                C << two - one, three - one, -four;

                Eigen::Vector3d gammaBetaTL(C.inverse()*(intersectionPoint - one)); // Gets the values of Gamma, Beta, and T from the C matrix.

                double finalGammaL = gammaBetaTL[0];
                double finalBetaL = gammaBetaTL[1];
                double finalTL = gammaBetaTL[2];

                shadowed = doesIntersect(finalTL, finalGammaL, finalBetaL); // We see if the pixel should be shadowed based on the intersection of the new matrix.
                counter++; // Increment index so we don't get an infinite loop.

              }
	    
	      nVector = nVector.normalized(); // Normalize the nVector.
	      
              
              Eigen::Vector3d hVector = (lVector + vVector)/((lVector + vVector).norm()); // Get the value of the hVector.

              hVector.normalized(); // Normalize the hVector.

              float zero = 0.0;
              float dotProduct = nVector.dot(lVector);
              float dotProduct2 = nVector.dot(hVector);
              float diffuse = max(zero, dotProduct); // Gets the diffuse value.

              float specular = pow(max(zero, dotProduct2), picture.allPolygons[k]->fillVariables[5]); // Gets the specular value.


	      if (shadowed == false){ // If the pixel is not in a shadow, give it all of the proper color.

                finalR += ((picture.allPolygons[k]->fillVariables[3] * picture.allPolygons[k]->fillVariables[0]*255*diffuse) + (picture.allPolygons[k]->fillVariables[4] * specular * 255)) * lightIntensity;
                finalG += ((picture.allPolygons[k]->fillVariables[3] * picture.allPolygons[k]->fillVariables[1]*255*diffuse) + (picture.allPolygons[k]->fillVariables[4] * specular * 255)) * lightIntensity;
                finalB += ((picture.allPolygons[k]->fillVariables[3] * picture.allPolygons[k]->fillVariables[2]*255*diffuse) + (picture.allPolygons[k]->fillVariables[4] * specular * 255)) * lightIntensity;

              }

              else{ // Otherwise, add just a black value to R, G, and B.

		finalR += 0;
                finalG += 0;
                finalB += 0;

              }

	    }
	    
	    if (finalR > 255){ // If finalR is greater than 255, make it 255 so it does not overflow.
		
	      finalR = 255;
	      
	    }
	    
	    pixels[j][i][0] = finalR; // Sets the pixel's R value.
	    pixels[j][i][1] = finalG; // Sets the pixel's G value.
	    pixels[j][i][2] = finalB; // Sets the pixel's B value.

	    beenHit = true; // Makes the hitrecord true.
	    minT = finalT; // Sets the minT to the current T.
	      
	  }

	  else if(beenHit2 != true && beenHit != true){ // Otherwise, we set it to the background color.
	    
            pixels[j][i][0] = picture.R*255;
            pixels[j][i][1] = picture.G*255;
            pixels[j][i][2] = picture.B*255;

	  }


	  if ((doesIntersect(finalTB, finalGammaB, finalBetaB) == true && beenHit2 == false) || (doesIntersect(finalTB, finalGammaB, finalBetaB) == true && finalT < minT)){ // If our ray intersects a triangle we color it the specified color from the nff file.

            float finalR = 0;
            float finalG = 0;
            float finalB = 0;

            float lightIntensity = 1/sqrt(picture.numLightPositions); // Gets the value of the lightIntensity.

            for (int f = 0; f < picture.numLightPositions; f++){ // For each of the lights, calculate the shadows and the color values.

	      bool shadowed = false; // Initalize the pixel's shadowed variable to false.
              Eigen::Vector3d intersectionPoint = (from + (gammaBetaTB[2] * pixelDirection)); // Gives us the intersectionPoint.
              Eigen::Vector3d nVector = (xa - fourthSide).cross(xc - fourthSide); // Gives us the nVector.

              Eigen::Vector3d lightVector(picture.lightPositions[f].positions[0].verticeX, picture.lightPositions[f].positions[0].verticeY, picture.lightPositions[f].positions[0].verticeZ); // Gives us the lightVector
	      
	      Eigen::Vector3d lVector = (lightVector - intersectionPoint).normalized(); // Gives us the lVector.
	      
	      Eigen::Vector3d vVector = (from-intersectionPoint).normalized(); // Gives us the vVector.
	      
	      long unsigned int counter = 0; // Sets a counter so we don't get an infinite loop.

	      intersectionPoint += bias; // Adds the shadow bias.
	      
              while (counter < picture.allPolygons.size() && shadowed == false){ // Looks for a shadow.

                Eigen::Vector3d one(picture.allPolygons[counter]->vertices[0].verticeX, picture.allPolygons[counter]->vertices[0].verticeY, picture.allPolygons[counter]->vertices[0].verticeZ);
                Eigen::Vector3d two(picture.allPolygons[counter]->vertices[1].verticeX, picture.allPolygons[counter]->vertices[1].verticeY, picture.allPolygons[counter]->vertices[1].verticeZ);
                Eigen::Vector3d three(picture.allPolygons[counter]->vertices[2].verticeX, picture.allPolygons[counter]->vertices[2].verticeY, picture.allPolygons[counter]->vertices[2].verticeZ);
                Eigen::Vector3d four = lVector;
		
                Eigen::Matrix3d C;
		
                C << two - one, three - one, -four; // Creates the C matrix.
		
		Eigen::Vector3d gammaBetaTL(C.inverse()*(intersectionPoint - one)); // Gives us the Gamma, Beta, and T values of the C matrix.

		double finalGammaL = gammaBetaTL[0];
                double finalBetaL = gammaBetaTL[1];
                double finalTL = gammaBetaTL[2];

                shadowed = doesIntersect(finalTL, finalGammaL, finalBetaL); // If the doesIntersect function returns true on the C matrix values, then we have a shadow.
                counter++; // Increment the index so that we don't have an infinite loop.

              }
	      
	      Eigen::Vector3d hVector = (lVector + vVector)/((lVector + vVector).norm()); // Creates the hVector.

	      nVector = nVector.normalized(); // Normalizes the nVector.
	      lVector = lVector.normalized(); // Normalizes the lVector.
              hVector = hVector.normalized(); // Normalizes the hVector.

              float zero = 0.0;
              float dotProduct = nVector.dot(lVector);
              float dotProduct2 = nVector.dot(hVector);
              float diffuse = max(zero, dotProduct); // Gives us the diffuse value.

	      float specular = pow(max(zero, dotProduct2), picture.allPolygons[k]->fillVariables[5]); // Gives us the specular value.

	      if (shadowed == false){ // If there is no shadow on the pixel, add all of the colors properly.

                finalR += ((picture.allPolygons[k]->fillVariables[3] * picture.allPolygons[k]->fillVariables[0]*255*diffuse) + (picture.allPolygons[k]->fillVariables[4] * specular * 255)) * lightIntensity;
                finalG += ((picture.allPolygons[k]->fillVariables[3] * picture.allPolygons[k]->fillVariables[1]*255*diffuse) + (picture.allPolygons[k]->fillVariables[4] * specular * 255)) * lightIntensity;
                finalB += ((picture.allPolygons[k]->fillVariables[3] * picture.allPolygons[k]->fillVariables[2]*255*diffuse) + (picture.allPolygons[k]->fillVariables[4] * specular * 255)) * lightIntensity;

              }

              else{ // Otherwise, add black values for R, G, and B.

		finalR += 0;
		finalG += 0;
		finalB += 0;

	      }

	    }

	    if (finalR > 255){ // If R is over 255, set it to 255 so we don't overflow.

              finalR = 255;

            }

	    pixels[j][i][0] = finalR; // Set the pixel's R value to the finalR.
            pixels[j][i][1] = finalG; // Set the pixel's G value to the finalG.
            pixels[j][i][2] = finalB; // Set the pixel's B value to the finalB.

            beenHit = true; // Set beenHit to true.
            minT = finalT; // Set the minT to the most recent T.

          }
	  
	  else if(beenHit2 != true && beenHit != true){ // Otherwise, we set it to the background color.

            pixels[j][i][0] = picture.R*255;
            pixels[j][i][1] = picture.G*255;
            pixels[j][i][2] = picture.B*255;

          }

        }

	else if (picture.allPolygons[k]->numberOfSides == 3){ // If we are dealing with the teapot/polygonal patches.

	  Eigen::Vector3d xa(picture.allPolygons[k]->vertices[0].verticeX, picture.allPolygons[k]->vertices[0].verticeY, picture.allPolygons[k]->vertices[0].verticeZ);
	  Eigen::Vector3d xb(picture.allPolygons[k]->vertices[1].verticeX, picture.allPolygons[k]->vertices[1].verticeY, picture.allPolygons[k]->vertices[1].verticeZ);
	  Eigen::Vector3d xc(picture.allPolygons[k]->vertices[2].verticeX, picture.allPolygons[k]->vertices[2].verticeY, picture.allPolygons[k]->vertices[2].verticeZ);
	  Eigen::Vector3d xd = pixelDirection;

	  Eigen::Matrix3d A;

	  A << xb - xa, xc - xa, -xd; // Creates the matrix A, that we can use to get the values of gamma, beta and T.

	  Eigen::Vector3d gammaBetaT(A.inverse()*(from - xa)); // Gives us a vector containing the values of gamma, beta, and T.

	  double finalGamma = gammaBetaT[0];
	  double finalBeta = gammaBetaT[1];
	  double finalT = gammaBetaT[2];

	  if ((doesIntersect(finalT, finalGamma, finalBeta) == true && beenHit == false) || (doesIntersect(finalT, finalGamma, finalBeta) == true && finalT < minT)){ // If our ray intersects a triangle, we color it the specified color from the nff file.

	    float finalR = 0;
	    float finalG = 0;
	    float finalB = 0;
	    
	    float lightIntensity = 1/sqrt(picture.numLightPositions);

	    for (int f = 0; f < picture.numLightPositions; f++){ // For each of the lights, look for a shadow and shade the pixel properly.

	      bool shadowed = false; // Initialize the shadowed variable to be false.
	      Eigen::Vector3d intersectionPoint = (from + (gammaBetaT[2] * pixelDirection)); // Makes the intersection point.
	      Eigen::Vector3d nVector = (xb - xa).cross(xc - xa); // Makes the nVector.
	      
	      Eigen::Vector3d lightVector(picture.lightPositions[f].positions[0].verticeX, picture.lightPositions[f].positions[0].verticeY, picture.lightPositions[f].positions[0].verticeZ); // Makes the lightVector
	      intersectionPoint += bias; // Adds the shadow bias to the intersection point.
	      
	      Eigen::Vector3d lVector = (lightVector - intersectionPoint).normalized(); // Creates the lVector.

	      Eigen::Vector3d vVector = (from - intersectionPoint).normalized(); // Creates the vVector.
	      
	      long unsigned int counter = 0; // Creates an index so we don't have an infinite loop.

	      while (counter < picture.allPolygons.size() && shadowed == false){ // While we don't find a shadow, and we haven't gone through all the polygons, look for a shadow.
		
		Eigen::Vector3d one(picture.allPolygons[counter]->vertices[0].verticeX, picture.allPolygons[counter]->vertices[0].verticeY, picture.allPolygons[counter]->vertices[0].verticeZ);
		Eigen::Vector3d two(picture.allPolygons[counter]->vertices[1].verticeX, picture.allPolygons[counter]->vertices[1].verticeY, picture.allPolygons[counter]->vertices[1].verticeZ);
		Eigen::Vector3d three(picture.allPolygons[counter]->vertices[2].verticeX, picture.allPolygons[counter]->vertices[2].verticeY, picture.allPolygons[counter]->vertices[2].verticeZ);
		Eigen::Vector3d four = lVector;

		Eigen::Matrix3d C;
	      
		C << two - one, three - one, -four; // Creates the matrix C, that we can use to get the values of gamma, beta and T.
	      
		Eigen::Vector3d gammaBetaTL(C.inverse()*(intersectionPoint - one)); // Gives us a vector containing the values of gamma, beta, and T.
	      
		double finalGammaL = gammaBetaTL[0];
		double finalBetaL = gammaBetaTL[1];
		double finalTL = gammaBetaTL[2];

		shadowed = doesIntersect(finalTL, finalGammaL, finalBetaL); // Checks if we get a shadow by calling the doesIntersect function on the new C matrix.
		counter++; // Increments the index so that we don't get an infinite loop.
		
	      }

	      nVector = nVector.normalized(); // Normalizes the nVector.
	      	      
	      Eigen::Vector3d hVector = (lVector + vVector)/((lVector + vVector).norm()); // Creates the hVector.

	      hVector.normalized(); // Normalizes the hVector.
	      
	      float zero = 0.0;
	      float dotProduct = nVector.dot(lVector);
	      float dotProduct2 = nVector.dot(hVector);
	      float diffuse = max(zero, dotProduct); // Gives the diffuse value.
	      
	      float specular = pow(max(zero, dotProduct2), picture.fillVariables3[5]); // Gives the specular value.

	      if (shadowed == false){ // If there are no shadows, add all the proper values.

		cout << diffuse << " " << specular << endl;
	       
		finalR += ((picture.fillVariables3[3] * picture.fillVariables3[0]*255*diffuse) + (picture.fillVariables3[4] * specular * 255)) * lightIntensity;
		finalG += ((picture.fillVariables3[3] * picture.fillVariables3[1]*255*diffuse) + (picture.fillVariables3[4] * specular * 255)) * lightIntensity;
		finalB += ((picture.fillVariables3[3] * picture.fillVariables3[2]*255*diffuse) + (picture.fillVariables3[4] * specular * 255)) * lightIntensity;
	      
	      }

	      else{ // Otherwise, add black values to R, G, and B.

		finalR += 0;
		finalG += 0;
		finalB += 0;

	      }
	      
	      if (finalR > 255){ // If the finalR value is greater than 255, set it to 255, so we don't get any overflow.

		finalR = 255;

	      }
	    
	      pixels[j][i][0] = finalR; // Set the pixel's red value to the finalR value.
	      pixels[j][i][1] = finalG; // Set the pixel's green value to the finalG value.
	      pixels[j][i][2] = finalB; // Set the pixel's blue value to the finalB value.
	    
	      beenHit = true; // Set beenHit to true so we know it has been hit.
	      minT = finalT; // Set minT to the finalT as it is the closest polygon we've found.
	    
	    }

	  }

	  else if(beenHit != true && beenHit2 != true){ // Otherwise, we set it to the background color.

	    pixels[j][i][0] = picture.R*255;
	    pixels[j][i][1] = picture.G*255;
	    pixels[j][i][2] = picture.B*255;
	      
	  }

	}

      }

    }

  }

  writePPM(pixels); // Writes the file.

  while(!picture.allPolygons.empty()){ // Deallocates allocated memory.

    delete picture.allPolygons.back();
    picture.allPolygons.pop_back();
    
  }

  return 0;

}

finalPicture parse(){
  
  FILE* imageFile;
  char line[100];
  char trash;
  char trash2;
  char trash3;
  char trash4;
  char trash5;
  char trash6;
  char trash7;
  char trash8;
  char trash9;
  char trash10;
  int numberOfSides;
  finalPicture picture;

  imageFile = fopen("teapot-3.nff", "r");

  if (imageFile != NULL){ // If the imageFile exists.

    while (fgets(line, 50, imageFile) != NULL){ // As long as the line isn't empty we keep reading the file.
	
      if(line[0] == 'b'){ // If the line starts with the character b, we scan for the background color.

	sscanf(line, "%s %g %g %g", &trash, &picture.R, &picture.G, &picture.B);

      }

      if(line[0] == 'f'){ // If the line starts with f we check for the second character, and depending on the second character we do two different things.

	if(line[1] == 'r'){ // If the line's second character is an r we get the from coordinates.

	  sscanf(line, "%s %g %g %g", &trash2, &picture.fx, &picture.fy, &picture.fz);

	}

	else{ // Otherwise, we get the fill color and other variables.

	  if (picture.fillVariables.size() == 0){ // If the first fillVariables variable is empty, put the values in there.

	    picture.fillVariables.resize(7);
	    
	    sscanf(line, "%s %g %g %g %g %g %g %g %g", &trash3, &picture.fillVariables[0], &picture.fillVariables[1], &picture.fillVariables[2], &picture.fillVariables[3], &picture.fillVariables[4], &picture.fillVariables[5], &picture.fillVariables[6], &picture.fillVariables[7]);

	  }

	  else if (picture.fillVariables2.size() == 0){ // If the second fillVariables variable is empty, put the values in there.

	    picture.fillVariables2.resize(7);

	    sscanf(line, "%s %g %g %g %g %g %g %g %g", &trash3, &picture.fillVariables2[0], &picture.fillVariables2[1], &picture.fillVariables2[2], &picture.fillVariables2[3], &picture.fillVariables2[4], &picture.fillVariables2[5], &picture.fillVariables2[6], &picture.fillVariables2[7]);
	  
	  }

	  else{ // Otherwise, put the fillVariables in fillVariables3.

	    picture.fillVariables3.resize(7);

            sscanf(line, "%s %g %g %g %g %g %g %g %g", &trash3, &picture.fillVariables3[0], &picture.fillVariables3[1], &picture.fillVariables3[2], &picture.fillVariables3[3], &picture.fillVariables3[4], &picture.fillVariables3[5], &picture.fillVariables3[6], &picture.fillVariables3[7]);

	  }

	}

      }

      else if(line[0] == 'a'){ // If the first character in the line is an a, we check if the second character is a t, and if it isn't we take the angle.

	if(line[1] == 't'){ // We get the values for the at coordinates.
	  
	  sscanf(line, "%s %g %g %g", &trash4, &picture.ax, &picture.ay, &picture.az);
	    
	}

	else{ // We get the angle value.

	  sscanf(line, "%s %g", &trash5, &picture.angle);

	}

      }

      else if(line[0] == 'u'){ // If the first character in the line is a u, we get the coordinates for the up value.

	sscanf(line, "%s %g %g %g", &trash6, &picture.ux, &picture.uy, &picture.uz);
       
      }

      else if (line[0] == 'h'){ // If the first character in the line is an h, we get the value for the hither.

	sscanf(line, "%s %g", &trash7, &picture.hither);

      }

      else if (line[0] == 'r'){ // If the first character in the line is an r, we get the resolution values.

	sscanf(line, "%s %d %d", &trash8, &picture.xres, &picture.yres);

      }

      else if (line[0] == 'p'){ // If the first character in the line is a p, we create a new polygon based on the information in the nff file.

	if (line[1] == 'p'){

	  sscanf(line, "%s %d", &trash9, &numberOfSides);
	  polygon* newPolygon2 = new(polygon);
	  newPolygon2->numberOfSides = numberOfSides;
	  newPolygon2->vertices.resize(numberOfSides);

	  for (int i = 0; i < newPolygon2->numberOfSides; i++){

	    if (fgets(line, 100, imageFile) != NULL){

	      sscanf(line, "%g %g %g %g %g %g", &newPolygon2->vertices[i].verticeX, &newPolygon2->vertices[i].verticeY, &newPolygon2->vertices[i].verticeZ, &newPolygon2->vertices[i].normalX, &newPolygon2->vertices[i].normalY, &newPolygon2->vertices[i].normalZ);

	    }

	  }

	  picture.allPolygons.push_back(newPolygon2);

	}

	else{
	  
	  sscanf(line, "%s %d", &trash9, &numberOfSides);
	  polygon* newPolygon = new(polygon); // Creates a new polygon.
	  newPolygon->numberOfSides = numberOfSides;
	  newPolygon->vertices.resize(numberOfSides);

	  if (picture.fillVariables2.size() == 0){ // If fillVariables2 has nothing in it, set the polygon's fillVariables variable to the fillVariables variable.

	    newPolygon->fillVariables = picture.fillVariables;

	  }

	  else{ // If fillVariables2 does have values in it, set the polygon's fillVariables variable to the fillVariables2 variable.

	    newPolygon->fillVariables = picture.fillVariables2;

	  }

	  for (int i = 0; i < newPolygon->numberOfSides; i++){ // For all of the sides in the polygon create vertices.

	    if (fgets(line, 100, imageFile) != NULL){

	      sscanf(line, "%g %g %g", &newPolygon->vertices[i].verticeX, &newPolygon->vertices[i].verticeY, &newPolygon->vertices[i].verticeZ);

	    }

	  }

	  picture.allPolygons.push_back(newPolygon); // Put the newPolygon in the allPolygons vector.

	}

      }

      else if (line[0] == 'l'){ // If the first character in the line is l, we need to get the lightPosition.

	if (picture.numLightPositions == 0){ // If the number of lightPositions is 0, resize the vector for lightPositions.

	  
	  picture.lightPositions.resize(1);
	  	  
	  point newPoint;
	  sscanf(line, "%s %g %g %g", &trash10, &newPoint.verticeX, &newPoint.verticeY, &newPoint.verticeZ);

	  picture.lightPositions[0].positions.push_back(newPoint); // Add the point to the lightPositions vector.

	}

	else{ // Otherwise, do the same, but for the next lightPosition in the lightPosition vector.

	  picture.lightPositions.resize(2);

	  point newPoint;
	  
	  sscanf(line, "%s %g %g %g", &trash9, &newPoint.verticeX, &newPoint.verticeY, &newPoint.verticeZ);

	  picture.lightPositions[1].positions.push_back(newPoint);

	}

	picture.numLightPositions++; // Increments number of lightPositions.

      }

    }

    fclose(imageFile);
    
  }

  else{ // If we couldn't read the file, cout that we couldn't and end this program.

    cout << "Error reading file." << endl;

  }

  return picture;

}

void writePPM(unsigned char pixels[512][512][3]){

  FILE *f = fopen("final.ppm", "wb");
  fprintf(f, "P6\n%d %d\n%d\n", 512, 512, 255);
  fwrite(pixels, 1, 512*512*3, f);
  fclose(f);

}
  
