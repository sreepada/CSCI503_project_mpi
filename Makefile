default:
	g++ `pkg-config --cflags opencv` facedetect-test.cpp `pkg-config --libs opencv` -o facedetect-test
	g++ `pkg-config --cflags opencv` facerec_demo.cpp `pkg-config --libs opencv` -o facerec_demo
mpi:
	mpic++ `pkg-config --cflags opencv` -lstdc++ imagedetect.cpp facedetect.h facedetectfn-test.cpp `pkg-config --libs opencv` -o imagedetect
	mpic++ `pkg-config --cflags opencv` facerec_demo.cpp `pkg-config --libs opencv` -o facerec_demo
facerec:
	g++ `pkg-config --cflags opencv` facerec_demo.cpp `pkg-config --libs opencv` -o facerec_demo
clean:
	rm *.png imagedetect facedetectfn-test facedetect-test facerec_demo
