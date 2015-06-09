all: hc-verifier hc-gen

hc-verifier: hc-verifier.cc
	g++ hc-verifier.cc -o hc-verifier -O0 -g
	
hc-gen: hc-gen.cc
	g++ hc-gen.cc -o hc-gen -O0 -g

clean:
	rm -f hc-verifier hc-gen
