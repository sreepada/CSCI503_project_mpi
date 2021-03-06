default:
	g++ `pkg-config --cflags opencv` facedetect-test.cpp `pkg-config --libs opencv` -o facedetect-test
	g++ `pkg-config --cflags opencv` facerec_demo.cpp `pkg-config --libs opencv` -o facerec_demo
mpi:
	mpic++ `pkg-config --cflags opencv` -lstdc++ imagedetect.cpp `pkg-config --libs opencv` -o imagedetect
ompmpi:
	mpic++ `pkg-config --cflags opencv` -lstdc++ imagedetect_omp.cpp `pkg-config --libs opencv` -o imagedetectomp
serial:
	g++ `pkg-config --cflags opencv` serial.cpp `pkg-config --libs opencv` -o serial
trainer:
	g++ `pkg-config --cflags opencv` trainer.cpp `pkg-config --libs opencv` -o trainer
facerec:
	g++ `pkg-config --cflags opencv` facerec_demo.cpp `pkg-config --libs opencv` -o facerec_demo
clean:
	rm *.png serial imagedetect facedetectfn-test facedetect-test facerec_demo trainer
