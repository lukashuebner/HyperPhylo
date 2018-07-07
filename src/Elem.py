class Elem:
    def __init__(self, name, contained_set, contained_hypernodes=None):
        self.name = name
        self.contained_set = contained_set
        # Only used for E
        self.contained_hypernodes = contained_hypernodes

    def __str__(self):
        return self.name

    def __hash__(self):
        return hash(self.name)

    def __eq__(self, other):
        return self.contained_set == other.contained_set

    def __ne__(self, other):
        return not self == other

    def __iter__(self):
        return iter(self.contained_set)

    def __next__(self):
        return next(self.contained_set)

    def __sub__(self, other):
        if isinstance(other, set):
            return self.contained_set - other
        else:
            return NotImplemented

    def __len__(self):
        return len(self.contained_set)
