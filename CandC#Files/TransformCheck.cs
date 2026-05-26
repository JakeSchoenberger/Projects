using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// This code is used to change the animation of the AI.

public class TransformCheck : MonoBehaviour
{

    public GameObject playerObj = null; // Player
    public Animator animator; // Monster animations
    public SpriteRenderer renderer; // Used to flip monster

    // Start is called before the first frame update
    void Start()
    {
     
        playerObj = GameObject.FindWithTag("Player"); // Get Player

    }

    // Update is called once per frame
    void Update()
    {
    
     Vector2 forward = this.transform.right * -1;
     Vector2 firstVector = new Vector2(playerObj.transform.position.x, playerObj.transform.position.y); // Gets position of player in 2D Space
     Vector2 secondVector = new Vector2(this.transform.position.x, this.transform.position.y); // Gets position of monster in 2D Space

     Vector2 vecSubtract = secondVector - firstVector; // Subtract player position from monster position.

     float final = Vector2.SignedAngle(forward, vecSubtract); // Get the angle between the player and the monster.

     chooseDirection(final); // Chooses the direction for which animation should play.

     Debug.DrawRay(transform.position, transform.TransformDirection(Vector2.up) * 10f, Color.red); // Draws the angle for testing.

    }


    void chooseDirection(float degrees){

        if (degrees < -135 || degrees > 135){ // Look to the left.

            animator.SetBool("Side", true);
            animator.SetBool("Up", false);
            animator.SetBool("Down", false);

            
            renderer.flipX = true; // Flip the right facing animation to make it look left.

        }

        else if (degrees >= 45 && degrees < 135){ // Look up.

            animator.SetBool("Side", false);
            animator.SetBool("Up", true);
            animator.SetBool("Down", false);
        
        }

        else if (degrees > -45 && degrees < 45){ // Look to the right.

            animator.SetBool("Side", true);
            animator.SetBool("Up", false);
            animator.SetBool("Down", false);

            renderer.flipX = false; // If we have flipped the x previously, we flip it back to the right now.
        
        }

        else if (degrees <= -45 && degrees > -135){ // Look down.

            animator.SetBool("Side", false);
            animator.SetBool("Up", false);
            animator.SetBool("Down", true);
        
        }

    }
}
