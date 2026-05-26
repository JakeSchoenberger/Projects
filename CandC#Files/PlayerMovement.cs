using UnityEngine;
using System.Collections;

// This is the movement for the 2D Platformer.

public class PlayerMovement : MonoBehaviour {

	[HideInInspector] public bool facingRight = true;

	public Animator animator;

	public AudioSource audioSource;

	public AudioClip jumpSound;

	public float moveSpeed = 25f;			// Value that controls movement speed
	
	private float moveVelocity = 0f;		// Placeholder used for calculated value

	private Rigidbody2D rb2d;				// Reference to physics component on player
	
	public bool jump = false;				// Condition for whether the player inputs jump.

	public float jumpForce = 2000f;			// Amount of force added when the player jumps.

	public bool grounded = false;			// Whether or not the player standing on the ground.
	



	// Use this for initialization
	void Start () {
		
		rb2d = GetComponent<Rigidbody2D>();
		audioSource = GetComponent<AudioSource>();
	
	}
	
	// Update is called once per frame
	void Update () {

		if (Input.GetAxis("Horizontal") > 0 && !facingRight){
			Flip();
		}
		else if (Input.GetAxis("Horizontal") < 0 && facingRight){
			Flip();
		}


		if(Input.GetButtonDown("Jump") && grounded) {
			jump = true;
		}

		
		if(grounded){
			//Only allow movement when player is in contact with asset tagged "Ground"
			MoveControl();
		}
	}

	//Called at a fixed framerate used for physics calculations and logic
	void FixedUpdate(){
	

		rb2d.velocity = new Vector2(moveVelocity, rb2d.velocity.y);

		// If the player should jump...
		if(jump)
		{

			// Add a vertical force to the player.
			GetComponent<Rigidbody2D>().AddForce(new Vector2(0f, jumpForce));

			audioSource.PlayOneShot(jumpSound, 0.7F);
			animator.SetBool("Jump", true);

			// Make sure the player can't jump again until the jump conditions from Update are satisfied.
			jump = false;

		}
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


	//Custom function called as needed
	void MoveControl () {
		
		//Reset Velocity value.
		moveVelocity = 0f;

		//Changed GetAxis to GetAxisRaw to eliminate annoying slide
		if (Input.GetAxisRaw("Horizontal") < 0)
		{
			//rb2d.velocity = new Vector2(-moveSpeed, rb2d.velocity.y);
			moveVelocity = -moveSpeed;
		}
		else if (Input.GetAxisRaw("Horizontal") > 0)
		{
			//rb2d.velocity = new Vector2(moveSpeed, rb2d.velocity.y);
			moveVelocity = moveSpeed;
		}

		animator.SetFloat("Speed", Mathf.Abs(moveVelocity));

	}
	
	//Detects when a game collider starts to overlap with this GameObject
	void OnTriggerEnter2D(Collider2D coll) {
		
		//Debug.Log ("Collided with: " + coll.gameObject.tag);
		if (coll.gameObject.tag == "Ground") {
			animator.SetBool("Jump", false);
			grounded = true;
		}

	}


	//Detects when a game collider stops overlapping with this GameObject
	void OnTriggerExit2D(Collider2D coll) {
		
		//Debug.Log ("Collided with: " + coll.gameObject.tag);
		if (coll.gameObject.tag == "Ground") {
			grounded = false;
		}
		
	}

}
