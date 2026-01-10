# =====================
# Variables generales
# =====================
CXX = g++
STD = -std=c++17
WARN = -Wall
INCLUDES = -Iinclude -Iinclude/third_party

# Opciones de compilación (descomentar para habilitar)
# USE_OPENEXR = 1
# USE_LIBPNG = 1

OBJDIR = obj
SRCDIR = src
BINDIR = bin
TARGET = $(BINDIR)/pathtracer

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# =====================
# Detección de sistema
# =====================
# Detectar Windows por la variable de entorno OS
ifdef OS
    # ---- Windows ----
    CXXFLAGS = $(STD) $(WARN) $(INCLUDES) -mconsole
    LDFLAGS  =
    RM = del /Q /S
    RMDIR = rmdir /S /Q
    MKDIR_P = mkdir
else
    # ---- Linux / WSL ---- (Embree SIEMPRE activo)
    EMBREE_INC ?= /usr/include
    EMBREE_LIB ?= /usr/lib/x86_64-linux-gnu
    CXXFLAGS = $(STD) $(WARN) $(INCLUDES) -O2 -DUSE_EMBREE -I$(EMBREE_INC)
    LDFLAGS  = -L$(EMBREE_LIB) -lembree4 -ltbb
    RM = rm -f
    RMDIR = rm -rf
    MKDIR_P = mkdir -p
    
    # Agregar soporte para OpenEXR en Linux/WSL (opcional)
    ifdef USE_OPENEXR
        CXXFLAGS += -DUSE_OPENEXR
        LDFLAGS += -lOpenEXR -lIlmImf -lImath
    endif
    
    # Agregar soporte para libpng en Linux/WSL (opcional)
    ifdef USE_LIBPNG
        CXXFLAGS += -DUSE_LIBPNG $(shell pkg-config --cflags libpng)
        LDFLAGS += $(shell pkg-config --libs libpng)
    endif
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