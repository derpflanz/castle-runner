class Directives:
    _breakpoints = []
    _filename = None

    def __init__(self, filename):
        self._filename = filename

    def _process_break(self, address):
        print(f"Adding breakpoint at address ${address:04x} to list")
        self._breakpoints.append(address)

    def Finalise(self):
        with open(self._filename, "w") as ofile:
            for address in self._breakpoints:
                ofile.write(f"{address:04x}\n")

    def Process(self, direc_name, address):
        if direc_name == "!break":
            self._process_break(address)
        else:
            raise SyntaxError(f"Directive {direc_name} is invalid.")
        
