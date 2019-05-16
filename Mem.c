#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 64

unsigned char memory[SIZE];
// The memory is divided into 4 16 bit pages

unsigned char disc[5*SIZE];
// The disc memory for part 2

 int pt_register[4] = {-1,-1,-1,-1};
 /*pointer to where pid's page table is
 stored in the memory array (-1) means
 rhat the page table is not stored in memory*/

 int pt_disc[4] = {-1,-1,-1,-1};
 /*same thing but points to page
 table in disk instead of memory*/

int free_pages[4] = {1,1,1,1};
/* index is pfn, 1 is free, 0 is
taken */
int free_disc_pages[20];

int next_swap = 0;

int swap(void) { // moves page to disk
  int disc_pfn;
  //find space in disk
  for (int i = 0; i < 20; i++){
    if(free_disc_pages[i]){
      disc_pfn = i;
      free_disc_pages[i] = 0;
      break;
    }
  }
  //write memory of page removed to disk
  for(int i = 0; i < 16; i++) {
    disc[16*disc_pfn + i] = memory[16*next_swap + i];
  }
  int pt = -1; //boolean for if page being swapped is page tables
  for(int i = 0; i < 4;i++){
    if (pt_register[i] == next_swap) pt = i;
  }
  if(pt < 0){ //not a page table
    //update page table
    int pid = -1;
    int vpn;
    int in_disk = 1;
    //check memory page tables to see if its the one for the removed page
    for(int i = 0; i < 4; i++){
      for(int j = 0; j < 4; j++){
        if(memory[16*pt_register[i]+(4*j)+1] == next_swap && pt_register[i] >= 0){
          pid = i;
          vpn = j;
          in_disk = 0;
          //printf("swapping virtual page %d of process %d\n",j,i);
        }
      }
    }
    if (in_disk){ // check disk for table
      for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
          if(disc[16*pt_disc[i]+(4*j)+1] == next_swap && pt_disc[i] >= 0){
            pid = i;
            vpn = j;
          }
        }
      }
      if(pid < 0){
        printf("Page table not found while swapping pages!\n");
        return -1;
      }
      //update table in disk
      disc[16*pt_disc[pid] + 4*vpn + 1] = 255; //invalid bit
      disc[16*pt_disc[pid] + 4*vpn + 2] = 2;
      disc[16*pt_disc[pid] + 4*vpn + 3] = disc_pfn;
    }
    //update table in memory
    else{
      memory[16*pt_register[pid] + 4*vpn + 1] = 255;
      memory[16*pt_register[pid] + 4*vpn + 2] = 2;
      memory[16*pt_register[pid] + 4*vpn + 3] = disc_pfn;
    }
  }else{
    pt_register[pt] = -1;
    pt_disc[pt] = disc_pfn;
  }
  printf("Swapped frame %d to disc frame %d\n", next_swap,disc_pfn);
  free_pages[next_swap] = 1; //say page is free
  next_swap++; // round robin for swapping pages
  if (next_swap == 4) next_swap = 0;
  return 0;
}

int put_pt(int pid) {
  if(pt_register[pid]>=0) return 1;
  for(int i = 0; i < 4; i++) { // put page table in memory
    if(free_pages[i]) {
      if(pt_disc[pid] < 0 && pt_register[pid] < 0){ //page table not in disc
        pt_register[pid] = i;
        free_pages[i] = 0;
        for(int j = 0; j < 16; j++) memory[16*i + j] = 0;
      }
      else{ //page table on disk
        pt_register[pid] = i;
        free_pages[i] = 0;
        for(int j = 0; j < 16; j++) memory[16*i + j] = disc[16*pt_disc[pid] + j];
        pt_disc[pid] = -1;
        printf("Put page table for pid %d into physical frame %d\n",pid,i);
      }
      break;
    }
    if(i == 3){ //memory full
      swap();
      put_pt(pid);
    }
  }
  return 1;
}

int put_page(int pid, int perm, int adress) {
  int phys_page;
  if(pt_register[pid] < 0) put_pt(pid);
  for(int i = 0; i < 4; i++) { // put virtual page in memory
    if(free_pages[i]) {
      phys_page = i;
      free_pages[i] = 0;
      break;
    }
    if (i == 3) {//memory full
      swap(); //get rid of a page
      phys_page = put_page(pid,perm,adress);
      return phys_page;
    }
  }
  // entery to page table
  int pte = 16*pt_register[pid] + 4*(adress/16); //physical adress of PTE
   // copy over data if on disk
  if(memory[pte+2] == 2){
    for (int i = 0; i < 16; i++){
      memory[16*phys_page + i] = disc[16*memory[pte+3] + i];
    }
    free_disc_pages[memory[pte+3]] = -1;
    printf("Put disc frame %d into physical frame %d\n",memory[pte+3],phys_page);
  }
  memory[pte] =  perm;// byte 0 is permission
  memory[pte+1] = phys_page; // byte 1 is the physical pfn
  memory[pte+2] = 1; /*byte 2 is if the page is in physical memory
  val of 2 means that the page is stored in disc*/
  memory[pte+3] = 255; //byte 3 is pfn in hard disk, 255 if not present in disk
  return phys_page;
}

 int map(int pid, int adress, int perm){
   int phys_page;
   if(pt_register[pid] < 0) {// no page table for current process
     put_pt(pid);
   }
   int pte = 16*pt_register[pid] + 4*(adress/16); //physical adress of PTE
   if(memory[pte+2] != 1) { //not created yet
     phys_page = put_page(pid,perm,adress);
     if (phys_page < 0) return -1;
   }
   else phys_page = memory[pte+1];
   pte = pte = 16*pt_register[pid] + 4*(adress/16); //physical adress of PTE
   memory[pte] = perm; //overwrite permissions
   printf("Mapped virtual adress %d (Page %d) into physical frame %d\n",adress,adress/16,phys_page);
   return 1;
 }

 int store(int pid,  int adress, int value) {
   if(pt_register[pid] < 0) {
     put_pt(pid);
   }
   int pte = 16*pt_register[pid] + 4*(adress/16); //physical adress of PTE
   if(memory[pte] == 0) {
     printf("Page is not writable!\n");
     return -1;
   }
   if(memory[pte+2] != 1){ // if the page is not in memory
     put_page(pid,1,adress);
   }
   pte = 16*pt_register[pid] + 4*(adress/16); //physical adress of PTE
   int pfn = memory[pte + 1];
   int phys_adr = 16*pfn + adress - 16*(adress/16);
   memory[phys_adr] = value;
   printf("Stored value %d into virtual adress %d (Physical adress %d)\n",value,adress,phys_adr);
   return 0;
 }

 int load(int pid, int adress) {
   if(pt_register[pid] < 0) put_pt(pid);
   int vpn = 16*pt_register[pid] + 4*(adress/16); //physical adress of PTE
   if(memory[vpn+2] != 1) put_page(pid,memory[vpn],adress);
   vpn = 16*pt_register[pid] + 4*(adress/16);
   int pfn = memory[vpn + 1];
   int phys_adr = 16*pfn + adress - 16*(adress/16);
   int val = (int)memory[phys_adr];
   printf("The value %d is virtual adress %d (Physical adress %d)\n",val,adress,phys_adr);
   return 0;
 }

int main(int argc, char *argv) {
  for(int i = 0; i < 20; i++) free_disc_pages[i] = 1;
  printf("Write commands below in form:\n");
  printf("\tprocess_id,instruction_type,virtual_adress,value\n");
  printf("Valid instruction types are:\n");
  printf("\tmap\n\tstore\n\tload");
  printf("\n");
  while(1){
    if(feof(stdin))break;
    printf("Instruction? ");
    char str[100];
    fgets(str,100,stdin);
    char *args[4];
    char *tok = strtok(str,",");
    args[0] = strdup(tok);
    int skip = 1;
    for (int i = 0; i < 3; i++){
      tok = strtok(NULL,",");
      if (tok == NULL){
        printf("Invalid number of commands!\n");
        skip = 0;
        break;
      }
      args[1+i] = strdup(tok);
    }
    if(atoi(args[0]) < 0 || atoi(args[0]) > 3 || atoi(args[2]) < 0 ||
  atoi(args[2]) > 63 || atoi(args[3]) < 0 || atoi(args[3]) > 255) {
      skip = 0;
      printf("improper integer amount!\n");
    }
    if(skip){
      if(strcmp(args[1],"map\0")==0) {
        map(atoi(args[0]),atoi(args[2]),atoi(args[3]));
      }
      else if(strcmp(args[1],"store") == 0) {
        store(atoi(args[0]),atoi(args[2]),atoi(args[3]));
      }
      else if(strcmp(args[1],"load") == 0) {
        load(atoi(args[0]),atoi(args[2]));
      }
      else printf("unknown command, try again.\n");
    }
  }
  return 1;
}
