include $(PWD)/config.mk

INCLUDES = -I$(INCDIR) -I$(INCDIR)/Core
LIBS =
LDFLAGS = $(INCLUDES) $(LIBS)

OWNDIR = $(notdir $(shell pwd))
SRCDIR = .
INCDIR = $(PWD)/include
BUILDDIR = $(PWD)/build/$(OWNDIR)

SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

all: $(OBJECTS)

$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDFLAGS)

.PHONY: all
