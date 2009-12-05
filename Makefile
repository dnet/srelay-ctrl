relay: relay.c
	gcc relay.c -o relay

.PHONY: clean
clean:
	rm -f relay
