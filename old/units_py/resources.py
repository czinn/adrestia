"""Defines Resources, an object for managing a resource set."""


import attr


@attr.s
class Resources:
    """@brief An object for managing a resource set."""
    red: int   = attr.ib(default = 0)
    green: int = attr.ib(default = 0)
    blue: int  = attr.ib(default = 0)


    @staticmethod
    def of_string_exn(s: str) -> 'Resources':
        """@brief Creates and returns Resources object from a string formatted like '{red} {green} {blue}'.

        @param s: The input string.

        @return Resources object formed from s.
        """
        red, green, blue = map(int, s.split(' '))

        return Resources(red=red, green=green, blue=blue)


    def to_string_hum(self) -> str:
        """@brief Returns a human-readable string describing this object.

        @return string "((red <red count>) (green <green count>) (blue <blue count>))"
        """

        return '((red {red}) (green {green}) (blue {blue})'.format(red=self.red, green=self.green, blue=self.blue)


    def subsumes(self, other: 'Resources') -> bool:
        """@brief Returns whether the parameter 'other' represents less or equal resources than are in this object.

        @param other: A resources object.

        @return boolean representing whether other represents less or equal resources than are in this object.
        """

        return self.red >= other.red and self.green >= other.green and self.blue >= other.blue


    def add(self, other: 'Resources') -> None:
        """@brief Adds resources represented by parameter 'other' to this object.

        @param other: A Resources object; its contents are added to this object.
        """

        self.red += other.red
        self.green += other.green
        self.blue += other.blue


    def subtract(self, other: 'Resources') -> None:
        """@brief Subtracts resources represented by parameter 'other' from this object. This subtraction can allow
        this object's resources to go into negatives.

        @param other: A Resources object; its contents are subtracted from this object.
        """

        self.red -= other.red
        self.green -= other.green
        self.blue -= other.blue
