"""A file defining the class Unit, representing a single unit in play."""


import attr

from unit_kind import UnitKind


@attr.s
class Unit:
    """A class defining a single unit in play."""
    kind: UnitKind = attr.ib()  # What kind of unit is this?
    health: int    = attr.ib()  # How much health does this unit have, right now?


    @staticmethod
    def of_kind(kind: UnitKind) -> 'Unit':
        """@brief Creates a Unit from a given UnitKind. The unit starts at full health.

        @param kind: The UnitKind of unit this will be.

        @return A Unit object, at full health and of the given kind.
        """

        return Unit(kind, kind.health)


    def to_string_hum(self) -> str:
        """@brief Creates a string representation of this unit and its attributes.

        @return A human-readable string describing this unit and its attributes.
        """

        attack_string = ""
        if self.kind.attack:
            attack_string = " (attack {attack})".format(attack=self.kind.attack)

        return '({name} (width {width}) (hp {remaining_hp}/{max_hp}){attack})'.format(
                    name=self.kind.name,
                    width=self.kind.width,
                    remaining_hp=self.health,
                    max_hp=self.kind.health,
                    attack=attack_string
                )
