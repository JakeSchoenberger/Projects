"""

File: switchboard.py
Author: Jacob Schoenberger
Date: 12/30/2020
Section: 24
E-mail: jacobs9@umbc.edu
Description: The switchboard file that contains the switchboard class and all of its functions for project 3!

"""

"""
    Switchboard class

"""

from phone import Phone


class Switchboard:
    def __init__(self, area_code, network):
        self.listOfPhones = []
        self.listOfConnections = []
        """
        :param area_code: the area code to which the switchboard will be associated.
        """
        self.area_code = area_code
        self.network = network

        # Here is our Switchboard constructor it needs a list of phone objects as well as a list of connections it is connected to, its own area code and the network it belongs to.

    def add_phone(self, phone_number):
        self.listOfPhones.append(Phone(phone_number, self))
        """
        This function should add a local phone connection by creating a phone object
        and storing it in this class.  How you do that is up to you.

        :param phone_number: phone number without area code
        :return: depends on implementation / None
        """

        # Our add_phone function adds a phone object to a switchboard's list of phones.

    def add_trunk_connection(self, switchboard):
        self.listOfConnections.append(str(switchboard.area_code))
        """
        Connect the switchboard (self) to the switchboard (switchboard)

        :param switchboard: should be either the area code or switchboard object to connect.
        :return: success/failure, None, or it's up to you
        """
        # Our add_trunk_connection function connects a switchboard to another by appending the other switchboard's area code to its list of connections.

    def connect_call(self, area_code, number, previous_codes, start_area_code):
        if str(area_code) in self.listOfConnections or str(area_code) == str(self.area_code):
            return(True)
        else:
            while len(self.network.listOfSwitchboards) > len(previous_codes):
                previous_codes.append(str(self.area_code))
                for i in range(0, len(self.listOfConnections)):
                    if str(self.listOfConnections[i]) not in previous_codes:
                        for k in range(0, len(self.network.listOfSwitchboards)):
                            if str(self.network.listOfSwitchboards[k].area_code) == str(self.listOfConnections[i]):
                                return(self.network.listOfSwitchboards[k].connect_call(area_code, number, previous_codes, start_area_code))

                    for j in range(0, len(self.network.listOfSwitchboards)):
                        if str(self.network.listOfSwitchboards[j].area_code) == str(start_area_code):
                            return(self.network.listOfSwitchboards[j].connect_call(area_code, number, previous_codes, start_area_code))
        return(False)

        # Our connect_call function will look to see if the area code of the phone number is in the switchboard's list of connections if not, it goes to its list of connections and calls the function again until it either finds a pathway or it goes through all the switchboards. Which will return True or False respectively.

        """
        This must be a recursive function.

        :param area_code: the area code to which the destination phone belongs
        :param number: the phone number of the destination phone without area code.
        :param previous_codes: you must keep track of the previously tracked codes
        :return: Depends on your implementation, possibly the path to the destination phone.
        """
