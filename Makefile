make rad_dose:
	g++ -c rad_dose_NEW.cc -o rad_dose.o `root-config --cflags --glibs`
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

make pruneTreeEnvelope:
	g++ -g -c pruneTreeEnvelope.c -o pruneTreeEnvelope.o `root-config --cflags --glibs`
	g++ pruneTreeEnvelope.o libremoll.so -o pruneTreeEnvelope `root-config --cflags --glibs` -L. -lremoll -Wl,-R.
	rm pruneTreeEnvelope.o

make pruneTreeLGtest:
	g++ -g -c pruneTreeLGtest.c -o pruneTreeLGtest.o `root-config --cflags --glibs`
	g++ pruneTreeLGtest.o libremoll.so -o pruneTreeLGtest `root-config --cflags --glibs` -L. -lremoll -Wl,-R.
	rm pruneTreeLGtest.o

make pe:
	g++ libremoll.so -shared -o libpe.so -fPIC pe.cc `root-config --cflags --glibs` -L. -lremoll -Wl,-R.
	g++ -g -c pe.cc -o pe.o `root-config --cflags --glibs`
	g++ pe.o libremoll.so -o pe `root-config --cflags --glibs` -L. -lremoll -Wl,-R.
	rm pe.o

clean:
	rm ./*.o
	rm ./fom
	rm hallRad; rm rad_dose; rm pruneTree; rm pruneTreeEnvelope; rm pruneTreeLGtest; rm pe;
