// CMSC 341 - Fall 2021 - Project 4
#include "hash.h"
#include "math.h"

/*
Function Name: Overloaded Constructor
Desc: Creates the HashTable object with initialized member variables.
Preconditions: None.
Postconditions: A HashTable object is created.
*/

HashTable::HashTable(unsigned size, hash_fn hash){

  if (size < MINPRIME){ // If the size is less than minprime we set m_capacity1 to minprime.

    m_capacity1 = MINPRIME;

  }

  else if (size > MAXPRIME){ // If the size is greater than maxprime we set m_capacity1 to maxprime.

    m_capacity1 = MAXPRIME;

  }

  else if (isPrime(size) == false){ // If the size is not a prime number, we set it to the next prime number.

    m_capacity1 = findNextPrime(size);

  }

  else{ // Otherwise, m_capacity1 just becomes the size.

    m_capacity1 = size;

  }

  m_hash = hash;

  m_table1 = new File[m_capacity1]; // Create a new table.

  for (int i = 0; i < m_capacity1; i++){ // Fill the new table with empty files.

    m_table1[i] = EMPTY;

  }

  // Initialize all the member variables.
  
  m_newTable = TABLE1;

  m_numDeleted1 = 0;

  m_numDeleted2 = 0;

  m_size1 = 0;

  m_size2 = 0;

  m_table2 = NULL;

  m_timesRehashed = 0;

  m_isRehash = false;
  
}

/*
Function Name: HashTable Destructor
Desc: Deallocates all memory in the Hash Table.
Preconditions: Memory is allocated in the Hash Table.
Postconditions: We free all allocated memory.
*/

HashTable::~HashTable(){

  if (m_table1 != NULL){ // If there is a table and there is a file in the table, delete it.
    
    delete [] m_table1;

  }

  if (m_table2 != NULL){ // If there is a table and there is a file in the table, delete it.
    
    delete [] m_table2;

  }
  
}

/*
Function Name: getFile
Desc: Searches to see if a file exists, if it does we return the file.
Preconditions: None.
Postconditiosn Returns a file if found, if not, returns empty.
*/

File HashTable::getFile(string name, unsigned int diskBlock){

  if (m_table1 != NULL){ // If m_table1 exists we will give it an index from the hash function and search for a file with the same name and diskBlock in that index.

    for (int i = 0; i < m_capacity1; i++){

      unsigned int index = ((m_hash(name) % m_capacity1) + (i * i)) % m_capacity1;

      if (m_table1[index].key() == name && m_table1[index].diskBlock() == diskBlock){ // If the file has the same key and the same diskBlock as the file we are looking for, return the file.
	
	return m_table1[index];
	
      }

    }
      
  }

  if (m_table2 != NULL){ // Do the same thing but we must also check m_table2.

    for (int j = 0; j < m_capacity2; j++){
      
      unsigned int index = ((m_hash(name) % m_capacity2) + (j * j)) % m_capacity2;
      
      if (m_table2[index].key() == name && m_table2[index].diskBlock() == diskBlock){
	
	return m_table2[index];
	
      }
      
    }
    
  }

  return EMPTY; // If we didn't find it, return an empty file.
    
}

/*
Function Name: insert
Desc: Inserts a file into the table we are working in as long as it doesn't already exist.
Preconditions: None.
Postconditions: A new file has been inserted into the table.
*/

bool HashTable::insert(File file){

  if (getFile(file.key(), file.diskBlock()).key() != "" && getFile(file.key(), file.diskBlock()).diskBlock() != 0){ // If the file's key and diskBlock already exist, we cannot insert it.

    return false;

  }

  else{ // Otherwise we see where we need to insert it, and insert it.

    if (m_newTable == TABLE1){ // If the table we are inserting to is m_table1.

      for (int i = 0; i < m_capacity1; i++){ // Until we find the correct index, we keep looking for it.

	int index = ((m_hash(file.key()) % m_capacity1) + (i * i)) % m_capacity1; // Quadratic probing.
	
	if (m_table1[index].diskBlock() == 0){ // If the index is empty.

	  if (file.diskBlock() >= DISKMIN && file.diskBlock() <= DISKMAX){ // Makes sure that the diskBlock value is between DISKMIN and DISKMAX.

	    m_table1[index] = file; // Inserts the file.

	    if (m_isRehash == false && shouldRehash(TABLE1) == true){ // Checks if we should be rehashing.

	      rehash(TABLE1, m_size1); // rehashes the table.

	    }

	    m_size1++; // Increases m_size1.
	    
	    return true; // Returns true because we have inserted the file.
	    	    
	  }

	}

      }

    }

    else if (m_newTable == TABLE2){ // The same thing occurs as TABLE1, however, we check if we must insert to TABLE2 instead.

      for (int j = 0; j < m_capacity2; j++){

      	int index = ((m_hash(file.key()) % m_capacity2) + (j * j)) % m_capacity2;
	
	if (m_table2[index].diskBlock() == 0){

	  if (file.diskBlock() >= DISKMIN && file.diskBlock() <= DISKMAX){

	    m_table2[index] = file;

	    if (shouldRehash(TABLE2) == true && m_isRehash == false){
	      
	      rehash(TABLE2, m_size2);
	      
	    }
	    
	    m_size2++;
	    
	    return true;
	    
	  }
	  
	}
	
      }
      
    }
    
  }

  return false;
  
}

/*
Function Name: remove
Desc: Takes a file out of the Hash Table.
Preconditions: None.
Postconditions: The file is removed from the Hash Table.
*/

bool HashTable::remove(File file){

  if (m_table1 != NULL){ // If m_table1 exists we will check to see if we can find the right file, so we can remove it.

    for (int i = 0; i < m_capacity1; i++){ // Until we find the index we need we will continue to look for it.

      int index = ((m_hash(file.key()) % m_capacity1) + (i * i)) % m_capacity1; // Quadratic probing.

      if (m_table1[index].diskBlock() == file.diskBlock()){ // If the diskBlock is the one we are searching for We delete it.

	m_table1[index] = DELETED;

	m_numDeleted1++;

	if (m_isRehash == false && shouldRehash(TABLE1) == true){ // If this deletion leads us to rehash, we rehash.

	  rehash(TABLE1, m_size1);

	}

	return true;

      }

    }

  }

  if (m_table2 != NULL){ // The same thing that happened with m_table1, however, we must also check m_table2.

    for (int j = 0; j < m_capacity2; j++){

      int index = ((m_hash(file.key()) % m_capacity2) + (j * j)) % m_capacity2;

      if (m_table2[index].diskBlock() == file.diskBlock()){

        m_table2[index] = DELETED;

	m_numDeleted2++;

	if (m_isRehash == false && shouldRehash(TABLE2) == true){

	  rehash(TABLE1, m_size2);

	}

	return true;

      }

    }
      
  }

  return false; // If we could not find a file to remove, just return false.
  
}

/*
Function Name: lambda
Desc: Returns the load factor of the table we are working in.
Preconditions: None.
Postconditions: We find out what the load factor is to determine if we need to rehash.
*/

float HashTable::lambda(TABLENAME tablename) const {

  if (tablename == TABLE1){ // If we are using m_table1, find the float value of m_size1/m_capacity1 to get the load factor.

    return float(m_size1)/m_capacity1;

  }

  else if (tablename == TABLE2){ // If we are using m_table2, find the float value of m_size1/m_capacity1 to get the load factor.

    return float(m_size2)/m_capacity2;

  }
  
}

/*
Function Name: deletedRatio
Desc: Checks the Deleted Ratio of the table we are working in to see if we need to rehash.
Preconditions: None.
Postconditions: We find out if we should rehash because our deletedRatio is too high.
*/

float HashTable::deletedRatio(TABLENAME tableName) const {
  
  if (tableName == TABLE1){ // If we are using m_table1, find the float value of m_numDeleted1/m_size1 to get the Deleted Ratio.

    return float(m_numDeleted1)/m_size1;

  }

  else if (tableName == TABLE2){ // If we are using m_table2, find the float value of m_numDeleted2/m_size2 to get the Deleted Ratio.

    return float(m_numDeleted2)/m_size2;

  }
  
}

void HashTable::dump() const {
    cout << "Dump for table 1: " << endl;
    if (m_table1 != nullptr)
        for (int i = 0; i < m_capacity1; i++) {
            cout << "[" << i << "] : " << m_table1[i] << endl;
        }
    cout << "Dump for table 2: " << endl;
    if (m_table2 != nullptr)
        for (int i = 0; i < m_capacity2; i++) {
	  cout << "[" << i << "] : " << m_table2[i] << endl;
        }
}

bool HashTable::isPrime(int number){
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

int HashTable::findNextPrime(int current){
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME-1;
    for (int i=current; i<MAXPRIME; i++) { 
        for (int j=2; j*j<=i; j++) {
            if (i % j == 0) 
                break;
            else if (j+1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}

/*
Function Name: shouldRehash
Desc: Checks to see if we should rehash or not, based on load factor, the deleted ratio, or if we are in the middle of rehashing currently.
Preconditions: None.
Postconditions: We know whether to rehash or not.
*/

bool HashTable::shouldRehash(TABLENAME tableName){

  if (m_timesRehashed > 0 || lambda(tableName) >= 0.5 || deletedRatio(tableName) >= 0.8) { // If we are in the process of rehashing, the load factor is equal to or over 0.5, or the deleted ratio is equal to or over 0.8, we return true so we rehash.

    return true;

  }

  else{ // Otherwise return false.

    return false;

  }

}

/*
Function Name: rehash
Desc: Rehashes the table to m_table2.
Preconditions: None.
Postconditions: We properly rehash m_table1 incrementally to m_table2.
*/

void HashTable::rehash(TABLENAME tableName, int size){

  if (m_newTable == TABLE1){ // If the table we are working with is table1, we must use those variables.

    if (m_table2 == NULL){ // If m_table2 is not created yet, we must create it.
      
      m_capacity2 = findNextPrime((m_size1-m_numDeleted1)*4); // We make a larger table based on the capacity of the already existing m_table1.
	
      m_table2 = new File[m_capacity2];
      
      for (int i = 0; i < m_capacity2; i++){ // Fill the table with empty files.
	
	m_table2[i] = EMPTY;
	  
      }
	
      m_newTable = TABLE2; // Set the new table we work in to TABLE2.
       
    }

  }
    
  else if (m_timesRehashed == 4){ // If we have rehashed fully, we must delete the old table and reset all of the member variables.
      
    delete [] m_table1;
      
    m_size1 = 0;
      
    m_capacity1 = 0;
    
    m_timesRehashed = 0;
      
    m_isRehash = false;
      
  }

  else{

    m_isRehash = true; // Lets the program know that we are currently rehashing, as to not have it happen while we insert into the new table.
    
    int counter = 0; // Will count how many files get inserted into the new table.
    
    int oneFourthSize = (m_size1/4)*(m_timesRehashed + 1); // How many files we should add each time to the new table.
    
    int infiniteStop = 0; // Works to make sure we don't have an infinite while loop.
    
    while (counter < oneFourthSize && infiniteStop < m_capacity1){ // Checks to make sure we are under oneFourth of our size (25%) and we haven't gone over m_capacity1.
      
      for (int i = (m_size1/4)*(m_timesRehashed); i < m_capacity1; i++){ // We go through the table and check a file each time to see if it isn't empty, if it isn't we add it to the new table.
	
	if (m_table1[i].diskBlock() != 0){ // Checks to see if the file is empty.
	  
	  File fileToInsert = m_table1[i]; // This is the non-empty file we want to insert.
	  
	  m_table1[i] = DELETED; // Set the file to deleted in the original table.
	  
	  insert(fileToInsert); // Inserts the file into the new working hash table.
	  
	  counter++; // Increments the counter for how many files have been added. 
	  
	}
	
	infiniteStop++; // Increases the counter of files we've looked at.
	
      }
      
    }
    
    m_isRehash = false; // Once we finish this we set m_isRehash back to false so that we can rehash again on our next insert.
    
    m_timesRehashed++; // We increment the number of times rehashed so when we get to 4 (100%) the program knows to stop rehashing.
    
  }
  
}

