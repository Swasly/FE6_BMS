# Path to cyprjmgr.exe
CMD=/cygdrive/c/Program\ Files\ \(x86\)/Cypress/PSoC\ Creator/3.1/PSoC\ \
Creator/bin/cyprjmgr.exe

# Path to the .cywrk project file
SRC="../alpaca-bms.cywrk"

FLAGS= -wrk $(SRC) -warn High

.PHONY: all build rebuild clean



# RULES

all: build 

build:
	$(CMD) $(FLAGS) -build

rebuild:
	$(CMD) $(FLAGS) -rebuild

clean:
	$(CMD) $(FLAGS) -clean
