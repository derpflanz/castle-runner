class Directives:
    def _process_break(self, address):
        print(f"Processing breakpoint at address ${address:04x}")

    def Process(self, direc_name, address):
        if direc_name == "!break":
            self._process_break(address)
        else:
            raise SyntaxError(f"Directive {direc_name} is invalid.")
        
