__all__ = ["register", "lookup"]

_registry = {}

def register(*, artifact, language, func):
    """Register a callable to generate an artifact in a language"""
    _registry[artifact][language] = func

def lookup(*, artifact, language):
    """Lookup a callable to handle the generation of an artifact in a language"""
    return _registry[artifact][language]

def generate(args):
    """Implement the ``generate`` sub-command"""

