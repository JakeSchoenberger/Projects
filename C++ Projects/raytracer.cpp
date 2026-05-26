#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <Eigen/Dense>

using namespace std;

struct point
{

  float verticeX;
  float verticeY;
  float verticeZ;

};

struct polygon
{

  int numberOfSides;
  vector<point> vertices;
  
};

struct ray{

  Eigen::Vector3d originPoint;
  Eigen::Vector3d direction;

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
  float fillRed;
  float fillGreen;
  float fillBlue;
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
  
};

finalPicture parse();

void writePPM(unsigned char pixels[512][512][3]);

int fieldOfView(finalPicture picture);

bool doesIntersect(double t, double gamma, double beta);

double calculateDeterminant(Eigen::Matrix3d matrix);

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
      
      ray ray1;
      ray1.originPoint = from;
      float p = -1 * tan(picture.angle/2) + (pixelWidth/2) + (i * pixelWidth);
      float q = (tan(picture.angle/2)) - (pixelWidth/2) - (j * pixelWidth);
      float r = -1;
      
      Eigen::Vector3d pixelDirection = (p * u) + (q * v) + (r * w); // Does the dot product of p q r and u v w to transfer us into world coordinates.
      
      ray1.direction = pixelDirection;
      
      for (long unsigned int k = 0; k < picture.allPolygons.size(); k++){ // We check every polygon to see if our ray intersects them.

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

	
	if (doesIntersect(finalT, finalGamma, finalBeta) == true && beenHit == false){ // If our ray intersects a triangle, we color it the specified color from the nff file.

	  pixels[j][i][0] = picture.fillRed*255;
	  pixels[j][i][1] = picture.fillGreen*255;
	  pixels[j][i][2] = picture.fillBlue*255;

	  beenHit = true;
	  
	}

	else if(beenHit != true){ // Otherwise, we set it to the background color.

	  pixels[j][i][0] = picture.R*255;
	  pixels[j][i][1] = picture.G*255;
	  pixels[j][i][2] = picture.B*255;

	}

      }

    }
    
  }

  writePPM(pixels);

  while(!picture.allPolygons.empty()){ // Deallocates allocated memory.

    delete picture.allPolygons.back();
    picture.allPolygons.pop_back();
    
  }

  return 0;

}

finalPicture parse(){
  
  FILE* imageFile;
  char line[50];
  char trash;
  char trash2;
  char trash3;
  char trash4;
  char trash5;
  char trash6;
  char trash7;
  char trash8;
  char trash9;
  int numberOfSides;
  finalPicture picture;

  imageFile = fopen("tetra-3.nff", "r");

  if (imageFile != NULL){ // If the imageFile exists.

    while (fgets(line, 50, imageFile) != NULL){ // As long as the line isn't empty we keep reading the file.
	
      if(line[0] == 'b'){ // If the line starts with the character b, we scan for the background color.

	sscanf(line, "%s %g %g %g", &trash, &picture.R, &picture.G, &picture.B);

      }

      if(line[0] == 'f'){ // If the line starts with f we check for the second character, and depending on the second character we do two different things.

	if(line[1] == 'r'){ // If the line's second character is an r we get the from coordinates.

	  sscanf(line, "%s %g %g %g", &trash2, &picture.fx, &picture.fy, &picture.fz);

	}

	else{ // Otherwise, we get the fill color.

	  sscanf(line, "%s %g %g %g", &trash3, &picture.fillRed, &picture.fillGreen, &picture.fillBlue);

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

	sscanf(line, "%s %d", &trash9, &numberOfSides);
	polygon* newPolygon = new(polygon); // Creates a new polygon.
	newPolygon->numberOfSides = numberOfSides;
	newPolygon->vertices.resize(numberOfSides);

	for (int i = 0; i < newPolygon->numberOfSides; i++){ // For all of the sides in the polygon create vertices.

	  if (fgets(line, 50, imageFile) != NULL){

	    sscanf(line, "%g %g %g", &newPolygon->vertices[i].verticeX, &newPolygon->vertices[i].verticeY, &newPolygon->vertices[i].verticeZ);

	  }

	}

	picture.allPolygons.push_back(newPolygon); // Put the newPolygon in the allPolygons vector.

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
  
