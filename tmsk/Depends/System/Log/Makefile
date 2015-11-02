
objs = log.o


liblog.so : $(objs)
	g++ -shared -o liblog.so  $(objs) 

log.o : log.cpp log.h
	g++ -c -g -w log.cpp






#make test module
tobj = test.o

test : $(tobj)
	g++ -o test.sh  $(tobj) ./liblog.so

test.o : test.cpp
	g++ -c test.cpp






# clean temp files
clean :
	rm -rf liblog.so $(objs) test.sh $(tobj)

