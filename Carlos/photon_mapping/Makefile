# Compilador
CXX = g++

# Flags de compilación
CXXFLAGS = -Wall -std=c++17 -Iheaders

# Directorios
SRCDIR = src
HEADERDIR = headers
OBJDIR = obj
BINDIR = bin

# Archivos fuente y objetos
SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))

# Nombre del ejecutable
EXEC = $(BINDIR)/program

# Detectar sistema operativo
ifeq ($(OS),Windows_NT)
    MKDIR = mkdir
    RMDIR = rmdir /S /Q
    SEP = /
else
    MKDIR = mkdir -p
    RMDIR = rm -rf
    SEP = /
endif

# Regla principal para compilar todo
all: create_dirs $(EXEC)

# Regla para crear el ejecutable
$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para compilar archivos .cpp a .o (sin necesidad de .hpp)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Crear directorios si no existen
create_dirs:
	@if [ ! -d $(OBJDIR) ]; then $(MKDIR) $(OBJDIR); fi
	@if [ ! -d $(BINDIR) ]; then $(MKDIR) $(BINDIR); fi

# Limpiar archivos compilados
clean:
	$(RMDIR) $(OBJDIR)
	$(RMDIR) $(BINDIR)

# Limpiar todo
distclean: clean
	rm -f $(EXEC)
