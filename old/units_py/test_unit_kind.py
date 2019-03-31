"""Tests the unit dictionary to ensure that all units match "requirements"."""


import unit_kind


def test_unit_dictionary():
    for unit_internal_name in unit_kind.unit_kinds.keys():
        unitkind = unit_kind.unit_kinds[unit_internal_name]
        assert unitkind.name
        assert unitkind.colour
        assert unitkind.health
        assert unitkind.width