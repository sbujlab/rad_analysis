make:
	g++ -c rad_dose.cc -o rad_dose.o `root-config --cflags --glibs`
	g++ rad_dose.o -o rad_dose `root-config --cflags --glibs`
	rm rad_dose.o

	g++ -c rad_dose_cip.cc -o rad_dose_cip.o `root-config --cflags --glibs`
	g++ rad_dose_cip.o -o rad_dose_cip `root-config --cflags --glibs`
	rm rad_dose_cip.o
clean:
	rm ./*.o
	rm ./fom
