#include <iostream>
#include <fstream>
#include <stdlib.h>


using namespace std;

#include "cache.h"
cacheline line[2][512];  //2 caches L1
cacheline principal[4096]; //cache L2


int main()
{

    char rw;
    int indexTempL1,indexTempL2; //index storage
    unsigned int tagTempL1, tagTempL2; //tag storage
    ///short pro_id=0;
    ///int istate; //initial state checking variable
    long address;//char address[10];
    ifstream myFile;
    myFile.open("aligned.trace", fstream::in);

    long HitsL1[2], MissL1[2], HitsL2, MissL2;
    HitsL1[0] = 0;
    MissL1[0] = 0;
    HitsL1[1] = 0;
    MissL1[1] = 0;
    HitsL2 = 0;
    MissL2 = 0;

    if (!myFile)
    {
        cout << "Error abriendo archivo" << endl;
        exit(1);
    }
//Initializing all lines to 'i' state and storing tag = 0x00000000 just as an indicator in them
    for (int i = 0; i<2; i++) //Caches L1
    {
        for (unsigned int j=0; j<512; j++)
        {
            line[i][j].state = 'i';
            line[i][j].tag = 0x00000000;
        }
    }

    for (unsigned int j=0; j<4096; j++) //Cache L2
        {
            principal[j].state = 'i';
            principal[j].tag = 0x00000000;
        }

    int i = 0;
    int j = 1;


    //const char *filename = "blah.txt";
    //ifstream infile(filename, fstream::in);

    long counter = 0;
    while (!myFile.eof())
    {
        myFile >> hex >> address;
        myFile >> rw;
        ///cout << hex <<  address << endl;
        ///cout << rw << endl;
        counter++;
        ///if((counter % 2000000)==0)
        //    cout << "Leidas " << counter << " lineas" << endl;
	//cache L2
        tagTempL2 = address >> 16; //4 bits de byte offset y 12 bits de index.
        indexTempL2 = (address >> 4) & 4095;

	//caches L1
        tagTempL1 = address >> 13; //4 bits de byte offset y 9 bits de index.
        indexTempL1 = (address >> 4) & 511;

	/*
	//conversion a tipo L1 a partir de tag e indice de L2
	tagL1 = (tagTempL2 << 3) | (indexTempL2 >> 9);
	indexL1 = indexTempL2 & 511;

	//conversion a tipo L2 a partir de tag e indice  de L1
	tagL2 = (tagTempL1 >> 3);
	indexL2 = ((tagTempL1 & 7) << 9) | (indexTempL1);
	*/

	if (line[i][indexTempL1].tag == tagTempL1 && line[i][indexTempL1].state != 'i') ///si tengo el tag y si su estado es distinto de 'i'.
	{
	    presentNonInvalid(i, j, indexTempL1, tagTempL1, indexTempL2, tagTempL2, rw);
	    HitsL1[i]++;
	}
    else ///si no tengo el tag o su estado es 'i'.
    {
        MissL1[i]++;
        if (L2statechecker(indexTempL2, tagTempL2) && principal[indexTempL2].state != 'i') ///si L2 tiene el tag y es distinto de 'i'.
        {
            HitsL2++;
            if (rw == 'L') ///si es solo lectura paso el dato como share.
            {
                principal[indexTempL2].state = 's';
                line[i][indexTempL1].state = 's';
            }else {///si es escitura paso el dato como exclusive.

                principal[indexTempL2].state = 'i';
                line[i][indexTempL1].state = 'e';
            }
            line[i][indexTempL1].tag = tagTempL1; ///lo paso al cache desde el cual pregunte.

            presentNonInvalid(i, j, indexTempL1, tagTempL1, indexTempL2, tagTempL2, rw); ///ahora que lo tengo valido en el cache, sigo.
        }
        else if ((L2statechecker(indexTempL2, tagTempL2) && principal[indexTempL2].state == 'i') || (L1statechecker(j, indexTempL1, tagTempL1) && line[j][indexTempL1].state != 'i'))///si L2 tiene el tag pero es 'i', o el otro L1 lo tiene y es valido.
        {
            if (L1statechecker(j, indexTempL1, tagTempL1) && line[j][indexTempL1].state != 'i') ///si lo tiene el otro L1 y es valido.
            {
                if (rw == 'L') ///si es solo lectura paso el dato como share.
                {
                    principal[indexTempL2].state = 's';
                    line[i][indexTempL1].state = 's';
                    line[j][indexTempL1].state = 's';
                }else {///si es escitura paso el dato como exclusive.

                    principal[indexTempL2].state = 'i';
                    line[j][indexTempL1].state = 'i';
                    line[i][indexTempL1].state = 'e';
                }
                line[i][indexTempL1].tag = tagTempL1; ///lo paso al cache desde el cual pregunte.

                presentNonInvalid(i, j, indexTempL1, tagTempL1, indexTempL2, tagTempL2, rw); ///ahora que lo tengo valido en el cache, sigo.
            }
            else ///si tampoco lo tiene el otro L1 o lo tiene invalido.
            {
                if (rw == 'L') ///si es solo lectura cargo el dato a L2 como share (como si fuera desde memoria) y paso el dato como share al L1 original.
                {
                    principal[indexTempL2].state = 's';
                    line[i][indexTempL1].state = 's';
                }else {///si es escitura cargo el dato a L2 como invalido (como si fuera desde memoria) y paso el dato como exclusive al L1 original.

                    principal[indexTempL2].state = 'i';
                    line[i][indexTempL1].state = 'e';
                }
                principal[indexTempL2].tag = tagTempL2; //como en ambos cargo dato a L2, en el codigo se pone en comun.
                line[i][indexTempL1].tag = tagTempL1; ///lo paso al cache desde el cual pregunte.

                presentNonInvalid(i, j, indexTempL1, tagTempL1, indexTempL2, tagTempL2, rw); ///ahora que lo tengo valido en el cache, sigo.
            }
        }else ///si no esta en el L2, ni lo tiene el otro L1, o lo tiene pero invalido.
        {
            MissL2++;
            if (rw == 'L') ///si es solo lectura cargo el dato a L2 como share (como si fuera desde memoria) y paso el dato como share al L1 original.
            {
                principal[indexTempL2].state = 's';
                line[i][indexTempL1].state = 's';
            }else {///si es escitura cargo el dato a L2 como invalido (como si fuera desde memoria) y paso el dato como exclusive al L1 original.

                principal[indexTempL2].state = 'i';
                line[i][indexTempL1].state = 'e';
            }
            principal[indexTempL2].tag = tagTempL2; //como en ambos cargo dato a L2, en el codigo se pone en comun.
            line[i][indexTempL1].tag = tagTempL1; ///lo paso al cache desde el cual pregunte.

            presentNonInvalid(i, j, indexTempL1, tagTempL1, indexTempL2, tagTempL2, rw); ///ahora que lo tengo valido en el cache, sigo.
        }
    }


	if (i == 1)
	    i = 0;
	else if (i == 0)
	    i = 1;

	if (j == 1)
	    j = 0;
	else if (j == 0)
	    j = 1;

    }

    cout << "Leidas " << counter << " lineas" << endl << endl;

    cout << "Hits de 1er cache L1: " << HitsL1[0] << endl;
    cout << "Hits de 2do cache L1: " << HitsL1[1] << endl;
    cout << "Hits de cache L2: " << HitsL2 << endl << endl;

    cout << "Misses de 1er cache L1: " << MissL1[0] << endl;
    cout << "Misses de 2do cache L1: " << MissL1[1] << endl;
    cout << "Misses de cache L2: " << MissL2 << endl << endl << endl;



    cout << "Estado final de los datos cuyos index fueron los 6 primeros en ser leidos distintos, en los diversos caches:" << endl << endl;

    cout << "Indice: 34, 1er cache L1, estado: " << line[0][34].state << endl;
    cout << "Indice: 34, 2do cache L1, estado: " << line[1][34].state << endl;
    cout << "Indice: 3618, cache L2, estado: " << principal[3618].state << endl << endl;

    cout << "Indice: 33, 1er cache L1, estado: " << line[0][33].state << endl;
    cout << "Indice: 33, 2do cache L1, estado: " << line[1][33].state << endl;
    cout << "Indice: 3617, cache L2, estado: " << principal[3617].state << endl << endl;

    cout << "Indice: 32, 1er cache L1, estado: " << line[0][32].state << endl;
    cout << "Indice: 32, 2do cache L1, estado: " << line[1][32].state << endl;
    cout << "Indice: 3616, cache L2, estado: " << principal[3616].state << endl << endl;

    cout << "Indice: 255, 1er cache L1, estado: " << line[0][255].state << endl;
    cout << "Indice: 255, 2do cache L1, estado: " << line[1][255].state << endl;
    cout << "Indice: 255, cache L2, estado: " << principal[255].state << endl << endl;

    cout << "Indice: 305, 1er cache L1, estado: " << line[0][305].state << endl;
    cout << "Indice: 305, 2do cache L1, estado: " << line[1][305].state << endl;
    cout << "Indice: 305, cache L2, estado: " << principal[305].state << endl << endl;

    cout << "Indice: 306, 1er cache L1, estado: " << line[0][306].state << endl;
    cout << "Indice: 306, 2do cache L1, estado: " << line[1][306].state << endl;
    cout << "Indice: 306, cache L2, estado: " << principal[306].state << endl << endl;


}


