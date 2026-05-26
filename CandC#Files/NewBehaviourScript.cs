using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// This script was used for creating a platform for a 2D platformer.

public class NewBehaviourScript : MonoBehaviour
{

    public bool platformExists;
    public GameObject prefab; // Platform
    public Transform targetPosition; // Where platform will spawn.
    public GameObject platformClone; // New Platform to create.
    public GameObject playerObject = null; // Player
    public Vector3 playerPos; // Where the player is.

    // Start is called before the first frame update
    void Start()
    {
       
        platformExists = false; // Initial status is that platform does not exist.

        if (playerObject == null){ // Sets the playerObject variable to the Player.

            playerObject = GameObject.FindWithTag("Player");
        
        }
       
    }

    // Update is called once per frame
    void Update()
    {

        if (Input.GetKeyDown(KeyCode.Z)){ // If Z is pressed, create a platform.

            createPlatform();
       
        }

    }

    void createPlatform(){

        if (platformExists == true){ // If there is already a platform, delete the platform, then create a new one.
    	
            deletePlatform();
            createPlatform();
    
        }

    else{ // If there is not a platform, set the location for the platform that will be created.

           if (playerObject.transform.localScale.x > 0){ // If the player is looking to the right...
            
            	float newX = playerObject.transform.position.x + 35.0f;
            	float newY = playerObject.transform.position.y + 8.0f;
            	float newZ = playerObject.transform.position.z;

            	playerPos = new Vector3(newX, newY, newZ); // Set the location of the to-be platform to this new location.

       		}

       		else{ // If the player is looking left.

       			float newX = playerObject.transform.position.x - 35.0f;
            	float newY = playerObject.transform.position.y + 8.0f;
            	float newZ = playerObject.transform.position.z;

            	playerPos = new Vector3(newX, newY, newZ); // Set the location of the to-be platform to this new location.

       		}
            
            targetPosition.position = playerPos; 

            platformClone = GameObject.Instantiate(prefab, targetPosition); // Creates a platform in the target position that looks the same as the prefab.
            platformClone.SetActive(true); // Set the platform to true so it appears in the game.
            platformExists = true; // Set the platformExists boolean to true for the rest of the code.
            
    //Code to create a platform.

        }       

    }   

    void deletePlatform(){

        platformExists = false; 
        Destroy(platformClone); // Deletes the platform from the game.
        platformClone.SetActive(false); // Sets the platform to false so it cannot be interacted with.
        

    }

}