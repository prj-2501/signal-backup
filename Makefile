# appropriate version of sqlcipher is assumed to be installed in /usr/local
CXXFLAGS += -O3 -Wall -Werror -I /usr/local/include
LDFLAGS += -L/usr/local/lib -lsqlcipher

OBJS := scab

all: $(OBJS)

%: %.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(OBJS)
.PHONY: all clean
