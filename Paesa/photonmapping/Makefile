#---------------------------------------------------------
# macros
RM = /bin/rm -f  # para limpiar
CC = g++         # para compilar
BIN = ../bin     # para los binarios
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SRCS))
#---------------------------------------------------------
# opciones de compilación
CPPFLAGS= -std=c++17 -fmax-errors=1 -Ofast # opciones compilación
#---------------------------------------------------------
#---------------------------------------------------------
photonmapping: $(OBJS)
	$(CC) $(CPPFLAGS) -o photonmapping $^
#---------------------------------------------------------
%.o: %.cpp %.h
	$(CC) $(CPPFLAGS)  -c  $<
#---------------------------------------------------------
clean:
	$(RM) pruebas $(OBJS)
