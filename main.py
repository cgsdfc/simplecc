from pathlib import Path as P

COMMENT_CHARS = "//"
lic = P("./LICENSE").read_text().splitlines()
lic = [" ".join([COMMENT_CHARS, line]) for line in lic]
lic = "\n".join(lic) + "\n\n"

import itertools

for f in itertools.chain(
    P("./src").rglob("*.cpp"),
    P("./src").rglob("*.h"),
):
    code = lic + f.read_text()
    f.write_text(code)
