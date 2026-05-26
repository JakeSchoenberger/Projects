#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <cstring>
#include "kdTree.H"
using namespace std;

struct boidUnit{

  Eigen::Vector3d position;
  Eigen::Vector3d velocity;

};

struct boidSystem{

  float size;
  float neighbor_radius;
  int num_neighbors;
  float mass;
  float collision;
  float centering;
  float velocity;
  float hunger;
  float damping;
  float dt;
  int length;
  int nboids;
  
  vector<boidUnit*> allBoids;
  vector<Eigen::Vector3d> positions;
  
};

void simulate();

int main(){

  simulate();
  
  return 0;
  
}

void simulate(){

  FILE* file;
  char line[300];
  vector<Eigen::Vector3d> pts;
  boidSystem system1;
  
  file = fopen("sample.in", "r"); // Opens up the input file.

  if (file != NULL){ // If the file exists...

    if (fgets(line, 300, file) != NULL){ // If the line isn't empty...

      sscanf(line, "%g %g %d %g %g %g %g %g %g %g %d", &system1.size, &system1.neighbor_radius, &system1.num_neighbors, &system1.mass, &system1.collision, &system1.centering, &system1.velocity, &system1.hunger, &system1.damping, &system1.dt, &system1.length); // Gives us the values from the file.

    }

    if (fgets(line, 100, file) != NULL){ // If the line isn't empty, get the number of boids.

      sscanf(line, "%d", &system1.nboids);

    }

    for (int i = 0; i < system1.nboids; i++){ // For all of the boids...

      boidUnit *newBoid = new boidUnit(); // Creates a new boidUnit.

      vector<char> position;
      vector<char> velocity;

      char position1[100];
      char velocity1[100];

      if (fgets(line, 100, file) != NULL){ // If the line is not empty, get the position and velocity as strings.
	
	sscanf(line, "%s %s", &position1, &velocity1);
	
      }
      
      for (int j = 0; j < sizeof(position1); j++){ // For everything in the position...

	if (position1[j] == '['){ // If any of the characters is a left bracket...

	  for (int k = 0; k < sizeof(position1) - 1; k++){ // Move everything over by one character, removing the left bracket.

	    position1[k] = position1[k + 1];

	  }

	}

	else if (position1[j] == ']'){ // If there is a right bracket character, change it to 0 not changing the value of anything, but getting rid of that bracket.

	  position1[j] = '0';

	}

      }

      char* splitPosition;
      
      splitPosition = strtok(position1, ","); // Splits up the position string.

      newBoid->position[0] = atof(splitPosition); // Sets the first value of the vector to the first float of the string.

      splitPosition = strtok(NULL, ","); // Repeat for the next two values...

      newBoid->position[1] = atof(splitPosition);

      splitPosition = strtok(NULL, ",");

      newBoid->position[2] = atof(splitPosition);

      for (int m = 0; m < sizeof(velocity1); m++){ // Repeat what we did for position with velocity.

	if (velocity1[m] == '['){

	  for (int n = 0; n < sizeof(velocity1) - 1; n++){

	    velocity1[n] = velocity1[n + 1];

	  }

	}

	else if (velocity1[m] == ']'){

	  velocity1[m] = '0';

	}

      }

      char* splitVelocity;

      splitVelocity = strtok(velocity1, ",");

      newBoid->velocity[0] = atof(splitVelocity);

      splitVelocity = strtok(NULL, ",");

      newBoid->velocity[1] = atof(splitVelocity);

      splitVelocity = strtok(NULL, ",");

      newBoid->velocity[2] = atof(splitVelocity);

      system1.allBoids.push_back(newBoid); // Puts the new boid in the allBoids vector.

      pts.push_back(newBoid->position); // Pushes back the position to the pts vector.
      
    }

  }

  fclose(file); // Closes the read in file.

  FILE* writeFile;
  writeFile = fopen("sample.out", "w"); // Opens the writeFile.
  
  char lines[100] = "600 \n50 \n"; 

  for (int u = 0; u < strlen(lines); u++){ // Writes the first two lines into the write file.

    putc(lines[u], writeFile);

  }

  system1.positions = pts;
  
  string boidLine;
  float total = 0.0;
  
  for (int v = 0; v < system1.allBoids.size(); v++){ // Creates the string format to write to the file.

    boidLine = "[" + to_string(system1.allBoids[v]->position[0]) + "," + to_string(system1.allBoids[v]->position[1]) + "," + to_string(system1.allBoids[v]->position[2]) + "] [" + to_string(system1.allBoids[v]->velocity[0]) + "," + to_string(system1.allBoids[v]->velocity[1]) + "," + to_string(system1.allBoids[v]->velocity[2]) + "]\n"; 

    for (int w = 0; w < boidLine.length(); w++){ // Writes the boidline to the file.

      putc(boidLine[w], writeFile);

    }

  }

  while(total <= 20){ // While loop that will go through the animation the proper amount of times.
    
    for (int q = 0; q < system1.allBoids.size(); q++){ // Goes through every boidUnit.

      KDTree treeOfNeighbors(system1.positions); // Creates a KDTree that will give us all the neighbors based on the positions of all the boidUnits.
      vector<int> neighborsVector;
      treeOfNeighbors.neighbors(system1.positions, system1.allBoids[q]->position, system1.num_neighbors, system1.neighbor_radius, neighborsVector); // Populates the neighborsVector.

      Eigen::Vector3d flockCenteringForce = {0, 0, 0};
      Eigen::Vector3d velocityMatchingForce = {0, 0, 0};
      Eigen::Vector3d collisionAvoidanceForce = {0, 0, 0};

      for (int r = 0; r < neighborsVector.size(); r++){ // Sums up all of the neighbors positions and velocity.
	
	flockCenteringForce += system1.allBoids[neighborsVector[r]]->position;
	velocityMatchingForce += system1.allBoids[neighborsVector[r]]->velocity;
	
      }

      for (int s = 0; s < neighborsVector.size(); s++){ // For all of the neighbors...

	if ((system1.allBoids[neighborsVector[s]]->position - system1.allBoids[q]->position).norm() != 0){ // If the positions aren't the same add to the collisionAvoidanceForce (This is to avoid dividing by 0.
	  
	  collisionAvoidanceForce += (-1 * (system1.allBoids[neighborsVector[s]]->position - system1.allBoids[q]->position)) / pow((system1.allBoids[neighborsVector[s]]->position - system1.allBoids[q]->position).norm(), 2);

	}

      }

      if (neighborsVector.size() != 0){ // If statement to avoid dividing by 0.

	flockCenteringForce /= neighborsVector.size();
	velocityMatchingForce /= neighborsVector.size();
	
      }

      flockCenteringForce -= system1.allBoids[q]->position; // Subtract the position from the centering force.
      velocityMatchingForce -= system1.allBoids[q]->velocity; // Subtract the velocity from the Matching force.

      flockCenteringForce *= system1.centering; // Multiply the forces by their constants.
      collisionAvoidanceForce *= system1.collision;
      velocityMatchingForce *= system1.velocity;

      Eigen::Vector3d totalForce = flockCenteringForce + velocityMatchingForce + collisionAvoidanceForce; // Sum up the total force.
      
      system1.allBoids[q]->velocity += (1/system1.mass) * system1.dt * totalForce; // Calculate the new velocity of the boidUnit.

      system1.allBoids[q]->position = system1.allBoids[q]->position + (system1.dt * system1.allBoids[q]->velocity); // Calculate the new position of the boid unit.

      if (system1.allBoids[q]->position[0] < -0.5 || system1.allBoids[q]->position[0] > 0.5){ // If we go to the outskirts of the x bounding box, flip the sign of the x velocity.

	system1.allBoids[q]->velocity[0] *= -1;

      }

      else if (system1.allBoids[q]->position[1] < -0.25 || system1.allBoids[q]->position[1] > 0.25){ // If we go to the outskirts of the y bounding box, flip the sign of the y velocity.

	system1.allBoids[q]->velocity[1] *= -1;

      }

      else if (system1.allBoids[q]->position[2] < -0.125 || system1.allBoids[q]->position[2] > 0.125){ // If we go to the outskirts of the z bounding box, flip the sign of the z velocity.

	system1.allBoids[q]->velocity[2] *= -1;

      }

      system1.allBoids[q]->velocity *= 0.999; // Damping.

    }


    char lines2[100] = "0\n50 \n";

    for (int g = 0; g < strlen(lines2); g++){ // Writes to the writeFile.

      putc(lines2[g], writeFile);

    }

    for (int v = 0; v < system1.allBoids.size(); v++){ // Creates the boidLine string in its proper format and writes the new position and velocity to the writeFile.

      boidLine = "[" + to_string(system1.allBoids[v]->position[0]) + "," + to_string(system1.allBoids[v]->position[1]) + "," + to_string(system1.allBoids[v]->position[2]) + "] [" + to_string(system1.allBoids[v]->velocity[0]) + "," + to_string(system1.allBoids[v]->velocity[1]) + "," + to_string(system1.allBoids[v]->velocity[2]) + "]\n";

      for (int w = 0; w < boidLine.length(); w++){
      
	putc(boidLine[w], writeFile);

      }

    }

    total += system1.dt; // Changes the value of total so we eventually end the loop.
    
  }

}
