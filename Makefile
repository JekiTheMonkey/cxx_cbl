include config.mk

INCLUDES = -I$(INCDIR)
LIBS =
LDFLAGS = $(INCLUDES) $(LIBS)

SRCDIR = src
INCDIR = include
BUILDDIR = build
INNERDIRS = $(shell find src -type d | tail -n +2 | cut -d/ -f2-)
BUILDDIRS = $(patsubst %,$(BUILDDIR)/%,$(INNERDIRS))

SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

all: options mkbuilddir $(TARGET)

run: all
	$(BUILDDIR)/$(TARGET)

$(TARGET):
	@+$(MAKE) -C src/Core
	@+$(MAKE) -C src/Client
	$(CXX) -o $(BUILDDIR)/$@ $(CXXFLAGS) $(LDFLAGS) $(OBJECTS)

options:
	@echo "$(TARGET) build options:"
	@echo "CXXFLAGS = $(CXXFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "CXX      = $(CXX)"

mkbuilddir:
	@mkdir -p $(BUILDDIRS)

clean:
	rm -f $(OBJECTS) $(BUILDDIR)/$(TARGET)

.PHONY: all, run, clean, options, mkbuilddir
