import re

def mainMenu():

    answer = "" # Sets the answer to an empty string.

    while (answer != "exit" and answer != "Yes"): # While we don't have the keywords, ask for input.

        print("Would you like to add an event? Type Yes to add an event, or exit if you are finished.")
        answer = input() # Get user input.

    return answer
def dayMenu():

    days = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"] # List of days.
    answer = "" # Empty string for while loop.

    while(answer not in days): # While we haven't gotten a valid answer...

        print("What day would you like to add an event?")
        print("Monday\nTuesday\nWednesday\nThursday\nFriday\nSaturday\nSunday\n")
        answer = input() # Get user input.

        if(answer not in days): # If we don't get a valid response, let the user know.

            print("That is not an acceptable answer. Please try again.")

    return answer
def allDishes():

    newDish = "" # Empty string for verification.
    dishList = [] # New empty dishList.
    while (newDish != "FINISHED"): # While we can keep adding dishes to the event...

        print("What dish do you want to add to the event? Type FINISHED when done.") # Asks the user what we want to add.
        print("1. Teriyaki Salmon\n2. Filet Mignon\n3. Chicken Roulad\n4. Vegan Meal")
        newDish = input() # Takes in the dish we want to add.

        if (newDish != "FINISHED"): # If the new dish is not the keyword FINISHED, append it to the list of dishes for the event.

            dishList.append(newDish) # Add the newDish to the list.

    return dishList # Once we get the keyword, we return the list.

def guestNumber(dishList):

    guestForDish = [] # List for all the guest numbers.

    for i in dishList: # For all of the dishes...

        print("How many guests ordered " + i + "?") # Ask the user how many of each dish were ordered.
        numberOfDish = input()

        while (numberOfDish.isdigit() == False):

            print("That is not a valid response. Please try again.")
            numberOfDish = input()

        guestForDish.append(numberOfDish) # Get the amount from the user input.

    return guestForDish # Returns the list of orders for each item.

def recipes(dishList, guestNumberList):

    # Recipes for all the dishes.
    teriyakiSalmon = ["\n1/6 oz. Salmon\n", "1/4 cups teriyaki sauce\n", "1/10 oz. spinach\n", "1/10 oz. caramelized onions\n"]
    chickenRoulad = ["\n1/6 oz. Chicken Breast\n", "1/3 cups chicken juice\n", "2 tomatoes\n, 2/3 oz. butter\n"]
    filetMignon = ["\n1/5 oz. Filet Mignon\n", "1/10 oz. gravy\n", "2/5 oz. Mashed Potatoes\n", "1/6 oz. Asparagus\n"]
    veganMeal = ["\n2/3 oz. Pepper\n", "1/10 oz. Soy sauce\n", "1/3 oz. Mashed Potatoes\n", "1/6 oz. Asparagus\n"]

    # Dictionary of recipes.
    recipeDict = {"Teriyaki Salmon": ["0.167 oz. Salmon", "0.25 Cups Teriyaki Sauce", "0.1 oz. Spinach", "0.1 oz. Caramelized Onions"],
                  "Chicken Roulad": ["0.167 oz. Chicken Breast", "0.33 Cups Chicken Juice", "2.0 Tomatoes", "0.67 oz. Butter"],
                  "Filet Mignon" : ["0.2 oz. Filet Mignon", "0.1 oz. Gravy", "0.4 oz. Mashed Potatoes", "0.167 oz. Asparagus"],
                  "Vegan Meal" : ["0.67 oz. Pepper (Vegetable)", "0.1 oz. Soy Sauce", "0.33 oz. Mashed Potatoes", "0.167 oz. Asparagus"]}

    finalList = [] # List with the final strings for the menu.

    print("\n")

    for i in range(0, len(dishList)): # For all of the dishes...

        if dishList[i] in recipeDict: # If the dish is a dish on the menu...

            currentDish = "Ingredients needed for " + dishList[i] + ":\n" # Print out the dish name.

            for j in range(0, len(recipeDict[dishList[i]])): # For all the items in the recipe...

                numberFinder = re.findall(r'\d+\.\d+', recipeDict[dishList[i]][j]) # Find the number in the string.
                recipeDict[dishList[i]][j] = re.sub(r'\d+\.\d+', '', recipeDict[dishList[i]][j]) # Remove the number in the string.
                convertedNumber = float(numberFinder[0]) # Convert the number that is currently a string to a float.
                totalIngredientNumber = float("%.2f"%(convertedNumber * float(guestNumberList[i]))) # Multiply the convertedNumber to the number of guests that ordered that dish.
                currentDish = currentDish + str(totalIngredientNumber) + " " + recipeDict[dishList[i]][j] + "\n" # Adds to the string for the current dish.

            finalList.append(currentDish) # Adds the current dish to the list.

    return finalList

def weekDictionaryHelper(weekDictionary, dayList, selectedDay):

    weekDictionary[selectedDay] = dayList # Updates the dictionary to add the list from the day.

def weekSummary(weekDictionary):

    print("Summary of the week's necessary prep:\n")

    for i in weekDictionary: # For all the keys in the dictionary...

        print(i + ":") # Print the key and a colon.

        for j in weekDictionary[i]: # For all the values in the dictionary.

            print(j) # Print the values.

        print("-----------------------------------")


# Press the green button in the gutter to run the script.
if __name__ == '__main__':

    weekDictionary = {"Monday": ["None\n"], "Tuesday": ["None\n"], "Wednesday": ["None\n"], "Thursday": ["None\n"], "Friday": ["None\n"], "Saturday": ["None\n"], "Sunday": ["None\n"]}
    answer = mainMenu()

    while (answer == "Yes"): # While we continue to add events, call all of our functions.

        selectedDay = dayMenu()
        allDishesList = allDishes()
        guestNumberList = guestNumber(allDishesList)
        finalList = recipes(allDishesList, guestNumberList)
        weekDictionaryHelper(weekDictionary, finalList, selectedDay)
        answer = mainMenu()

    else:

        weekSummary(weekDictionary)

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
