make rad_dose:
	g++ -c rad_dose_v2.cc -o rad_dose.o `root-config --cflags --glibs`
	g++ rad_dose.o libremoll.so -o rad_dose `root-config --cflags --glibs` -L. -lremoll -Wl,-R.
	rm rad_dose.o

make hallRad:
	g++ -g -c hallRadiation.cc -o hallRad.o `root-config --cflags --glibs`
	g++ -c radDamage.cc -o radDamage.o `root-config --cflags --glibs`
	g++ hallRad.o radDamage.o libremoll.so -o hallRad `root-config --cflags --glibs` -L. -lremoll -Wl,-R.
	rm hallRad.o
	rm radDamage.o

make pruneTree:
	g++ -g -c pruneTree.c -o pruneTree.o `root-config --cflags --glibs`
	g++ pruneTree.o libremoll.so -o pruneTree `root-config --cflags --glibs` -L. -lremoll -Wl,-R.
	rm pruneTree.o

clean:
	rm hallRad;rm rad_dose; rm pruneTree;
	rm ./*.o
	rm ./fom
