include kx.mk

AMPS_VERSION = 5.3.5.6
CXX = g++
CXXFLAGS += -Wall -fPIC -fpermissive -Werror -Wno-unused-variable -std=c++11 -DKXVER=3 -m64 -fvisibility=hidden
CPPFLAGS += -MMD -MP -I ./external/amps-c++-client-$(AMPS_VERSION)-Linux/include/ -I ./src
LDFLAGS  += -shared -L ./external/amps-c++-client-$(AMPS_VERSION)-Linux/lib/x86_64/release/
LDLIBS = -lamps 
SRCS = $(wildcard ./src/*.cpp)
OBJS = $(patsubst %.cpp,%.o, $(SRCS))

OUTDIR = ./build/kx/amps
NAME = clib.$A.$E
TGT = $(OUTDIR)/$(NAME)

.PHONY: all clean

all: $(TGT) $(OUTDIR)/init.q

-include $(OBJS:.o=.d)

$(TGT): $(OBJS)
	mkdir -p $(OUTDIR)
	$(CXX) -o $(TGT) $(OBJS) $(LDFLAGS) $(LDLIBS)

$(OUTDIR)/init.q: q/init.q
	mkdir -p $(OUTDIR)
	cp q/init.q $(OUTDIR)

clean:
	rm -rf $(OUTDIR) src/*.o src/*.d
