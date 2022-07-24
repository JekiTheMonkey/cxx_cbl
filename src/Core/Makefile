include $(PWD)/config.mk

INCLUDES = -I$(INCDIR)
LIBS =
LDFLAGS = $(INCLUDES) $(LIBS)

OWNDIR = $(notdir $(shell pwd))
SRCDIR = .
INCDIR = $(PWD)/include/$(OWNDIR)
BUILDDIR = $(PWD)/build/$(OWNDIR)

SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

all: $(OBJECTS)

$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDFLAGS)

.PHONY: all
