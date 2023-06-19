bf:
	mkdir -p build
	gcc src/*.c -o ./build/bf
clean:
	rm -r build
test:
	./build/bf '+++++++++++++[>+++++<-]>.' # print 'A'
