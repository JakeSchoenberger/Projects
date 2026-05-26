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

  Eigen::Vector3f coordinates;
  Eigen::Vector3f normalCoordinates;
  float normalX;
  float normalY;
  float normalZ;


  Eigen::Vector3f color;
  float red;
  float green;
  float blue;

};

struct polygon
{

  int numberOfSides;
  vector<point> vertices;
  vector<float> fillVariables;
  int trueNumberOfSides;
  
};

struct lightPosition{

  vector<point> positions;

};

struct fragment{

  Eigen::Vector3f color;
  Eigen::Vector3f normal;
  Eigen::Vector3f worldSpace;
    
  float zValue;
  int whatPolygon;
    
};

struct finalPicture
{

  vector<polygon*> allPolygons;
  float R;
  float G;
  float B;
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
  unsigned char pixels[512][512][3];

};

finalPicture parse();

finalPicture rasterize(finalPicture * picture);

void writePPM(unsigned char pixels[512][512][3]);

int main(){

  finalPicture picture = parse();

  for (int i = 0; i < picture.yres; i++){ // Sets all of the pixels' initial color values to the background color values.

    for (int j = 0; j < picture.xres; j++){

      picture.pixels[j][i][0] = picture.R * 255;
      picture.pixels[j][i][1] = picture.G * 255;
      picture.pixels[j][i][2] = picture.B * 255;

    }

  }
    
  picture = rasterize(&picture);
  writePPM(picture.pixels);

  while(!picture.allPolygons.empty()){ // Deallocates allocated memory.

    delete picture.allPolygons.back();
    picture.allPolygons.pop_back();

  }
  
  return 0;

}

finalPicture rasterize(finalPicture * picture){

  Eigen::Vector3f from(picture->fx, picture->fy, picture->fz); // Creates the eye of the image from the nff file.
  Eigen::Vector3f at(picture->ax, picture->ay, picture->az); // Creates what the eye is looking at from the nff file.
  Eigen::Vector3f up(picture->ux, picture->uy, picture->uz); // Creates the up vector from the eye.

  picture->aspectRatio = picture->xres/picture->yres;

  float converter = 3.14159265 / 180;
  float theta = picture->angle / 2;
  theta = theta * converter;
  theta = tan(theta); // Converts the angle to degrees from radians.

  // The below values are necessary for setting up our matrix.
  
  float l = theta;
  float r = -theta;
  float t = theta/picture->aspectRatio;
  float b = -theta/picture->aspectRatio;
  float n = picture->hither;
  float f = n*1000;
  float zero = 0.0;
  float one = 1.0;

  Eigen::Vector3f w = (from - at).normalized(); // Creates the w vector.
  Eigen::Vector3f u = up.cross(w).normalized(); // Creates the u vector.
  Eigen::Vector3f v = w.cross(u); // Creates the v vector.

  fragment * listOfFragments[512][512]; // Creates a list of fragments that we will use to find the closest one to change the color of the pixels array.

  for (int g = 0; g < 512; g++){ // For everything in the listOfFragments list, initialize it to a nullpointer.

    for (int h = 0; h < 512; h++){

      listOfFragments[g][h] = NULL;

    }

  }
  
  for (unsigned long i = 0; i < picture->allPolygons.size(); i++){ // For all of the polygons...

    Eigen::Matrix4f mvp{ // Creates the mvp matrix.

      {float(picture->xres/2), zero, zero, float((float(picture->xres) - one)/float(2))},
      {zero, float(picture->yres/2), zero, float((float(picture->yres) - one)/float(2))},
      {zero, zero, one, zero},
      {zero, zero, zero, one}

    };

    cout << "MVP \n" << mvp << endl;

    Eigen::Matrix4f morth{ // Creates the morth matrix.

      {2/(r - l), zero, zero, -((r + l)/(r - l))},
      {zero, 2/(t - b), zero, -((t + b)/(t - b))},
      {zero, zero, 2/(n - f), -((n + f)/(n - f))},
      {zero, zero, zero, one}

    };

    Eigen::Matrix4f mCam1{

      {float(u[0]), float(u[1]), float(u[2]), zero},
      {float(v[0]), float(v[1]), float(v[2]), zero},
      {float(w[0]), float(w[1]), float(w[2]), zero},
      {zero, zero, zero, one}

    };

    Eigen::Matrix4f mCam2{

      {one, zero, zero, float(-from[0])},
      {zero, one, zero, float(-from[1])},
      {zero, zero, one, float(-from[2])},
      {zero, zero, zero, one}

    };
    
    Eigen::Matrix4f mCamFinal = mCam1 * mCam2; // Creates the mCam matrix.

    cout << "Cam \n" << mCamFinal << endl;
    Eigen::Matrix4f mPer{ // Creates the mPer matrix.

      {(2*n) / (r - l), 0, (l + r) / (l - r), 0},
      {0, -(2 * n)/(b - t), (b + t) / (b - t), 0},
      {0, 0, (f + n)/(n - f), -(2 * f * n) / (f - n)},
      {0, 0, 1, 0}

  };

    cout << "MPer \n" << mPer << endl;
    
    Eigen::Matrix4f bigMatrix = mvp * mPer * mCamFinal; // Creates the transformation matrix.

    cout << "Matrix: \n" << bigMatrix << endl;
    
    if (picture->allPolygons[i]->numberOfSides == 3){ // Goes through all of the polygons.

      Eigen::Vector4f v0(picture->allPolygons[i]->vertices[0].coordinates[0], picture->allPolygons[i]->vertices[0].coordinates[1], picture->allPolygons[i]->vertices[0].coordinates[2], 1); // Creates the first vertex.

      Eigen::Vector4f v1(picture->allPolygons[i]->vertices[1].coordinates[0], picture->allPolygons[i]->vertices[1].coordinates[1], picture->allPolygons[i]->vertices[1].coordinates[2], 1); // Creates the second vertex.
      
      Eigen::Vector4f v2(picture->allPolygons[i]->vertices[2].coordinates[0], picture->allPolygons[i]->vertices[2].coordinates[1], picture->allPolygons[i]->vertices[2].coordinates[2], 1); // Creates the third vertex.

      Eigen::Vector4f v0Prime = bigMatrix * v0;

      v0Prime /= v0Prime[3]; // Transforms the vertex.
      
      Eigen::Vector4f v1Prime = bigMatrix * v1;

      v1Prime /= v1Prime[3]; // Transforms the vertex.

      Eigen::Vector4f v2Prime = bigMatrix * v2;

      v2Prime /= v2Prime[3]; // Transforms the vertex.

      // The below 8 lines of code calculate the bounding box for the polygon.

      int xMin = min(int(v0Prime[0]), int(v1Prime[0]));
      xMin = min(xMin, int(v2Prime[0]));
      
      int xMax = max(int(v0Prime[0]), int(v1Prime[0]));
      xMax = max(xMax, int(v2Prime[0]));

      int yMin = min(int(v0Prime[1]), int(v1Prime[1]));
      yMin = min(yMin, int(v2Prime[1]));
      
      int yMax = max(int(v0Prime[1]), int(v1Prime[1]));
      yMax = max(yMax, int(v2Prime[1]));

      Eigen::Vector2f v0Final(v0Prime[0], v0Prime[1]);
      Eigen::Vector2f v1Final(v1Prime[0], v1Prime[1]);
      Eigen::Vector2f v2Final(v2Prime[0], v2Prime[1]);

      // The below 6 lines of code are going to be used to calculate alpha, beta, and gamma.

      float fAlpha = ((v1Prime[1] - v2Prime[1]) * v0Prime[0]) + ((v2Prime[0] - v1Prime[0]) * v0Prime[1]) + ((v1Prime[0] * v2Prime[1]) - (v2Prime[0] * v1Prime[1]));
      float fBeta = ((v2Prime[1] - v0Prime[1]) * v1Prime[0]) + ((v0Prime[0] - v2Prime[0]) * v1Prime[1]) + ((v2Prime[0] * v0Prime[1]) - (v0Prime[0] * v2Prime[1]));
      float fGamma = ((v0Prime[1] - v1Prime[1]) * v2Prime[0]) + ((v1Prime[0] - v0Prime[0]) * v2Prime[1]) + ((v0Prime[0] * v1Prime[1]) - (v1Prime[0] * v0Prime[1]));

      float twelveNegativeOne = ((v1Prime[1] - v2Prime[1]) * -1) + ((v2Prime[0] - v1Prime[0]) * -1) + ((v1Prime[0] * v2Prime[1]) - (v2Prime[0] * v1Prime[1]));
      float twentyNegativeOne = ((v2Prime[1] - v0Prime[1]) * -1) + ((v0Prime[0] - v2Prime[0]) * -1) + ((v2Prime[0] * v0Prime[1]) - (v0Prime[0] * v2Prime[1]));
      float oneNegativeOne = ((v0Prime[1] - v1Prime[1]) * -1) + ((v1Prime[0] - v0Prime[0]) * -1) + ((v0Prime[0] * v1Prime[1]) - (v1Prime[0] * v0Prime[1]));

      for (int j = xMin; j <= xMax; j++){ // For every pixel in the bounding box...

	for (int k = yMin; k <= yMax; k++){

	  if ((k < 512 && k >= 0) && (j < 512 && j >= 0)){ // Makes sure the value is within the bounds of our final image.

	    float alpha = (((v1Final[1] - v2Final[1]) * j) + ((v2Final[0] - v1Final[0]) * k) + ((v1Final[0] * v2Final[1]) - (v2Final[0] * v1Final[1]))) / fAlpha; // Calculates alpha.
	    float beta = (((v2Final[1] - v0Final[1]) * j) + ((v0Final[0] - v2Final[0]) * k) + ((v2Final[0] * v0Final[1]) - (v0Final[0] * v2Final[1]))) / fBeta; // Calculates beta.
	    float gamma = (((v0Final[1] - v1Final[1]) * j) + ((v1Final[0] - v0Final[0]) * k) + ((v0Final[0] * v1Final[1]) - (v1Final[0] * v0Final[1]))) / fGamma; // Calculates gamma.
  
	      if ((alpha >= 0 && alpha <= 1) && (beta >= 0 && beta <= 1) && (gamma >= 0 && gamma <= 1)){

		if ((alpha > 0 || (fAlpha * twelveNegativeOne) > 0) && (beta > 0 || (fBeta * twentyNegativeOne) > 0) && (gamma > 0 || (fGamma * oneNegativeOne > 0))){ // If the pixel is within the polygon...

		  fragment * fragment1 = new fragment(); // Creates a new fragment.
		  
		  fragment1->worldSpace = (alpha * picture->allPolygons[i]->vertices[0].coordinates) + (beta * picture->allPolygons[i]->vertices[1].coordinates) + (gamma * picture->allPolygons[i]->vertices[2].coordinates); // Gives the fragment a pixel value in worldspace.

		  fragment1->whatPolygon = i; // Sets the polygon number of the fragment to the current polygon.

		  // The below nine lines of code are used to set the color of the vertices.
		  
		  picture->allPolygons[i]->vertices[0].color[0] = picture->allPolygons[i]->fillVariables[0];
		  picture->allPolygons[i]->vertices[0].color[1] = picture->allPolygons[i]->fillVariables[1];
		  picture->allPolygons[i]->vertices[0].color[2] = picture->allPolygons[i]->fillVariables[2];
		  
		  picture->allPolygons[i]->vertices[1].color[0] = picture->allPolygons[i]->fillVariables[0];
		  picture->allPolygons[i]->vertices[1].color[1] = picture->allPolygons[i]->fillVariables[1];
		  picture->allPolygons[i]->vertices[1].color[2] = picture->allPolygons[i]->fillVariables[2];
		  
		  picture->allPolygons[i]->vertices[2].color[0] = picture->allPolygons[i]->fillVariables[0];
		  picture->allPolygons[i]->vertices[2].color[1] = picture->allPolygons[i]->fillVariables[1];
		  picture->allPolygons[i]->vertices[2].color[2] = picture->allPolygons[i]->fillVariables[2];

		  fragment1->color = (alpha * picture->allPolygons[i]->vertices[0].color) + (beta * picture->allPolygons[i]->vertices[1].color) + (gamma * picture->allPolygons[i]->vertices[2].color); // Sets the color of the fragment.

		  fragment1->normal = (alpha * picture->allPolygons[i]->vertices[0].normalCoordinates) + (beta * picture->allPolygons[i]->vertices[1].normalCoordinates) + (gamma * picture->allPolygons[i]->vertices[2].normalCoordinates); // Interpolating the fragment normal.

		  // The below four lines of code are used to calculate the Z-value which we will use to determine the closest fragment.
		  
		  Eigen::Vector3f newV0Prime(v0Prime[0], v0Prime[1], v0Prime[2]);
		  Eigen::Vector3f newV1Prime(v1Prime[0], v1Prime[1], v1Prime[2]);
		  Eigen::Vector3f newV2Prime(v2Prime[0], v2Prime[1], v2Prime[2]);
		  
		  Eigen::Vector3f interpolated = (alpha * newV0Prime) + (beta * newV1Prime) + (gamma * newV2Prime);

		  fragment1->zValue = interpolated[2];

		  if (listOfFragments[k][j] == NULL){ // If there is nothing at this pixel, set the fragment to be the fragment we have created.

		    listOfFragments[k][j] = fragment1;

		  }

		  
		  else if (listOfFragments[k][j]->zValue < fragment1->zValue){ // If there is something at the pixel, and it is farther away than our current fragment, set the fragment at this pixel to our current fragment.

		    listOfFragments[k][j] = fragment1;

		  }
		  
		}
		
	      }

	  }
	  
	}
	
      }

    }

  }

  
  for (int o = 0; o < picture->yres; o++){ // For every pixel...

    for (int m = 0; m < picture->xres; m++){

      if (listOfFragments[m][o] != NULL){ // If this pixel has a fragment in it.

	float finalR = 0;
	float finalG = 0;
	float finalB = 0;

	Eigen::Vector3f nVector;
	
	if (picture->allPolygons[listOfFragments[m][o]->whatPolygon]->trueNumberOfSides == 4){ // If the polygon is a part of the checkerboard, calculate the nVector NOT using normals.

	  Eigen::Vector3f xa(picture->allPolygons[listOfFragments[m][o]->whatPolygon]->vertices[0].coordinates[0], picture->allPolygons[listOfFragments[m][o]->whatPolygon]->vertices[0].coordinates[1], picture->allPolygons[listOfFragments[m][o]->whatPolygon]->vertices[0].coordinates[2]);
	  Eigen::Vector3f xb(picture->allPolygons[listOfFragments[m][o]->whatPolygon]->vertices[1].coordinates[0], picture->allPolygons[listOfFragments[m][o]->whatPolygon]->vertices[1].coordinates[1], picture->allPolygons[listOfFragments[m][o]->whatPolygon]->vertices[1].coordinates[2]);
	  Eigen::Vector3f xc(picture->allPolygons[listOfFragments[m][o]->whatPolygon]->vertices[2].coordinates[0], picture->allPolygons[listOfFragments[m][o]->whatPolygon]->vertices[2].coordinates[1], picture->allPolygons[listOfFragments[m][o]->whatPolygon]->vertices[2].coordinates[2]);

	  nVector = (xb - xa).cross(xc - xa);

	}

	else{ // Otherwise, if it is a part of the teapot, use the interpolated normal.

	  nVector = listOfFragments[m][o]->normal;

	}
	
	Eigen::Vector3f intersectionPoint = listOfFragments[m][o]->worldSpace;
	
	for (int v = 0; v < picture->numLightPositions; v++){ // For all of the lights...
	  
	  Eigen::Vector3f lightVector(picture->lightPositions[v].positions[0].coordinates[0], picture->lightPositions[v].positions[0].coordinates[1], picture->lightPositions[v].positions[0].coordinates[2]);
	  
	  Eigen::Vector3f lVector = (lightVector - intersectionPoint).normalized();
	  
	  Eigen::Vector3f vVector = (from - intersectionPoint).normalized();

	  nVector = nVector.normalized();

	  Eigen::Vector3f hVector = (lVector + vVector)/((lVector + vVector).norm());

	  hVector = hVector.normalized();
	  
	  float dotProduct = nVector.dot(lVector);
	  float dotProduct2 = nVector.dot(hVector);
	  float diffuse = max(zero, dotProduct); // Gives the diffuse value.
	  
	  float specular = pow(max(zero, dotProduct2), picture->allPolygons[listOfFragments[m][o]->whatPolygon]->fillVariables[5]); // Gives the specular value.
	  
	  float lightIntensity = 1/sqrt(picture->numLightPositions); // Gives the lightIntensity value.

	  finalR += ((picture->allPolygons[listOfFragments[m][o]->whatPolygon]->fillVariables[3] * listOfFragments[m][o]->color[0] * 255 * diffuse) + (picture->allPolygons[listOfFragments[m][o]->whatPolygon]->fillVariables[4] * 255 * specular)) * lightIntensity;
	  finalG += ((picture->allPolygons[listOfFragments[m][o]->whatPolygon]->fillVariables[3] * listOfFragments[m][o]->color[1] * 255 * diffuse) + (picture->allPolygons[listOfFragments[m][o]->whatPolygon]->fillVariables[4] * 255 * specular)) * lightIntensity;
	  finalB += ((picture->allPolygons[listOfFragments[m][o]->whatPolygon]->fillVariables[3] * listOfFragments[m][o]->color[2] * 255 * diffuse) + (picture->allPolygons[listOfFragments[m][o]->whatPolygon]->fillVariables[4] * 255 * specular)) * lightIntensity;
	  
	}
	
	if (finalR > 255){ // Checks for overflow.

	  finalR = 255;

	}

	else if(finalR < 0){

	  finalR = 0;

	}

	if (finalG > 255){ // Checks for overflow.

	  finalG = 255;

	}

	else if (finalG < 0){

	  finalG = 0;

	}

	if (finalB > 255){ // Checks for overflow.

	  finalB = 255;

	}

	else if (finalB < 0){

	  finalB = 0;

	}
	
	
	picture->pixels[m][o][0] = finalR ; // Sets the pixel's red value.
	picture->pixels[m][o][1] = finalG ; // Sets the pixel's green value.
	picture->pixels[m][o][2] = finalB ; // Sets the pixel's blue value.
	

      }
      
    }

  }

  for (int u = 0; u < 512; u++){ // For all of the fragments created in the listOfFragments delete them.

    for (int r = 0; r < 512; r++){

      delete listOfFragments[u][r];

    }

  }

  return *picture;
  
}




finalPicture parse(){

  FILE* imageFile;
  char line[100];
  char trash;
  int numberOfSides;
  finalPicture picture;

  imageFile = fopen("teapot-3.nff", "r");

  if (imageFile != NULL){

    while (fgets(line, 100, imageFile) != NULL){

      if(line[0] == 'b'){

	sscanf(line, "%s %g %g %g", &trash, &picture.R, &picture.G, &picture.B);

      }

      if(line[0] == 'f'){

	if(line[1] == 'r'){

	  sscanf(line, "%s %g %g %g", &trash, &picture.fx, &picture.fy, &picture.fz);

	}

	else{

	  if (picture.fillVariables.size() == 0){

	    picture.fillVariables.resize(7);

	    sscanf(line, "%s %g %g %g %g %g %g %g %g", &trash, &picture.fillVariables[0], &picture.fillVariables[1], &picture.fillVariables[2], &picture.fillVariables[3], &picture.fillVariables[4], &picture.
fillVariables[5], &picture.fillVariables[6], &picture.fillVariables[7]);

	  }

	  else if (picture.fillVariables2.size() == 0){

	     picture.fillVariables2.resize(7);

	     sscanf(line, "%s %g %g %g %g %g %g %g %g", &trash, &picture.fillVariables2[0], &picture.fillVariables2[1], &picture.fillVariables2[2], &picture.fillVariables2[3], &picture.fillVariables2[4], &picture.fillVariables2[5], &picture.fillVariables2[6], &picture.fillVariables2[7]);

	  }

	  else{ // Otherwise, put the fillVariables in fillVariables3.

            picture.fillVariables3.resize(7);

            sscanf(line, "%s %g %g %g %g %g %g %g %g", &trash, &picture.fillVariables3[0], &picture.fillVariables3[1], &picture.fillVariables3[2], &picture.fillVariables3[3], &picture.fillVariables3[4], &picture.fillVariables3[5], &picture.fillVariables3[6], &picture.fillVariables3[7]);

          }

	}

      }

      else if(line[0] == 'a'){

	if(line[1] == 't'){

	  sscanf(line, "%s %g %g %g", &trash, &picture.ax, &picture.ay, &picture.az);

	}

	else{

	  sscanf(line, "%s %g", &trash, &picture.angle);

	}

      }

      else if(line[0] == 'u'){

	sscanf(line, "%s %g %g %g", &trash, &picture.ux, &picture.uy, &picture.uz);

      }

      else if(line[0] == 'h'){

	sscanf(line, "%s %g", &trash, &picture.hither);

      }

      else if(line[0] == 'r'){

	sscanf(line, "%s %d %d", &trash, &picture.xres, &picture.yres);

      }

      else if(line[0] == 'p'){

	if(line[1] == 'p'){

	  sscanf(line, "%s %d", &trash, &numberOfSides);
	  polygon* newPolygon = new(polygon);
	  newPolygon->numberOfSides = numberOfSides;
	  newPolygon->vertices.resize(numberOfSides);
	  
	  for (int i = 0; i < newPolygon->numberOfSides; i++){

	    if(fgets(line, 100, imageFile) != NULL){

	      sscanf(line, "%g %g %g %g %g %g", &newPolygon->vertices[i].coordinates[0], &newPolygon->vertices[i].coordinates[1], &newPolygon->vertices[i].coordinates[2], &newPolygon->vertices[i].normalCoordinates[0], &newPolygon->vertices[i].normalCoordinates[1], &newPolygon->vertices[i].normalCoordinates[2]);

	    }

	  }

	  newPolygon->fillVariables = picture.fillVariables3;

	  picture.allPolygons.push_back(newPolygon);
	  
	}

	else{

	  sscanf(line, "%s %d", &trash, &numberOfSides);
          polygon* newPolygon1 = new(polygon);
	  polygon* newPolygon2 = new(polygon); // Creates a new polygon.
          newPolygon1->numberOfSides = 3;
          newPolygon1->vertices.resize(3);
	  newPolygon1->trueNumberOfSides = 4;
	  newPolygon2->numberOfSides = 3;
	  newPolygon2->vertices.resize(3);
	  newPolygon2->trueNumberOfSides = 4;

	  if (picture.fillVariables2.size() == 0){ // If fillVariables2 has nothing in it, set the polygon's fillVariables variable to the fillVariables variable.

            newPolygon1->fillVariables = picture.fillVariables;
	    newPolygon2->fillVariables = picture.fillVariables;
	    
          }

          else{ // If fillVariables2 does have values in it, set the polygon's fillVariables variable to the fillVariables2 variable.

            newPolygon1->fillVariables = picture.fillVariables2;
	    newPolygon2->fillVariables = picture.fillVariables2;

          }

	  int counter = 0;
	  
          for (int i = 0; i < numberOfSides; i++){ // For all of the sides in the polygon create vertices.

            if (fgets(line, 100, imageFile) != NULL){

	      if (i < 3){

		sscanf(line, "%g %g %g", &newPolygon1->vertices[i].coordinates[0], &newPolygon1->vertices[i].coordinates[1], &newPolygon1->vertices[i].coordinates[2]);

	      }

	      if (i != 1){

		sscanf(line, "%g %g %g", &newPolygon2->vertices[counter].coordinates[0], &newPolygon2->vertices[counter].coordinates[1], &newPolygon2->vertices[counter].coordinates[2]);

		counter++;
		
	      }

	    }

          }

	  picture.allPolygons.push_back(newPolygon1);
	  picture.allPolygons.push_back(newPolygon2);

	}
	
      }

      else if(line[0] == 'l'){

	if(picture.numLightPositions == 0){

	  picture.lightPositions.resize(1);

	  point newPoint;

	  sscanf(line, "%s %g %g %g", &trash, &newPoint.coordinates[0], &newPoint.coordinates[1], &newPoint.coordinates[2]);

	  picture.lightPositions[0].positions.push_back(newPoint);

	}

	else{

	  picture.lightPositions.resize(2);

	  point newPoint;

          sscanf(line, "%s %g %g %g", &trash, &newPoint.coordinates[0], &newPoint.coordinates[1], &newPoint.coordinates[2]);

          picture.lightPositions[1].positions.push_back(newPoint);

        }

        picture.numLightPositions++; // Increments number of lightPositions.

      }

    }

    fclose(imageFile);

  }

  else{

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
