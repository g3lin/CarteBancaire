all:	 TestMessage TestRedirection TestLectureEcriture testAnnuaire Terminal Autorisation Acquisition InterBancaire generationAnnuaire

generationAnnuaire:generationAnnuaire.c annuaire.o alea.o lectureEcriture.o
	gcc -w generationAnnuaire.c annuaire.o alea.o lectureEcriture.o -o generationAnnuaire
	./generationAnnuaire 5 50 > log_generation_annuaires.txt

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

Acquisition_Demande.o: Acquisition_Demande.c Acquisition.h annuaire.h
	gcc -c -Wall annuaire.h Acquisition.h Acquisition_Demande.c

Acquisition_Reponse.o: Acquisition_Reponse.c Acquisition.h 
	gcc -c -Wall Acquisition.h Acquisition_Reponse.c

InterB_Demande.o: InterB_Demande.c InterBancaire.h 
	gcc -c -Wall  InterBancaire.h InterB_Demande.c

InterB_Reponse.o: InterB_Reponse.c InterBancaire.h 
	gcc -c -Wall InterBancaire.h InterB_Reponse.c

Acquisition: Acquisition.c Acquisition.h lectureEcriture.o message.o Acquisition_Demande.o Acquisition_Reponse.o annuaire.o alea.o Autorisation Terminal 
	gcc -pthread Acquisition.c lectureEcriture.o message.o Acquisition_Demande.o Acquisition_Reponse.o annuaire.o alea.o -g -o Acquisition

InterBancaire: InterBancaire.c InterBancaire.h Acquisition lectureEcriture.o message.o InterB_Demande.o InterB_Reponse.o
	gcc -pthread InterBancaire.c lectureEcriture.o message.o InterB_Demande.o InterB_Reponse.o -g -o InterBancaire

clean:	
	rm -f *.o *~ *.an

cleanall: clean
	rm TestRedirection TestMessage TestLectureEcriture Terminal Autorisation Acquisition testAnnuaire InterBancaire generationAnnuaire
