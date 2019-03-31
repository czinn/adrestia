"""Tests Resources object."""


import resources


def test_Resources_of_string_exn():
    """@brief Tests creation of Resources object from valid string."""

    # Define parameters
    red = 1
    green = 2
    blue = 3

    resources_string = "{red} {green} {blue}".format(red=red, green=green, blue=blue)

    # Run test
    test_Resources = resources.Resources.of_string_exn(resources_string)

    # Assert
    assert test_Resources.red == red
    assert test_Resources.green == green
    assert test_Resources.blue == blue


def test_Resources_to_string_hum():
    """@brief Tests that to_string_hum returns a correctly-formatted string."""

    # Define parameters
    red = 1
    green = 2
    blue = 3

    test_Resources = resources.Resources(red=red, green=green, blue=blue)

    # Run test
    resources_string = test_Resources.to_string_hum()

    # Assert
    assert resources_string == "((red {red}) (green {green}) (blue {blue})".format(red=red, green=green, blue=blue)


def test_Resources_subsumes_true():
    """@brief Tests that subsumes returns true when passed a "smaller" resource object."""

    # Define parameters
    big_resource_set = resources.Resources(red=2, green=3, blue=5)
    small_resource_set = resources.Resources(red=1, green=2, blue=4)

    # Run test
    is_subsumed = big_resource_set.subsumes(small_resource_set)

    # Assert
    assert is_subsumed, "Larger resources should subsume smaller ones."


def test_Resources_subsumes_equal():
    """@brief Tests that subsumes returns true when passed an "equal" resource object."""

    # Define parameters
    resource_set_1= resources.Resources(red=2, green=3, blue=5)
    resource_set_2 = resources.Resources(red=2, green=3, blue=5)

    # Run test
    is_subsumed = resource_set_1.subsumes(resource_set_2)

    # Assert
    assert is_subsumed, "Equal resource sets should subsume each other."


def test_Resources_subsumes_false():
    """@brief Tests that subsumes returns false when passed a "bigger" resource object."""

    # Define parameters
    big_resource_set = resources.Resources(red=2, green=3, blue=5)
    small_resource_set = resources.Resources(red=1, green=2, blue=4)

    # Run test
    is_subsumed = small_resource_set.subsumes(big_resource_set)

    # Assert
    assert not is_subsumed, "Smaller resource sets should not subsume larger ones."


def test_Resources_add():
    """@brief Tests the addition of resources."""

    # Define parameters
    resource_set_1 = resources.Resources(red=2, green=3, blue=5)
    resource_set_2 = resources.Resources(red=1, green=2, blue=4)

    # Run test
    resource_set_1.add(resource_set_2)

    # Assert
    assert resource_set_1.red == 3
    assert resource_set_1.green == 5
    assert resource_set_1.blue == 9


def test_Resources_subtract_positive_result():
    """@brief Tests the subtraction of resources, when the result is positive."""

    # Define parameters
    resource_set_1 = resources.Resources(red=2, green=4, blue=5)
    resource_set_2 = resources.Resources(red=1, green=2, blue=2)

    # Run test
    resource_set_1.subtract(resource_set_2)

    # Assert
    assert resource_set_1.red == 1
    assert resource_set_1.green == 2
    assert resource_set_1.blue == 3


def test_Resources_subtract_negative_result():
    """@brief Tests the subtraction of resources, when the result is negative."""

    # Define parameters
    resource_set_1 = resources.Resources(red=2, green=4, blue=5)
    resource_set_2 = resources.Resources(red=1, green=2, blue=2)

    # Run test
    resource_set_2.subtract(resource_set_1)

    # Assert
    assert resource_set_2.red == -1
    assert resource_set_2.green == -2
    assert resource_set_2.blue == -3
