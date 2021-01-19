COMPILER_FLAGS = -Wall -O2 -std=c++11
OBJECT_FOLDER = random_walks_emulator_obj
COMPONENTS = metric_graph





ifeq ($(OS),Windows_NT)

CPP_COMPILER = g++.exe
CPP_LINKER = g++.exe
OBJECTS = $(addprefix $(OBJECT_FOLDER)\\,$(addsuffix .o,$(COMPONENTS)))
OUT_FILE = random_walks_emulator.exe

release: release_prepare_win $(OBJECTS) release_object_main_win release_assemble_win release_clean_win

else

CPP_COMPILER = g++
CPP_LINKER = g++
OBJECTS = $(addprefix $(OBJECT_FOLDER)/,$(addsuffix .o,$(COMPONENTS)))
OUT_FILE = random_walks_emulator.out

release: release_prepare_unx $(OBJECTS) release_object_main_unx release_assemble_unx release_clean_unx

endif





# Targets for Windows

release_prepare_win:
	powershell New-Item -ItemType Directory -Force -Path $(OBJECT_FOLDER)

$(OBJECT_FOLDER)\\%.o:
	$(CPP_COMPILER) $(COMPILER_FLAGS) -c $(patsubst %.o,%,$(subst $(OBJECT_FOLDER)\\,,$@))\\$(patsubst %.o,%,$(subst $(OBJECT_FOLDER)\\,,$@)).cpp -o $@

release_object_main_win:
	$(CPP_COMPILER) $(COMPILER_FLAGS) -c main.cpp -o $(OBJECT_FOLDER)\\main.o

release_assemble_win:
	$(CPP_LINKER) -o $(OUT_FILE) $(OBJECTS) $(OBJECT_FOLDER)\\main.o

release_clean_win:
	powershell Remove-Item -Force -Recurse -Path $(OBJECT_FOLDER)





# Targets for Unix

release_prepare_unx:
	mkdir $(OBJECT_FOLDER)

$(OBJECT_FOLDER)/%.o:
	$(CPP_COMPILER) $(COMPILER_FLAGS) -c $(patsubst %.o,%,$(subst $(OBJECT_FOLDER)/,,$@))/$(patsubst %.o,%,$(subst $(OBJECT_FOLDER)/,,$@)).cpp -o $@

release_object_main_unx:
	$(CPP_COMPILER) $(COMPILER_FLAGS) -c main.cpp -o $(OBJECT_FOLDER)/main.o

release_assemble_unx:
	$(CPP_LINKER) -o $(OUT_FILE) $(OBJECTS) $(OBJECT_FOLDER)/main.o

release_clean_unx:
	rm -rf $(OBJECT_FOLDER)
