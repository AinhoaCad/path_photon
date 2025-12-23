# Variables
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -Iinclude/third_party -mconsole
OBJDIR = obj
SRCDIR = src
BINDIR = bin
TARGET = $(BINDIR)/photonmapping

# Archivos fuente y objetos
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Detectar sistema operativo
ifeq ($(OS),Windows_NT)
    RM = del /Q /S
    RMDIR = rmdir /S /Q
else
    RM = rm -f
    RMDIR = rm -rf
endif

# Regla principal
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) main.cpp -o $(TARGET)

# Regla para compilar archivos .cpp
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR) $(BINDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Crear directorios si no existen (Windows-friendly)
$(OBJDIR):
	@if not exist $(OBJDIR) mkdir $(OBJDIR)

$(BINDIR):
	@if not exist $(BINDIR) mkdir $(BINDIR)

# Limpieza
clean:
	-$(RM) $(OBJDIR)\*.o
	-$(RMDIR) $(OBJDIR)
	-$(RMDIR) $(BINDIR)

# Phony targets
.PHONY: all clean
