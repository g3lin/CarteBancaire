all:	 TestMessage TestRedirection TestLectureEcriture testAnnuaire Terminal Autorisation Acquisition

message.o: message.c message.h
	gcc -Wall -c message.c

alea.o: alea.h alea.c
	gcc -Wall -c alea.c

TestMessage: message.o alea.o TestMessage.c
	gcc -Wall TestMessage.c message.o alea.o -o  TestMessage

testAnnuaire: testAnnuaire.c annuaire.o alea.o lectureEcriture.o
	gcc testAnnuaire.c annuaire.o alea.o lectureEcriture.o -o testAnnuaire

annuaire.o: annuaire.c annuaire.h alea.h lectureEcriture.h
	gcc -c annuaire.c

TestRedirection: TestRedirection.c
	gcc -Wall TestRedirection.c -o  TestRedirection

lectureEcriture.o: lectureEcriture.c lectureEcriture.h
	gcc -c -Wall lectureEcriture.c

TestLectureEcriture: lectureEcriture.o TestLectureEcriture.c
	gcc lectureEcriture.o TestLectureEcriture.c -o TestLectureEcriture

Terminal: lectureEcriture.o Terminal.c message.o annuaire.o alea.o
	gcc lectureEcriture.o message.o annuaire.o alea.o Terminal.c -o Terminal

Autorisation: lectureEcriture.o message.o annuaire.o Autorisation.c alea.o
	gcc lectureEcriture.o message.o annuaire.o alea.o Autorisation.c -o Autorisation

Acquisition_Demande.o: Acquisition_Demande.c Acquisition.h 
	gcc -c -Wall  Acquisition.h Acquisition_Demande.c

Acquisition_Reponse.o: Acquisition_Reponse.c Acquisition.h 
	gcc -c -Wall Acquisition.h Acquisition_Reponse.c

Acquisition: Acquisition.c Acquisition.h lectureEcriture.o message.o Acquisition_Demande.o Acquisition_Reponse.o Autorisation Terminal 
	gcc -pthread Acquisition.c lectureEcriture.o message.o Acquisition_Demande.o Acquisition_Reponse.o -g -o Acquisition

clean:	
	rm -f *.o *~ 

cleanall: clean
	rm TestRedirection TestMessage TestLectureEcriture Terminal Autorisation Acquisition testAnnuaire
