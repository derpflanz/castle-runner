import os

class Symbols:
    def __init__(self, filename):
        if os.path.exists(filename):
            print(f"{filename} exists, will be overwritten")

        print(f"Using {filename} as symbolfile")       
        self.symbolfile = filename

    def write_labels(self, labels):
        with open(self.symbolfile, "w") as sym:
            for (label, address) in labels.items():
                sym.write(f"{label}:{address}\n")

