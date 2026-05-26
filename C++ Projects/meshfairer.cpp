#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include "io.cpp"

using namespace std;

int main(int argc, char ** argv){

  vector<Eigen::Vector3d> pts; // Creates a vector of points.
  vector<Tri> triangles; // Creates a vector of triangles.

  char *fname = argv[1]; // filename "bunny.obj"
  int stepsize = atoi(argv[3]); // stepsize variable (1)
  int niterations = atoi(argv[4]); // niterations variable (50)

  bool x = readObjFile(fname, pts, triangles); // Reads an obj file and populates the pts and triangles vectors.

  if (x == true){ // If we can properly read the obj file...
    
    for (int r = 0; r < niterations; r++){ // For the amount of niterations...
    
      for (unsigned int i = 0; i < pts.size(); i++){ // For all of the points in the pts vector...

	Eigen::Vector3d laplacian; // Create a variable laplacian
	double m = 0; // Creates a variable m.
	Eigen::Vector3d summation = {0, 0, 0}; // Creates a variable summation.
	
	for (unsigned int j = 0; j < triangles.size(); j++){ // For all of the triangles...
	
	  for (int k = 0; k < 3; k++){ // For all the points in a triangle...
	    
	    if (i == triangles[j][k]){ // If the current point is in a triangle...

	      m += 2; // Add 2 to the amount of neighbors of the current point (m).
	      
	      if (k == 0){ // If k is 0, add to the summation variable the values of the other two points - the current point.

		summation += pts[triangles[j][1]] - pts[i];
		summation += pts[triangles[j][2]] - pts[i];

	      }

	      else if(k == 1){ // If k is 1, add to the summation variable the values of the other two points - the current point.

		summation += pts[triangles[j][0]] - pts[i];
		summation += pts[triangles[j][2]] - pts[i];

	      }

	      else{ // If k is 2, add the summation variable the values of the other two points - the current point.

		summation += pts[triangles[j][0]] - pts[i];
		summation += pts[triangles[j][1]] - pts[i];

	      }
	      
	    }
	    
	  }
	  
	}
	
	laplacian = (1.0/m) * summation; // Calculates the laplacian value.
	pts[i] += stepsize * laplacian; // Changes the position of the point by the stepsize * laplacian value.
	
      }
      
    }
    
    writeObjFile(argv[2], pts, triangles); // Creates the output file.

  }
  
  return 0;
  
}
