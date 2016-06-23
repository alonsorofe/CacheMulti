#ifndef CACHE_H_INCLUDED
#define CACHE_H_INCLUDED
struct cacheline
{
    char state;
    unsigned int tag;
};
extern cacheline line[2][512];  //2 caches L1
extern cacheline principal[4096]; //cache L2

char ownstatechanger (char ownstate, char rw);
char otherstatechanger (char ownstate, char rw);
bool L1statechecker (int proc_id, int index_temp, unsigned int tag_temp);
bool L2statechecker (int index_temp, unsigned int tag_temp);
void presentNonInvalid (int miL1, int otroL1, int index_temp, unsigned int tag_temp, int indexL2, unsigned int tagL2, char rw);


#endif // CACHE_H_INCLUDED
