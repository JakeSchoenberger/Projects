"""

File: pyopoly.py
Author: Jacob Schoenberger
Date: 10/19/2020
Section: 24
E-mail: jacobs9@umbc.edu
Description: We make monopoly using a multitude of coding tools, but most prominently utilizing dictionaries. 

"""

from sys import argv
from random import randint, seed
from board_methods import load_map, display_board

STARTING_MONEY = 1500
PASS_GO_MONEY = 200

def play_game(starting_money, pass_go_money):
    board = []
    players = {}
    player1Owned = []
    player2Owned = []
    bankOwned = []
    firstPlayerInitial = ""
    secondPlayerInitial = ""
    firstPlayerPosition = 0
    secondPlayerPosition = 0
    whoseTurn = True
    readBoard = load_map(input("What file do you want to load? "))
    firstPlayerName = input("First player, what is your name? ")
    while(len(firstPlayerInitial) != 1):
        firstPlayerInitial = input(firstPlayerName + ", what initial do you want to use? (Only one letter.) ")
    secondPlayerName = input("Second player, what is your name? ")
    while(len(secondPlayerInitial) != 1):
        secondPlayerInitial = input(secondPlayerName + ", what initial do you want to use? (Only one letter.) ")
    format_display(readBoard, board)
    boardCopy = list(board)
    board[secondPlayerPosition] += secondPlayerInitial
    player1Money = STARTING_MONEY
    player2Money = STARTING_MONEY
    players = {
        "player1" : {
            "playerName" : firstPlayerName,
            "Owned Properties" : player1Owned,
            "playerMoney" : player1Money,
            "playerInitial" : firstPlayerInitial
            },
        "player2" : {
            "playerName" : secondPlayerName,
            "Owned Properties" : player2Owned,
            "playerMoney" : player2Money,
            "playerInitial" : secondPlayerInitial
            },
        "BANK" : {
            "Owned Properties" : bankOwned
            }
        }

    # Above are all of our important variables, the most important however is the dictionary for players, this is what keeps a lot of our information together and we reference it throughout the rest of the code. 
    
    while(int(players["player1"]["playerMoney"]) > 0 and int(players["player2"]["playerMoney"]) > 0):
        diceRoll = roll_the_dice()
        if(whoseTurn == True):
            firstPlayerPosition += diceRoll
            if(firstPlayerPosition >= len(board)):
                firstPlayerPosition -= len(board)
                players["player1"]["playerMoney"] += PASS_GO_MONEY
            for l in range(0, len(board)):
                if(l != secondPlayerPosition):
                    board[l] = boardCopy[l]
            movedPlayer = board_movement(firstPlayerInitial, board, firstPlayerPosition)
            display_board(board)
            if(readBoard[firstPlayerPosition]["Place"] in players["player2"]["Owned Properties"]):
                print("You landed on", secondPlayerName + "'s", "property, you must pay the rent.")
                print(firstPlayerName, "paid", secondPlayerName, readBoard[firstPlayerPosition]["Rent"], "in due rent." + "\n")
                players["player1"]["playerMoney"] -= int(readBoard[firstPlayerPosition]["Rent"])
                players["player2"]["playerMoney"] += int(readBoard[firstPlayerPosition]["Rent"])
            take_turn(firstPlayerName, players, firstPlayerPosition, readBoard, diceRoll)
            whoseTurn = False

        # Above is our while loop to determine when the game ends if someone's money hits below 0 or 0 we always roll the dice and then we use the whoseTurn variable to switch between each player's turns. We first check if the player landed on another players space so they have to pay rent, then we let them take their turn with the take_turn function, below is the same for player2 however we change the values of the dictionary to fit player2's parameters.  

        elif(whoseTurn == False):
            secondPlayerPosition += diceRoll
            if(secondPlayerPosition >= len(board)):
                secondPlayerPosition -= len(board)
                players["player2"]["playerMoney"] += PASS_GO_MONEY
            for m in range(0, len(board)):
                if(m != firstPlayerPosition):
                    board[m] = boardCopy[m]
            movedPlayer = board_movement(secondPlayerInitial, board, secondPlayerPosition)
            display_board(board)
            if(readBoard[secondPlayerPosition]["Place"] in players["player1"]["Owned Properties"]):
                print("You landed on", firstPlayerName + "'s", "property, you must pay the rent.")
                print(secondPlayerName, "paid", firstPlayerName, readBoard[secondPlayerPosition]["Rent"], "in due rent." + "\n")
                players["player2"]["playerMoney"] -= int(readBoard[secondPlayerPosition]["Rent"])
                players["player1"]["playerMoney"] += int(readBoard[secondPlayerPosition]["Rent"])
            take_turn(secondPlayerName, players, secondPlayerPosition, readBoard, diceRoll)
            whoseTurn = True

    if(players["player1"]["playerMoney"] <= 0):
        print(firstPlayerName, "has run out of funds!", secondPlayerName, "wins!")

    elif(players["player2"]["playerMoney"] <= 0):
        print(secondPlayerName, "has run out of funds!", firstPlayerName, "wins!")

        # Above are the two scenarios of how someone loses. 


def take_turn(player, players, playerPosition, board, diceRoll):
    bankProperties = []
    for i in range(0, len(board)):
        if(board[i]["Place"] not in players["player1"]["Owned Properties"] and board[i]["Place"] not in players["player2"]["Owned Properties"]):
            bankProperties.append(board[i]["Place"])
    userDecision = ""
    print(player, "you have rolled a", diceRoll)
    print(player, "you landed on", board[playerPosition]["Place"] + "\n")

    while(userDecision != "5"):
        print("\n" + "1. Buy Property" + "\n" + "2. Get Property Info" + "\n" + "3. Get Player Info" + "\n" + "4. Build a building" + "\n" + "5. End Turn" + "\n")
        userDecision = input("What do you want to do?" + "\n")
        if(userDecision != "1" and userDecision != "2" and userDecision != "3" and userDecision != "4" and userDecision != "5"):
            print("Please enter 1, 2, 3, 4, or 5.")

            # take_turn gives the user several options on what they can do. We have to keep giving the user the option to choose what they want to do until they choose 5. Then we make conditionals based on what they do pick. 
            
        elif(userDecision == "1"):
            if(board[playerPosition]["Place"] in bankProperties and int(board[playerPosition]["Price"]) > 0):
                confirmationPurchase = input(board[playerPosition]["Place"] + " is unowned, would you like to buy it? " + "\n")
                confirmationPurchase = confirmationPurchase.lower()
                if(confirmationPurchase == "yes"):
                    if(players["player1"]["playerName"] == player):
                        if(players["player1"]["playerMoney"] < int(board[playerPosition]["Price"])):
                            print("You do not have the proper funds to purchase this establishment.")
                        else:
                            players["player1"]["playerMoney"] -= int(board[playerPosition]["Price"])
                            print("You have purchased", board[playerPosition]["Place"])
                            bankProperties.remove(board[playerPosition]["Place"])
                            players["player1"]["Owned Properties"].append(board[playerPosition]["Place"])
                    elif(players["player2"]["playerName"] == player):
                        if(players["player2"]["playerMoney"] < int(board[playerPosition]["Price"])):
                            print("You do not have the proper funds to purchase this establishment.")
                        else:
                            players["player2"]["playerMoney"] -= int(board[playerPosition]["Price"])
                            print("You have purchased", board[playerPosition]["Place"])
                            bankProperties.remove(board[playerPosition]["Place"])
                            players["player2"]["Owned Properties"].append(board[playerPosition]["Place"])
            elif(int(board[playerPosition]["Price"]) < 0 or board[playerPosition]["Place"] in players["player1"]["Owned Properties"] or board[playerPosition]["Place"] in players["player2"]["Owned Properties"]):
                print("This space cannot be bought.")

                # Here we have to check if the place is available for purchase first, and if it is we have to make sure the player wants to buy it, then we must check if they have enough money to do so, then we have to take the property out of the bank's properties and put it in the player's properties. 

        elif(userDecision == "2"):
            placeInfo = input("What location do you want to know about? ")
            for i in range(0, len(board)):
                if(placeInfo == board[i]["Place"] or placeInfo == board[i]["Abbrev"]):
                    print("\n" + "Name :", board[i]["Place"] + "\n" + "Price: ", board[i]["Price"] + "\n")
                    if(board[i]["Place"] in bankProperties):
                        print("Owner: BANK" + "\n" + "Rent:", board[i]["Rent"] + ", " + board[i]["BuildingRent"], "(with a building)" + "\n")
                    elif(board[i]["Place"] in players["player1"]["Owned Properties"]):
                        print("Owner : " + players["player1"]["playerName"] + "\n" + "Rent:", board[i]["Rent"] + ", " + board[i]["BuildingRent"], "(with a building)" + "\n")
                    elif(board[i]["Place"] in players["player2"]["Owned Properties"]):
                        print("Owner : " + players["player2"]["playerName"] + "\n" + "Rent:", board[i]["Rent"] + ", " + board[i]["BuildingRent"], "(with a building)" + "\n")

        # Here we see all the information we can get for a space, we get most of it from the board, but the owner depends on who the owner is, which we determine by using conditionals. 
                        
        elif(userDecision == "3"):
            whichPlayer = ""
            while(whichPlayer != players["player1"]["playerName"] and whichPlayer != players["player2"]["playerName"]):
                print("\n" + players["player1"]["playerName"] + "\n" + players["player2"]["playerName"] + "\n")
                whichPlayer = input("Which player did you want information on? ")
                if(whichPlayer == players["player1"]["playerName"]):
                    print("Name:", players["player1"]["playerName"] + "\n" + "Game Piece:", players["player1"]["playerInitial"] + "\n" + "Funds:", str(players["player1"]["playerMoney"]))
                    print("Properties Owned:" + "\n")
                    print(", ".join(players["player1"]["Owned Properties"]))
                if(whichPlayer == players["player2"]["playerName"]):
                    print("Name:", players["player2"]["playerName"] + "\n" + "Game Piece:", players["player2"]["playerInitial"] + "\n" + "Funds:", str(players["player2"]["playerMoney"]))
                    print("Properties Owned:" + "\n")
                    print(", ".join(players["player2"]["Owned Properties"]) + "\n")

        # Here we get the information for the players from the dictionary we created previously called players, depending on what player the user says they want information on. 

        elif(userDecision == "4"):
            print("Oopsie Poopsie! This function has not been implemented yet! Please don't pick it. :)")
            print("But even if you do you'll just see me again.")
            print("Please, just visit sometime in the future...I get lonely.")

def format_display(readBoard, board):
    for i in range(0, len(readBoard)):
        placeName = readBoard[i]["Abbrev"]
        board.append(placeName)
    for j in range(0, len(board)):
        board[j] = board[j] + "\n"
    
    return(board)

# Above is the method that makes our board appear in a way that is appealing to the eye, and to make sure that it works. 

def roll_the_dice():
    dice1 = randint(1,6)
    dice2 = randint(1,6)
    diceRoll = dice1 + dice2
    
    return(diceRoll)

# Above is the method that creates a random roll for our dice to see how much our player moves. 
    
def board_movement(player, board, playerPosition):
    board[playerPosition] += player

    return(playerPosition)

# Above is our method for moving the players around the board we must keep track of the playerPosition as it is what tracks the variables of every position on the board when the player wants to interact with it. 

if len(argv) >=2:
    seed(argv[1])

if __name__ == "__main__":
    play_game(STARTING_MONEY, PASS_GO_MONEY) 

