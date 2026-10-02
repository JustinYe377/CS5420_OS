PYTHON ?= python
INPUT ?= input.1
ALGORITHM ?= FCFS
QUANTUM ?= 5

.PHONY: all run fcfs sjf rr test clean

all:
	$(PYTHON) -m py_compile proj2.py

run:
	$(PYTHON) proj2.py $(INPUT) $(ALGORITHM) $(if $(filter RR,$(ALGORITHM)),$(QUANTUM))

fcfs:
	$(PYTHON) proj2.py $(INPUT) FCFS

sjf:
	$(PYTHON) proj2.py $(INPUT) SJF

rr:
	$(PYTHON) proj2.py $(INPUT) RR $(QUANTUM)

test:
	$(PYTHON) proj2.py input.1 FCFS
	$(PYTHON) proj2.py input.1 SJF
	$(PYTHON) proj2.py input.1 RR 5
	$(PYTHON) proj2.py input.2 FCFS
	$(PYTHON) proj2.py input.2 SJF
	$(PYTHON) proj2.py input.2 RR 2
	$(PYTHON) proj2.py input.3 FCFS
	$(PYTHON) proj2.py input.3 SJF
	$(PYTHON) proj2.py input.3 RR 2

clean:
	$(PYTHON) -c "import shutil; shutil.rmtree('__pycache__', ignore_errors=True)"
