class Elem:
	def __init__(self, name, containing_set):
		self.name = name
		self.containing_set = containing_set

	def __str__(self):
		return self.name

	def __hash__(self):
		return hash(self.name)

	def __eq__(self, other):
		return self.containing_set == other.containing_set

	def __ne__(self, other):
		return not self == other

	def __iter__(self):
		return iter(self.containing_set)

	def __next__(self):
		return next(self.containing_set)

	def __sub__(self, other):
		if isinstance(other, set):
			return self.containing_set - other
		else:
			return NotImplemented

