// CMSC 341 - Fall 2021 - Project 3 - MQueue Class 

#include "mqueue.h"
using namespace std;
using std::domain_error;

/*
Function Name: Overloaded Constructor
Desc: Creates an MQueue object with a priority function.
Preconditions: The priority function is already defined.
Postconditions: A new MQueue object is created.
*/

MQueue::MQueue(prifn_t priFn)
{

  m_priorFunc = priFn; // Sets the member variable m_priorFunc to the priFn.

}

/*
Function Name: Destructor
Desc: Deletes all allocated memory in MQueue.
Preconditions: None.
Postconditions: All allocated memory is deallocated.
*/

MQueue::~MQueue()
{
  
  if (m_heap == NULL){ // If the heap is already empty, return.

    return;

  }

  else{ // If the heap is not empty, call the clear function.
    
    clear();

  }

}

/*
Function Name: Copy Constructor
Desc: Creates a new MQueue object by copying an existing MQueue object.
Precondition: None.
Postcondition: A new MQueue object that is a deep copy of the argument passed in is created.
*/

MQueue::MQueue(const MQueue& rhs)
{

  if (rhs.m_heap == NULL){ // If rhs is empty, we create an empty heap.

    cout << "Rhs is empty." << endl;
    this->m_heap = NULL;
    this->m_size = 0;

  }

  else{ // If rhs is populated, we set the priority function to the rhs priority function. We set the size to the number of rhs size. Then we copy all of the nodes to the new heap.
    
    this->setPriorityFn(rhs.getPriorityFn());
    this->m_size = rhs.numOrders();
    
    heapCopy(this->m_heap, rhs.m_heap);

  }

}

/*
Function Name: Overloaded Assignment Operator
Desc: Makes an MQueue object a deep copy of another MQueue object.
Preconditions: None.
Postconditions: A deep copy of rhs is created.
*/

MQueue& MQueue::operator=(const MQueue& rhs)
{

  if (this == &rhs){ // If the left hand side is the right hand side, we don't want to do anything.

    cout << "Left and right side are the same!" << endl;

  }

  else{ // If it is not the same, clear the current heap, take its member variables and then copy the heap with heapCopy.

    this->clear();

    this->m_heap = NULL;
    
    this->m_priorFunc = rhs.getPriorityFn();
    
    this->m_size = rhs.numOrders();

    heapCopy(this->m_heap, rhs.m_heap);
    
  }

  return *this; // Returns the new copied heap.
  
}

/*
Function Name: heapCopy
Desc: Copies everything from an existing heap to a new heap.
Preconditions: None
Postconditions: We copy everything from one heap to another.
*/

void MQueue::heapCopy(Node*& newHeap, Node* existingHeap){

  if (existingHeap == NULL){ // If the existingHeap is an empty heap, the new heap is also empty.

    newHeap = NULL;

  }

  else{ // Otherwise, create a new node with the order of the existingHeap's node and copy the left and right nodes.

    newHeap = new Node(existingHeap->getOrder()); // Creates a new Node with the same Order value from the existingHeap.

    heapCopy(newHeap->m_left, existingHeap->m_left); // Copies the left child.
    heapCopy(newHeap->m_right, existingHeap->m_right); // Copies the right child.

  }
    
}

/*
Function Name: insertOrder
Desc: Inserts a new node into the heap.
Preconditions: None.
Postconditions: A new node is inserted into the heap.
*/

void MQueue::insertOrder(const Order& input) {

  Node* newInput = new Node(input); // Creates a new Node with the order value argument.

  if (m_heap == NULL){ // If m_heap is empty, set m_heap to the newInput.

    m_heap = newInput;
    m_size++;

  }

  else{ // If m_heap is not empty, merge m_heap with the newInput node.

    m_heap = mergeWithQueueHelper(m_heap, newInput);
    m_size++;
    
  }
  
}

/*
Function Name: getNextOrder
Desc: Our removal function, will take out the root of the heap and then merge the left and right child of the root to maintain the heap.
Preconditions: None.
Postconditions: The root is extracted and the heap fixes itself.
*/

Order MQueue::getNextOrder() {

  if (m_heap == NULL){ // If the heap is empty we want to throw a domain_error.

    throw std::domain_error("The Queue is empty! Cannot remove!");

  }

  else{ // If it isnt, we want to take the left and right children of m_heap and merge them, and then delete m_heap. 

    Order firstOrder = m_heap->getOrder(); // Gets the order of m_heap.

    Node* temp = m_heap; // Used to later delete m_heap.
    Node* leftChild = m_heap->m_left; 
    Node* rightChild = m_heap->m_right;

    m_heap = mergeWithQueueHelper(leftChild, rightChild); // Makes the new heap from the children of m_heap.

    delete temp; // Deletes the original m_heap.
    
    m_size--;

    return firstOrder; // Returns the original order.

  }

}

/*
Function Name: mergeWithQueue
Desc: Merges the already existing heap with another heap.
Preconditions: None.
Postconditions: We have created a joint heap.
*/

void MQueue::mergeWithQueue(MQueue& rhs) {

  if (&rhs == this){ // If the rhs and the current object are the same thing we just return.

    return;

  }

  else if (rhs.m_priorFunc != this->m_priorFunc){ // If they have different priority functions we cannot merge them.

    throw domain_error("Cannot merge with two different priority functions!");

  }

  else{ // If they pass those two booleans we recursively merge with the helper function.
    
    m_heap = mergeWithQueueHelper(this->m_heap, rhs.m_heap);

    m_size += rhs.m_size;

    rhs.m_heap = NULL;
    
  }
  
}

/*
Function Name: mergeWithQueueHelper
Desc: Recursively goes through the heaps to help merge them together properly.
Preconditions: None.
Postconditions: The heaps will be merged properly.
*/

Node* MQueue::mergeWithQueueHelper(Node* lhs, Node* rhs){

  if (lhs == NULL){ // If the left hand side is null we return the right hand side.

    return rhs;

  }

  else if(rhs == NULL){ // If the right hand side is null we return the left hand side.

    return lhs;

  }

  if (m_priorFunc(rhs->getOrder()) < m_priorFunc(lhs->getOrder())){ // If the right hand side has a lower priority we swap the left and right sides.

    swap(lhs, rhs);
      
  }
  
  swap(lhs->m_left, lhs->m_right); // Swap the chidren of the left hand side.
  
  lhs->m_left = mergeWithQueueHelper(rhs, lhs->m_left); // Make the left child of the left hand side a recursive call with the right hand side and the current left child of the left hand side.
  
  return lhs; // Return the new heap.
  
}

/*
Function Name: clear
Desc: Deallocates all memory from a heap.
Preconditions: None.
Postconditions: All memory is deallocated from the heap.
*/

void MQueue::clear() {

  if (m_heap == NULL){ // If the heap is empty, just return.

    return;

  }

  else{ // Otherwise call the clear recursive helper function. 

    clear(m_heap);

  }
  
}

/*
Function Name: Recursive clear helper function
Desc: Goes through each individual node and deletes the children before deleting the node itself.
Preconditions: None.
Postconditions: All nodes are deallocated in postorder traversal.
*/

void MQueue::clear(Node* aNode){

  if (aNode == NULL){ // If aNode is a nullptr we do nothing.

    return;

  }

  else{ 

    if (aNode->m_left != NULL){ // Otherwise, if the node has a left child we clear the left child.
    
      clear(aNode->m_left);

    }

    if (aNode->m_right != NULL){ // If the node has a right child we call the function to clear the right child.

      clear(aNode->m_right);
      
    }
    
    delete aNode; // Deallocates the memory if it has no children.

  }

}

/*
Function Name: numOrders
Desc: Returns the size of the heap.
Preconditions: None.
Postconditions: We get the size of the heap.
*/

int MQueue::numOrders() const
{
  
  return m_size; 
  
}

/*
Function Name: printOrderQueue
Desc: Calls the recursive helper function for printOrderQueue.
Preconditions: None.
Postconditions: We output all the information from the heap.
*/

void MQueue::printOrderQueue() const {

  printOrderQueue(m_heap); // Calls the recursive printOrderQueue helper function.
  
}

/*
Function Name: printOrderQueue recursive helper
Desc: Uses preorder traversal to output all the nodes to the screen.
Preconditions: None.
Postconditions: All the nodes' information is output to the screen.
*/

void MQueue::printOrderQueue(Node* heap) const {

  cout << "[" << m_priorFunc(heap->m_order) << "]" << heap->getOrder()<< endl; // Outputs the priority as well as the Order output to the screen.

  if (heap->m_left != NULL){ // Outputs the left child if the left child exists.

    printOrderQueue(heap->m_left);

  }

  if (heap->m_right != NULL){ // Outputs the right child if the right child exists.

    printOrderQueue(heap->m_right);

  }

}

/*
Function Name: getPriorityFn
Desc: Returns the member variable m_priorFunc.
Preconditions: None.
Postconditions: Gets the priority function.
*/

prifn_t MQueue::getPriorityFn() const {

  return m_priorFunc;
  
}

/*
Function Name: setPriorityFn
Desc: Sets the priority function to a new one, and rebuilds the heap based on that new priority function.
Preconditions: None.
Postconditions: Rebuilds the heap with a brand new priority.
*/

void MQueue::setPriorityFn(prifn_t priFn) {

  m_priorFunc = priFn; // Sets m_priorFunc to the priFn argument.
  int amountOfCars = m_size;
  int index = 0;

  Node** carArray = new Node*[amountOfCars]; // Creates an array of Node pointers called carArray.

  traverse(m_heap, carArray, index); // Populates the carArray.

  m_heap = NULL;

  for (int i = 0; i < amountOfCars; i++){ // For the amount of items originally in the heap, we merge them with the queue after we set each of the nodes' children to NULL before we merge it.

    carArray[i]->m_right = NULL;
    carArray[i]->m_left = NULL;

    m_heap = mergeWithQueueHelper(m_heap, carArray[i]); // Merges the carArray node with m_heap.

  }

  delete[] carArray; // Deletes the dynamically allocated array.


}

/*
Function Name: traverse
Desc: Goes through the heap and copies every node into an array.
Preconditions: None.
Postconditions: All of the nodes are stored as pointers in an array.
*/

int MQueue::traverse(Node* aNode, Node** carArray, int index){

  if (aNode == NULL){ // If aNode is a nullptr we just return the index.

    return index;

  }

  else{ // Otherwise, we go to the left child until we hit a nullptr and set the index to that, then we set the index to the node. Then we increment the index number and then go to the right side.

    index = traverse(aNode->m_left, carArray, index); // Goes to the leftmost node.

    carArray[index] = aNode; // Sets the index for the carArray to the node.
    
    index++; // Increments the index.
    
    index = traverse(aNode->m_right, carArray, index); // Goes to the right child of the node.

  }

  return index; // Return the index for the array that the node is located.
  
}

// overloaded insertion operator for Order class
ostream& operator<<(ostream& sout, const Order& order) {
  sout << "Order: " << order.getCustomer() << ", PM priority: " << order.getPMPriority()
       << ", quantity: " << order.getQuantityValue() 
       << ", material availability: " << order.getAvailabilityProbablity() << "%"
       << ", arrival time: " << order.getArrivalTime();
  return sout;
}

// overloaded insertion operator for Node class
ostream& operator<<(ostream& sout, const Node& node) {
  sout << node.getOrder();
  return sout;
}

// for debugging
void MQueue::dump() const
{
  if (m_size == 0) {
    cout << "Empty skew heap.\n" ;
  } else {
    dump(m_heap);
    cout << endl;
  }
}

// for debugging
void MQueue::dump(Node *pos) const {
  if ( pos != nullptr ) {
    cout << "(";
    dump(pos->m_left);
    cout << m_priorFunc(pos->m_order) << ":" << pos->m_order.getCustomer();
    dump(pos->m_right);
    cout << ")";
  }
}

