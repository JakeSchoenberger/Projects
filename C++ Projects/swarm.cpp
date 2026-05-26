//UMBC - CSEE - CMSC 341 - Fall 2021 - Proj2
#include "swarm.h"

/*
Function Name: Swarm constructor
Desc: Creates a swarm object with a NULL root.
Preconditions: None.
Postconditions: A new swarm object is created.
*/

Swarm::Swarm(){

  m_root = NULL; // Initializes m_root to a null pointer.
  
}

/*
Function Name: Swarm deconstructor
Desc: Deallocates all memory within a swarm object.
Preconditions: A swarm object exists.
Postconditions: All robots in the swarm object are deleted.
*/

Swarm::~Swarm(){

  clear(); // Calls the clear function to deallocate all memory.

}

/*
Function Name: insert
Desc: Tries to insert a robot object, to do so will call the recursive insert function.
Preconditions: None.
Postconditions: A new robot is inserted into the swarm.
*/

void Swarm::insert(const Robot& robot){

  if (findBot(robot.getID()) == true){ // If there is already an existing robot with that id, we do nothing.

    return;

  }

  else{ // If there isn't we insert starting at the root, update the height of the root and rebalance at the root.
    
    insert(m_root, robot); // Call the insert function on the root that will recursively go through the AVL Tree.
    updateHeight(m_root);
    rebalance(findBot(robot.getID(), m_root));
    
  }
  
}

/*
Function Name: insert (recursive)
Desc: Recursively calls the insert function to put a robot in its proper position in the swarm.
Preconditions: None.
Postconditions: A robot is inserted to its correct position depending on its ID value.
*/

Robot* Swarm::insert(Robot*& aBot, const Robot& newBot){

  if (m_root == NULL){ // If the tree has no root, create a new robot and set the root to the new robot created.

    Robot* newRoot = new Robot(newBot.getID(), newBot.getType(), newBot.getState());
    m_root = newRoot;
    return newRoot;

  }

  
  else if (aBot == NULL){ // If we are at a NULL pointer, we are at a place to add the new robot, so create a new one in the position and return the newly created robot.

    Robot* newRobot = new Robot(newBot.getID(), newBot.getType(), newBot.getState());
    aBot = newRobot;
    return aBot;
    
  }
  
  else if (newBot.getID() < aBot->getID()){ // If the id is less than the current robot's id in the Tree, we go to the left child.

    aBot->setLeft(insert(aBot->m_left, newBot));
    
    updateHeight(aBot); // Update the height after each insertion.
    
    return rebalance(aBot); // Rebalance after each insertion.
        
  }
  
  else if (newBot.getID() > aBot->getID()){ // If the id is more than the current robot's id in the Tree, we go to the right child.

    aBot->setRight(insert(aBot->m_right, newBot));
    
    updateHeight(aBot); // Update the height after each insertion.
    
    return rebalance(aBot); // Rebalance after each insertion.
    
  }

  else{ // If we find a node that already based on an already existing id, we just return that bot.
    
    return aBot;
    
  }

  return NULL; // If all else fails, return NULL.
  
}

/*
Function Name: clear
Desc: Clears the swarm by calling the recursive clear function to delete every robot in the tree.
Preconditions: None.
Postconditions: The tree becomes empty.
*/

void Swarm::clear(){

  clear(m_root); // Calls the clear function starting at m_root, we will then recursively call that function to delete every node.
  
}

/*
Function Name: clear (recursive)
Desc: Traverses through the list and deletes in post-order traversal.
Preconditions: The tree has robots.
Postconditions: The tree has no robots.
*/

void Swarm::clear(Robot* aBot){

  if (aBot != NULL){ // Checks to see if aBot is a null pointer, if it isn't we continue.
    
    if (aBot->m_left != NULL){ // If the left child of aBot is not a null pointer we recursively call the clear function on the left child.

      clear(aBot->m_left);
  
    }
    
    if (aBot->m_right != NULL){ // If the right child of aBot is not a null pointer we recursively call the clear function on the right child.

      clear(aBot->m_right);

    }

  }

  delete aBot;
  
  return;
  
}

/*
Function Name: remove
Desc: Calls the recursive remove function on a tree.
Preconditions: None.
Postconditions: A specified robot will be removed from the tree.
*/

void Swarm::remove(int id){

  remove(m_root, id); // Calls a recursive remove function that utilizes the root so we can go through the tree.

}

/*
Function Name: remove (recursive)
Desc: Traverses through the tree to find a robot with a specified ID and if it does, we remove it from the tree.
Preconditions: The robot with the id exists.
Postconditions: The robot is removed from the tree.
*/

Robot* Swarm::remove(Robot* aBot, const int& id){

  if (findBot(id) == false){ // If the robot does not exist within the tree, we return NULL.

    return NULL;

  }

  if (aBot == NULL){ // If aBot is null, we return NULL, as there is no node we can remove.
  
    return NULL;

  }

  else{ // If we pass those tests, we will recursively go through the tree to find the correct one we want to remove.

    if (id < aBot->getID()){ // If the id of the bot we are looking for is less than the id of the current bot in the tree, we set the left child to the recursive call of remove for the current bot.
      
      aBot->setLeft(remove(aBot->getLeft(), id));
      
    }
    
    else if (id > aBot->getID()){ // If the id of the bot we are looking for is greater than the id of the current bot in the tree, we set the right child to the recursive call of remove for the current bot.
      
      aBot->setRight(remove(aBot->getRight(), id));
      
    }

    else if (aBot->getLeft() != NULL && aBot->getRight() != NULL){ // If the bot has two children we have to find the maximum value of its left subtree and take its values and set it to the current bot. Then we recursively call and set the left child.
      
      Robot* botToRemove = findMax(aBot->getLeft());
      
      aBot->setID(botToRemove->getID());
      aBot->setType(botToRemove->getType());
      aBot->setState(botToRemove->getState());
      
      aBot->setLeft(remove(aBot->getLeft(), aBot->getID()));
      
    }
    
    else{ // If the current bot has only one or zero children, we check what child it has, if any. And set the current bot to the child that exists. Then we delete the bot we were supposed to remove.
      
      Robot* botToRemove = aBot;
      
      if (aBot->getLeft() != NULL){
	
	aBot = aBot->getLeft();
	
      }
      
      else if (aBot->getRight() != NULL){
	
	aBot = aBot->getRight();
	
      }
      
      else if (aBot == botToRemove){

	aBot = NULL;

      }
	
      delete botToRemove;
      
    }
    
    updateHeight(aBot); // Updates the height after every removal.

    aBot = rebalance(aBot); // Rebalances the tree after ever removal.

    return aBot; // Returns the bot in the location that belonged to the node that was deleted.
         
  }

}

/*
Function Name: updateHeight
Desc: Updates the height of a robot based on its child bots and adds 1 to the highest child height.
Preconditions: None.
Postconditions: The height of the node will be updated.
*/

void Swarm::updateHeight(Robot* aBot){
 
  int leftHeight = -1; // We want to set leftheight and rightheight to -1, so when we add 1 in the case that it is a leaf node the height is 0.
  int rightHeight = -1;
  
  if (aBot == NULL){ // If aBot is a null pointer we return because we don't want to check the left or right children since they don't exist.

    return;

  }

  if (aBot->getLeft() != NULL){ // If aBot has a left child, we set the leftHeight to the height of the left child. 

    leftHeight = aBot->getLeft()->getHeight();

  }

  if (aBot->getRight() != NULL){ // If aBot has a right child, we set the rightHeight to the height of the right child.

    rightHeight = aBot->getRight()->getHeight();

  }
  
  if (leftHeight > rightHeight){ // If the leftHeight is greater than the rightHeight we set the height of aBot to the leftHeight + 1, we always want to consider the highest height.

    aBot->setHeight(leftHeight + 1);

  }

  else if (rightHeight > leftHeight){ // If the rightHeight is greater than the leftHeight we set the height of aBot to the rightHeight + 1, we always want to consider the highest height.

    aBot->setHeight(rightHeight + 1);

  }

  else{ // If the leftHeight and rightHeight are the same, it is irrelevant which one we choose to represent the height for aBot, so we just choose left.

    aBot->setHeight(leftHeight + 1);

  }
  
}

/*
Function Name: checkImbalance
Desc: Checks to see if the children of a robot are of too big of a height difference (more than one).
Preconditions: None.
Postconditions: We find if the tree needs to be rebalanced.
*/

int Swarm::checkImbalance(Robot* aBot){

  if (aBot == NULL){ // If aBot is a NULL pointer, we can't check anything, so we just return -1.

    return -1;

  }

  else { // If aBot is not a NULL pointer, we set the left and right heights to -1.
    
    int leftHeight = -1;
    int rightHeight = -1;
    
    if (aBot->getLeft() != NULL){ // If the node has a left child, we set the left height to the height of the left child.
      
      leftHeight = aBot->getLeft()->getHeight();
      
    }
    
    if (aBot->getRight() != NULL){ // If the node has a right child, we set the right height to the height of the left child.
      
      rightHeight = aBot->getRight()->getHeight();
      
    }
    
    int heightDifference = leftHeight - rightHeight; // We want the height difference, and depending on if it is positive or negative, we know which way we need to rebalance.
    
    return heightDifference;

  }

}

/*
Function Name: rebalance
Desc: We fix any imabalances found by checkImbalance depending on the structure of the tree. 
Preconditions: There is an imbalance in the tree.
Postconditions: The tree is rebalanced so it preserves the AVL property.
*/

Robot* Swarm::rebalance(Robot* aBot){

  if (checkImbalance(aBot) > 1){ // If the imbalance is > 1 we know that it is left heavy.

    if (checkImbalance(aBot->getLeft()) >= 0){ // If the left subtree is also left heavy we only need to do a right rotation.

      return rightRotation(aBot);

    }

    else if (checkImbalance(aBot->getLeft()) <= 0){ // If the left subtree is right heavy, however, we need to do a double left-right rotation.

      aBot->setLeft(leftRotation(aBot->getLeft()));
      return rightRotation(aBot);
      
    }

  }

  else if (checkImbalance(aBot) < -1){ // This is the case if we have a right heavy grandparent bot.

    if (checkImbalance(aBot->getRight()) <= 0){ // If the right subtree is also right heavy, all we have to do is a left rotation.

      return leftRotation(aBot);

    }

    else if (checkImbalance(aBot->getRight()) >= 0){ // However, if the right subtree is left heavy, we have to do a double right-left rotation.

      aBot->setRight(rightRotation(aBot->getRight()));
      return leftRotation(aBot);

    }

  }

  else{ // If there exists no imbalance, just return the bot as it is.

    return aBot;

  }

  return NULL; // If all else fails return NULL.
  
}

/*
Function Name: leftRotation
Desc: A type of rotation used to rebalance a tree to preserve AVL property, it changes the right child of the robot where there is an imbalance as well as the robot itself.
Preconditions: There is an imbalance in the tree that requires a left rotation.
Postconditions: The leftRotation balances, or helps in balancing the tree.
*/

Robot* Swarm::leftRotation(Robot* aBot){

  Robot* grandparent = aBot; // Set the grandparent to aBot.
  Robot* parent = aBot->getRight(); // Set the right child of aBot to the parent, this is because we are doing a left rotation.

  if (grandparent == m_root){ // If the grandparent is the root, we have to make sure to change m_root to the parent.

    m_root = parent;

  }

  grandparent->setRight(parent->getLeft()); // Set the grandparent's right child to the parent's left child.

  parent->setLeft(grandparent); // Set the parent's left child to the grandparent.

  updateHeight(grandparent); // Update the height of the grandparent and parent now that we have rearranged them.

  updateHeight(parent);

  return parent; // Return the parent bot in its new position in the tree.

}

/*
Function Name: rightRotation
Desc: A type of rotation used to rebalane a tree to preserve AVL property, it changes the right child of the robot where there is an imbalance as well as the robot itself.
Preconditions: There is an imbalance in the tree that requires a right rotation.
Postconditions: The rightRotation balances, or helps in balancing the tree.
*/

Robot* Swarm::rightRotation(Robot* aBot){

  Robot* grandparent = aBot; // Set the grandparent to aBot.
  Robot* parent = aBot->getLeft(); // Set the left child of aBot to the parent, this is because we are doing a right rotation.

  if (grandparent == m_root){ // If the grandparent is the root of the tree, set the root to the parent.

    m_root = parent;

  }
  
  grandparent->setLeft(parent->getRight()); // Set the grandparent's left child to the parent's right child.

  parent->setRight(grandparent); // Set the parent's right child to the grandparent.

  updateHeight(grandparent); // Update the heights of the grandparent and the parent to now that we have rearranged them.

  updateHeight(parent);

  return parent; // Return the parent bot in its new position in the tree.

}

/*
Function Name: listRobots
Desc: Calls the recursive listRobots function that will show all the robots in the tree.
Preconditions: None.
Postconditions: We will output all the nodes in ascending order.
*/

void Swarm::listRobots() const{

  listRobots(m_root); // Call the recursive listRobots from the root of the tree.
  
}

/*
Function Name: listRobots (recursive)
Desc: Traverses through the tree to show the id of every robot in ascending order.
Preconditions: None.
Postconditions: All the robots are outputted in ascending order.
*/

void Swarm::listRobots(Robot* aBot) const{

  if (aBot == NULL){ // If aBot is a null pointer, we just want to return.

    return;

  }

  else{ // If aBot is not a null pointer, we want to traverse through the list recursively and then show the ID, state, and type of each bot from left to right.

    if (aBot->getLeft() != NULL){ // If the left child is not a null pointer, we recursively call the function on the left child.

      listRobots(aBot->getLeft());

    }

    cout << aBot->getID() << ":" << aBot->getStateStr() << ":" << aBot->getTypeStr() << endl; // The output statement of each bot. We do it before checking the right bot, because it has a higher id value.

    if (aBot->getRight() != NULL){ // If the right child is not a null pointer, we recursively call the function on the right child.

      listRobots(aBot->getRight());

    }

  }

}

/*
Function Name: setState
Desc: Tries to set the state of the robot with the id parameter, if it does we return true.
Preconditions: The robot with that id exists.
Postconditions: We set the state to the state parameter.
*/

bool Swarm::setState(int id, STATE state){

  if (findBot(id) == true){ // We check if the bot exists in the Tree, if it does, we set the state to the state parameter. Then we return true.

    findBot(id, m_root)->setState(state);

    return true;
    
  }

  else{ // If the bot does not exist in the Tree, we return false because it was not successful.

    return false;

  }
  
}

/*
Function Name: removeDead
Desc: Calls the recursive removeDead function.
Preconditions: None.
Postconditions: All robots with a DEAD state will be removed from the tree.
*/

void Swarm::removeDead(){

  removeDead(m_root); // We recursively will call removeDead starting with the root so we can traverse through the Tree.
  
}

/*
Function Name: removeDead (recursive)
Desc: Goes throughout the tree to delete any robot with a DEAD state.
Preconditions: There are DEAD bots.
Postconditions: All of the dead bots are removed from the tree.
*/

void Swarm::removeDead(Robot* aBot){

  if (aBot->m_left != NULL){ // If aBot has a left child, we will go to the left child through a recursive call.

    removeDead(aBot->m_left);

  }

  if (aBot->m_right != NULL){ // If aBot has a right child we will go to the right child through a recursive call.

    removeDead(aBot->m_right);

  }

  if (aBot->getStateStr() == "DEAD"){ // If aBot is dead, we remove it from the Tree.

    remove(aBot->getID());

  }

}

/*
Function Name: findBot
Desc: Calls the findBot recursive function, and if it finds a bot, we return true.
Preconditions: None.
Postconditions: We can tell if that bot already exists in the tree.
*/

bool Swarm::findBot(int id) const {
  
  if (findBot(id, m_root) == NULL){ // If after traversing the tree, we don't find it, return false.

    return false;

  }

  else{ // If we find the node, return true.

    return true;

  }
  
}

/*
Function Name: findBot (recursive)
Desc: Looks for the bot with the id parameter, traverses the tree until it finds it.
Preconditions: None.
Postconditions: The bot is either found or we return NULL because there is no bot in the tree already.
*/

Robot* Swarm::findBot(int id, Robot* aBot) const {

  if (aBot == NULL){ // If aBot is a null pointer, that means we have reached a leaf node, meaning we have reached an endpoint and the bot does not exist.

    return NULL;

  }

  else{ // Otherwise, if the id is less than the id of aBot and a left child exists we will go to the left child.

    if (id < aBot->getID()){

      return findBot(id, aBot->getLeft());
      
    }
    
    else if (id > aBot->getID()){ // If the id is more than the id of aBot and a right child exists we will go to the right child.

      return findBot(id, aBot->getRight());
      
    }
    
    else if (id == aBot->getID()){ // If the id of aBot is equal to the id we are searching for, return true.

      return aBot;
      
    }
    
  }

  return NULL; // If all else fails, return NULL.
  
}

/*
Function Name: findMax
Desc: Looks for the maximum value in the tree by going right until there is not another right child.
Preconditions: None.
Postconditions: We find the Robot with the highest ID in the tree.
*/

Robot* Swarm::findMax(Robot* aBot){

  if (aBot == NULL){ // If the bot we are looking at is a NULL pointer, just return NULL.

    return NULL;

  }

  else{ // Otherwise, if there is a right child, go to it and recursively call the function until we are fully right, which would be the maximum value.
    
    if(aBot->getRight() != NULL){
      
      return findMax(aBot->getRight());
      
    }
    
    else{ // When there isn't a right child return the current node.
      
      return aBot;
      
    }
    
  }
  
}

/*
Function Name: dumpTree
Desc: Outputs the entire tree showing each bot's ID and height.
Preconditions: The tree is populated.
Postconditions: The entire tree is shown with all of the individual bot's ids and heights.
*/

void Swarm::dumpTree() const {
    dump(m_root);
}

/*
Function Name: dump
Desc: Outputs every robot in the swarm by their ids and their heights.
Preconditions: The tree is populated.
Postconditions: Every individual node is outputted with their id and height.
*/

void Swarm::dump(Robot* aBot) const{
    if (aBot != nullptr){
        cout << "(";
        dump(aBot->m_left);//first visit the left child
        cout << aBot->m_id << ":" << aBot->m_height;//second visit the node itself
        dump(aBot->m_right);//third visit the right child
        cout << ")";
    }
}
