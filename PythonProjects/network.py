"""

File: switchboard.py
Author: Jacob Schoenberger
Date: 12/30/2020
Section: 24
E-mail: jacobs9@umbc.edu
Description: The main file for project three that houses the network that we work with as well as all of the network functions for project 3!

"""

"""
network.py is both the definition file for the Network class as well as the driver for the program.

In network you need to implement the functions which the driver will call for the all the different commands.
"""

from phone import Phone
from switchboard import Switchboard


HYPHEN = "-"
QUIT = 'quit'
SWITCH_CONNECT = 'switch-connect'
SWITCH_ADD = 'switch-add'
PHONE_ADD = 'phone-add'
NETWORK_SAVE = 'network-save'
NETWORK_LOAD = 'network-load'
START_CALL = 'start-call'
END_CALL = 'end-call'
DISPLAY = 'display'
COMMA = ","


class Network:
    def __init__(self):
        self.listOfSwitchboards = []
        """
            Construct a network by creating the switchboard container object

            You are free to create any additional data/members necessary to maintain this class.
        """

        # Here we create a list of switchboards so that we know what switchboards are in the network.

    def load_network(self, filename):
        new_network = open(filename, "r")
        for line in new_network:
            loadedNetwork = line.split(HYPHEN)
            newSwitchboard = self.add_switchboard(loadedNetwork[0])
            newSwitchboard.listOfConnections = loadedNetwork[1].split(COMMA)
            for i in range(2, len(loadedNetwork)-1):
                newSwitchboard.listOfPhones.append(Switchboard.add_phone(newSwitchboard, loadedNetwork[i]))
                newSwitchboard.listOfPhones.remove(newSwitchboard.listOfPhones[len(newSwitchboard.listOfPhones)-1])
        new_network.close()

        # The load_network functions looks for a file with the filename given and then reads it and creates phone objects as well as switchboard objects and connections from the file.

        """
        :param filename: the name of the file to be loaded.  Assume it exists and is in the right format.
                If not, it's ok if your program fails.
        :return: success?
        """


    def save_network(self, filename):
        new_network = open(filename, "w")
        for i in range(0, len(self.listOfSwitchboards)):
            new_network.write(str(self.listOfSwitchboards[i].area_code))
            new_network.write(HYPHEN)
            new_network.write(COMMA.join(self.listOfSwitchboards[i].listOfConnections))
            new_network.write(HYPHEN)
            for j in range(0, len(self.listOfSwitchboards[i].listOfPhones)):
                new_network.write(str(self.listOfSwitchboards[i].listOfPhones[j].number))
                new_network.write(HYPHEN)
            new_network.write("\n")
        new_network.close()

        # The save_network function takes the file name we give it and saves all of the area codes, phone numbers, and the list of connections each switchboard has, each line is separated by a different switchboard.
        """
        :param filename: the name of your file to save the network.  Remember that you need to save all the
            connections, but not the active phone calls (they can be forgotten between save and load).
            You must invent the format of the file, but if you wish you can use either json or csv libraries.
        :return: success?
        """


    def add_switchboard(self, area_code):
        newSwitchboard = Switchboard(area_code, the_network)
        self.listOfSwitchboards.append(newSwitchboard)
        return(newSwitchboard)
        """
        add switchboard should create a switchboard and add it to your network.

        By default it is not connected to any other boards and has no phone lines attached.
        :param area_code: the area code for the new switchboard
        :return:
        """

    def connect_switchboards(self, area_1, area_2):
        canBeConnected = False
        for i in range(0, len(self.listOfSwitchboards)):
            if str(area_1) == str(self.listOfSwitchboards[i].area_code):
                firstBoard = self.listOfSwitchboards[i]
                canBeConnected = True
            elif str(area_2) == str(self.listOfSwitchboards[i].area_code):
                secondBoard = self.listOfSwitchboards[i]
        firstBoard.add_trunk_connection(secondBoard)
        secondBoard.add_trunk_connection(firstBoard)
        return(canBeConnected)

        # The connect_switchboards function takes two area codes, finds what switchboards they belong to and then adds each other's area codes to their list of connections.

        """
            Connect switchboards should connect the two switchboards (creates a trunk line between them)
            so that long distance calls can be made.

        :param area_1: area-code 1
        :param area_2: area-code 2
        :return: success/failure
        """

    def display(self):
        for k in range(0, len(self.listOfSwitchboards)):
            print("\n" + "Switchboard with area code: ", str(self.listOfSwitchboards[k].area_code) + "\n" + "\t")
            print("Trunk lines are: ")
            for j in range(0, len(self.listOfSwitchboards[k].listOfConnections)):
                print("Trunk line connection to: ", str(self.listOfSwitchboards[k].listOfConnections[j]) + "\n")
            print("Local phone numbers are: ")
            for y in range(0, len(self.listOfSwitchboards[k].listOfPhones)):
                if self.listOfSwitchboards[k].listOfPhones[y].inUse == False:
                    print("Phone with number: ", self.listOfSwitchboards[k].listOfPhones[y].number, "is not in use." + "\n")
                else:
                    print("Phone with number: ", self.listOfSwitchboards[k].listOfPhones[y].number, "is connected to", self.listOfSwitchboards[k].listOfPhones[y].connectedTo + "\n")

        # The display function displays all the information we need on each switchboard, their connections, their phone numbers, and if those phone numbers are connected to anything.

        """
            Display should output the status of the phone network as described in the project.
        """

if __name__ == '__main__':
    the_network = Network()
    s = input('Enter command: ')
    while s.strip().lower() != QUIT:
        split_command = s.split()
        if len(split_command) == 3 and split_command[0].lower() == SWITCH_CONNECT:
            area_1 = int(split_command[1])
            area_2 = int(split_command[2])
            the_network.connect_switchboards(area_1, area_2)
        elif len(split_command) == 2 and split_command[0].lower() == SWITCH_ADD:
            the_network.add_switchboard(int(split_command[1]))
        elif len(split_command) == 2 and split_command[0].lower() == PHONE_ADD:
            number_parts = split_command[1].split(HYPHEN)
            area_code = int(number_parts[0])
            phone_number = int(''.join(number_parts[1:]))
            for i in range(0, len(the_network.listOfSwitchboards)):
                if int(the_network.listOfSwitchboards[i].area_code) == area_code:
                    the_network.listOfSwitchboards[i].add_phone(phone_number)

            # Here is where we add a phone to a switchboard as long as it has the same area code as the first 3 digits of the number.

        elif len(split_command) == 2 and split_command[0].lower() == NETWORK_SAVE:
            the_network.save_network(split_command[1])
            print('Network saved to {}.'.format(split_command[1]))
        elif len(split_command) == 2 and split_command[0].lower() == NETWORK_LOAD:
            the_network.load_network(split_command[1])
            print('Network loaded from {}.'.format(split_command[1]))
        elif len(split_command) == 3 and split_command[0].lower() == START_CALL:
            src_number_parts = split_command[1].split(HYPHEN)
            src_area_code = int(src_number_parts[0])
            src_number = int(''.join(src_number_parts[1:]))

            dest_number_parts = split_command[2].split(HYPHEN)
            dest_area_code = int(dest_number_parts[0])
            dest_number = int(''.join(dest_number_parts[1:]))
            for i in range(0, len(the_network.listOfSwitchboards)):
                if int(the_network.listOfSwitchboards[i].area_code) == src_area_code:
                    sourceSwitchboard = the_network.listOfSwitchboards[i]
                elif int(the_network.listOfSwitchboards[i].area_code) == dest_area_code:
                    destinationSwitchboard = the_network.listOfSwitchboards[i]
            if (sourceSwitchboard.connect_call(dest_area_code, dest_number, [], src_area_code)) == True:
                for j in range(0, len(sourceSwitchboard.listOfPhones)):
                    if int(sourceSwitchboard.listOfPhones[j].number) == src_number:
                        sourceSwitchboard.listOfPhones[j].connect(dest_area_code, dest_number)
                        sourcePhone = sourceSwitchboard.listOfPhones[j]
                for k in range(0, len(destinationSwitchboard.listOfPhones)):
                    if int(destinationSwitchboard.listOfPhones[k].number) == dest_number:
                        destinationSwitchboard.listOfPhones[k].connect(src_area_code, src_number)
                        destinationPhone = destinationSwitchboard.listOfPhones[k]
                print(sourcePhone.connectedTo, "and", destinationPhone.connectedTo, "are now connected.")
            else:
                for j in range(0, len(sourceSwitchboard.listOfPhones)):
                    if int(sourceSwitchboard.listOfPhones[j].number) == src_number:
                        sourcePhone = sourceSwitchboard.listOfPhones[j]
                for k in range(0, len(destinationSwitchboard.listOfPhones)):
                    if int(destinationSwitchboard.listOfPhones[k].number) == dest_number:
                        destinationPhone = destinationSwitchboard.listOfPhones[k]
                print(str(src_area_code) + HYPHEN + str(sourcePhone.number), "and", str(dest_area_code) + HYPHEN + str(destinationPhone.number), "were not connected.")

            # Here is where we actually start a phone call, our code will find out which switchboards we are working with using for loops, then we try to connect the call using the connect_call function, if it's True we find the phone objects with more for loops and connect the two phones, otherwise we find the two phone objects but do not connect them.

        elif len(split_command) == 2 and split_command[0].lower() == END_CALL:
            number_parts = split_command[1].split('-')
            area_code = int(number_parts[0])
            number = int(''.join(number_parts[1:]))
            for i in range(0, len(the_network.listOfSwitchboards)):
                if int(the_network.listOfSwitchboards[i].area_code) == area_code:
                    for j in range(0, len(the_network.listOfSwitchboards[i].listOfPhones)):
                        if number == int(the_network.listOfSwitchboards[i].listOfPhones[j].number):
                            starterPhone = the_network.listOfSwitchboards[i].listOfPhones[j]
            if starterPhone.connectedTo:
                endPhoneList = starterPhone.connectedTo.split(HYPHEN)
                endPhoneAreaCode = int(endPhoneList[0])
                endPhoneNumber = int(endPhoneList[1])
                for k in range(0, len(the_network.listOfSwitchboards)):
                    if int(the_network.listOfSwitchboards[k].area_code) == endPhoneAreaCode:
                        for l in range(0, len(the_network.listOfSwitchboards[k].listOfPhones)):
                            if endPhoneNumber == int(the_network.listOfSwitchboards[k].listOfPhones[l].number):
                                endPhone = the_network.listOfSwitchboards[k].listOfPhones[l]
                starterPhone.disconnect()
                endPhone.disconnect()
                print("Hanging up...")
                print("Connection Terminated.")
            else:
                print("Unable to disconnect.")

            # Here is where we would end the call, we find the phone object and if it is connected to something we find what phone it is connected to and we disconnect both phones from each other using the disconnect() function, however, if the first phone isn't connected to anything, we do nothing.
            
        elif len(split_command) >= 1 and split_command[0].lower() == DISPLAY:
            the_network.display()

        s = input('Enter command: ')
