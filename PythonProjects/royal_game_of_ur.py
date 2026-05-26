"""

File: royal_game_of_ur.py
Author: Jacob Schoenberger
Date: 11/15/2020
Section: 24
Email: jacobs9@umbc.edu
Description: We make the prehistoric Royal Game of Ur using classes, a lot of conditionals, and a little bit of recursion to make a fun "classic" game.

"""

from sys import argv
from random import choice
from board_square import BoardSquare, UrPiece


class RoyalGameOfUr:
    STARTING_PIECES = 7

    def __init__(self, board_file_name):
        self.whitePieces = []
        self.blackPieces = []
        self.WSYMBOL = "W"
        self.BSYMBOL = "B"
        self.BCOLOR = "Black"
        self.WCOLOR = "White"
        self.board = None
        self.load_board(board_file_name)

        # Here are all the variables we will need going forward with our board.

    def load_board(self, board_file_name):
        """
        This function takes a file name and loads the map, creating BoardSquare objects in a grid.

        :param board_file_name: the board file name
        :return: sets the self.board object within the class
        """

        import json
        try:
            with open(board_file_name) as board_file:
                board_json = json.loads(board_file.read())
                self.num_pieces = self.STARTING_PIECES
                self.board = []
                for x, row in enumerate(board_json):
                    self.board.append([])
                    for y, square in enumerate(row):
                        self.board[x].append(BoardSquare(x, y, entrance=square['entrance'], _exit=square['exit'],
                                                         rosette=square['rosette'], forbidden=square['forbidden']))

                for i in range(len(self.board)):
                    for j in range(len(self.board[i])):
                        if board_json[i][j]['next_white']:
                            x, y = board_json[i][j]['next_white']
                            self.board[i][j].next_white = self.board[x][y]
                        if board_json[i][j]['next_black']:
                            x, y = board_json[i][j]['next_black']
                            self.board[i][j].next_black = self.board[x][y]
        except OSError:
            print('The file was unable to be opened. ')

    def draw_block(self, output, i, j, square):
        """
        Helper function for the display_board method
        :param output: the 2d output list of strings
        :param i: grid position row = i
        :param j: grid position col = j
        :param square: square information, should be a BoardSquare object
        """
        MAX_X = 8
        MAX_Y = 5
        for y in range(MAX_Y):
            for x in range(MAX_X):
                if x == 0 or y == 0 or x == MAX_X - 1 or y == MAX_Y - 1:
                    output[MAX_Y * i + y][MAX_X * j + x] = '+'
                if square.rosette and (y, x) in [(1, 1), (1, MAX_X - 2), (MAX_Y - 2, 1), (MAX_Y - 2, MAX_X - 2)]:
                    output[MAX_Y * i + y][MAX_X * j + x] = '*'
                if square.piece:
                    # print(square.piece.symbol)
                    output[MAX_Y * i + 2][MAX_X * j + 3: MAX_X * j + 5] = square.piece.symbol

    def display_board(self):
        """
        Draws the board contained in the self.board object

        """
        if self.board:
            output = [[' ' for _ in range(8 * len(self.board[i // 5]))] for i in range(5 * len(self.board))]
            for i in range(len(self.board)):
                for j in range(len(self.board[i])):
                    if not self.board[i][j].forbidden:
                        self.draw_block(output, i, j, self.board[i][j])

            print('\n'.join(''.join(output[i]) for i in range(5 * len(self.board))))

    def roll_d4_dice(self, n=4):
        """
        Keep this function as is.  It ensures that we'll have the same runs with different random seeds for rolls.
        :param n: the number of tetrahedral d4 to roll, each with one dot on
        :return: the result of the four rolls.
        """
        dots = 0
        for _ in range(n):
            dots += choice([0, 1])
        return dots

    def check_if_won(self, listOfPieces):
        for i in range(0, len(listOfPieces)):
            if listOfPieces[i].complete == False:
                return(False)
        return(True)

    # Above is the function we use to check if the game is over, we check if every piece's value in one of the players' list is complete, if only one is not, it returns false, otherwise, it returns true.

    def piece_movement(self, whatMove, roll, listOfPieces, colorEntrance):

        if roll > 0:
            entranceLocation = rgu.find_entrances(self.board, colorEntrance)
            theMove = listOfPieces[whatMove - 1].can_move(roll, colorEntrance)
            oldLocation = listOfPieces[whatMove - 1].position
            
            # Here we have to check if roll is greater than 0, because if it is, we can take a turn, otherwise, it won't do anything.

            if theMove == "Starter":
                if roll == 1 and entranceLocation.piece == None:
                    for i in range(0, len(self.board)):
                        for j in range(0, len(self.board[i])):
                            if self.board[i][j].entrance == colorEntrance:
                                listOfPieces[whatMove - 1].position = self.board[i][j]
                                self.board[i][j].piece = listOfPieces[whatMove - 1]
                                roll -= 1
                                
            # We check theMove which comes from can_move and if it is starter it checks what the value of roll is, if it is one it has to only enter the board, otherwise we see what happens below which has to check where the piece would go depending on the roll.
                elif roll > 1:
                    for i in range(0, len(self.board)):
                        for j in range(0, len(self.board[i])):
                            if self.board[i][j].entrance == colorEntrance:
                                if self.board[i][j].piece == None:
                                    roll -=1
                                    listOfPieces[whatMove - 1].position = self.board[i][j]
                                    willMove = rgu.find_next_square(roll, colorEntrance, listOfPieces, whatMove, oldLocation)
                                    if willMove == False:
                                        roll +=1
                                        listOfPieces[whatMove - 1].position = None
                                        self.board[i][j].piece = None
                                        newMove = int(input("Which move do you want to make? "))
                                        rgu.piece_movement(newMove, roll, listOfPieces, colorEntrance)
                                    else:
                                        roll -= roll
                                elif self.board[i][j].piece:
                                    roll -= 1
                                    listOfPieces[whatMove - 1].position = self.board[i][j]
                                    willMove = rgu.find_next_square(roll, colorEntrance, listOfPieces, whatMove, oldLocation)
                                    if willMove == False:
                                        roll += 1
                                        listOfPieces[whatMove - 1].position = None
                                        self.board[i][j].piece = None
                                        newMove = int(input("Which move do you want to make? "))
                                        rgu.piece_movement(newMove, roll, listOfPieces, colorEntrance)
                                    else:
                                        roll -= roll
            elif theMove == "Mover":
                if roll > 0:
                    willMove = rgu.find_next_square(roll, colorEntrance, listOfPieces, whatMove, oldLocation)
                    if willMove == True:
                        roll -= roll
                    else:
                        newMove = int(input("Which move do you want to make? "))
                        rgu.piece_movement(newMove, roll, listOfPieces, colorEntrance)
                        
            # Here we check if the piece is already on the board and where we are supposed to move it to, we also see that we have to put a different move in if we choose an invalid move.

            elif theMove == "Nothing":
                roll -= roll
                
            # If there are no possible moves, we just skip the turn.

    def find_entrances(self, board, color):
        board = self.board
        for i in range(0, len(self.board)):
            for j in range(0, len(self.board[i])):
                if color == self.WCOLOR:
                    if self.board[i][j].entrance == color:
                        whiteEntrance = board[i][j]
                        return (whiteEntrance)
                if color == self.BCOLOR:
                    if self.board[i][j].entrance == color:
                        blackEntrance = board[i][j]
                        return (blackEntrance)
                    
    # We iterate through the board and find the one place that the entrance is depending on the color that is input into the function and we return the boardSquare for it.

    def find_exits(self, board, color):
        board = self.board
        for i in range(0, len(self.board)):
            for j in range(0, len(self.board[i])):
                if color == self.WCOLOR:
                    if self.board[i][j].exit == color:
                        whiteExit = board[i][j]
                        return (whiteExit)
                if color == self.BCOLOR:
                    if self.board[i][j].exit == color:
                        blackExit = board[i][j]
                        return (blackExit)
                    
    # Very similar to the find_entrances function, except we are looking for the exits rather than the entrances. 

    def create_pieces(self):
        for i in range(0, RoyalGameOfUr.STARTING_PIECES):
            self.whitePieces.append(UrPiece(self.WCOLOR, self.WSYMBOL + str(i)))
        for j in range(0, RoyalGameOfUr.STARTING_PIECES):
            self.blackPieces.append(UrPiece(self.BCOLOR, self.BSYMBOL + str(j)))
    
    # This function creates our lists of pieces we will use for the game.

    def find_next_square(self, roll, color, listOfPieces, whatMove, oldLocation):
        if roll == 1:
            if color == self.WCOLOR:
                exitLocation = rgu.find_exits(self.board, self.WCOLOR)
                if exitLocation == listOfPieces[whatMove -1].position:
                    listOfPieces[whatMove -1].position.piece = None
                    listOfPieces[whatMove -1].position = None
                    listOfPieces[whatMove -1].complete = True
                    return(True)
                else:
                    wantToMove = listOfPieces[whatMove - 1].position.next_white
                    if wantToMove.piece == None:
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    elif wantToMove.piece.color == self.BCOLOR and wantToMove.rosette == False:
                        wantToMove.piece.position = None
                        wantToMove.piece = None
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    else:
                        return (False)
            elif color == self.BCOLOR:
                exitLocation = rgu.find_exits(self.board, self.BCOLOR)
                if exitLocation == listOfPieces[whatMove - 1].position:
                    listOfPieces[whatMove - 1].position.piece = None
                    listOfPieces[whatMove - 1].position = None
                    listOfPieces[whatMove - 1].complete = True
                    return (True)
                else:
                    wantToMove = listOfPieces[whatMove - 1].position.next_black
                    if wantToMove.piece == None:
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    elif wantToMove.piece.color == self.WCOLOR and wantToMove.rosette == False:
                        wantToMove.piece.position = None
                        wantToMove.piece = None
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    else:
                        return (False)
        if roll == 2:
            if color == self.WCOLOR:
                exitLocation = rgu.find_exits(self.board, self.WCOLOR)
                if exitLocation == listOfPieces[whatMove - 1].position.next_white:
                    listOfPieces[whatMove - 1].position.piece = None
                    listOfPieces[whatMove - 1].position = None
                    listOfPieces[whatMove - 1].complete = True
                    return (True)
                else:
                    wantToMove = listOfPieces[whatMove - 1].position.next_white.next_white
                    if wantToMove.piece == None:
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    elif wantToMove.piece.color == self.BCOLOR and wantToMove.rosette == False:
                        wantToMove.piece.position = None
                        wantToMove.piece = None
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    else:
                        return (False)
            elif color == self.BCOLOR:
                exitLocation = rgu.find_exits(self.board, self.BCOLOR)
                if exitLocation == listOfPieces[whatMove - 1].position.next_black:
                    listOfPieces[whatMove - 1].position.piece = None
                    listOfPieces[whatMove - 1].position = None
                    listOfPieces[whatMove - 1].complete = True
                    return (True)
                else:
                    wantToMove = listOfPieces[whatMove - 1].position.next_black.next_black
                    if wantToMove.piece == None:
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    elif wantToMove.piece.color == self.WCOLOR and wantToMove.rosette == False:
                        wantToMove.piece.position = None
                        wantToMove.piece = None
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    else:
                        return (False)
        if roll == 3:
            if color == self.WCOLOR:
                exitLocation = rgu.find_exits(self.board, self.WCOLOR)
                if exitLocation == listOfPieces[whatMove - 1].position.next_white.next_white:
                    listOfPieces[whatMove - 1].position.piece = None
                    listOfPieces[whatMove - 1].position = None
                    listOfPieces[whatMove - 1].complete = True
                    return (True)
                else:
                    wantToMove = listOfPieces[whatMove - 1].position.next_white.next_white.next_white
                    if wantToMove.piece == None:
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    elif wantToMove.piece.color == self.BCOLOR and wantToMove.rosette == False:
                        wantToMove.piece.position = None
                        wantToMove.piece = None
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    else:
                        return (False)
            elif color == self.BCOLOR:
                exitLocation = rgu.find_exits(self.board, self.BCOLOR)
                if exitLocation == listOfPieces[whatMove - 1].position.next_black.next_black:
                    listOfPieces[whatMove - 1].position.piece = None
                    listOfPieces[whatMove - 1].position = None
                    listOfPieces[whatMove - 1].complete = True
                    return (True)
                else:
                    wantToMove = listOfPieces[whatMove - 1].position.next_black.next_black.next_black
                    if wantToMove.piece == None:
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    elif wantToMove.piece.color == self.WCOLOR and wantToMove.rosette == False:
                        wantToMove.piece.position = None
                        wantToMove.piece = None
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    else:
                        return (False)
        if roll == 4:
            if color == self.WCOLOR:
                exitLocation = rgu.find_exits(self.board, self.WCOLOR)
                if exitLocation == listOfPieces[whatMove - 1].position.next_white.next_white.next_white:
                    listOfPieces[whatMove - 1].position.piece = None
                    listOfPieces[whatMove - 1].position = None
                    listOfPieces[whatMove - 1].complete = True
                    return (True)
                else:
                    wantToMove = listOfPieces[whatMove - 1].position.next_white.next_white.next_white.next_white
                    if wantToMove.piece == None:
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    elif wantToMove.piece.color == self.BCOLOR and wantToMove.rosette == False:
                        wantToMove.piece.position = None
                        wantToMove.piece = None
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    else:
                        return (False)
            elif color == self.BCOLOR:
                exitLocation = rgu.find_exits(self.board, self.BCOLOR)
                if exitLocation == listOfPieces[whatMove - 1].position.next_black.next_black.next_black:
                    listOfPieces[whatMove - 1].position.piece = None
                    listOfPieces[whatMove - 1].position = None
                    listOfPieces[whatMove - 1].complete = True
                    return (True)
                else:
                    wantToMove = listOfPieces[whatMove - 1].position.next_black.next_black.next_black.next_black
                    if wantToMove.piece == None:
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    elif wantToMove.piece.color == self.WCOLOR and wantToMove.rosette == False:
                        wantToMove.piece.position = None
                        wantToMove.piece = None
                        listOfPieces[whatMove - 1].position = wantToMove
                        wantToMove.piece = listOfPieces[whatMove - 1]
                        if oldLocation:
                            oldLocation.piece = None
                        return (True)
                    else:
                        return (False)
                    
    # This function is the bread and butter of this algorithm, what it does is, it finds the square it needs to go on depending on the roll, it then checks if it will exit based on this roll and its current position, if it doesn't it checks the position of what it would be going to and sees if it has a piece, if it doesn't it goes there, if it has an opposing color's piece it makes that piece go off the board, and if it has one of the same piece, you can't move there.

    def play_game(self):
        RoyalGameOfUr.create_pieces(self)
        whoseTurn = True
        gameOver = False

        firstPlayerName = input("What is the first player's name? ")
        print(firstPlayerName, "you will play as white.")
        secondPlayerName = input("What is the second player's name? ")
        print(secondPlayerName, "you will play as black.")
        """
            Your job is to recode this function to play the game.
        """
        while (gameOver == False):
            self.display_board()
            roll = RoyalGameOfUr.roll_d4_dice(self)
            if whoseTurn == True:
                print(firstPlayerName, "You rolled", roll)
                for i in range(0, len(rgu.whitePieces)):
                    if (rgu.whitePieces[i].position == None):
                        print(i + 1, rgu.whitePieces[i].symbol, "currently off the board", "Completed:", rgu.whitePieces[i].complete)
                    elif (rgu.whitePieces[i].position != None):
                        print(i + 1, rgu.whitePieces[i].symbol, rgu.whitePieces[i].position.position, "Completed:", rgu.whitePieces[i].complete)
                whatMove = int(input("Which move do you wish to make? "))
                RoyalGameOfUr.piece_movement(self, whatMove, roll, rgu.whitePieces, self.WCOLOR)
                whoseTurn = False
                gameOver = rgu.check_if_won(rgu.whitePieces)
            elif whoseTurn == False:
                print(secondPlayerName, "You rolled", roll)
                for i in range(0, len(rgu.blackPieces)):
                    if (rgu.blackPieces[i].position == None):
                        print(i + 1, rgu.blackPieces[i].symbol, "currently off the board", "Completed:", rgu.blackPieces[i].complete)
                    elif (rgu.blackPieces[i].position != None):
                        print(i + 1, rgu.blackPieces[i].symbol, rgu.blackPieces[i].position.position, "Completed:", rgu.blackPieces[i].complete)
                whatMove = int(input("Which move do you wish to make? "))
                RoyalGameOfUr.piece_movement(self, whatMove, roll, rgu.blackPieces, self.BCOLOR)
                whoseTurn = True
                gameOver = rgu.check_if_won(rgu.blackPieces)
                
    # Here is our playGame function we use two variables whoseTurn and gameOver to determine whose turn it is and if the game ends, which we always change and then call respectively. We also always show the pieces that the respective player has on the board, and has completed.



if __name__ == '__main__':
    file_name = input('What is the file name of the board json? ') if len(argv) < 2 else argv[1]
    rgu = RoyalGameOfUr(file_name)
    rgu.play_game()
