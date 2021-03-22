TARGETS=player ringmaster

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

client: player.cpp
	gcc -g -o $@ $<

server: ringmaster.cpp
	gcc -g -o $@ $<