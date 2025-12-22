# =====================
# Variables generales
# =====================
CXX = g++
STD = -std=c++17
WARN = -Wall
INCLUDES = -Iinclude

OBJDIR = obj
SRCDIR = src
BINDIR = bin
TARGET = $(BINDIR)/pathtracer

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# =====================
# Detección de sistema
# =====================
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    # ---- Linux / WSL ---- (Embree SIEMPRE activo)
    EMBREE_INC ?= /usr/include
    EMBREE_LIB ?= /usr/lib/x86_64-linux-gnu
    CXXFLAGS = $(STD) $(WARN) $(INCLUDES) -O2 -DUSE_EMBREE -I$(EMBREE_INC)
    LDFLAGS  = -L$(EMBREE_LIB) -lembree4 -ltbb
    RM = rm -f
    RMDIR = rm -rf
    MKDIR_P = mkdir -p
else
    # ---- Windows ---- (Embree NO disponible)
    CXXFLAGS = $(STD) $(WARN) $(INCLUDES) -mconsole
    LDFLAGS  =
    RM = del /Q /S
    RMDIR = rmdir /S /Q
    MKDIR_P = mkdir
endif

# =====================
# Regla principal
# =====================
all: $(TARGET)

$(TARGET): $(OBJECTS) main.cpp
	$(CXX) $(CXXFLAGS) $(OBJECTS) main.cpp -o $(TARGET) $(LDFLAGS)

# =====================
# Compilación
# =====================
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR) $(BINDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# =====================
# Directorios
# =====================
$(OBJDIR):
	$(MKDIR_P) $(OBJDIR)

$(BINDIR):
	$(MKDIR_P) $(BINDIR)

# =====================
# Limpieza
# =====================
clean:
	-$(RM) $(OBJDIR)/*.o
	-$(RMDIR) $(OBJDIR)
	-$(RMDIR) $(BINDIR)

.PHONY: all clean