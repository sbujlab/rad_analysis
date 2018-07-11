make rad_dose:
	g++ -c rad_dose_v2.cc -o rad_dose.o `root-config --cflags --glibs`
	g++ rad_dose.o libremoll.so -o rad_dose `root-config --cflags --glibs`
	rm rad_dose.o

make hallRad:
	g++ -g -c hallRadiation_sum.cc -o hallRad.o `root-config --cflags --glibs`
	g++ -c radDamage.cc -o radDamage.o `root-config --cflags --glibs`
	g++ hallRad.o radDamage.o libremoll.so -o hallRad `root-config --cflags --glibs`
	rm hallRad.o
	rm radDamage.o

clean:
	rm rad_dose
	rm hallRad
	rm ./*.o
	rm ./fom
