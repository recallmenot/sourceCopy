CC = g++
CFLAGS = -Wall -g
LDFLAGS = -lncurses
TARGET = sourceCopy
SOURCES = sourceCopy.cpp
OBJECTS = $(SOURCES:.cpp=.o)
INSTALL_DIR = $(HOME)/.local/bin

# Default target: just build the binary
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Install target: copy to ~/.local/bin
install: $(TARGET)
	mkdir -p $(INSTALL_DIR)
	cp $(TARGET) $(INSTALL_DIR)/
	@echo "Installed $(TARGET) to $(INSTALL_DIR)"

# Check and add ~/.local/bin to PATH in .zshrc if not present
zsh_user_path_add:
	@if ! echo $$PATH | grep -q "$(INSTALL_DIR)"; then \
		echo 'Adding $(INSTALL_DIR) to PATH in ~/.zshrc'; \
		echo 'export PATH="$(INSTALL_DIR):$$PATH"' >> ~/.zshrc; \
		echo 'Please run "source ~/.zshrc" or restart your shell to apply changes'; \
	else \
		echo '$(INSTALL_DIR) is already in PATH'; \
	fi

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all install zsh_user_path_add clean
