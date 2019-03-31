"""Tests the Unit class."""


from unittest import mock

import unit


def test_Unit_of_kind():
    """@brief Tests the creation of a Unit from a given UnitKind."""

    # Define
    mock_unitkind = mock.Mock()
    mock_unitkind.health = 3

    # Test
    test_unit = unit.Unit.of_kind(mock_unitkind)

    # Assert
    assert test_unit.kind == mock_unitkind
    assert test_unit.health == mock_unitkind.health


def test_Unit_to_string_hum_attacker():
    """@brief Tests the human-readable string of an attacking unit."""

    # Define
    mock_unitkind = mock.Mock()
    mock_unitkind.name = "Star-Driver"
    mock_unitkind.width = 4
    mock_unitkind.health = 15
    mock_unitkind.attack = [7,4,8]

    current_health = 10

    # Define part 2
    test_unit = unit.Unit.of_kind(mock_unitkind)
    test_unit.health = current_health  # A different value than full

    expected_string = "({name} (width {width}) (hp {remaining_hp}/{max_hp}) (attack {attack}))".format(
                            name=mock_unitkind.name,
                            width=mock_unitkind.width,
                            remaining_hp=current_health,
                            max_hp=mock_unitkind.health,
                            attack=mock_unitkind.attack
                        )

    # Test
    human_string = test_unit.to_string_hum()

    # Assert
    assert human_string == expected_string


def test_Unit_to_string_hum_civilian():
    """@brief Tests the human-readable string of a unit with no attack."""

    # Define
    mock_unitkind = mock.Mock()
    mock_unitkind.name = "Calcicrete"
    mock_unitkind.width = 1
    mock_unitkind.health = 4000
    mock_unitkind.attack = []

    current_health = 10

    # Define part 2
    test_unit = unit.Unit.of_kind(mock_unitkind)
    test_unit.health = current_health  # A different value than full

    expected_string = "({name} (width {width}) (hp {remaining_hp}/{max_hp}))".format(
                            name=mock_unitkind.name,
                            width=mock_unitkind.width,
                            remaining_hp=current_health,
                            max_hp=mock_unitkind.health,
                            attack="[None]"
                        )

    # Test
    human_string = test_unit.to_string_hum()

    # Assert
    assert human_string == expected_string