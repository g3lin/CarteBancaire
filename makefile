all:	 TestMessage TestRedirection TestLectureEcriture Terminal Autorisation Aquisition

message.o: message.c message.h
	gcc -Wall -c message.c

alea.o: alea.h alea.c
	gcc -Wall -c alea.c

TestMessage: message.o alea.o TestMessage.c
	gcc -Wall TestMessage.c message.o alea.o -o  TestMessage

TestRedirection: TestRedirection.c
	gcc -Wall TestRedirection.c -o  TestRedirection

lectureEcriture.o: lectureEcriture.c lectureEcriture.h
	gcc -c -Wall lectureEcriture.c

TestLectureEcriture: lectureEcriture.o TestLectureEcriture.c
	gcc lectureEcriture.o TestLectureEcriture.c -o TestLectureEcriture

Terminal: lectureEcriture.o Terminal.c message.o alea.o
	gcc lectureEcriture.o message.o alea.o Terminal.c -o Terminal

Autorisation: lectureEcriture.o message.o Autorisation.c
	gcc lectureEcriture.o message.o Autorisation.c -o Autorisation

Aquisition: Aquisition.c lectureEcriture.o Autorisation Terminal 
	gcc Aquisition.c lectureEcriture.o -o Aquisition

clean:	
	rm -f *.o *~ 

cleanall: clean
	rm TestRedirection TestMessage TestLectureEcriture Terminal Autorisation Aquisition
