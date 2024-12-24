#include <stdio.h>
#include <string.h>

int main() {

   FILE *fp;
   char *sub[] = {"C Programming Tutorial\n", "C++ Tutorial\n", "Python Tutorial\n", "Java Tutorial\n"};
   char mstr[100];
   fp = fopen("file2.txt", "w");

   for (int i = 0; i < 4; i++) {
      fputs(sub[i], fp);
   }
   strcpy(mstr, "Copy a string of multiple lines.\n");
   for (int i = 0; i < 4; i++) {
      strcat(mstr, sub[i]);
   }
   
   fputs(mstr, fp);
   fclose(fp);
   
   return 0;
}
