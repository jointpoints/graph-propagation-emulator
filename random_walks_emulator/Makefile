COMPILER_FLAGS = -Wall -O2 -std=c++11
OBJECT_FOLDER = random_walks_emulator_obj
OUT_FOLDER = Random Walks Emulator (build)
UNITS = metric_graph





ifeq ($(OS),Windows_NT)

CPP_COMPILER = g++.exe
CPP_LINKER = g++.exe
OBJECTS = $(addprefix $(OBJECT_FOLDER)\\,$(addsuffix .o,$(UNITS)))
OUT_FILE = random_walks_emulator.exe

release: release_prepare_win $(OBJECTS) release_object_main_win release_assemble_win release_clean_win release_saturate_win

erase:
	powershell Remove-Item -Force -Recurse -Path \"$(OUT_FOLDER)\"

else

CPP_COMPILER = g++
CPP_LINKER = g++
OBJECTS = $(addprefix $(OBJECT_FOLDER)/,$(addsuffix .o,$(UNITS)))
OUT_FILE = random_walks_emulator.out

release: release_prepare_unx $(OBJECTS) release_object_main_unx release_assemble_unx release_clean_unx release_saturate_unx

erase:
	rm -rf \"$(OUT_FOLDER)\"

endif





# Targets for Windows

release_prepare_win:
	powershell New-Item -ItemType Directory -Force -Path $(OBJECT_FOLDER)
	powershell New-Item -ItemType Directory -Force -Path \"$(OUT_FOLDER)\"

$(OBJECT_FOLDER)\\%.o:
	$(CPP_COMPILER) $(COMPILER_FLAGS) -c $(patsubst %.o,%,$(subst $(OBJECT_FOLDER)\\,,$@))\\$(patsubst %.o,%,$(subst $(OBJECT_FOLDER)\\,,$@)).cpp -o $@

release_object_main_win:
	$(CPP_COMPILER) $(COMPILER_FLAGS) -c main.cpp -o $(OBJECT_FOLDER)\\main.o

release_assemble_win:
	$(CPP_LINKER) -o "$(OUT_FOLDER)\\$(OUT_FILE)" $(OBJECTS) $(OBJECT_FOLDER)\\main.o

release_clean_win:
	powershell Remove-Item -Force -Recurse -Path $(OBJECT_FOLDER)

release_saturate_win:
	powershell New-Item -ItemType Directory -Force -Path \"$(OUT_FOLDER)\\Configuration\"
	powershell New-Item -ItemType Directory -Force -Path \"$(OUT_FOLDER)\\Saved files\"
	powershell Copy-Item -Force -Path supply\\Sample1 -Destination \"$(OUT_FOLDER)\\Saved files\\Sample graph 1.rweg\"





# Targets for Unix

release_prepare_unx:
	mkdir $(OBJECT_FOLDER)
	mkdir \"$(OUT_FOLDER)\"

$(OBJECT_FOLDER)/%.o:
	$(CPP_COMPILER) $(COMPILER_FLAGS) -c $(patsubst %.o,%,$(subst $(OBJECT_FOLDER)/,,$@))/$(patsubst %.o,%,$(subst $(OBJECT_FOLDER)/,,$@)).cpp -o $@

release_object_main_unx:
	$(CPP_COMPILER) $(COMPILER_FLAGS) -c main.cpp -o $(OBJECT_FOLDER)/main.o

release_assemble_unx:
	$(CPP_LINKER) -o "$(OUT_FOLDER)/$(OUT_FILE)" $(OBJECTS) $(OBJECT_FOLDER)/main.o

release_clean_unx:
	rm -rf $(OBJECT_FOLDER)

release_saturate_unx:
	mkdir \"$(OUT_FOLDER)/Configuration\"
	mkdir \"$(OUT_FOLDER)/Saved files\"
	cp supply/Sample1 \"$(OUT_FOLDER)/Saved files/Sample graph 1.rweg\"
