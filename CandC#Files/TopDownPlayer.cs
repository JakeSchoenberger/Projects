using System.Collections;
using System.Collections.Generic;
using System.Net.Mime;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using UnityEngine.UIElements;

// This is a file that does a LOT of things, it deals with the dialogue box, the player's animations, and the puzzles.
// NOTE: As this was a group project not everything here is my code, there was another coder that put some of their code here. 

public class TopDownPlayer : MonoBehaviour
{
    //Two Animator for further use.
    public Animator animator;
    public Animator effectAnimator;
    //User Interface for indicator
    public TextMeshProUGUI dialogBox;
    public SpriteRenderer renderer;
    public GameObject keyBox;

    public GameObject sight;

    //Direction of player
    [HideInInspector] public bool facingRight = true;
    [HideInInspector] public bool facingUp = false;
    [HideInInspector] public bool facingLeft = false;
    [HideInInspector] public bool facingDown = false;

    public static string whichDirection = "Right";

    //Player movement parameters
    public float MoveXSpeed = 5f;

    public float MoveYSpeed = 5f;			// Value that controls movement speed

    private float MoveXVelocity = 0f;

    private float MoveYVelocity = 0f;

    public Vector2 facingDirection;

    public Rigidbody2D rb2d;
    
    //Player Action Parameter
    public bool Key;
    public bool Grab;

    public bool[] CheckKey;
    public Sprite[] keySprites;

    //For push & pull function
    public float distance = 5f;
    public LayerMask boxMask;
    public GameObject box;

    public Puzzle puz;

    private void Start()
    {
        keyBox.SetActive(false);
        dialogBox.gameObject.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        MoveControl();
        PushCheck();
    }

    //Custom function called as needed
    void MoveControl()
    {

        //Reset Velocity value.
        MoveXVelocity = 0f;
        MoveYVelocity = 0f;
        animator.SetFloat("Speed", 0);

        //Changed GetAxis to GetAxisRaw to eliminate annoying slide
        
        if (Input.GetAxisRaw("Horizontal") < 0) //Left
        {
            //rb2d.velocity = new Vector2(-moveSpeed, rb2d.velocity.x);
            animator.SetBool("Sideways", true);
            animator.SetBool("Down", false);
            animator.SetBool("Up", false);
            animator.SetFloat("Speed", 5);
            MoveXVelocity = -MoveXSpeed;
            sight.transform.eulerAngles = new Vector3(0, 0, 0);
            whichDirection = "Left";

            //renderer.flipX = true;

            if (facingRight)
            {
                //Reverse the X scale of player to make the face opposite direction.
                facingRight = !facingRight;
                Vector3 theScale = transform.localScale;
                theScale.x *= -1;
                transform.localScale = theScale;
            }

            

            facingDirection = new Vector2(1, 0);
        }
        else if (Input.GetAxisRaw("Horizontal") > 0) //Right
        {
            //rb2d.velocity = new Vector2(moveSpeed, rb2d.velocity.x);
            animator.SetBool("Sideways", true);
            animator.SetBool("Down", false);
            animator.SetBool("Up", false);
            animator.SetFloat("Speed", 5);
            MoveXVelocity = MoveXSpeed;
            sight.transform.eulerAngles = new Vector3(0, 0, 0);
            whichDirection = "Right";

            //renderer.flipX = false;

           if (!facingRight)
            {
                //Reverse the X scale of player to make the face opposite direction.
                facingRight = !facingRight;
                Vector3 theScale = transform.localScale;
                theScale.x *= -1;
                transform.localScale = theScale;
            }

            
            facingDirection = new Vector2(1, 0);
        }
        else if (Input.GetAxisRaw("Vertical") > 0) //UP
        {

            animator.SetBool("Sideways", false);
            animator.SetBool("Down", false);
            animator.SetBool("Up", true);
            animator.SetFloat("Speed", 5);
            
            MoveYVelocity = MoveYSpeed;

            whichDirection = "Up";
            if (facingRight)
            {
                facingDirection = new Vector2(0, 1);
                sight.transform.eulerAngles = new Vector3(0, 0, 90);
            } else
            {
                facingDirection = new Vector2(0, -1);
                sight.transform.eulerAngles = new Vector3(0, 0, -90);
            }
            
        }
        else if (Input.GetAxisRaw("Vertical") < 0) //Down
        {


            animator.SetBool("Sideways", false);
            animator.SetBool("Down", true);
            animator.SetBool("Up", false);
            animator.SetFloat("Speed", 5);
            
            MoveYVelocity = -MoveYSpeed;

            whichDirection = "Down";

            if (facingRight)
            {
                facingDirection = new Vector2(0, -1);
                sight.transform.eulerAngles = new Vector3(0, 0, -90);
            }
            else
            {
                facingDirection = new Vector2(0, 1);
                sight.transform.eulerAngles = new Vector3(0, 0, 90);
            }
        }

    }

    void FixedUpdate()
    {
        rb2d.velocity = new Vector2(MoveXVelocity, MoveYVelocity);
        //Move();
    }

    //Custom function called as needed
    void Flip()
    {
        //Reverse the X scale of player to make the face opposite direction.
        facingRight = !facingRight;
        Vector3 theScale = transform.localScale;
        theScale.x *= -1;
        transform.localScale = theScale;
    }

    void PushCheck()
    {
        Physics2D.queriesStartInColliders = false;
        RaycastHit2D hit = Physics2D.Raycast(transform.position, facingDirection * transform.localScale.x, boxMask);
        if (hit.collider != null && hit.collider.gameObject.tag == "Box" && Input.GetKey(KeyCode.E))
        {
            box = hit.collider.gameObject;
            if(Vector2.Distance(box.transform.position, gameObject.transform.position) < distance)
            {
                box.GetComponent<FixedJoint2D>().enabled = true;
                box.GetComponent<FixedJoint2D>().connectedBody = this.GetComponent<Rigidbody2D>();
            }
        }
        else if (Input.GetKeyUp(KeyCode.E))
        {
            if(box != null)
            {
                box.GetComponent<FixedJoint2D>().connectedBody = null;
            }
        }
    }    

    private void OnDrawGizmos()
    {
        Gizmos.color = Color.yellow;

        Gizmos.DrawLine(transform.position, (Vector2)transform.position + facingDirection * transform.localScale.x * distance);
    }

    //Detects when a game collider starts to overlap with this GameObject
    void OnTriggerEnter2D(Collider2D coll)
    {
        //Debug.Log ("Collided with: " + coll.gameObject.tag);
        if (Key && coll.gameObject.tag == "Door")
        {
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }

        if(coll.gameObject.tag == "Key1")
        {
            keyBox.SetActive(true);
            keyBox.GetComponent<SpriteRenderer>().sprite = keySprites[0];
            if (Key)
            {
                Color temp = keyBox.GetComponent<SpriteRenderer>().color;
                keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 1f);
            }
            dialogBox.gameObject.SetActive(true);
            keyBox.GetComponent<SpriteRenderer>().sprite = keySprites[0];
            dialogBox.text = coll.gameObject.GetComponent<Object>().description[0];
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }

        if(coll.gameObject.tag == "Key2")
        {
            keyBox.SetActive(true);
            if (Key)
            {
                Color temp = keyBox.GetComponent<SpriteRenderer>().color;
                keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 1f);
            }
            keyBox.GetComponent<SpriteRenderer>().sprite = keySprites[0];
            dialogBox.gameObject.SetActive(true);
            dialogBox.text = coll.gameObject.GetComponent<Object>().description[0];
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }

        if(coll.gameObject.tag == "Key3")
        {
            keyBox.SetActive(true);
            if (Key)
            {
                Color temp = keyBox.GetComponent<SpriteRenderer>().color;
                keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 1f);
            }
            keyBox.GetComponent<SpriteRenderer>().sprite = keySprites[0];
            dialogBox.gameObject.SetActive(true);
            dialogBox.text = coll.gameObject.GetComponent<Object>().description[0];
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }

        if(coll.gameObject.tag == "Key4")
        {
            keyBox.SetActive(true);
            if (Key)
            {
                Color temp = keyBox.GetComponent<SpriteRenderer>().color;
                keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 1f);
            }
            keyBox.GetComponent<SpriteRenderer>().sprite = keySprites[0];
            dialogBox.gameObject.SetActive(true);
            dialogBox.text = coll.gameObject.GetComponent<Object>().description[0];
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }

        if (coll.gameObject.tag == "Key5")
        {
            keyBox.SetActive(true);
            if (Key)
            {
                Color temp = keyBox.GetComponent<SpriteRenderer>().color;
                keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 1f);
            }
            keyBox.GetComponent<SpriteRenderer>().sprite = keySprites[0];
            dialogBox.gameObject.SetActive(true);
            dialogBox.text = coll.gameObject.GetComponent<Object>().description[0];
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }

        if (coll.gameObject.tag == "Skeleton")
        {
            puz.Puzzle3();
            dialogBox.gameObject.SetActive(true);
            dialogBox.text = coll.gameObject.GetComponent<Object>().description[0];
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }


        if (coll.gameObject.tag == "Object")
        {
            dialogBox.gameObject.SetActive(true);
            dialogBox.text = coll.gameObject.GetComponent<Object>().description[0];
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }

        if (coll.gameObject.tag == "Box")
        {
            keyBox.SetActive(true);
            if (Key)
            {
                Color temp = keyBox.GetComponent<SpriteRenderer>().color;
                keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 1f);
            }
            keyBox.GetComponent<SpriteRenderer>().sprite = keySprites[1];
            dialogBox.gameObject.SetActive(true);
            dialogBox.text = coll.gameObject.GetComponent<Object>().description[0];
            Debug.Log("Collided with: " + coll.gameObject.tag);
            //dialogBox.get
        }


        if (coll.gameObject.tag == "Exit")
        {

            SceneManager.LoadScene("End Scene");
        
        }
    }

    private void OnTriggerExit2D(Collider2D coll)
    {
        if (coll.gameObject.tag == "Key1")
        {
            Color temp = keyBox.GetComponent<SpriteRenderer>().color;
            keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 0.5f);
            keyBox.SetActive(false);
            dialogBox.gameObject.SetActive(false);
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }

        if (coll.gameObject.tag == "Key2")
        {
            Color temp = keyBox.GetComponent<SpriteRenderer>().color;
            keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 0.5f);
            keyBox.SetActive(false);
            dialogBox.gameObject.SetActive(false);
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }

        if (coll.gameObject.tag == "Key3")
        {
            Color temp = keyBox.GetComponent<SpriteRenderer>().color;
            keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 0.5f);
            keyBox.SetActive(false);
            dialogBox.gameObject.SetActive(false);
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }

        if (coll.gameObject.tag == "Key4")
        {
            Color temp = keyBox.GetComponent<SpriteRenderer>().color;
            keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 0.5f);
            keyBox.SetActive(false);
            dialogBox.gameObject.SetActive(false);
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }

        if (coll.gameObject.tag == "Key5")
        {
            Color temp = keyBox.GetComponent<SpriteRenderer>().color;
            keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 0.5f);
            keyBox.SetActive(false);
            dialogBox.gameObject.SetActive(false);
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }

        if (coll.gameObject.tag == "Object")
        {
            Color temp = keyBox.GetComponent<SpriteRenderer>().color;
            keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 0.5f);
            keyBox.SetActive(false);
            dialogBox.gameObject.SetActive(false);
            Debug.Log("Collided with: " + coll.gameObject.tag);
            //dialogBox.get
        }

        if (coll.gameObject.tag == "Box")
        {
            Color temp = keyBox.GetComponent<SpriteRenderer>().color;
            keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 0.5f);
            keyBox.SetActive(false);
            dialogBox.gameObject.SetActive(false);
            Debug.Log("Collided with: " + coll.gameObject.tag);
            //dialogBox.get
        }

        if (coll.gameObject.tag == "Skeleton")
        {
            Color temp = keyBox.GetComponent<SpriteRenderer>().color;
            keyBox.GetComponent<SpriteRenderer>().color = new Color(temp.r, temp.g, temp.b, 0.5f);
            keyBox.SetActive(false);
            dialogBox.gameObject.SetActive(false);
            Debug.Log("Collided with: " + coll.gameObject.tag);
        }
    }
}
    