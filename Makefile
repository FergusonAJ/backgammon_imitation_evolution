EMP_SRC := /home/austin/research/tools/empirical_clean/Empirical/source

# Flags to use regardless of compiler
CFLAGS_all := -std=c++17 -Wall -Wno-unused-function -I$(EMP_SRC)

# Emscripten compiler information
CXX_web := emcc
CXX_native := g++

OFLAGS_native_debug := -g -DEMP_TRACK_MEM  -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -Wconversion -Weffc++
OFLAGS_native_opt := -O3 -DNDEBUG

OFLAGS_web_debug := -g4 -Wno-dollar-in-identifier-extension -s TOTAL_MEMORY=67108864 -s ASSERTIONS=2 -s DEMANGLE_SUPPORT=1 -s -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -Wconversion -Weffc++ #WASM=0 
 # -s SAFE_HEAP=1
OFLAGS_web_opt := -Os -DNDEBUG -s TOTAL_MEMORY=67108864 #-s WASM=0

CFLAGS_native_debug := $(CFLAGS_all) $(OFLAGS_native_debug)
CFLAGS_native_opt := $(CFLAGS_all) $(OFLAGS_native_opt)
CFLAGS_native := $(CFLAGS_native_opt)

CFLAGS_web_debug := $(CFLAGS_all) $(OFLAGS_web_debug) --js-library $(EMP_SRC)/web/library_emp.js -s EXPORTED_FUNCTIONS="['_main', '_empCppCallback']" -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -s NO_EXIT_RUNTIME=1
CFLAGS_web_opt := $(CFLAGS_all) $(OFLAGS_web_opt) --js-library $(EMP_SRC)/web/library_emp.js -s EXPORTED_FUNCTIONS="['_main', '_empCppCallback']" -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -s NO_EXIT_RUNTIME=1


#JS_TARGETS := main.js

#TARGETS := debug-web

#default: data-collection
default: test-game

#WEB := $(EMP_SRC)/web

CXX := $(CXX_native)
CFLAGS_web := $(CFLAGS_web_opt)

#debug: CFLAGS_native := $(CFLAGS_native_debug)
#debug: test-game
#debug: CFLAGS_web := $(CFLAGS_web_debug)
#debug: data-collection

#web: CXX := $(CXX_web)
#web: CFLAGS_web := $(CFLAGS_web_opt)
#web: $(JS_TARGETS)

#web-debug: CXX := $(CXX_web)
#web-debug: CFLAGS := $(CFLAGS_web_debug)
#web-debug: all

#native: all

#all: data-collection#$(TARGETS)

data-collection: ./data_collection/backgammon.js

./data_collection/backgammon.js: ./src/data_collection.cc ./src/*h ./src/ui/*.h 
	$(CXX_web) ./src/data_collection.cc -o ./data_collection/backgammon.js $(CFLAGS_web)

test-game: ./bin/test_game
test-game: CFLAGS_native := $(CFLAGS_native_opt)

./bin/test_game: ./src/test_game.cc ./src/*h 
	$(CXX_native) ./src/test_game.cc -o ./bin/test_game $(CFLAGS_native)

#debug-%: $*.cc
#	$(CXX_native) $(CFLAGS_native) $< -o $@

clean:
	rm -f debug-* $(JS_TARGETS) *.js.map *.js.mem *.wasm *.wasm.map *.wast *~ source/*.o source/*/*.o
	rm -f debug-* ./data_collection/backgammon.j*
	rm -f debug-* ./data_collection/*.wasm
	rm -f debug-* ./data_collection/*.wasm.map
	rm -f debug-* ./bin/test_game

# Debugging information
#print-%: ; @echo $*=$($*)
#print-%: ; @echo '$(subst ','\'',$*=$($*))'
