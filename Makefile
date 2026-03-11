SRC_DIR = implementation
HDR_DIR = headers
BLD_DIR = build
CCFLAGS = -I$(HDR_DIR) -Ilibs/include -lcurl -Wfatal-errors -Wno-sign-compare -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG_BACKTRACE -fsanitize=undefined -fsanitize=address -w -std=c++26
LDFLAGS = -lstdc++exp
SRCS = $(wildcard $(SRC_DIR)/*.cc)
OBJS = $(SRCS:$(SRC_DIR)/%.cc=$(BLD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

a.out: $(OBJS)
	@g++ $(CCFLAGS) $^ -o $@ $(LDFLAGS)
	@echo Done linking
	@echo -e '\t$(OBJS)'
	@echo to 'a.out'!

$(BLD_DIR)/%.o: $(SRC_DIR)/%.cc 
	@mkdir -p $(BLD_DIR)
	@g++ $(CCFLAGS) -MMD -MP -c $< -o $@
	@echo Done compiling '$<'!

-include $(DEPS)

clean:
	rm -rf $(BLD_DIR) a.out

.PHONY: clean
