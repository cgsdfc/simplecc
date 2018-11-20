"""Grammar Generation"""

from simplecompiler.generate import register

register(artifact="grammar", language="cpp", func=gencpp)
register(artifact="grammar", language="python", func=genpy)
