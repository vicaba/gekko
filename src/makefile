# Author: Victor Caballero (vicaba)
# Makefile that automagically resolves file dependencies using several linux programs. 
# Source directories and files cannot contain any whitespace in their name.
# This is the command used to resolve dependencies: gcc -MM ./$(N_DIR)/$(RAW_N).c | sed s/$(RAW_N).o\:\ //g | sed  s/\\.h/\\.c/g | sed s/\ \ \ \ //g | sed ':a;N;$!ba;s/\n/ /g' | sed s/\ \ //g | sed 's/\ \\//g' | tr " " "\n" | while read -r line; do readlink -f $$line; done | sort | uniq | sed  s/\ /\\\\\ /g | tr "\n" " "))
# With some versions of readlink, it may throw a warning but the compile still works.
# All files need a .c and .h version
RAW_N1 = gekkoclient
N1_DIR = gekkoclient.d
RAW_N2 = gekkoserver
N2_DIR = gekkoserver.d

default: $(RAW_N1).exe $(RAW_N2).exe


$(RAW_N1).exe: ./$(N1_DIR)/$(RAW_N1).c
	$(eval RAW_N = $(RAW_N1))
	$(eval N_DIR = $(N1_DIR))
	$(eval DEPS = $(shell gcc -MM ./$(N_DIR)/$(RAW_N).c | sed s/$(RAW_N).o\:\ //g | sed  s/\\.h/\\.c/g | sed s/\ \ \ \ //g | sed ':a;N;$!ba;s/\n/ /g' | sed s/\ \ //g | sed 's/\ \\//g' | tr " " "\n" | while read -r line; do readlink -f $$line; done | sort | uniq | sed  s/\ /\\\\\ /g | tr "\n" " "))
	gcc $(DEPS) -o ../bin/$(RAW_N).exe -Wall -Wextra -lpthread
	@echo "--->Executable at Gekko/bin, execute it in a Gekko/dozer_exampleX directory<---"

$(RAW_N2).exe: ./$(N2_DIR)/$(RAW_N2).c
	$(eval RAW_N = $(RAW_N2))
	$(eval N_DIR = $(N2_DIR))
	$(eval DEPS = $(shell gcc -MM ./$(N_DIR)/$(RAW_N).c | sed s/$(RAW_N).o\:\ //g | sed  s/\\.h/\\.c/g | sed s/\ \ \ \ //g | sed ':a;N;$!ba;s/\n/ /g' | sed s/\ \ //g | sed 's/\ \\//g' | tr " " "\n" | while read -r line; do readlink -f $$line; done | sort | uniq | sed  s/\ /\\\\\ /g | tr "\n" " "))
	gcc $(DEPS) -o ../bin/$(RAW_N).exe -Wall -Wextra -lpthread
	@echo "--->Executable at Gekko/bin, execute it in Gekko/geeko_example directory<---"

clean:
	rm -rf ../bin/*.exe;
	rm -rf *.o;
