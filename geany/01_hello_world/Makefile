#
# @version 250101
#
CC := @gcc

TARGET = $(shell basename $(shell pwd)).so

SRCDIR := src

OUTDIR := dest
BINDIR := $(OUTDIR)/.bin
OBJDIR := $(OUTDIR)/.obj
DEPDIR := $(OUTDIR)/.dep

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))
DEPS := $(addprefix $(DEPDIR)/, $(notdir $(SRCS:.c=.d)))

LIBDIR := $(HOME)/.local/usr/lib/geany/plugins/


#CFLAGS := -g3 -MMD -O0
CFLAGS = -DLOCALEDIR=\"\" -DGETTEXT_PACKAGE=\"foobar\" -fPIC $(shell pkg-config --cflags geany)

#LDFLAGS :=
LDFLAGS = -shared $(shell pkg-config --libs geany)

#DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d


$(BINDIR)/$(TARGET): $(OBJS) | $(BINDIR)
	@echo ...comple
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPDIR)/%.d | $(OBJDIR) $(DEPDIR)
	@echo ...link
	$(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $<

# The empty rule is required to handle the case where the dependency file is deleted.
$(DEPS):

include $(wildcard $(DEPS))

$(BINDIR):
	@mkdir -p $@

$(OBJDIR):
	@mkdir -p $@

$(DEPDIR):
	@mkdir -p $@

.PHONY: all
all: clean $(TARGET)

.PHONY: install
install:
	@mkdir -p $(LIBDIR)
	@cp $(BINDIR)/$(TARGET) $(LIBDIR)
	@echo ...installed $(LIBDIR)

.PHONY: uninstall
uninstall:
	@rm $(LIBDIR)/$(TARGET)
	@echo ...uninstalled $(LIBDIR)

.PHONY: clean
clean:
	@rm -rf $(OUTDIR)
