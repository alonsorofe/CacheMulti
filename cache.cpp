#include <iostream>
#include <fstream>

using namespace std;

#include "cache.h"

static int error = 0;


//This function changes the state of the line whose processor is issuing the read.write request
char ownstatechanger (char ownstate, char rw) //ownstate, rw. Se quito: istate
{
    switch (ownstate)
    {
    /*case 'i':		////////////estos cambios de estado a partir de 'i' si se realizan pero en otros momentos.
        if (rw == 'L')
        {
            if (istate == 0)
                return ('e');
            else
                return ('s');
        }
        else
            return ('m');
        break;*/
    case 'i':
        if (rw == 'L')
            return ('i');
        else
            return ('i');
        break;
    case 'e':
        if (rw == 'L')
            return ('e');
        else
            return ('m');
        break;
    case 'm':
        if (rw == 'L')
            return ('m');
        else
            return ('m');
        break;
    case 's':
        if (rw == 'L')
            return ('s');
        else
            return ('m');
        break;
    default:
        error++;
    }
    return ownstate;
}


//This function changes the state of the line of the other processor
char otherstatechanger (char ownstate, char rw) //ownstate, rw. Se quito: istate
{
    switch (ownstate)
    {
    case 'i':
        if (rw == 'L')
            return ('i');
        else
            return ('i');
        break;
    case 'e':
        if (rw == 'L')
            return ('s');
        else
            return ('i');
        break;
    case 'm':
        if (rw == 'L')
            return ('s');
        else
            return ('i');
        break;
    case 's':
        if (rw == 'L')
            return ('s');
        else
            return ('i');
        break;
    default:
        error++;
    }
  return ownstate;
}


// This Function is used to check if the specified L1 cache with same index has the same tag, serves for both my L1 and the other L1.
bool L1statechecker (int proc_id, int index_temp, unsigned int tag_temp)
{
    if (line[proc_id][index_temp].tag == tag_temp) //&& (line[i][index_temp].state != 'i')
        return (true);
    else
        return (false);
}


// This Function is used to check if L2 cache with same index has the same tag.
bool L2statechecker (int index_temp, unsigned int tag_temp)
{
    if (principal[index_temp].tag == tag_temp) //&& (linep[index_temp].state != 'i')
        return (true);
    else
        return (false);
}


void presentNonInvalid (int miL1, int otroL1, int index_temp, unsigned int tag_temp, int indexL2, unsigned int tagL2, char rw)
{
    char ownstate;
    char otherstate;
    char newstate;

    ownstate = line[miL1][index_temp].state;
    newstate = ownstatechanger(ownstate, rw);

    if (L1statechecker(otroL1, index_temp, tag_temp))
    {
	otherstate = otherstatechanger (newstate, rw);
	line[otroL1][index_temp].state = otherstate;
    }

    if (L2statechecker(indexL2, tagL2))
    {
	otherstate = otherstatechanger (newstate, rw);
	principal[indexL2].state = otherstate;
    }

    line[miL1][index_temp].state = newstate;
    line[miL1][index_temp].tag = tag_temp;
}

