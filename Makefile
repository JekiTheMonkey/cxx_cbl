TARGET = CBL
CXX = g++
CXXFLAGS = -std=c++98 -ansi -Wall -Wextra -Werror -O0 -g
INCLUDES = -I$(INCDIR)
LIBS =
LDFLAGS = $(INCLUDES) $(LIBS)

SRCDIR = src
INCDIR = include
BUILDDIR = build
SRCEXT = cpp
INCEXT = hpp
OBJEXT = o

SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

all: options mkbuilddir $(TARGET)

run: all
	$(BUILDDIR)/$(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $(BUILDDIR)/$@ $(CXXFLAGS) $(LDFLAGS) $^

$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDFLAGS)

options:
	@echo "$(TARGET) build options:"
	@echo "CXXFLAGS = $(CXXFLAGS)"
	@echo "LDLAGS   = $(LDLAGS)"
	@echo "CXX      = $(CXX)"
	@echo "SOURCES  = $(SOURCES)"
	@echo "OBJECTS  = $(OBJECTS)"

mkbuilddir:
	@mkdir -p $(BUILDDIR)

clean:
	rm -f $(OBJECTS) $(BUILDDIR)/$(TARGET)

.PHONY: all, clean, options, mkbuilddir
