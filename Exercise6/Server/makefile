SOURCES= file_server.cpp iknlib.cpp
EXE = prog
CXXFLAGS =-I. 
ARCH?=x86-64
ifeq (${ARCH},x86-64) 
CXX= g++
BUILD_DIR=build/x86-64
BIN_DIR=bin/x86-64
endif 
ifeq (${ARCH},arm) 
CXX=arm-rpizw-g++
BUILD_DIR=build/arm
BIN_DIR=bin/arm
endif
OBJECTS = $(addprefix $(BUILD_DIR)/,${SOURCES:.cpp=.o})
DEPS= $(addprefix $(BUILD_DIR)/,${SOURCES:.cpp=.d})
all: $(BIN_DIR)/$(EXE)
$(BIN_DIR)/$(EXE): $(OBJECTS)  $(DEPS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) 
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)
## 3 lines under generates .d files in the build directory
## 3 lines under makes .d to .0 and appends to target
$(BUILD_DIR)/%.d: %.cpp #Makes a .d file for every %.cpp file
	mkdir -p $(BUILD_DIR)
	$(CXX) -MT$@ -MM $(CXXFLAGS) $< > $@
	$(CXX) -MT$(@:.d=.o) -MM $(CXXFLAGS) $< >> $@
.PHONY: clean
clean:
	rm $(OBJECTS) $(DEPS) $(BIN_DIR)/$(EXE) *.tidy *.format
-PHONY: help
help:
	echo $(SOURCES)
format: $(SOURCES:.cpp=.format)
%.format: %.cpp
	@echo "Formatting file $<'"...
	@clang-format -i $<
	@echo "" > $@
tidy: $(SOURCES:.cpp=.tidy)
%.tidy: %.cpp
	@echo "Tidying file '$<'"...
	@clang-tidy $< -- $(CXXFLAGS)
	@echo "" > $@
ifneq ($(filter-out clean format tidy,$(MAKECMDGOALS)),)
-include $(DEPS)
endif