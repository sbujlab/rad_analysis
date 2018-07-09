make rad_dose:
	g++ -c rad_dose_v2.cc -o rad_dose.o `root-config --cflags --glibs`
	g++ rad_dose.o -o rad_dose `root-config --cflags --glibs`
	rm rad_dose.o

make hallRad:
	g++ -c hallRadiation.cc -o hallRad.o `root-config --cflags --glibs`
	g++ -c radDamage.cc -o radDamage.o `root-config --cflags --glibs`
	g++ hallRad.o radDamage.o -o hallRad `root-config --cflags --glibs`
	rm hallRad.o
	rm radDamage.o

clean:
	rm ./*.o
	rm ./fom
